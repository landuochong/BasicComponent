#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include <iostream>
#include <thread>
#include <QDateTime>
#include <windows.h>
#include <unordered_map>
#include <string>
#include <QTime>
#include <sstream>
#include <io.h>
#include<iomanip>
#include "windows.h"
#include <QDateTime>
#include <QTextCodec>
#include <QDir>

using namespace std;

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    thread.Start();
}

void blockingTask() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    qDebug() << "Running blocking task"<< QTime::currentTime().toString("hh:mm:ss.zzz");
}


void Widget::on_btn_send_clicked(){
    qDebug()<<"on_btn_send_clicked "<< QTime::currentTime().msec();;

    thread.PostTask([&]{
        std::this_thread::sleep_for(std::chrono::seconds(1));
        qDebug()<<"PostTask " << QTime::currentTime().toString("hh:mm:ss.zzz");
    });

    thread.PostDelayedTask([&]{
        qDebug()<<"DelayedTask " << QTime::currentTime().toString("hh:mm:ss.zzz");
    }, 3500000);

    thread.PostDelayedTask([&]{
        qDebug()<<"DelayedTask " << QTime::currentTime().toString("hh:mm:ss.zzz");
    }, 3'000'000);

    std::thread([&]() {
        qDebug()<<"start BlockingCall"<< QTime::currentTime().toString("hh:mm:ss.zzz");
        thread.BlockingCall([]{
            blockingTask();
        });
    }).detach();;

}

void Widget::on_btn_update_clicked(){

}

void Widget::on_btn_net_clicked(){

}

void Widget::on_btn_media_clicked(){

}

void Widget::on_btn_connect_clicked(){

}

void Widget::test(int value){
    qDebug()<<"pool end id:"<<value;
}
