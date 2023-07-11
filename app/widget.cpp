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
#include <QTimer>
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
    qDebug() << "Running blocking task";
    std::this_thread::sleep_for(std::chrono::seconds(5));
}


void Widget::on_btn_send_clicked(){
    qDebug()<<"on_btn_send_clicked";

    thread.PostTask([&]{
        std::this_thread::sleep_for(std::chrono::seconds(2));
        qDebug()<<"PostTask";
    });

    thread.PostTask([&]{
        std::this_thread::sleep_for(std::chrono::seconds(1));
        qDebug()<<"PostTask2";
    });

    std::thread([&]() {
        qDebug()<<"start BlockingCall";
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
