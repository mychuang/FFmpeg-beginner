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

    /* 顯示版本號 */
    unsigned version = avcodec_version();
    cout << "Hello FFmpeg: version = " << version << endl;
    cout << "=================================================" << endl;


    return a.exec();
}
