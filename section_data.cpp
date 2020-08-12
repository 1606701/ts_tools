#include "section_data.h"
#include "ui_section_data.h"
#include <QDragEnterEvent>
#include <QDebug>

using namespace std;


char *pathbuf = new char[100];
int now_counter = 0;

section_data::section_data(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::section_data)
{
    ui->setupUi(this);
    connect(ui->treeWidget,SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),this,SLOT(slotDoubleClickItem(QTreeWidgetItem* ,int)));
}

section_data::~section_data()
{
    delete ui;
    delete []tempbuf;
    //delete [] path_buf;
}
bool sort_pid(section_info a,section_info b)
{
    return (a.PID < b.PID);
}
void section_data::init_tree()
{
    qDebug()<<"clear new"<<endl;
    ui->treeWidget->clear();
    ui->textBrowser->clear();
    qDebug()<<"click1"<<endl;
    sort(Section_tree.begin(),Section_tree.end(),sort_pid);
    for(int i = 0;i < Section_tree.length();i++)
    {
        QTreeWidgetItem *group = new QTreeWidgetItem(ui->treeWidget);

        sprintf(tempbuf,"PID:%x(%d),type:%s",Section_tree[i].PID,
                                             Section_tree[i].PID,
                                             Section_tree[i].type.toLatin1().data());
        group->setText(0,tempbuf);
        sprintf(tempbuf,"%d",Section_tree[i].PID);
        group->setText(1,tempbuf);
    }
}

void section_data::slotDoubleClickItem(QTreeWidgetItem* item,int count)
{
    unsigned int get_pid = item->text(1).toInt();
    for(QVector<section_info>::iterator it = Section_tree.begin(); it != Section_tree.end();it++)
    {
        if(it->PID == get_pid)
        {
            d.createConnection(QString::number(get_pid));
            d.queryAll(get_pid);
            sprintf(tempbuf,"%04x",it->PID);
            ui->label_2->setText(tempbuf);
            now_counter = 0;
            sprintf(tempbuf,"(%d/%d)",now_counter + 1,d.data.length());
            ui->label_4->setText(tempbuf);
            QString temp = d.data.at(now_counter).toString();
            for(int i = 2;i < temp.length();i+=2)
            {
               temp.insert(i,QString(" "));
               i++;
            }
            ui->textBrowser->setText(temp);
        }
    }
}

void section_data::on_pushButton_clicked()
{
    if(now_counter > 0)
    {
        now_counter--;
        sprintf(tempbuf,"(%d/%d)",now_counter + 1,d.data.length());
        ui->label_4->setText(tempbuf);
        QString temp = d.data.at(now_counter).toString();
        for(int i = 2;i < temp.length();i+=2)
        {
           temp.insert(i,QString(" "));
           i++;
        }
        ui->textBrowser->setText(temp);
    }
    else
    {
        return ;
    }

}

void section_data::on_pushButton_2_clicked()
{
    if(now_counter < d.data.length() - 1)
    {
        now_counter ++;
        sprintf(tempbuf,"(%d/%d)",now_counter + 1,d.data.length());
        ui->label_4->setText(tempbuf);
        QString temp = d.data.at(now_counter).toString();
        for(int i = 2;i < temp.length();i+=2)
        {
           temp.insert(i,QString(" "));
           i++;
        }
        ui->textBrowser->setText(temp);
    }
    else
    {
        return ;
    }

}
