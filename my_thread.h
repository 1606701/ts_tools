#ifndef MY_THREAD_H
#define MY_THREAD_H
#include <QObject>
#include <QMutex>
#include <fstream>

class my_thread: public QObject
{
    Q_OBJECT;
public:
    explicit my_thread(QObject *parent = 0);
    /*
    Request the process to start
    It is thread safe as it uses mutex to protect to run variable
    */
    void request_thread();
    void request_thread1();
    void abort();
private:
    bool _abort;
    bool _working;

    QMutex mutex;

signals:
    void thread_requested();
    void thread_requested1();
    void valueChanged(const QString &value);
    void  finished();

public slots:
    void doWork();
};

#endif // MY_THREAD_H
