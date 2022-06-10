#include "simpleplayer.h"
#include <QDebug>

SimplePlayer::SimplePlayer(){
}

void SimplePlayer::run(){
    /* 1. 讀取容器 */
    AVFormatContext *pFormatContext = avformat_alloc_context();
    int ret = avformat_open_input(&pFormatContext, "D:/Git/ffmpeg_study/input.mp4", NULL, NULL);
    if(ret < 0){
        qDebug()<< "ERROR: fail to open input.";
        return;
    }

    qDebug("Format %s, duration %lld us \n", pFormatContext->iformat->long_name, pFormatContext->duration);
    qDebug() << "=================================================";

    /* 2. 讀取 data stream */
    if(avformat_find_stream_info(pFormatContext,  NULL) < 0){
        qDebug("ERROR: could not get the stream info");
    }

    int video_stream_index = -1;
    const AVCodec *pCodec = NULL;
    AVCodecParameters *pCodecParameters =  NULL;

    /* 3. 尋訪 data stream */
    for (int i = 0; i < pFormatContext->nb_streams; i++){

        qDebug("finding the proper decoder (CODEC) ");

        /* 4. 查找解碼器 */
        AVCodecParameters *pLocalCodecParameters = pFormatContext->streams[i]->codecpar;
        const AVCodec *pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);

        // when the stream is a video, we store its index, codec parameters and codec
        if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO){
            if (video_stream_index == -1) {
                video_stream_index = i;
                pCodec = pLocalCodec;
                pCodecParameters = pLocalCodecParameters;
            }
            qDebug("Video Codec: resolution %d x %d \n", pLocalCodecParameters->width, pLocalCodecParameters->height);
        }
    }

    if (video_stream_index == -1) {
        qDebug("Does not contain a video stream! ");
        return;
    }

    /* 5. 打開解碼器 */
    AVCodecContext *pCodecContext = avcodec_alloc_context3(pCodec);
    if (!pCodecContext){
        qDebug("failed to allocated memory for AVCodecContext ");
        return;
    }

    avcodec_parameters_to_context(pCodecContext, pCodecParameters);
    if (avcodec_parameters_to_context(pCodecContext, pCodecParameters) < 0){
        qDebug("failed to copy codec params to codec context ");
        return;
    }

    if (avcodec_open2(pCodecContext, pCodec, NULL) < 0){
        qDebug("failed to copy codec params to codec context ");
        return;
    }

    /* 6. 讀取 packet & frame */
    AVPacket *pPacket = av_packet_alloc();
    if (!pPacket){
        qDebug("failed to allocate memory for AVPacket \n");
        return;
    }

    AVFrame *pFrame = av_frame_alloc();
    if (!pFrame){
        qDebug("failed to allocate memory for AVFrame \n");
        return;
    }

    //int how_many_packets_to_process = 8;
    int response = 0;

    while (av_read_frame(pFormatContext, pPacket) >= 0){

        // if it's the video stream
        if (pPacket->stream_index == video_stream_index) {
            response = decode_packet(pPacket, pCodecContext, pFrame);

            if (response < 0) break;

            // stop it, otherwise we'll be saving hundreds of frames
            //if (--how_many_packets_to_process <= 0) break;
        }

        av_packet_unref(pPacket); //清理AVPacket中的所有空间数据
    }

    printf("releasing all the resources");
    avformat_close_input(&pFormatContext);
    av_packet_free(&pPacket);
    avcodec_free_context(&pCodecContext);
}

int SimplePlayer::decode_packet(AVPacket *pPacket, AVCodecContext *pCodecContext, AVFrame *pFrame){
    struct SwsContext* sws_ctx = NULL;

    //發送 raw data packet (壓縮的frame) 至解碼器
    int response = avcodec_send_packet(pCodecContext, pPacket);

    if (response < 0) {
        printf("Error while sending a packet to the decoder \n");
        return response;
    }

    //Create SWS Context for converting from decode pixel format (like YUV420) to RGB
    sws_ctx = sws_getContext(pCodecContext->width,
                             pCodecContext->height,
                             pCodecContext->pix_fmt,
                             pCodecContext->width,
                             pCodecContext->height,
                             AV_PIX_FMT_RGB32,
                             SWS_BICUBIC,
                             NULL,
                             NULL,
                             NULL);
    if (sws_ctx == nullptr){
        return -1;
    }

    //Allocate frame for storing image converted to RGB
    AVFrame* pRGBFrame = av_frame_alloc();

    pRGBFrame->format = AV_PIX_FMT_RGB32;
    pRGBFrame->width = pCodecContext->width;
    pRGBFrame->height = pCodecContext->height;

    int sts = av_frame_get_buffer(pRGBFrame, 0);
    if(sts < 0 ){
        return -1;
    }

    while (response >= 0){

        //從解碼器接收 raw data frame（解壓後的frame）
        response = avcodec_receive_frame(pCodecContext, pFrame);
        if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
            break;
        } else if (response < 0) {
            printf("Error while receiving a frame from the decoder \n");
            return response;
        }


        if (response >= 0){
            printf(
                "Frame (%d) pts %d dts %d key_frame %d [ coded_picture_number %d, display_picture_number %d ] \n",
                pCodecContext->frame_number,
                pFrame->pts,
                pFrame->pkt_dts,
                pFrame->key_frame,
                pFrame->coded_picture_number,
                pFrame->display_picture_number
            );

            printf("frame => image \n");
            cout << "------------------------------------------------------------------" << endl;

            // 解碼後得到的圖像都是 YUV 4:2:0, 12bpp
            // That is the format of the provided .mp4 file
            //Convert from input format (e.g YUV420) to RGB and save to PPM:
            char frame_filename[1024];
            snprintf(frame_filename, sizeof(frame_filename), "%s-%d.ppm", "frame", pCodecContext->frame_number);

            sts = sws_scale(sws_ctx,                //struct SwsContext* c,
                           pFrame->data,            //const uint8_t* const srcSlice[],
                           pFrame->linesize,        //const int srcStride[],
                           0,                      //int srcSliceY,
                           pFrame->height,          //int srcSliceH,
                           pRGBFrame->data,        //uint8_t* const dst[],
                           pRGBFrame->linesize);   //const int dstStride[]);
            if (sts != pFrame->height){
                return -1;
            }

            //RGB -> QImage
            QImage tmpImg((uchar *)pRGBFrame->data[0],
                    pRGBFrame->width,
                    pRGBFrame->height,
                    QImage::Format_RGB32);
            QImage image = tmpImg.copy();
            emit sig_GetOneFrame(image);
        }
    }

    //Free
    sws_freeContext(sws_ctx);
    av_frame_free(&pRGBFrame);

    return 0;
}
