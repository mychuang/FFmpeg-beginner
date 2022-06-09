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

    //loop though all the streams
    for (int i = 0; i < pFormatContext->nb_streams; i++){
        printf("finding the proper decoder (CODEC) \n");

        //獲取數據流的codec屬性: AVCodecParameters
        AVCodecParameters *pLocalCodecParameters = pFormatContext->streams[i]->codecpar;

        //獲取數據流的编解码方式: AVCodec
        const AVCodec *pLocalCodec = avcodec_find_decoder(pLocalCodecParameters->codec_id);

        // 输出邊解碼資訊
        if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_VIDEO){
            printf("Video Codec: resolution %d x %d \n", pLocalCodecParameters->width, pLocalCodecParameters->height);
        }else if (pLocalCodecParameters->codec_type == AVMEDIA_TYPE_AUDIO){
            printf("Audio Codec: %d channels, sample rate %d \n", pLocalCodecParameters->channels, pLocalCodecParameters->sample_rate);
        }
        printf("\tCodec %s ID %d bit_rate %lld \n", pLocalCodec->long_name, pLocalCodec->id, pLocalCodecParameters->bit_rate);
        cout << "-------------------------------------------------" << endl;
    }

    return a.exec();
}
