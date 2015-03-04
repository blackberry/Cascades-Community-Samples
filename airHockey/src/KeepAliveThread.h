
#ifndef KEEPALIVETHREAD_H_
#define KEEPALIVETHREAD_H_
#include <QThread>
#include <errno.h>
#include <iostream>

#include <btapi/btdevice.h>
#include <btapi/btspp.h>
#include <bb/cascades/Application> //for qDebug, why?
class KeepAliveThread : public QThread
{
    Q_OBJECT

public:
    KeepAliveThread(QObject *parent = 0);
     void run();
     void init(int fd);

private:
    int m_sppFD;
};

#endif
