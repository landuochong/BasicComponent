#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QString>
#include <QDebug>
#include <mutex>
#include "thread.h"
#include "EventBus/EventHandler.hpp"
#include "EventBus/Event.hpp"

namespace Ui {
class Widget;
}

class ThreadPool;

class CustomEvent : public basic_comm_eventbus::Event
{
public:
    CustomEvent(std::string msg){
        this->msg = msg;
    }

    std::string GetMsg(){
        return msg;
    }

private:
    std::string msg;
};

class CustomListener : public basic_comm_eventbus::EventHandler<CustomEvent>
{
public:
  virtual void onEvent(CustomEvent& e) override {
    //处理事件

  }
};


class Widget : public QWidget, public basic_comm_eventbus::EventHandler<CustomEvent>
{
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

private slots:
  void on_btn_thread_test_clicked();
  void on_btn_eventbus_clicked();

  void on_btn_net_clicked();
  void on_btn_media_clicked();
  void on_btn_connect_clicked();

private:
 Widget(const Widget&);
 Widget& operator=(const Widget&);

 void test(int value);

 virtual void onEvent(CustomEvent& e)override;

signals:
  void sig_send(int);

private:
    Ui::Widget *ui;
    volatile bool start;
    std::mutex mux;

    basic_comm::Thread thread_;
};

#endif // WIDGET_H
