#include "my_thread.h"
#include <QThread>
#include <QDebug>
#include <QTimer>
#include <QEventLoop>
#include <QString>
#include <iostream>
#include "mainwindow.h"

using namespace std;

my_thread::my_thread(QObject *parent) :
    QObject(parent)
{
    _working = false;
    _abort   = false;
}

void my_thread::request_thread()
{
    mutex.lock();
    _working = true;
    _abort   = false;
    qDebug()<<"thread num:"<<thread()->currentThreadId();
    mutex.unlock();
    emit thread_requested();
}

void my_thread::request_thread1()
{
    mutex.lock();
    _working = true;
    _abort   = false;
    qDebug()<<"thread num:"<<thread()->currentThreadId();
    mutex.unlock();
    emit thread_requested();
}



void my_thread::abort()
{
    mutex.lock();
    if(_working)
    {
        _abort = true;
        qDebug()<<"thread num:"<<thread()->currentThreadId();
    }
    mutex.unlock();
}

void my_thread::doWork()
{
    qDebug()<<"thread num:"<<thread()->currentThreadId();
    int filesize = 0;
    QString str = nullptr;
    char *eit_buf = new char[1024];
    fstream fp;
    fp.open(eit_buf,ios::in);
    fp.seekg(0,ios::end);
    filesize = fp.tellg();
    fp.seekg(0,ios::beg);
    qDebug()<<"filesize:"<<filesize;
    while(filesize > 0)
    {
        //qDebug()<<"filesize:"<<filename;
        memset(eit_buf,0,1024);
        fp.read(eit_buf,1024);
        filesize -= 1024;
        mutex.lock();
        bool abort = _abort;
        mutex.unlock();
        if(abort)
        {
            qDebug()<<"break: thread num:"<<thread()->currentThreadId();
            break;
        }
        str = QString(QLatin1String(eit_buf));
        emit valueChanged(str);
    }
    fp.close();
    delete []eit_buf;
    mutex.lock();
    _working = false;
    mutex.unlock();
    qDebug()<<"thread num:"<<thread()->currentThreadId();
    emit finished();
}
