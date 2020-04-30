#include "basic_info.h"
#include "ui_basic_info.h"
#include <fstream>
#include <iostream>
using namespace std;
basic_info::basic_info(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::basic_info)
{
    ui->setupUi(this);
}

basic_info::~basic_info()
{
    delete ui;
    delete []tempbuf;
}

void basic_info::init_tree()
{
    ui->treeWidget->clear();
    for(int i = 0;i < PID_tree.length();i++)
    {

        QTreeWidgetItem *group = new QTreeWidgetItem(ui->treeWidget);
        sprintf(tempbuf,"service_id:%d(PMT_PID:%x)",PID_tree[i].service_id,PID_tree[i].program_PID);
        group->setText(0,tempbuf);
        for(int j = 0;j < PID_tree[i].stream_information.length();j++)
        {
            QTreeWidgetItem *subItem = new QTreeWidgetItem(group);
            sprintf(tempbuf,"PID:%x ,type: %s",PID_tree[i].stream_information[j].PID,
                                               PID_tree[i].stream_information[j].type.toLatin1().data());
            subItem->setText(0,tempbuf);
        }
        QTreeWidgetItem *subItem1 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"PCR:%x",PID_tree[i].PCR_id);
        subItem1->setText(0,tempbuf);
    }
}


