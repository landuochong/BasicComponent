#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QString>
#include <QDebug>
#include "thread.h"

namespace Ui {
class Widget;
}

class ThreadPool;

class Widget : public QWidget
{
    Q_OBJECT
public:
    explicit Widget(QWidget *parent = nullptr);

private slots:
  void on_btn_send_clicked();
  void on_btn_update_clicked();

  void on_btn_net_clicked();
  void on_btn_media_clicked();
  void on_btn_connect_clicked();

private:
 Widget(const Widget&);
 Widget& operator=(const Widget&);

 void test(int value);

signals:
  void sig_send(int);

private:
    Ui::Widget *ui;
    volatile bool start;
    std::mutex mux;

    basic_comm::Thread thread;
};

#endif // WIDGET_H
