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
                        strcat(buf_category,"reserved_for_future_use");
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
                    case 0xF:
                        strcat(buf_category,"user_defined");
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
                case 0xF:
                    strcat(buf_category,"user_defined");
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
                    case 0xF:
                        strcat(buf_category,"user_defined");
                        break;
                    default:
                        strcat(buf_category,"unkonwn");
                        break;
                }
                break;
            case 5:sprintf(buf_category,"%s","Children's_");
                switch (level_2)
                {
                    case 0:
                        strcat(buf_category,"general");
                        break;
                    case 1:
                        strcat(buf_category,"pre-school_children's_programmes");
                        break;
                    case 2:
                        strcat(buf_category,"entertainment programmes for 6 to 14");
                        break;
                    case 3:
                        strcat(buf_category,"entertainment programmes for 10 to 16");
                        break;
                    case 4:
                        strcat(buf_category,"informational");
                        break;
                    case 5:
                        strcat(buf_category,"cartoons/puppets");
                        break;
                    case 0xF:
                        strcat(buf_category,"user_defined");
                        break;
                    default:
                        strcat(buf_category,"unkonwn");
                        break;
                }
            case 6:
                sprintf(buf_category,"%s","Music_");
                switch(level_2)
                {
                    case 0:
                        strcat(buf_category,"general");
                        break;
                    case 1:
                        strcat(buf_category,"rock/pop");
                        break;
                    case 2:
                        strcat(buf_category,"serious music/classical music");
                        break;
                    case 3:
                        strcat(buf_category,"folk/traditional music");
                        break;
                    case 4:
                        strcat(buf_category,"jazz");
                        break;
                    case 5:
                        strcat(buf_category,"musical/opera");
                        break;
                    case 6:
                        strcat(buf_category,"ballet");
                        break;
                    case 0xF:
                        strcat(buf_category,"user defined");
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
                    case 0:
                        strcat(buf_category,"arts/culture");
                        break;
                    case 1:
                        strcat(buf_category,"performing arts");
                        break;
                    case 2:
                        strcat(buf_category,"fine arts");
                        break;
                    case 3:
                        strcat(buf_category,"religion");
                        break;
                    case 4:
                        strcat(buf_category,"popular culture/traditional arts");
                        break;
                    case 5:
                        strcat(buf_category,"literature");
                        break;
                    case 6:
                        strcat(buf_category,"film/cinema");
                        break;
                    case 7:
                        strcat(buf_category,"experimental film/video");
                        break;
                    case 8:
                        strcat(buf_category,"broadcasting/press");
                        break;
                    case 9:
                        strcat(buf_category,"new media");
                        break;
                    case 0xA:
                        strcat(buf_category,"arts/culture magazines");
                        break;
                    case 0xB:
                        strcat(buf_category,"fashion");
                        break;
                    case 0xF:
                        strcat(buf_category,"user defined");
                        break;
                    default:
                        strcat(buf_category,"unkonwn");
                        break;
                }
                break;
            case 8:
                sprintf(buf_category,"%s","Social_");
                switch(level_2)
                {
                    case 0:
                        strcat(buf_category,"general");
                        break;
                    case 1:
                        strcat(buf_category,"magazines");
                        break;
                    case 2:
                        strcat(buf_category,"economics/social advisory");
                        break;
                    case 3:
                        strcat(buf_category,"remarkable people");
                        break;
                    case 0xF:
                        strcat(buf_category,"user defined");
                        break;
                    default:
                        strcat(buf_category,"unkonwn");
                        break;
                }
                break;
            case 9:
                sprintf(buf_category,"%s","Education_");
                switch(level_2)
                {
                    case 0:
                        strcat(buf_category,"general");
                        break;
                    case 1:
                        strcat(buf_category,"nature/animals");
                        break;
                    case 2:
                        strcat(buf_category,"technology/natural sciences");
                        break;
                    case 3:
                        strcat(buf_category,"medicine/physiology/psychology");
                        break;
                    case 4:
                        strcat(buf_category,"foreign countries/expeditions");
                        break;
                    case 5:
                        strcat(buf_category,"social/spiritual sciences");
                        break;
                    case 6:
                        strcat(buf_category,"further education");
                        break;
                    case 7:
                        strcat(buf_category,"languages");
                        break;
                    case 0xF:
                        strcat(buf_category,"user defined");
                        break;
                    default:
                        strcat(buf_category,"unkonwn");
                        break;
                }
                break;
            case 0xA:
                sprintf(buf_category,"%s","Leisure hobbies");
                switch(level_2)
                {
                    case 0:
                        strcat(buf_category,"general");
                        break;
                    case 1:
                        strcat(buf_category,"tourism/travel");
                        break;
                    case 2:
                        strcat(buf_category,"handicraft");
                        break;
                    case 3:
                        strcat(buf_category,"motoring");
                        break;
                    case 4:
                        strcat(buf_category,"fitness and health");
                        break;
                    case 5:
                        strcat(buf_category,"cooking");
                        break;
                    case 6:
                        strcat(buf_category,"advertisement/shopping");
                        break;
                    case 7:
                        strcat(buf_category,"gardening");
                        break;
                    case 0xF:
                        strcat(buf_category,"user defined");
                        break;
                    default:
                        strcat(buf_category,"unkonwn");
                        break;
                }
                break;
            case 0xB:
                sprintf(buf_category,"%s","Special characteristics");
                switch(level_2)
                {
                    case 0:
                        strcat(buf_category,"original language");
                        break;
                    case 1:
                        strcat(buf_category,"black and white");
                        break;
                    case 2:
                        strcat(buf_category,"unpublished");
                        break;
                    case 3:
                        strcat(buf_category,"live broadcast");
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
                    add_category(temp_event_info);
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
                add_category(temp_event_info);
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
                    add_category(temp_event_info);
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
                add_category(temp_event_info);
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
                                                        ,epg_info[i].event_info[j].UTC_time.Hour+8
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
void EPG::add_category(event_information e)
{

    for(QVector<CATEGORY_information>::Iterator it = category_info.begin();it != category_info.end();it++)
    {
        if(it->category_information == e.category)
        {
            it->event_info.push_back(e);
            return ;
        }
    }
    CATEGORY_information temp_cat_info;
    temp_cat_info.category_information = e.category;
    temp_cat_info.event_info.push_back(e);
    category_info.push_back(temp_cat_info);
}
void EPG::init_epg_info_category()
{
    QTreeWidgetItem *group = new QTreeWidgetItem(ui->treeWidget_2);
    group->setText(0,"CATEGORY");
    for(int i = 0;i < category_info.length();i++)
    {
        QTreeWidgetItem *name1 = new QTreeWidgetItem(group);
        name1->setText(0,category_info[i].category_information);
        for(int j = 0;j < category_info[i].event_info.length();j++)
        {
            QTreeWidgetItem *name = new QTreeWidgetItem(name1);
            sprintf(tempbuf,"tiele%d",j+1);
            name->setText(0,tempbuf);
            QTreeWidgetItem *group1 = new QTreeWidgetItem(name);
            sprintf(tempbuf,"category:%s",category_info[i].event_info[j].category.toLatin1().data());
            group1->setText(0,tempbuf);
            QTreeWidgetItem *group2 = new QTreeWidgetItem(name);
            sprintf(tempbuf,"event_id:%d",category_info[i].event_info[j].event_id);
            group2->setText(0,tempbuf);
            QTreeWidgetItem *group3 = new QTreeWidgetItem(name);
            sprintf(tempbuf,"UTC_time:%d/%d/%d %d:%d:%d",category_info[i].event_info[j].UTC_time.Year
                                                        ,category_info[i].event_info[j].UTC_time.Mouth
                                                        ,category_info[i].event_info[j].UTC_time.Day
                                                        ,category_info[i].event_info[j].UTC_time.Hour+8
                                                        ,category_info[i].event_info[j].UTC_time.Min
                                                        ,category_info[i].event_info[j].UTC_time.Sec);
            group3->setText(0,tempbuf);
            QTreeWidgetItem *group4 = new QTreeWidgetItem(name);
            sprintf(tempbuf,"duration:%d:%d:%d",category_info[i].event_info[j].duration_hour
                                               ,category_info[i].event_info[j].duration_min
                                               ,category_info[i].event_info[j].duration_sec);
            group4->setText(0,tempbuf);
            QTreeWidgetItem *group5 = new QTreeWidgetItem(name);
            sprintf(tempbuf,"(%c%c%c)%s",category_info[i].event_info[j].ISO_639_language_code >> 16
                                        ,(category_info[i].event_info[j].ISO_639_language_code >> 8 )& 0xff
                                        ,category_info[i].event_info[j].ISO_639_language_code & 0xff
                                        ,category_info[i].event_info[j].event_name.toLatin1().data());
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
