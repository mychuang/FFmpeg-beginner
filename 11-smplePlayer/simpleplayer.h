#ifndef SIMPLEPLAYER_H
#define SIMPLEPLAYER_H

#include <QObject>
#include <QThread>
#include <QImage>

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

class SimplePlayer : public QThread
{
    Q_OBJECT
public:
    SimplePlayer();
    void run() override;
    int decode_packet(AVPacket*, AVCodecContext*, AVFrame*);

signals:
    void sig_GetOneFrame(QImage); //每獲取到一張圖像，就發射訊號
};

#endif // SIMPLEPLAYER_H
