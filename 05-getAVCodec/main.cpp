#include <QCoreApplication>
#include <iostream>
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

int main(int argc, char *argv[]){
    QCoreApplication a(argc, argv);

    // get content
    AVFormatContext *pFormatContext = avformat_alloc_context();
    int ret = avformat_open_input(&pFormatContext, "D:/Git/ffmpeg_study/input.mp4", NULL, NULL);
    if(ret < 0){
        printf("ERROR: fail to open input.");
        exit(1);
    }

    //Acess stream
    if(avformat_find_stream_info(pFormatContext,  NULL) < 0){
        printf("ERROR: could not get the stream info");
    }

    int video_stream_index = -1; //it will be used checking stream index
    const AVCodec *pCodec = NULL; //it will be used saving video codec
    AVCodecParameters *pCodecParameters =  NULL; //this component describes the properties of a codec used by the stream i

    //loop though all the streams
    for (int i = 0; i < pFormatContext->nb_streams; i++){
        printf("finding the proper decoder (CODEC) \n");

        //獲取數據流的codec屬性: AVCodecParameters
        AVCodecParameters *pLocalCodecParameters = pFormatContext->streams[i]->codecpar;

        //獲取數據流的编解码方式: AVCodec
        const AVCodec *pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);

        // when the stream is a video, we store its index, codec parameters and codec
        if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO){
            if (video_stream_index == -1) {
                video_stream_index = i; //it will be used checking stream index
                pCodec = pLocalCodec; //it will be used saving video codec
                pCodecParameters = pLocalCodecParameters; //this component describes the properties of a codec used by the stream i
            }
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

    //代入AVCodecParameters(properties of a codec)以獲取AVCodecContext
    avcodec_parameters_to_context(pCodecContext, pCodecParameters);

    //使用 avcodec_open2 打開解碼器
    avcodec_open2(pCodecContext, pCodec, NULL);


    return a.exec();
}
