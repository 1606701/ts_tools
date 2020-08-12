#include "task.h"
#include <QDebug>
#include <QThread>
#include <QDateTime>
#include <iostream>
#include <fstream>
//#include "section_data.h"
#include <QFile>
Task::Task(QString filename) :m_filename(filename),m_isstop(false) {
}

Task::~Task() {
    //qDebug().noquote() << QString("~Task() with ID %1").arg(m_id); // 方便查看对象是否被 delete
}


void Task::run() {
    d.deleteDbFile();
    d.createConnection("get");
    //d.droptable();
    d.createTable();
    unsigned char *buf = new unsigned char[188];
    QFile fp(m_filename);
    if(!fp.open(QIODevice::ReadOnly))
       qDebug()<<"open falid";
    else
       qDebug()<<"open success";
    QTextStream stream(&fp);
    int filesize = fp.size();
    while(filesize > 0)
    {
        if(m_isstop)
        {
            qDebug()<<"Stop thread Work. thread ID:"<<QThread::currentThreadId();
            break;
        }
        fp.read((char *)buf,188);
        filesize -= 188;
        if(buf[0]==0x47)
        {
            QString temp;
            unsigned int id = (buf[1] & 0x1F) << 8 | buf[2];

            for(int i = 0;i < 188;i++)
            {
                sprintf(tempbuf,"%02x",buf[i]);
                temp += tempbuf;
            }
            id_set.append(id);
            data_set.append(temp);
            if(id_set.length() == 1000)
            {
                d.insert(id_set,data_set);
                id_set.clear();
                data_set.clear();
            }
        }
        else
        {
            for(int i = 0;i < 188;i++)
            {
                if(buf[i] == 0x47)
                {
                    fp.seek(i);
                    cout<<"move once,offest="<<i<<endl;
                    break;
                }
            }
        }
        memset(buf,0,188);
    }
    if(id_set.length() != 0 && data_set.length() != 0)
    {
        d.insert(id_set,data_set);
        id_set.clear();
        data_set.clear();
    }
    emit load_over();
}

void Task::setCancel()
{
    m_isstop = true;
}
