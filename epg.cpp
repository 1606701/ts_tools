#include "epg.h"
#include "ui_epg.h"
#include <iostream>
#include <fstream>
#include "mainwindow.h"
using namespace std;

EPG::EPG(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EPG)
{
    ui->setupUi(this);
}

EPG::~EPG()
{
    delete ui;
    delete []tempbuf;
}
bool sort_service_id(EPG_information a,EPG_information b)
{
    return (a.service_id < b.service_id);
}
bool sort_event_id(event_information a,event_information b)
{
    return (a.event_id < b.event_id);
}
void EPG::category_choice_epg(unsigned int level_1, unsigned int level_2,char *buf_category)
{
    switch(level_1)
        {
            case 0:
                sprintf(buf_category,"%s","undefined");
                break;
            case 1:
                sprintf(buf_category,"%s","Movie_Drama_");
                switch(level_2)
                {
                    case 0:
                        strcat(buf_category,"general");
                        break;
                    case 1:
                        strcat(buf_category,"detective");
                        break;
                    case 2:
                        strcat(buf_category,"war");
                        break;
                    case 3:
                        strcat(buf_category,"horror");
                        break;
                    case 5:
                        strcat(buf_category,"soap");
                        break;
                    case 7:
                        strcat(buf_category,"serious");
                        break;
                    case 8:
                        strcat(buf_category,"adult");
                        break;
                    case 0xf:
                        strcat(buf_category,"user_define");
                        break;
                    default:
                        strcat(buf_category,"unkonwn");
                        break;
                }
                break;
            case 2:
                sprintf(buf_category,"%s","News_");
                switch (level_2)
                {
                    case 0:
                        strcat(buf_category,"current_affairs");
                        break;
                    case 1:
                        strcat(buf_category,"weather_report");
                        break;
                    case 2:
                        strcat(buf_category,"magazine");
                        break;
                    case 3:
                        strcat(buf_category,"documentary");
                        break;
                    case 4:
                        strcat(buf_category,"debate");
                        break;
                    default:
                        strcat(buf_category,"unkonwn");
                        break;
                }
                break;
            case 3:
                sprintf(buf_category,"%s","show_");
                switch (level_2)
                {
                case 0:
                    strcat(buf_category,"game_show");
                    break;
                case 1:
                    strcat(buf_category,"quiz");
                    break;
                case 2:
                    strcat(buf_category,"variety");
                    break;
                case 3:
                    strcat(buf_category,"talk_show");
                    break;
                default:
                    strcat(buf_category,"unkonwn");
                    break;
                }
                break;
            case 4:
                sprintf(buf_category,"%s","Sports_");
                switch(level_2)
                {
                    case 0:
                        strcat(buf_category,"general");
                        break;
                    case 1:
                        strcat(buf_category,"special");
                        break;
                    case 2:
                        strcat(buf_category,"magazines");
                        break;
                    case 3:
                        strcat(buf_category,"football");
                        break;
                    case 4:
                        strcat(buf_category,"tennis");
                        break;
                    case 5:
                        strcat(buf_category,"team");
                        break;
                    case 6:
                        strcat(buf_category,"athletics");
                        break;
                    case 7:
                        strcat(buf_category,"motor");
                        break;
                    case 8:
                        strcat(buf_category,"water");
                        break;
                    case 9:
                        strcat(buf_category,"winter");
                        break;
                    case 0xA:
                        strcat(buf_category,"equestrian");
                        break;
                    case 0xB:
                        strcat(buf_category,"martial");
                        break;
                    default:
                        strcat(buf_category,"unkonwn");
                        break;
                }
                break;
            case 6:
                sprintf(buf_category,"%s","Music_");
                switch(level_2)
                {
                    case 0:
                        strcat(buf_category,"general");
                        break;
                    case 1:
                        strcat(buf_category,"rock");
                        break;
                    default:
                        strcat(buf_category,"unkonwn");
                        break;
                }
                break;
            case 7:
                sprintf(buf_category,"%s","Arts_");
                switch(level_2)
                {
                    case 6:
                        strcat(buf_category,"film");
                        break;
                    default:
                        strcat(buf_category,"unkonwn");
                        break;
                }
                break;
            default:
                sprintf(buf_category,"%x%x",level_1,level_2);
                break;
        }
}
void EPG::init_epg_info_pf()
{
    int flag = 0;
    for(int i = 0; i < ts_eit_pf_actual.length();i++)
    {
        flag = 0;
        for(int j = 0 ;j < epg_info.length();j++)
        {
            if(ts_eit_pf_actual[i].service_id == epg_info[j].service_id)
            {
                for(int k = 0;k < ts_eit_pf_actual[i].eit_loop.length();k++)
                {
                    event_information temp_event_info;
                    temp_event_info.UTC_time = ts_eit_pf_actual[i].eit_loop[k].UTC_time;
                    temp_event_info.event_id = ts_eit_pf_actual[i].eit_loop[k].event_id;
                    temp_event_info.duration_hour = ts_eit_pf_actual[i].eit_loop[k].duration_hour;
                    temp_event_info.duration_min = ts_eit_pf_actual[i].eit_loop[k].duration_min;
                    temp_event_info.duration_sec = ts_eit_pf_actual[i].eit_loop[k].duration_sec;
                    temp_event_info.ISO_639_language_code = ts_eit_pf_actual[i].eit_loop[k].short_eve.begin()->ISO_639_language_code;
                    temp_event_info.event_name = ts_eit_pf_actual[i].eit_loop[k].short_eve.begin()->event_name;
                    if(ts_eit_pf_actual[i].eit_loop[k].content_des.length() == 0)
                    {
                        temp_event_info.category = QString(QLatin1String("no category"));
                    }
                    else
                    {
                        category_choice_epg(ts_eit_pf_actual[i].eit_loop[k].content_des.begin()->cat_info.begin()->content_nibble_level_1
                                           ,ts_eit_pf_actual[i].eit_loop[k].content_des.begin()->cat_info.begin()->content_nibble_level_2
                                           ,tempbuf);
                        temp_event_info.category = QString(QLatin1String(tempbuf));
                    }
                    epg_info[j].event_info.push_back(temp_event_info);
                }
                flag = 1;
            }
        }
        if(flag == 0)
        {
            EPG_information temp_epg_info;
            temp_epg_info.service_id = ts_eit_pf_actual[i].service_id;
            for(int k = 0;k < ts_eit_pf_actual[i].eit_loop.length();k++)
            {
                event_information temp_event_info;
                temp_event_info.UTC_time = ts_eit_pf_actual[i].eit_loop[k].UTC_time;
                temp_event_info.event_id = ts_eit_pf_actual[i].eit_loop[k].event_id;
                temp_event_info.duration_hour = ts_eit_pf_actual[i].eit_loop[k].duration_hour;
                temp_event_info.duration_min = ts_eit_pf_actual[i].eit_loop[k].duration_min;
                temp_event_info.duration_sec = ts_eit_pf_actual[i].eit_loop[k].duration_sec;
                temp_event_info.ISO_639_language_code = ts_eit_pf_actual[i].eit_loop[k].short_eve.begin()->ISO_639_language_code;
                temp_event_info.event_name = ts_eit_pf_actual[i].eit_loop[k].short_eve.begin()->event_name;
                if(ts_eit_pf_actual[i].eit_loop[k].content_des.length() == 0)
                {
                    temp_event_info.category = QString(QLatin1String("no category"));
                }
                else
                {
                    category_choice_epg(ts_eit_pf_actual[i].eit_loop[k].content_des.begin()->cat_info.begin()->content_nibble_level_1
                                       ,ts_eit_pf_actual[i].eit_loop[k].content_des.begin()->cat_info.begin()->content_nibble_level_2
                                       ,tempbuf);
                    temp_event_info.category = QString(QLatin1String(tempbuf));
                }
                temp_epg_info.event_info.push_back(temp_event_info);
            }
            epg_info.push_back(temp_epg_info);
        }
    }
}
void EPG::init_epg_info_schedule()
{
    int flag = 0;
    for(int i = 0; i < ts_eit_schedule_actual.length();i++)
    {
        flag = 0;
        for(int j = 0 ;j < epg_info.length();j++)
        {
            if(ts_eit_schedule_actual[i].service_id == epg_info[j].service_id)
            {
                for(int k = 0;k < ts_eit_schedule_actual[i].eit_loop.length();k++)
                {
                    event_information temp_event_info;
                    temp_event_info.UTC_time = ts_eit_schedule_actual[i].eit_loop[k].UTC_time;
                    temp_event_info.event_id = ts_eit_schedule_actual[i].eit_loop[k].event_id;
                    temp_event_info.duration_hour = ts_eit_schedule_actual[i].eit_loop[k].duration_hour;
                    temp_event_info.duration_min = ts_eit_schedule_actual[i].eit_loop[k].duration_min;
                    temp_event_info.duration_sec = ts_eit_schedule_actual[i].eit_loop[k].duration_sec;
                    temp_event_info.ISO_639_language_code = ts_eit_schedule_actual[i].eit_loop[k].short_eve.begin()->ISO_639_language_code;
                    temp_event_info.event_name = ts_eit_schedule_actual[i].eit_loop[k].short_eve.begin()->event_name;
                    if(ts_eit_schedule_actual[i].eit_loop[k].content_des.length() == 0)
                    {
                        temp_event_info.category = QString(QLatin1String("no category"));
                    }
                    else
                    {
                        category_choice_epg(ts_eit_schedule_actual[i].eit_loop[k].content_des.begin()->cat_info.begin()->content_nibble_level_1
                                           ,ts_eit_schedule_actual[i].eit_loop[k].content_des.begin()->cat_info.begin()->content_nibble_level_2
                                           ,tempbuf);
                        temp_event_info.category = QString(QLatin1String(tempbuf));
                    }
                    epg_info[j].event_info.push_back(temp_event_info);
                }
                flag = 1;
            }
        }
        if(flag == 0)
        {
            EPG_information temp_epg_info;
            temp_epg_info.service_id = ts_eit_schedule_actual[i].service_id;
            for(int k = 0;k < ts_eit_schedule_actual[i].eit_loop.length();k++)
            {
                event_information temp_event_info;
                temp_event_info.UTC_time = ts_eit_schedule_actual[i].eit_loop[k].UTC_time;
                temp_event_info.event_id = ts_eit_schedule_actual[i].eit_loop[k].event_id;
                temp_event_info.duration_hour = ts_eit_schedule_actual[i].eit_loop[k].duration_hour;
                temp_event_info.duration_min = ts_eit_schedule_actual[i].eit_loop[k].duration_min;
                temp_event_info.duration_sec = ts_eit_schedule_actual[i].eit_loop[k].duration_sec;
                temp_event_info.ISO_639_language_code = ts_eit_schedule_actual[i].eit_loop[k].short_eve.begin()->ISO_639_language_code;
                temp_event_info.event_name = ts_eit_schedule_actual[i].eit_loop[k].short_eve.begin()->event_name;
                if(ts_eit_schedule_actual[i].eit_loop[k].content_des.length() == 0)
                {
                    temp_event_info.category = QString(QLatin1String("no category"));
                }
                else
                {
                    category_choice_epg(ts_eit_schedule_actual[i].eit_loop[k].content_des.begin()->cat_info.begin()->content_nibble_level_1
                                       ,ts_eit_schedule_actual[i].eit_loop[k].content_des.begin()->cat_info.begin()->content_nibble_level_2
                                       ,tempbuf);
                    temp_event_info.category = QString(QLatin1String(tempbuf));
                }
                temp_epg_info.event_info.push_back(temp_event_info);
            }
            epg_info.push_back(temp_epg_info);
        }
    }
}
void EPG::init_epg_info_service_id()
{
    for(int i = 0;i < epg_info.length();i++)
    {
        QTreeWidgetItem *group = new QTreeWidgetItem(ui->treeWidget);
        sprintf(tempbuf,"%d",epg_info[i].service_id);
        group->setText(0,tempbuf);
        sort(epg_info[i].event_info.begin(),epg_info[i].event_info.end(),sort_event_id);
        epg_info[i].event_info.erase(unique(epg_info[i].event_info.begin(),epg_info[i].event_info.end()),epg_info[i].event_info.end());
        for(int j = 0;j < epg_info[i].event_info.length();j++)
        {
            QTreeWidgetItem *group1 = new QTreeWidgetItem(group);
            sprintf(tempbuf,"event_id:%d",epg_info[i].event_info[j].event_id);
            group1->setText(0,tempbuf);
            QTreeWidgetItem *group2 = new QTreeWidgetItem(group1);
            sprintf(tempbuf,"UTC_time:%d/%d/%d %d:%d:%d",epg_info[i].event_info[j].UTC_time.Year
                                                        ,epg_info[i].event_info[j].UTC_time.Mouth
                                                        ,epg_info[i].event_info[j].UTC_time.Day
                                                        ,epg_info[i].event_info[j].UTC_time.Hour
                                                        ,epg_info[i].event_info[j].UTC_time.Min
                                                        ,epg_info[i].event_info[j].UTC_time.Sec);
            group2->setText(0,tempbuf);
            QTreeWidgetItem *group3 = new QTreeWidgetItem(group1);
            sprintf(tempbuf,"duration:%d:%d:%d",epg_info[i].event_info[j].duration_hour
                                               ,epg_info[i].event_info[j].duration_min
                                               ,epg_info[i].event_info[j].duration_sec);
            group3->setText(0,tempbuf);
            QTreeWidgetItem *group4 = new QTreeWidgetItem(group1);
            sprintf(tempbuf,"(%c%c%c)%s",epg_info[i].event_info[j].ISO_639_language_code >> 16
                                        ,(epg_info[i].event_info[j].ISO_639_language_code >> 8 )& 0xff
                                        ,epg_info[i].event_info[j].ISO_639_language_code & 0xff
                                        ,epg_info[i].event_info[j].event_name.toLatin1().data());
            group4->setText(0,tempbuf);
            QTreeWidgetItem *group5 = new QTreeWidgetItem(group1);
            sprintf(tempbuf,"categroy:%s",epg_info[i].event_info[j].category.toLatin1().data());
            group5->setText(0,tempbuf);
        }
    }
}
void EPG::init_epg_info_category()
{
    QTreeWidgetItem *group = new QTreeWidgetItem(ui->treeWidget_2);
    group->setText(0,"CATEGORY");
    for(int i = 0;i < epg_info.length();i++)
    {
        sort(epg_info[i].event_info.begin(),epg_info[i].event_info.end(),sort_event_id);
        epg_info[i].event_info.erase(unique(epg_info[i].event_info.begin(),epg_info[i].event_info.end()),epg_info[i].event_info.end());
        for(int j = 0;j < epg_info[i].event_info.length();j++)
        {
            QTreeWidgetItem *group1 = new QTreeWidgetItem(group);
            sprintf(tempbuf,"category:%s",epg_info[i].event_info[j].category.toLatin1().data());
            group1->setText(0,tempbuf);
            QTreeWidgetItem *group2 = new QTreeWidgetItem(group1);
            sprintf(tempbuf,"event_id:%d",epg_info[i].event_info[j].event_id);
            group2->setText(0,tempbuf);
            QTreeWidgetItem *group3 = new QTreeWidgetItem(group1);
            sprintf(tempbuf,"UTC_time:%d/%d/%d %d:%d:%d",epg_info[i].event_info[j].UTC_time.Year
                                                        ,epg_info[i].event_info[j].UTC_time.Mouth
                                                        ,epg_info[i].event_info[j].UTC_time.Day
                                                        ,epg_info[i].event_info[j].UTC_time.Hour
                                                        ,epg_info[i].event_info[j].UTC_time.Min
                                                        ,epg_info[i].event_info[j].UTC_time.Sec);
            group3->setText(0,tempbuf);
            QTreeWidgetItem *group4 = new QTreeWidgetItem(group1);
            sprintf(tempbuf,"duration:%d:%d:%d",epg_info[i].event_info[j].duration_hour
                                               ,epg_info[i].event_info[j].duration_min
                                               ,epg_info[i].event_info[j].duration_sec);
            group4->setText(0,tempbuf);
            QTreeWidgetItem *group5 = new QTreeWidgetItem(group1);
            sprintf(tempbuf,"(%c%c%c)%s",epg_info[i].event_info[j].ISO_639_language_code >> 16
                                        ,(epg_info[i].event_info[j].ISO_639_language_code >> 8 )& 0xff
                                        ,epg_info[i].event_info[j].ISO_639_language_code & 0xff
                                        ,epg_info[i].event_info[j].event_name.toLatin1().data());
            group5->setText(0,tempbuf);
        }
    }
}


void EPG::init_tree()
{
    ui->treeWidget->clear();
    ui->treeWidget_2->clear();
    epg_info.clear();
    init_epg_info_pf();
    init_epg_info_schedule();
    sort(epg_info.begin(),epg_info.end(),sort_service_id);
    init_epg_info_service_id();
    init_epg_info_category();
}
//void EPG::on_pushButton_clicked()
//{
//    ui->textBrowser->clear();
//    char *time_buf = new char[1024];
//    int filesize;
//    const char *filename = "/home/china/Time_info/Time";
//    fstream fp;
//    fp.open(filename,ios::in);
//    fp.seekg(0,ios::end);
//    filesize = fp.tellg();
//    fp.seekg(0,ios::beg);
//    while(filesize > 0)
//    {
//        memset(time_buf,0,1024);
//        fp.read(time_buf,1024);
//        filesize -= 1024;
//        ui->textBrowser->append(time_buf);
//    }
//    delete []time_buf;
//    fp.close();
//}

//void EPG::on_pushButton_3_clicked()
//{
//    cout<<dst<<endl;
//    ui->textBrowser_2->clear();
//    char *temp = new char[1024];
//    int filesize = 0;
//    ui->label_2->setText(QString::number(service_id[dst]));
//    const char *filename = "/home/china/EPG_info";
//    sprintf(temp,"%s/%d",filename,service_id[dst]);
//    fstream fp;
//    fp.open(temp,ios::in);
//    if(!fp)
//    {
//        ui->textBrowser_2->setText("the service_id no information");
//    }
//    fp.seekg(0,ios::end);
//    filesize = fp.tellg();
//    fp.seekg(0,ios::beg);
//    while(filesize > 0)
//    {
//        memset(temp,0,1024);
//        fp.read(temp,1024);
//        filesize -= 1024;
//        ui->textBrowser_2->append(temp);
//    }
//    fp.close();
//    delete []temp;
//    if(dst>=0 && dst<service_id.length())
//        dst++;
//}

//void EPG::on_pushButton_4_clicked()
//{
//    cout<<dst<<endl;
//    if(dst>0 && dst<=service_id.length())
//        dst--;
//    ui->textBrowser_2->clear();
//    char *temp = new char[1024];
//    int filesize = 0;
//    ui->label_2->setText(QString::number(service_id[dst]));
//    const char *filename = "/home/china/EPG_info";
//    sprintf(temp,"%s/%d",filename,service_id[dst]);
//    fstream fp;
//    fp.open(temp,ios::in);
//    if(!fp)
//    {
//        ui->textBrowser_2->setText("the service_id no information");
//    }
//    fp.seekg(0,ios::end);
//    filesize = fp.tellg();
//    fp.seekg(0,ios::beg);
//    while(filesize > 0)
//    {
//        memset(temp,0,1024);
//        fp.read(temp,1024);
//        filesize -= 1024;
//        ui->textBrowser_2->append(temp);
//    }
//    fp.close();
//    delete []temp;
//}
