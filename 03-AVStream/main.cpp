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

    // 訪問數據流
    avformat_find_stream_info(pFormatContext,  NULL);

    //loop though all the streams
    //pFormatContext->nb_streams: get the amount of streams
    for (int i = 0; i < pFormatContext->nb_streams; i++){
        printf("AVStream->time_base before open coded %d/%d \n",
               pFormatContext->streams[i]->time_base.num, pFormatContext->streams[i]->time_base.den);
        printf("AVStream->r_frame_rate before open coded %d/%d \n",
               pFormatContext->streams[i]->r_frame_rate.num, pFormatContext->streams[i]->r_frame_rate.den);
        printf("AVStream->start_time %lld \n",
               pFormatContext->streams[i]->start_time);
        printf("AVStream->duration %lld \n",
               pFormatContext->streams[i]->duration);

        cout << "-------------------------------------------------" << endl;
    }

    return a.exec();
}
