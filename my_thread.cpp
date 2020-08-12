#include "my_thread.h"
#include <QThread>
#include <QDebug>
#include <QTimer>
#include <QEventLoop>
#include <QString>
#include <iostream>
#include <QWaitCondition>



unsigned int flag = 0;
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

void my_thread::doWork(char *filename,unsigned char *buffer)
{
    ofstream fp;
    fp.open(filename,ios::app);
    if(!fp)
    {
        qDebug()<<"open file failed save failed"<<endl;
        return ;
    }
    for(int i = 0;i < 188;i++)
    {
        sprintf(tempbuf,"%02x ",buffer[i]);
        fp<<tempbuf;
    }
    fp<<endl;
    fp.close();
    mutex.lock();
    wait_list.wakeAll();
    mutex.unlock();
    emit finished();
}
void my_thread::doWork1(char *filename,unsigned char *buffer)
{
    mutex.lock();
    if(flag == 1)
        wait_list.wait(&mutex);
    ofstream fp;
    fp.open(filename,ios::app);
    if(!fp)
    {
        qDebug()<<"open file failed save failed"<<endl;
        return ;
    }
    for(int i = 0;i < 188;i++)
    {
        sprintf(tempbuf,"%02x ",buffer[i]);
        fp<<tempbuf;
    }
    fp<<endl;
    fp.close();
    flag = 0;
    emit finished();
    wait_list1.wakeAll();
    mutex.unlock();
}
