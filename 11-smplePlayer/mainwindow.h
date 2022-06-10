#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include "simpleplayer.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QImage mImage;
    void paintEvent(QPaintEvent *event);

public slots:
    void slotGetOneFrame(QImage);

private:
    Ui::MainWindow *ui;
    SimplePlayer *mplayer;
};
#endif // MAINWINDOW_H
