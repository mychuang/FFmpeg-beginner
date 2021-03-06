#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    mplayer = new SimplePlayer();
    mplayer->start();
    connect(mplayer, &SimplePlayer::sig_GetOneFrame, this, &MainWindow::slotGetOneFrame);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotGetOneFrame(QImage img){
    mImage = img;
    qDebug()<<"get";
    update(); //调用update将执行 paintEvent函数
}

void MainWindow::paintEvent(QPaintEvent *event){
    QPainter painter(this);
    painter.setBrush(Qt::black);
    painter.drawRect(0, 0, this->width(), this->height()); //先画成黑色

    if (mImage.size().width() <= 0) return;

    ///將圖像比例縮放成和窗口一樣的大小
    QImage img = mImage.scaled(this->size(),Qt::KeepAspectRatio);

    int x = this->width() - img.width();
    int y = this->height() - img.height();

    x /= 2;
    y /= 2;

    painter.drawImage(QPoint(x,y),img); //画出图像

}
