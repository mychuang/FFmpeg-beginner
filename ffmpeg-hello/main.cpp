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


int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    /* check version of FFmpeg */
    unsigned version = avcodec_version();
    cout << "Hello FFmpeg: version = " << version << endl;


    /* check media information */

    // allocate memory for AVFormatContext, that will hold information about the format (container)
    AVFormatContext *pFormatContext = avformat_alloc_context();

    // open the file and read its header
    // two optional arguments set null, FFmpeg will guess the format：
    // AVInputFormat & AVDictionary（demuxer）
    int ret;
    ret = avformat_open_input(&pFormatContext, "D:/Git/ffmpeg_study/input.mp4", NULL, NULL);
    if(ret < 0){
        printf("fail to open input.");
        exit(1);
    }

    //We can print the format name and the media duration
    printf("Format %s, duration %lld us", pFormatContext->iformat->long_name, pFormatContext->duration);

    return a.exec();
}
