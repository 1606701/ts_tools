#ifndef TASK_H
#define TASK_H


#include <QRunnable>
#include <QMutex>
#include <QMap>
#include <QObject>
#include "database.h"
using namespace std;

class Task : public QObject,public QRunnable {
    Q_OBJECT
    char *tempbuf = new char[100];
public:

    Task(QString filename);
    ~Task();
    void run() Q_DECL_OVERRIDE;

public slots:
    void setCancel();

private:
    QMutex mutex;
    QString m_filename;
    database d;
    QVariantList data_set;
    QVariantList id_set;
    bool m_isstop;
signals:
    void load_over();
};

#endif // TASK_H
