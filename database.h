#ifndef DATABASE_H
#define DATABASE_H

#include <QTextCodec>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QTime>
#include <QtSql/QSqlError>
#include <QtDebug>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlRecord>
#include <QThread>
#include <QVector>
typedef struct my_section_data
{
    unsigned int id;
    QString data;
}my_section_data;

class database
{
public:
    char *tempbuf = new char[200];
    database();
    ~database();
    bool createConnection(QString name);  //创建一个连接
    bool createTable();       //创建数据库表
    bool insert(QVariantList id,QVariantList data);            //插入数据
    bool queryAll(int id);          //查询所有此ID信息
    bool updateById(int id);  //更新
    bool deleteById(int id);  //删除
    bool sortById();          //排序
    bool droptable(); //删除表
    void deleteDbFile();
    QVariantList data;

private:
    QSqlDatabase db;

};

#endif // DATABASE_H
