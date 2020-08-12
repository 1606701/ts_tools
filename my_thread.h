#ifndef MY_THREAD_H
#define MY_THREAD_H
#include <QObject>
#include <QMutex>
#include <fstream>
#include <QWaitCondition>

class my_thread: public QObject
{
    Q_OBJECT;

    char *tempbuf = new char[100];
public:
    explicit my_thread(QObject *parent = 0);
    /*
    Request the process to start
    It is thread safe as it uses mutex to protect to run variable
    */
    void request_thread();
    void request_thread1();
    void abort();
public:
    bool _abort;
    bool _working = true;

    QMutex mutex;
    QWaitCondition wait_list;
    QWaitCondition wait_list1;

signals:
    void thread_requested();
    void thread_requested1();
    void valueChanged(const QString &value);
    void  finished();

public slots:
    void doWork(char *filename,unsigned char *buffer);
    void doWork1(char *filename,unsigned char *buffer);
};

#endif // MY_THREAD_H
