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

    // 分配記憶體給AVFormatContext: pFormatContext
    AVFormatContext *pFormatContext = avformat_alloc_context();

    // 打開media檔案，更新pFormatContext
    avformat_open_input(&pFormatContext, "D:/Git/ffmpeg_study/input.mp4", NULL, NULL);

    printf("Format %s, duration %lld us \n", pFormatContext->iformat->long_name, pFormatContext->duration);
    cout << "=================================================" << endl;


    return a.exec();
}
