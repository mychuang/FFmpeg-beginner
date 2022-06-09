#include <QCoreApplication>
#include <iostream>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
using namespace std;

extern "C"{
#include "libavutil/avutil.h"
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libavfilter/avfilter.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"
}

// decode packets into frames
static int decode_packet(AVPacket *pPacket, AVCodecContext *pCodecContext, AVFrame *pFrame);

// save a frame into a .pgm file
static void save_gray_frame(unsigned char *buf, int wrap, int xsize, int ysize, char *filename);

int main(int argc, char *argv[]){
    QCoreApplication a(argc, argv);

    // get content
    AVFormatContext *pFormatContext = avformat_alloc_context();
    int ret = avformat_open_input(&pFormatContext, "D:/Git/ffmpeg_study/input.mp4", NULL, NULL);
    if(ret < 0){
        printf("ERROR: fail to open input.");
        exit(1);
    }

    printf("Format %s, duration %lld us \n", pFormatContext->iformat->long_name, pFormatContext->duration);
    cout << "=================================================" << endl;

    //Acess stream
    if(avformat_find_stream_info(pFormatContext,  NULL) < 0){
        printf("ERROR: could not get the stream info");
    }

    int video_stream_index = -1;
    const AVCodec *pCodec = NULL;
    AVCodecParameters *pCodecParameters =  NULL;

    for (int i = 0; i < pFormatContext->nb_streams; i++){
        printf("AVStream->time_base before open coded %d/%d \n",
               pFormatContext->streams[i]->time_base.num, pFormatContext->streams[i]->time_base.den);
        printf("AVStream->r_frame_rate before open coded %d/%d \n",
               pFormatContext->streams[i]->r_frame_rate.num, pFormatContext->streams[i]->r_frame_rate.den);
        printf("AVStream->start_time %lld \n",
               pFormatContext->streams[i]->start_time);
        printf("AVStream->duration %lld \n",
               pFormatContext->streams[i]->duration);


        printf("finding the proper decoder (CODEC) \n");
        AVCodecParameters *pLocalCodecParameters = pFormatContext->streams[i]->codecpar;
        const AVCodec *pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);

        // when the stream is a video, we store its index, codec parameters and codec
        if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO){
            if (video_stream_index == -1) {
                video_stream_index = i;
                pCodec = pLocalCodec;
                pCodecParameters = pLocalCodecParameters;
            }
            printf("Video Codec: resolution %d x %d \n", pLocalCodecParameters->width, pLocalCodecParameters->height);
        }else if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO){
            printf("Audio Codec: %d channels, sample rate %d \n", pLocalCodecParameters->channels, pLocalCodecParameters->sample_rate);
        }

        printf("\tCodec %s ID %d bit_rate %lld \n", pLocalCodec->long_name, pLocalCodec->id, pLocalCodecParameters->bit_rate);
        cout << "-------------------------------------------------" << endl;
    }

    if (video_stream_index == -1) {
        printf("Does not contain a video stream! \n");
        return -1;
    }

    //利用獲取到的AVCodec幫AVCodecContext分配記憶體, AVCodecContext用以維護編解碼過程
    AVCodecContext *pCodecContext = avcodec_alloc_context3(pCodec);
    if (!pCodecContext){
        printf("failed to allocated memory for AVCodecContext \n");
        return -1;
    }

    //代入AVCodecParameters(properties of a codec)以獲取AVCodecContext
    avcodec_parameters_to_context(pCodecContext, pCodecParameters);
    if (avcodec_parameters_to_context(pCodecContext, pCodecParameters) < 0){
        printf("failed to copy codec params to codec context \n");
        return -1;
    }

    //使用 avcodec_open2 打開解碼器
    if (avcodec_open2(pCodecContext, pCodec, NULL) < 0){
        printf("failed to copy codec params to codec context \n");
        return -1;
    }

    AVPacket *pPacket = av_packet_alloc();
    if (!pPacket){
        printf("failed to allocate memory for AVPacket \n");
        return -1;
    }

    AVFrame *pFrame = av_frame_alloc();
    if (!pFrame){
        printf("failed to allocate memory for AVFrame \n");
        return -1;
    }

    int how_many_packets_to_process = 8;
    int response = 0;

    //Let's feed our packets from the streams
    while (av_read_frame(pFormatContext, pPacket) >= 0){

        // if it's the video stream
        if (pPacket->stream_index == video_stream_index) {
            response = decode_packet(pPacket, pCodecContext, pFrame);

            if (response < 0) break;

            // stop it, otherwise we'll be saving hundreds of frames
            if (--how_many_packets_to_process <= 0) break;
        }

        av_packet_unref(pPacket); //清理AVPacket中的所有空间数据
    }


    printf("releasing all the resources");
    avformat_close_input(&pFormatContext);
    av_packet_free(&pPacket);
    avcodec_free_context(&pCodecContext);

    return a.exec();
}

static int decode_packet(AVPacket *pPacket, AVCodecContext *pCodecContext, AVFrame *pFrame){

    //發送 raw data packet (壓縮的frame) 至解碼器
    int response = avcodec_send_packet(pCodecContext, pPacket);

    if (response < 0) {
        printf("Error while sending a packet to the decoder \n");
        return response;
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

            char frame_filename[1024];
            snprintf(frame_filename, sizeof(frame_filename), "%s-%d.pgm", "frame", pCodecContext->frame_number);

            // Check if the frame is a planar YUV 4:2:0, 12bpp
            // That is the format of the provided .mp4 file
            // RGB formats will definitely not give a gray image
            // Other YUV image may do so, but untested, so give a warning

            if (pFrame->format != AV_PIX_FMT_YUV420P){
                printf("Warning: the generated file may not be a grayscale image, but could e.g. be just the R component if the video format is RGB");
            }


            save_gray_frame(pFrame->data[0], pFrame->linesize[0], pFrame->width, pFrame->height, frame_filename);
        }
    }
    return 0;
}

static void save_gray_frame(unsigned char *buf, int wrap, int xsize, int ysize, char *filename){
    FILE *f;
    int i;
    f = fopen(filename, "w");

    // writing the minimal required header for a pgm file format
    // portable graymap format
    fprintf(f, "P5\n%d %d\n%d\n", xsize, ysize, 255);

    // writing line by line
    for (i = 0; i < ysize; i++)
        fwrite(buf + i * wrap, 1, xsize, f);
    fclose(f);
}
