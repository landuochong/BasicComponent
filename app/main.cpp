#include <QApplication>
#include <QDebug>
#include<iostream>
#include<memory>
#include "widget.h"


int main(int argc, char *argv[])
{    
    QApplication a(argc, argv);
    Widget widget;
    widget.show();
    return a.exec();
}
