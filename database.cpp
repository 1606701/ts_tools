#include "database.h"
#include <QFile>
database::database()
{

}
database::~database()
{
    delete [] tempbuf;
}
bool database::createConnection(QString name)
{
    db = QSqlDatabase::addDatabase("QSQLITE", name);
    db.setDatabaseName(".//qtDb.db");
    if( !db.open())
    {
        qDebug() << "无法建立数据库连接";
        return false;
    }
    return true;
}

//创建数据库表
bool database::createTable()
{
    //db = QSqlDatabase::database("sqlite1"); //建立数据库连接
    db.open();
    QSqlQuery query(db);
    sprintf(tempbuf,"create table my_data (id integer,mydata varchar)");
    bool success = query.exec(tempbuf);
    db.close();
    if(success)
    {
        qDebug() << QObject::tr("数据库表创建成功！");
        return true;
    }
    else
    {
        qDebug() << QObject::tr("数据库表创建失败！");
        return false;
    }

}

//向数据库中插入记录
bool database::insert(QVariantList id,QVariantList data)
{
    db.open();
    db.transaction();
    //db = QSqlDatabase::database("sqlite1"); //建立数据库连接

    QSqlQuery query(db);
    sprintf(tempbuf,"insert into my_data values(?, ?)");
    query.prepare(tempbuf);
    query.addBindValue(id);
    query.addBindValue(data);
    bool success=query.execBatch();

    db.commit();
    db.close();
    if(!success)
    {
        QSqlError lastError = query.lastError();
        qDebug() << lastError.driverText() << QString(QObject::tr("插入失败"));
        return false;
    }
    return true;
}

//查询所有信息
bool database::queryAll(int id)
{
    //db = QSqlDatabase::database("sqlite1"); //建立数据库连接
    //data = new QVector<QString>;
    data.clear();
    db.open();
    QSqlQuery query(db);
    sprintf(tempbuf,"select * from my_data where id = %d",id);
    query.exec(tempbuf);
    QSqlRecord rec = query.record();
    //qDebug() << QObject::tr("表字段数：" ) << rec.count();
    while(query.next())
    {
//        for(int index = 0; index < 2; index++)
//            qDebug() << query.value(index) << " ";
//        qDebug() << "\n";
        data<<query.value(1);
        //data->push_back(query.value(1).toString());
    }
    db.close();
}

//根据ID删除记录
bool database::deleteById(int id)
{
    db = QSqlDatabase::database("sqlite1"); //建立数据库连接
    QSqlQuery query(db);
    query.prepare(QString("delete from automobil where id = %1").arg(id));
    if(!query.exec())
    {
        qDebug() << "删除记录失败！";
        return false;
    }
    return true;
}

//根据ID更新记录
bool database::updateById(int id)
{
    db = QSqlDatabase::database("sqlite1"); //建立数据库连接
    QSqlQuery query(db);
    query.prepare(QString("update automobil set attribute=?,type=?,"
                             "kind=?, nation=?,"
                             "carnumber=?, elevaltor=?,"
                             "distance=?, oil=?,"
                             "temperature=? where id=%1").arg(id));

     query.bindValue(0,"四轮");
     query.bindValue(1,"轿车");
     query.bindValue(2,"富康");
     query.bindValue(3,rand()%100);
     query.bindValue(4,rand()%10000);
     query.bindValue(5,rand()%300);
     query.bindValue(6,rand()%200000);
     query.bindValue(7,rand()%52);
     query.bindValue(8,rand()%100);

     bool success=query.exec();
     if(!success)
     {
          QSqlError lastError = query.lastError();
          qDebug() << lastError.driverText() << QString(QObject::tr("更新失败"));
     }
    return true;
}

//排序
bool database::sortById()
{
    db = QSqlDatabase::database("sqlite1"); //建立数据库连接
    QSqlQuery query(db);
    bool success=query.exec("select * from automobil order by id desc");
    if(success)
    {
        qDebug() << QObject::tr("排序成功");
        return true;
    }
    else
    {
        qDebug() << QObject::tr("排序失败！");
        return false;
    }
}


bool database::droptable()
{
    //db = QSqlDatabase::database("sqlite1"); //建立数据库连接
    db.open();
    QSqlQuery query(db);
    sprintf(tempbuf,"drop table my_data");
    bool success=query.exec(tempbuf);
    db.close();
    if(success)
    {
        qDebug() << QObject::tr("删除成功");
        return true;
    }
    else
    {
        qDebug() << QObject::tr("删除失败");
        return false;
    }
}

void database::deleteDbFile()
{
    QFile fileTemp(".//qtDb.db");
    if(fileTemp.remove())
        qDebug()<<"delete success"<<endl;
}
