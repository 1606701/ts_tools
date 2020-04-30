#include "dialog.h"
#include "ui_dialog.h"
using namespace std;
Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    thread = new QThread();
    mthread = new my_thread();
    mthread->moveToThread(thread);
//    connect(mthread,SIGNAL(valueChanged(QString)),ui->textBrowser,SLOT(append(QString)));
//    connect(mthread,SIGNAL(thread_requested()),thread,SLOT(start()));
//    connect(thread,SIGNAL(started()),mthread,SLOT(doWork()));
//    connect(mthread,SIGNAL(finished()),thread,SLOT(quit()),Qt::DirectConnection);
}

Dialog::~Dialog()
{
    mthread->abort();
    thread->wait();
    //qDebug()<<"Deleting thread and worker in Thread "<<this->QObject::thread()->currentThreadId();
    delete thread;
    delete mthread;
    delete []tempbuf;
    delete ui;
}
bool sort_service_id(TS_EIT a,TS_EIT b)
{
    return (a.service_id < b.service_id);
}
void Dialog::init_TOT_tree()
{
    QTreeWidgetItem *TOT = new QTreeWidgetItem(ui->treeWidget);
    TOT->setText(0,"TOT_information");
    for(int i = 0;i < ts_tot.length();i++)
    {
        QTreeWidgetItem *group = new QTreeWidgetItem(TOT);
        sprintf(tempbuf,"table_id:%x",ts_tot[i].table_id);
        group->setText(0,tempbuf);
        QTreeWidgetItem *group1 = new QTreeWidgetItem(TOT);
        sprintf(tempbuf,"section_syntax_indicator:%x",ts_tot[i].section_syntax_indicator);
        group1->setText(0,tempbuf);
        QTreeWidgetItem *group2 = new QTreeWidgetItem(TOT);
        sprintf(tempbuf,"section_length:%x",ts_tot[i].section_length);
        group2->setText(0,tempbuf);
        QTreeWidgetItem *group3 = new QTreeWidgetItem(TOT);
        sprintf(tempbuf,"UTC_time:%d/%d/%d %d:%d:%d",ts_tot[i].UTC_time.Year,
                                                     ts_tot[i].UTC_time.Mouth,
                                                     ts_tot[i].UTC_time.Day,
                                                     ts_tot[i].UTC_time.Hour,
                                                     ts_tot[i].UTC_time.Min,
                                                     ts_tot[i].UTC_time.Sec);
        group3->setText(0,tempbuf);
        QTreeWidgetItem *group4 = new QTreeWidgetItem(TOT);
        sprintf(tempbuf,"descriptor_loop_length:%x",ts_tot[i].descriptor_loop_length);
        group4->setText(0,tempbuf);
        QTreeWidgetItem *group5 = new QTreeWidgetItem(TOT);
        group5->setText(0,"descriptor");
        for(int j = 0;j < ts_tot[i].local_desc.length();j++)
        {
            QTreeWidgetItem *Item = new QTreeWidgetItem(group5);
            Item->setText(0,"local_time_offset_descriptor");
            QTreeWidgetItem *Item1 = new QTreeWidgetItem(Item);
            sprintf(tempbuf,"descriptor_tag:%x",ts_tot[i].local_desc[j].descriptor_tag);
            Item1->setText(0,tempbuf);
            QTreeWidgetItem *Item2 = new QTreeWidgetItem(Item);
            sprintf(tempbuf,"descriptor_length:%x",ts_tot[i].local_desc[j].descriptor_length);
            Item2->setText(0,tempbuf);
            for(int k = 0;k < ts_tot[i].local_desc[j].time_desc.length();k++)
            {
                QTreeWidgetItem *title = new QTreeWidgetItem(group5);
                sprintf(tempbuf,"time_descriptor%d",k+1);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"country_code:%c%c%c",ts_tot[i].local_desc[j].time_desc[k].country_code >> 16,
                                                      (ts_tot[i].local_desc[j].time_desc[k].country_code >> 8) & 0xFF,
                                                      ts_tot[i].local_desc[j].time_desc[k].country_code & 0xFF);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"country_region_id:%x",ts_tot[i].local_desc[j].time_desc[k].country_region_id);
                title2->setText(0,tempbuf);
                QTreeWidgetItem *title3 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"local_time_offset_polarity:%x",ts_tot[i].local_desc[j].time_desc[k].local_time_offset_polarity);
                title3->setText(0,tempbuf);
                QTreeWidgetItem *title4 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"local_time_offset:%x",ts_tot[i].local_desc[j].time_desc[k].local_time_offset);
                title4->setText(0,tempbuf);
                QTreeWidgetItem *title5 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"time_of_change:%d/%d/%d %d:%d:%d",ts_tot[i].local_desc[j].time_desc[k].time_of_change.Year,
                                                                   ts_tot[i].local_desc[j].time_desc[k].time_of_change.Mouth,
                                                                   ts_tot[i].local_desc[j].time_desc[k].time_of_change.Day,
                                                                   ts_tot[i].local_desc[j].time_desc[k].time_of_change.Hour,
                                                                   ts_tot[i].local_desc[j].time_desc[k].time_of_change.Min,
                                                                   ts_tot[i].local_desc[j].time_desc[k].time_of_change.Sec);
                title5->setText(0,tempbuf);
                QTreeWidgetItem *title6 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"next_time_offset:%x",ts_tot[i].local_desc[j].time_desc[k].next_time_offset);
                title6->setText(0,tempbuf);
            }
        }

    }
}
void Dialog::init_TDT_tree()
{
    QTreeWidgetItem *TDT = new QTreeWidgetItem(ui->treeWidget);
    TDT->setText(0,"TDT_information");
    for(int i = 0;i < ts_tdt.length();i++)
    {
        QTreeWidgetItem *group = new QTreeWidgetItem(TDT);
        sprintf(tempbuf,"table_id:%x",ts_tdt[i].table_id);
        group->setText(0,tempbuf);
        QTreeWidgetItem *group1 = new QTreeWidgetItem(TDT);
        sprintf(tempbuf,"section_syntax_indicator:%x",ts_tdt[i].section_syntax_indicator);
        group1->setText(0,tempbuf);
        QTreeWidgetItem *group2 = new QTreeWidgetItem(TDT);
        sprintf(tempbuf,"section_length:%x",ts_tdt[i].section_length);
        group2->setText(0,tempbuf);
        QTreeWidgetItem *group3 = new QTreeWidgetItem(TDT);
        sprintf(tempbuf,"UTC_time:%d/%d/%d %d:%d:%d",ts_tdt[i].UTC_time.Year,
                                                     ts_tdt[i].UTC_time.Mouth,
                                                     ts_tdt[i].UTC_time.Day,
                                                     ts_tdt[i].UTC_time.Hour,
                                                     ts_tdt[i].UTC_time.Min,
                                                     ts_tdt[i].UTC_time.Sec);
        group3->setText(0,tempbuf);

    }
}
void Dialog::init_SDT_tree()
{
    QTreeWidgetItem *SDT = new QTreeWidgetItem(ui->treeWidget);
    SDT->setText(0,"SDT_information");
    for(int i = 0;i < ts_sdt.length();i++)
    {
        QTreeWidgetItem *group = new QTreeWidgetItem(SDT);
        sprintf(tempbuf,"section%d",i+1);
        group->setText(0,tempbuf);
        QTreeWidgetItem *group1 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"table_id:%x",ts_sdt[i].table_id);
        group1->setText(0,tempbuf);
        QTreeWidgetItem *group2 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"section_syntax_indicator:%x",ts_sdt[i].section_syntax_indicator);
        group2->setText(0,tempbuf);
        QTreeWidgetItem *group3 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"section_length:%x",ts_sdt[i].section_length);
        group3->setText(0,tempbuf);
        QTreeWidgetItem *group4 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"transport_stream_id:%x",ts_sdt[i].transport_stream_id);
        group4->setText(0,tempbuf);
        QTreeWidgetItem *group5 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"version_number:%x",ts_sdt[i].version_number);
        group5->setText(0,tempbuf);
        QTreeWidgetItem *group6 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"section_number:%x",ts_sdt[i].section_number);
        group6->setText(0,tempbuf);
        QTreeWidgetItem *group7 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"last_section_number:%x",ts_sdt[i].last_section_number);
        group7->setText(0,tempbuf);
        QTreeWidgetItem *group8 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"original_network_id:%x",ts_sdt[i].original_network_id);
        group8->setText(0,tempbuf);
        for(int j = 0;j < ts_sdt[i].bus_desc.length();j++)
        {
            QTreeWidgetItem *item = new QTreeWidgetItem(group);
            sprintf(tempbuf,"bussiness_descriptor%d",j+1);
            item->setText(0,tempbuf);
            QTreeWidgetItem *item1 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"service_id:%d",ts_sdt[i].bus_desc[j].service_id);
            item1->setText(0,tempbuf);
            QTreeWidgetItem *item2 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"EIT_schedule_flag:%x",ts_sdt[i].bus_desc[j].EIT_schedule_flag);
            item2->setText(0,tempbuf);
            QTreeWidgetItem *item3 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"EIT_presnet_following_flag:%x",ts_sdt[i].bus_desc[j].EIT_presnet_following_flag);
            item3->setText(0,tempbuf);
            QTreeWidgetItem *item4 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"running_status:%x",ts_sdt[i].bus_desc[j].running_status);
            switch(ts_sdt[i].bus_desc[j].running_status)
            {
                case 0:
                    strcat(tempbuf,"(undeined)");
                    break;
                case 1:
                    strcat(tempbuf,"(not running)");
                    break;
                case 2:
                    strcat(tempbuf,"(starts in a few secoends)");
                    break;
                case 3:
                    strcat(tempbuf,"(pausing)");
                    break;
                case 4:
                    strcat(tempbuf,"(running)");
                    break;
                case 5:
                    strcat(tempbuf,"(service off-air)");
                    break;
                default:
                    strcat(tempbuf,"(reserved_future_use)");
                    break;
            }
            item4->setText(0,tempbuf);
            QTreeWidgetItem *item5 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"free_CA_mode:%x",ts_sdt[i].bus_desc[j].free_CA_mode);
            item5->setText(0,tempbuf);
            QTreeWidgetItem *item6 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"descriptor_loop_length:%x",ts_sdt[i].bus_desc[j].descriptor_loop_length);
            item6->setText(0,tempbuf);
            for(int k = 0;k < ts_sdt[i].bus_desc[j].ser_desc.length();k++)
            {
                QTreeWidgetItem *title = new QTreeWidgetItem(item);
                sprintf(tempbuf,"service_descriptor%d",k+1);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_tag:%x",ts_sdt[i].bus_desc[j].ser_desc[k].descriptor_tag);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_length:%x",ts_sdt[i].bus_desc[j].ser_desc[k].descriptor_length);
                title2->setText(0,tempbuf);
                QTreeWidgetItem *title3 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"service_type:%x",ts_sdt[i].bus_desc[j].ser_desc[k].service_type);
                title3->setText(0,tempbuf);
                QTreeWidgetItem *title4 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"service_provider_name_length:%x",ts_sdt[i].bus_desc[j].ser_desc[k].service_provider_name_length);
                title4->setText(0,tempbuf);
                QTreeWidgetItem *title5 = new QTreeWidgetItem(title);
                title5->setText(0,ts_sdt[i].bus_desc[j].ser_desc[k].service_provider_name);
                QTreeWidgetItem *title6 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"service_name_length:%x",ts_sdt[i].bus_desc[j].ser_desc[k].service_name_length);
                title6->setText(0,tempbuf);
                QTreeWidgetItem *title7 = new QTreeWidgetItem(title);
                title7->setText(0,ts_sdt[i].bus_desc[j].ser_desc[k].service_name);
            }
            for(int k = 0;k < ts_sdt[i].bus_desc[j].pri_data.length();k++)
            {
                QTreeWidgetItem *title = new QTreeWidgetItem(item);
                sprintf(tempbuf,"private_data_specifier_descriptor%d",k+1);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_tag:%x",ts_sdt[i].bus_desc[j].pri_data[k].descriptor_tag);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_length:%x",ts_sdt[i].bus_desc[j].pri_data[k].descriptor_length);
                title2->setText(0,tempbuf);
                QTreeWidgetItem *title3 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"private_data_specifier:%x",ts_sdt[i].bus_desc[j].pri_data[k].private_data_specifier);
                title3->setText(0,tempbuf);
            }
            for(int k = 0;k < ts_sdt[i].bus_desc[j].un_desc.length();k++)
            {
                QTreeWidgetItem *title = new QTreeWidgetItem(item);
                sprintf(tempbuf,"unkonwn_descriptor%d",k+1);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_tag:%x",ts_sdt[i].bus_desc[j].un_desc[k].descriptor_tag);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_length:%x",ts_sdt[i].bus_desc[j].un_desc[k].descriptor_length);
                title2->setText(0,tempbuf);
                QTreeWidgetItem *title3 = new QTreeWidgetItem(title);
                title3->setText(0,"data");
                for(int m = 0;m < ts_sdt[i].bus_desc[j].un_desc[k].data.length();m++)
                {
                    QTreeWidgetItem *title4 = new QTreeWidgetItem(title3);
                    sprintf(tempbuf,"%x ",ts_sdt[i].bus_desc[j].un_desc[k].data[m]);
                    title4->setText(0,tempbuf);
                }
            }
        }
        QTreeWidgetItem *group9 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"CRC32:%x",ts_sdt[i].CRC_32);
        group9->setText(0,tempbuf);
        QTreeWidgetItem *group10 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"check_CRC32:%x",ts_sdt[i].check_CRC32);
        group10->setText(0,tempbuf);
    }
}
void Dialog::init_EIT_tree()
{
    if(ts_eit_pf_actual.length() > 0)
        init_EIT_PF_actual();
    if(ts_eit_pf_other.length() > 0)
        init_EIT_PF_other();
    if(ts_eit_schedule_actual.length() > 0)
    {
        sort(ts_eit_schedule_actual.begin(),ts_eit_schedule_actual.end(),sort_service_id);
        init_EIT_schedule_actual();
    }
    if(ts_eit_schedule_other.length() > 0)
        init_EIT_schedule_other();
}
void Dialog::init_EIT_PF_actual()
{
    QTreeWidgetItem *EIT = new QTreeWidgetItem(ui->treeWidget);
    EIT->setText(0,"EIT_PF_actual_information");
    for(int i = 0;i < ts_eit_pf_actual.length();i++)
    {
        QTreeWidgetItem *group = new QTreeWidgetItem(EIT);
        sprintf(tempbuf,"section%d",i+1);
        group->setText(0,tempbuf);
        QTreeWidgetItem *group1 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"table_id:%x",ts_eit_pf_actual[i].table_id);
        group1->setText(0,tempbuf);
        QTreeWidgetItem *group2 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"section_length:%x",ts_eit_pf_actual[i].section_length);
        group2->setText(0,tempbuf);
        QTreeWidgetItem *group3 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"service_id:%d",ts_eit_pf_actual[i].service_id);
        group3->setText(0,tempbuf);
        QTreeWidgetItem *group4 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"version_number:%x",ts_eit_pf_actual[i].version_number);
        group4->setText(0,tempbuf);
        QTreeWidgetItem *group5 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"section_number:%x",ts_eit_pf_actual[i].section_number);
        group5->setText(0,tempbuf);
        QTreeWidgetItem *group6 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"last_section_number:%x",ts_eit_pf_actual[i].last_section_number);
        group6->setText(0,tempbuf);
        QTreeWidgetItem *group7 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"transport_stream_id:%x",ts_eit_pf_actual[i].transport_stream_id);
        group7->setText(0,tempbuf);
        QTreeWidgetItem *group8 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"original_network_id:%x",ts_eit_pf_actual[i].original_network_id);
        group8->setText(0,tempbuf);
        QTreeWidgetItem *group9 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"segment_last_section_number:%x",ts_eit_pf_actual[i].segment_last_section_number);
        group9->setText(0,tempbuf);
        QTreeWidgetItem *group10 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"last_table_id:%x",ts_eit_pf_actual[i].last_table_id);
        group10->setText(0,tempbuf);
        for(int j = 0;j < ts_eit_pf_actual[i].eit_loop.length();j++)
        {
            QTreeWidgetItem *item = new QTreeWidgetItem(group);
            sprintf(tempbuf,"event_descriptor%d",j+1);
            item->setText(0,tempbuf);
            QTreeWidgetItem *item1 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"event_id:%x",ts_eit_pf_actual[i].eit_loop[j].event_id);
            item1->setText(0,tempbuf);
            QTreeWidgetItem *item2 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"UTC_time:%d/%d/%d %d:%d:%d",ts_eit_pf_actual[i].eit_loop[j].UTC_time.Year
                                                        ,ts_eit_pf_actual[i].eit_loop[j].UTC_time.Mouth
                                                        ,ts_eit_pf_actual[i].eit_loop[j].UTC_time.Day
                                                        ,ts_eit_pf_actual[i].eit_loop[j].UTC_time.Hour
                                                        ,ts_eit_pf_actual[i].eit_loop[j].UTC_time.Min
                                                        ,ts_eit_pf_actual[i].eit_loop[j].UTC_time.Sec);
            item2->setText(0,tempbuf);
            QTreeWidgetItem *item3 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"duration:%d:%d:%d",ts_eit_pf_actual[i].eit_loop[j].duration_hour
                                               ,ts_eit_pf_actual[i].eit_loop[j].duration_min
                                               ,ts_eit_pf_actual[i].eit_loop[j].duration_sec);
            item3->setText(0,tempbuf);
            QTreeWidgetItem *item4 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"running_status:%x",ts_eit_pf_actual[i].eit_loop[j].running_status);
            switch(ts_eit_pf_actual[i].eit_loop[j].running_status)
            {
                case 0:
                    strcat(tempbuf,"(undeined)");
                    break;
                case 1:
                    strcat(tempbuf,"(not running)");
                    break;
                case 2:
                    strcat(tempbuf,"(starts in a few secoends)");
                    break;
                case 3:
                    strcat(tempbuf,"(pausing)");
                    break;
                case 4:
                    strcat(tempbuf,"(running)");
                    break;
                case 5:
                    strcat(tempbuf,"(service off-air)");
                    break;
                default:
                    strcat(tempbuf,"(reserved_future_use)");
                    break;
            }
            item4->setText(0,tempbuf);
            QTreeWidgetItem *item5 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"free_CA_mode:%x",ts_eit_pf_actual[i].eit_loop[j].free_CA_mode);
            item5->setText(0,tempbuf);
            QTreeWidgetItem *item6 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"descriptors_loop_length:%x",ts_eit_pf_actual[i].eit_loop[j].descriptors_loop_length);
            item6->setText(0,tempbuf);
            for(int k = 0;k < ts_eit_pf_actual[i].eit_loop[j].short_eve.length();k++)
            {
                QTreeWidgetItem *title = new QTreeWidgetItem(item);
                sprintf(tempbuf,"short_event_descriptor%d",k+1);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_tag:%x",ts_eit_pf_actual[i].eit_loop[j].short_eve[k].descriptor_tag);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_length:%x",ts_eit_pf_actual[i].eit_loop[j].short_eve[k].descriptor_length);
                title2->setText(0,tempbuf);
                QTreeWidgetItem *title3 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"ISO_639_language_code:%c%c%c",ts_eit_pf_actual[i].eit_loop[j].short_eve[k].ISO_639_language_code >> 16
                                                              ,(ts_eit_pf_actual[i].eit_loop[j].short_eve[k].ISO_639_language_code >> 8)&0xff
                                                              ,ts_eit_pf_actual[i].eit_loop[j].short_eve[k].ISO_639_language_code & 0xff);
                title3->setText(0,tempbuf);
                QTreeWidgetItem *title4 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"event_name_length:%x",ts_eit_pf_actual[i].eit_loop[j].short_eve[k].event_name_length);
                title4->setText(0,tempbuf);
                QTreeWidgetItem *title5 = new QTreeWidgetItem(title);
                title5->setText(0,ts_eit_pf_actual[i].eit_loop[j].short_eve[k].event_name);
                QTreeWidgetItem *title6 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"text_length:%x",ts_eit_pf_actual[i].eit_loop[j].short_eve[k].text_length);
                title6->setText(0,tempbuf);
                QTreeWidgetItem *title7 = new QTreeWidgetItem(title);
                title7->setText(0,ts_eit_pf_actual[i].eit_loop[j].short_eve[k].text_char);
            }
            for(int k = 0;k < ts_eit_pf_actual[i].eit_loop[j].content_des.length();k++)
            {
                QTreeWidgetItem *title = new QTreeWidgetItem(item);
                sprintf(tempbuf,"content_descriptor%d",k+1);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_tag:%x",ts_eit_pf_actual[i].eit_loop[j].content_des[k].descriptor_tag);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_length:%x",ts_eit_pf_actual[i].eit_loop[j].content_des[k].descriptor_length);
                title2->setText(0,tempbuf);
                for(int m = 0;m < ts_eit_pf_actual[i].eit_loop[j].content_des[k].cat_info.length();m++)
                {
                    QTreeWidgetItem *label = new QTreeWidgetItem(title);
                    label->setText(0,"category_information");
                    QTreeWidgetItem *label1 = new QTreeWidgetItem(label);
                    sprintf(tempbuf,"content_nibble_level_1:%x",ts_eit_pf_actual[i].eit_loop[j].content_des[k].cat_info[m].content_nibble_level_1);
                    label1->setText(0,tempbuf);
                    QTreeWidgetItem *label2 = new QTreeWidgetItem(label);
                    sprintf(tempbuf,"content_nibble_level_2:%x",ts_eit_pf_actual[i].eit_loop[j].content_des[k].cat_info[m].content_nibble_level_2);
                    label2->setText(0,tempbuf);
                    QTreeWidgetItem *label3 = new QTreeWidgetItem(label);
                    sprintf(tempbuf,"user_byte:%x",ts_eit_pf_actual[i].eit_loop[j].content_des[k].cat_info[m].user_byte);
                    label3->setText(0,tempbuf);
                }
            }
            for(int k = 0;k < ts_eit_pf_actual[i].eit_loop[j].componet_des.length();k++)
            {
                QTreeWidgetItem *title = new QTreeWidgetItem(item);
                sprintf(tempbuf,"component_descriptor%d",k+1);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_tag:%x",ts_eit_pf_actual[i].eit_loop[j].componet_des[k].descriptor_tag);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_length:%x",ts_eit_pf_actual[i].eit_loop[j].componet_des[k].descriptor_length);
                title2->setText(0,tempbuf);
                QTreeWidgetItem *title3 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"stream_content:%x",ts_eit_pf_actual[i].eit_loop[j].componet_des[k].stream_content);
                title3->setText(0,tempbuf);
                QTreeWidgetItem *title4 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"component_type:%x",ts_eit_pf_actual[i].eit_loop[j].componet_des[k].component_type);
                title4->setText(0,tempbuf);
                QTreeWidgetItem *title5 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"component_tag:%x",ts_eit_pf_actual[i].eit_loop[j].componet_des[k].component_tag);
                title5->setText(0,tempbuf);
                QTreeWidgetItem *title6 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"ISO_639_language_code:%c%c%c",ts_eit_pf_actual[i].eit_loop[j].componet_des[k].ISO_639_language_code >> 16
                                                              ,(ts_eit_pf_actual[i].eit_loop[j].componet_des[k].ISO_639_language_code >> 8)&0xff
                                                              ,ts_eit_pf_actual[i].eit_loop[j].componet_des[k].ISO_639_language_code & 0xff);
                title6->setText(0,tempbuf);
                QTreeWidgetItem *title7 = new QTreeWidgetItem(title);
                title7->setText(0,ts_eit_pf_actual[i].eit_loop[j].componet_des[k].text_char);
            }
            for(int k = 0;k < ts_eit_pf_actual[i].eit_loop[j].extended_eve_des.length();k++)
            {
                QTreeWidgetItem *title = new QTreeWidgetItem(item);
                sprintf(tempbuf,"extended_event_descriptor%d",k+1);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_tag:%x",ts_eit_pf_actual[i].eit_loop[j].extended_eve_des[k].descriptor_tag);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_length:%x",ts_eit_pf_actual[i].eit_loop[j].extended_eve_des[k].descriptor_length);
                title2->setText(0,tempbuf);
                QTreeWidgetItem *title3 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"ISO_639_language_code:%c%c%c",ts_eit_pf_actual[i].eit_loop[j].extended_eve_des[k].ISO_639_language_code >> 16
                                                              ,(ts_eit_pf_actual[i].eit_loop[j].extended_eve_des[k].ISO_639_language_code >> 8)&0xff
                                                              ,ts_eit_pf_actual[i].eit_loop[j].extended_eve_des[k].ISO_639_language_code & 0xff);
                title3->setText(0,tempbuf);
                QTreeWidgetItem *title4 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_number:%x",ts_eit_pf_actual[i].eit_loop[j].extended_eve_des[k].descriptor_number);
                title4->setText(0,tempbuf);
                QTreeWidgetItem *title5 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"last_descriptor_number:%x",ts_eit_pf_actual[i].eit_loop[j].extended_eve_des[k].last_descriptor_number);
                title5->setText(0,tempbuf);
                QTreeWidgetItem *title6 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"length_of_items:%x",ts_eit_pf_actual[i].eit_loop[j].extended_eve_des[k].length_of_items);
                title6->setText(0,tempbuf);
                for(int m = 0 ;m < ts_eit_pf_actual[i].eit_loop[j].extended_eve_des[k].item.length();m++)
                {
                    QTreeWidgetItem *lable = new QTreeWidgetItem(title);
                    lable->setText(0,"item");
                    QTreeWidgetItem *lable1 = new QTreeWidgetItem(lable);
                    sprintf(tempbuf,"item_description_length:%x",ts_eit_pf_actual[i].eit_loop[j].extended_eve_des[k].item[m].item_description_length);
                    lable1->setText(0,tempbuf);
                    QTreeWidgetItem *lable2 = new QTreeWidgetItem(lable);
                    lable2->setText(0,ts_eit_pf_actual[i].eit_loop[j].extended_eve_des[k].item[m].item_description_char);
                    QTreeWidgetItem *lable3 = new QTreeWidgetItem(lable);
                    sprintf(tempbuf,"item_length:%x",ts_eit_pf_actual[i].eit_loop[j].extended_eve_des[k].item[m].item_length);
                    lable3->setText(0,tempbuf);
                    QTreeWidgetItem *lable4 = new QTreeWidgetItem(lable);
                    lable4->setText(0,ts_eit_pf_actual[i].eit_loop[j].extended_eve_des[k].item[m].item_char);
                }
                QTreeWidgetItem *title8 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"text_length:%x",ts_eit_pf_actual[i].eit_loop[j].extended_eve_des[k].text_length);
                title8->setText(0,tempbuf);
                QTreeWidgetItem *title7 = new QTreeWidgetItem(title);
                title7->setText(0,ts_eit_pf_actual[i].eit_loop[j].extended_eve_des[k].text_char);
            }
            for(int k = 0;k < ts_eit_pf_actual[i].eit_loop[j].parental_rat_des.length();k++)
            {
                QTreeWidgetItem *title = new QTreeWidgetItem(item);
                sprintf(tempbuf,"parental_rating_descriptor%d",k+1);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_tag:%x",ts_eit_pf_actual[i].eit_loop[j].parental_rat_des[k].descriptor_tag);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_length:%x",ts_eit_pf_actual[i].eit_loop[j].parental_rat_des[k].descriptor_length);
                title2->setText(0,tempbuf);
                for(int m = 0; m < ts_eit_pf_actual[i].eit_loop[j].parental_rat_des[k].ctrl_info.length();m++)
                {
                    QTreeWidgetItem *lable = new QTreeWidgetItem(title);
                    lable->setText(0,"contrl_information");
                    QTreeWidgetItem *lable1 = new QTreeWidgetItem(lable);
                    sprintf(tempbuf,"country_code:%c%c%c",ts_eit_pf_actual[i].eit_loop[j].parental_rat_des[k].ctrl_info[m].country_code >> 16
                                                         ,(ts_eit_pf_actual[i].eit_loop[j].parental_rat_des[k].ctrl_info[m].country_code >> 8)&0xff
                                                         ,ts_eit_pf_actual[i].eit_loop[j].parental_rat_des[k].ctrl_info[m].country_code & 0xff);
                    lable1->setText(0,tempbuf);
                    QTreeWidgetItem *lable2 = new QTreeWidgetItem(lable);
                    sprintf(tempbuf,"rating:%x",ts_eit_pf_actual[i].eit_loop[j].parental_rat_des[k].ctrl_info[m].rating);
                    lable2->setText(0,tempbuf);
                }
            }
        }
        QTreeWidgetItem *group11 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"CRC32:%x",ts_eit_pf_actual[i].CRC_32);
        group11->setText(0,tempbuf);
        QTreeWidgetItem *group12 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"check_CRC32:%x",ts_eit_pf_actual[i].check_CRC32);
        group12->setText(0,tempbuf);
    }
}
void Dialog::init_EIT_PF_other()
{
    QTreeWidgetItem *EIT = new QTreeWidgetItem(ui->treeWidget);
    EIT->setText(0,"EIT_PF_other_information");
    for(int i = 0;i < ts_eit_pf_other.length();i++)
    {
        QTreeWidgetItem *group = new QTreeWidgetItem(EIT);
        sprintf(tempbuf,"section%d",i+1);
        group->setText(0,tempbuf);
        QTreeWidgetItem *group1 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"table_id:%x",ts_eit_pf_other[i].table_id);
        group1->setText(0,tempbuf);
        QTreeWidgetItem *group2 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"section_length:%x",ts_eit_pf_other[i].section_length);
        group2->setText(0,tempbuf);
        QTreeWidgetItem *group3 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"service_id:%d",ts_eit_pf_other[i].service_id);
        group3->setText(0,tempbuf);
        QTreeWidgetItem *group4 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"version_number:%x",ts_eit_pf_other[i].version_number);
        group4->setText(0,tempbuf);
        QTreeWidgetItem *group5 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"section_number:%x",ts_eit_pf_other[i].section_number);
        group5->setText(0,tempbuf);
        QTreeWidgetItem *group6 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"last_section_number:%x",ts_eit_pf_other[i].last_section_number);
        group6->setText(0,tempbuf);
        QTreeWidgetItem *group7 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"transport_stream_id:%x",ts_eit_pf_other[i].transport_stream_id);
        group7->setText(0,tempbuf);
        QTreeWidgetItem *group8 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"original_network_id:%x",ts_eit_pf_other[i].original_network_id);
        group8->setText(0,tempbuf);
        QTreeWidgetItem *group9 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"segment_last_section_number:%x",ts_eit_pf_other[i].segment_last_section_number);
        group9->setText(0,tempbuf);
        QTreeWidgetItem *group10 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"last_table_id:%x",ts_eit_pf_other[i].last_table_id);
        group10->setText(0,tempbuf);
        for(int j = 0;j < ts_eit_pf_other[i].eit_loop.length();j++)
        {
            QTreeWidgetItem *item = new QTreeWidgetItem(group);
            sprintf(tempbuf,"event_descriptor%d",j+1);
            item->setText(0,tempbuf);
            QTreeWidgetItem *item1 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"event_id:%x",ts_eit_pf_other[i].eit_loop[j].event_id);
            item1->setText(0,tempbuf);
            QTreeWidgetItem *item2 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"UTC_time:%d/%d/%d %d:%d:%d",ts_eit_pf_other[i].eit_loop[j].UTC_time.Year
                                                        ,ts_eit_pf_other[i].eit_loop[j].UTC_time.Mouth
                                                        ,ts_eit_pf_other[i].eit_loop[j].UTC_time.Day
                                                        ,ts_eit_pf_other[i].eit_loop[j].UTC_time.Hour
                                                        ,ts_eit_pf_other[i].eit_loop[j].UTC_time.Min
                                                        ,ts_eit_pf_other[i].eit_loop[j].UTC_time.Sec);
            item2->setText(0,tempbuf);
            QTreeWidgetItem *item3 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"duration:%d:%d:%d",ts_eit_pf_other[i].eit_loop[j].duration_hour
                                               ,ts_eit_pf_other[i].eit_loop[j].duration_min
                                               ,ts_eit_pf_other[i].eit_loop[j].duration_sec);
            item3->setText(0,tempbuf);
            QTreeWidgetItem *item4 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"running_status:%x",ts_eit_pf_other[i].eit_loop[j].running_status);
            switch(ts_eit_pf_other[i].eit_loop[j].running_status)
            {
                case 0:
                    strcat(tempbuf,"(undeined)");
                    break;
                case 1:
                    strcat(tempbuf,"(not running)");
                    break;
                case 2:
                    strcat(tempbuf,"(starts in a few secoends)");
                    break;
                case 3:
                    strcat(tempbuf,"(pausing)");
                    break;
                case 4:
                    strcat(tempbuf,"(running)");
                    break;
                case 5:
                    strcat(tempbuf,"(service off-air)");
                    break;
                default:
                    strcat(tempbuf,"(reserved_future_use)");
                    break;
            }
            item4->setText(0,tempbuf);
            QTreeWidgetItem *item5 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"free_CA_mode:%x",ts_eit_pf_other[i].eit_loop[j].free_CA_mode);
            item5->setText(0,tempbuf);
            QTreeWidgetItem *item6 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"descriptors_loop_length:%x",ts_eit_pf_other[i].eit_loop[j].descriptors_loop_length);
            item6->setText(0,tempbuf);
            for(int k = 0;k < ts_eit_pf_other[i].eit_loop[j].short_eve.length();k++)
            {
                QTreeWidgetItem *title = new QTreeWidgetItem(item);
                sprintf(tempbuf,"short_event_descriptor%d",k+1);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_tag:%x",ts_eit_pf_other[i].eit_loop[j].short_eve[k].descriptor_tag);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_length:%x",ts_eit_pf_other[i].eit_loop[j].short_eve[k].descriptor_length);
                title2->setText(0,tempbuf);
                QTreeWidgetItem *title3 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"ISO_639_language_code:%c%c%c",ts_eit_pf_other[i].eit_loop[j].short_eve[k].ISO_639_language_code >> 16
                                                              ,(ts_eit_pf_other[i].eit_loop[j].short_eve[k].ISO_639_language_code >> 8)&0xff
                                                              ,ts_eit_pf_other[i].eit_loop[j].short_eve[k].ISO_639_language_code & 0xff);
                title3->setText(0,tempbuf);
                QTreeWidgetItem *title4 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"event_name_length:%x",ts_eit_pf_other[i].eit_loop[j].short_eve[k].event_name_length);
                title4->setText(0,tempbuf);
                QTreeWidgetItem *title5 = new QTreeWidgetItem(title);
                title5->setText(0,ts_eit_pf_other[i].eit_loop[j].short_eve[k].event_name);
                QTreeWidgetItem *title6 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"text_length:%x",ts_eit_pf_other[i].eit_loop[j].short_eve[k].text_length);
                title6->setText(0,tempbuf);
                QTreeWidgetItem *title7 = new QTreeWidgetItem(title);
                title7->setText(0,ts_eit_pf_other[i].eit_loop[j].short_eve[k].text_char);
            }
            for(int k = 0;k < ts_eit_pf_other[i].eit_loop[j].content_des.length();k++)
            {
                QTreeWidgetItem *title = new QTreeWidgetItem(item);
                sprintf(tempbuf,"content_descriptor%d",k+1);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_tag:%x",ts_eit_pf_other[i].eit_loop[j].content_des[k].descriptor_tag);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_length:%x",ts_eit_pf_other[i].eit_loop[j].content_des[k].descriptor_length);
                title2->setText(0,tempbuf);
                for(int m = 0;m < ts_eit_pf_other[i].eit_loop[j].content_des[k].cat_info.length();m++)
                {
                    QTreeWidgetItem *label = new QTreeWidgetItem(title);
                    label->setText(0,"category_information");
                    QTreeWidgetItem *label1 = new QTreeWidgetItem(label);
                    sprintf(tempbuf,"content_nibble_level_1:%x",ts_eit_pf_other[i].eit_loop[j].content_des[k].cat_info[m].content_nibble_level_1);
                    label1->setText(0,tempbuf);
                    QTreeWidgetItem *label2 = new QTreeWidgetItem(label);
                    sprintf(tempbuf,"content_nibble_level_2:%x",ts_eit_pf_other[i].eit_loop[j].content_des[k].cat_info[m].content_nibble_level_2);
                    label2->setText(0,tempbuf);
                    QTreeWidgetItem *label3 = new QTreeWidgetItem(label);
                    sprintf(tempbuf,"user_byte:%x",ts_eit_pf_other[i].eit_loop[j].content_des[k].cat_info[m].user_byte);
                    label3->setText(0,tempbuf);
                }
            }
            for(int k = 0;k < ts_eit_pf_other[i].eit_loop[j].componet_des.length();k++)
            {
                QTreeWidgetItem *title = new QTreeWidgetItem(item);
                sprintf(tempbuf,"component_descriptor%d",k+1);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_tag:%x",ts_eit_pf_other[i].eit_loop[j].componet_des[k].descriptor_tag);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_length:%x",ts_eit_pf_other[i].eit_loop[j].componet_des[k].descriptor_length);
                title2->setText(0,tempbuf);
                QTreeWidgetItem *title3 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"stream_content:%x",ts_eit_pf_other[i].eit_loop[j].componet_des[k].stream_content);
                title3->setText(0,tempbuf);
                QTreeWidgetItem *title4 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"component_type:%x",ts_eit_pf_other[i].eit_loop[j].componet_des[k].component_type);
                title4->setText(0,tempbuf);
                QTreeWidgetItem *title5 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"component_tag:%x",ts_eit_pf_other[i].eit_loop[j].componet_des[k].component_tag);
                title5->setText(0,tempbuf);
                QTreeWidgetItem *title6 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"ISO_639_language_code:%c%c%c",ts_eit_pf_other[i].eit_loop[j].componet_des[k].ISO_639_language_code >> 16
                                                              ,(ts_eit_pf_other[i].eit_loop[j].componet_des[k].ISO_639_language_code >> 8)&0xff
                                                              ,ts_eit_pf_other[i].eit_loop[j].componet_des[k].ISO_639_language_code & 0xff);
                title6->setText(0,tempbuf);
                QTreeWidgetItem *title7 = new QTreeWidgetItem(title);
                title7->setText(0,ts_eit_pf_other[i].eit_loop[j].componet_des[k].text_char);
            }
            for(int k = 0;k < ts_eit_pf_other[i].eit_loop[j].extended_eve_des.length();k++)
            {
                QTreeWidgetItem *title = new QTreeWidgetItem(item);
                sprintf(tempbuf,"extended_event_descriptor%d",k+1);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_tag:%x",ts_eit_pf_other[i].eit_loop[j].extended_eve_des[k].descriptor_tag);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_length:%x",ts_eit_pf_other[i].eit_loop[j].extended_eve_des[k].descriptor_length);
                title2->setText(0,tempbuf);
                QTreeWidgetItem *title3 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"ISO_639_language_code:%c%c%c",ts_eit_pf_other[i].eit_loop[j].extended_eve_des[k].ISO_639_language_code >> 16
                                                              ,(ts_eit_pf_other[i].eit_loop[j].extended_eve_des[k].ISO_639_language_code >> 8)&0xff
                                                              ,ts_eit_pf_other[i].eit_loop[j].extended_eve_des[k].ISO_639_language_code & 0xff);
                title3->setText(0,tempbuf);
                QTreeWidgetItem *title4 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_number:%x",ts_eit_pf_other[i].eit_loop[j].extended_eve_des[k].descriptor_number);
                title4->setText(0,tempbuf);
                QTreeWidgetItem *title5 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"last_descriptor_number:%x",ts_eit_pf_other[i].eit_loop[j].extended_eve_des[k].last_descriptor_number);
                title5->setText(0,tempbuf);
                QTreeWidgetItem *title6 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"length_of_items:%x",ts_eit_pf_other[i].eit_loop[j].extended_eve_des[k].length_of_items);
                title6->setText(0,tempbuf);
                for(int m = 0 ;m < ts_eit_pf_other[i].eit_loop[j].extended_eve_des[k].item.length();m++)
                {
                    QTreeWidgetItem *lable = new QTreeWidgetItem(title);
                    lable->setText(0,"item");
                    QTreeWidgetItem *lable1 = new QTreeWidgetItem(lable);
                    sprintf(tempbuf,"item_description_length:%x",ts_eit_pf_other[i].eit_loop[j].extended_eve_des[k].item[m].item_description_length);
                    lable1->setText(0,tempbuf);
                    QTreeWidgetItem *lable2 = new QTreeWidgetItem(lable);
                    lable2->setText(0,ts_eit_pf_other[i].eit_loop[j].extended_eve_des[k].item[m].item_description_char);
                    QTreeWidgetItem *lable3 = new QTreeWidgetItem(lable);
                    sprintf(tempbuf,"item_length:%x",ts_eit_pf_other[i].eit_loop[j].extended_eve_des[k].item[m].item_length);
                    lable3->setText(0,tempbuf);
                    QTreeWidgetItem *lable4 = new QTreeWidgetItem(lable);
                    lable4->setText(0,ts_eit_pf_other[i].eit_loop[j].extended_eve_des[k].item[m].item_char);
                }
                QTreeWidgetItem *title8 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"text_length:%x",ts_eit_pf_other[i].eit_loop[j].extended_eve_des[k].text_length);
                title8->setText(0,tempbuf);
                QTreeWidgetItem *title7 = new QTreeWidgetItem(title);
                title7->setText(0,ts_eit_pf_other[i].eit_loop[j].extended_eve_des[k].text_char);
            }
            for(int k = 0;k < ts_eit_pf_other[i].eit_loop[j].parental_rat_des.length();k++)
            {
                QTreeWidgetItem *title = new QTreeWidgetItem(item);
                sprintf(tempbuf,"parental_rating_descriptor%d",k+1);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_tag:%x",ts_eit_pf_other[i].eit_loop[j].parental_rat_des[k].descriptor_tag);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_length:%x",ts_eit_pf_other[i].eit_loop[j].parental_rat_des[k].descriptor_length);
                title2->setText(0,tempbuf);
                for(int m = 0; m < ts_eit_pf_other[i].eit_loop[j].parental_rat_des[k].ctrl_info.length();m++)
                {
                    QTreeWidgetItem *lable = new QTreeWidgetItem(title);
                    lable->setText(0,"contrl_information");
                    QTreeWidgetItem *lable1 = new QTreeWidgetItem(lable);
                    sprintf(tempbuf,"country_code:%c%c%c",ts_eit_pf_other[i].eit_loop[j].parental_rat_des[k].ctrl_info[m].country_code >> 16
                                                         ,(ts_eit_pf_other[i].eit_loop[j].parental_rat_des[k].ctrl_info[m].country_code >> 8)&0xff
                                                         ,ts_eit_pf_other[i].eit_loop[j].parental_rat_des[k].ctrl_info[m].country_code & 0xff);
                    lable1->setText(0,tempbuf);
                    QTreeWidgetItem *lable2 = new QTreeWidgetItem(lable);
                    sprintf(tempbuf,"rating:%x",ts_eit_pf_other[i].eit_loop[j].parental_rat_des[k].ctrl_info[m].rating);
                    lable2->setText(0,tempbuf);
                }
            }
        }
        QTreeWidgetItem *group11 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"CRC32:%x",ts_eit_pf_other[i].CRC_32);
        group11->setText(0,tempbuf);
        QTreeWidgetItem *group12 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"check_CRC32:%x",ts_eit_pf_other[i].check_CRC32);
        group12->setText(0,tempbuf);
    }
}
void Dialog::init_EIT_schedule_actual()
{
    QTreeWidgetItem *EIT = new QTreeWidgetItem(ui->treeWidget);
    EIT->setText(0,"EIT_schedule_actual_information");
    for(int i = 0;i < ts_eit_schedule_actual.length();i++)
    {
        QTreeWidgetItem *group = new QTreeWidgetItem(EIT);
        sprintf(tempbuf,"section%d(tb_id:%x,sec_id:%d,sec_num:%x,ver_num:%x)",i+1
                                                                             ,ts_eit_schedule_actual[i].table_id
                                                                             ,ts_eit_schedule_actual[i].service_id
                                                                             ,ts_eit_schedule_actual[i].section_number
                                                                             ,ts_eit_schedule_actual[i].version_number);
        group->setText(0,tempbuf);
        QTreeWidgetItem *group1 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"table_id:%x",ts_eit_schedule_actual[i].table_id);
        group1->setText(0,tempbuf);
        QTreeWidgetItem *group2 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"section_length:%x",ts_eit_schedule_actual[i].section_length);
        group2->setText(0,tempbuf);
        QTreeWidgetItem *group3 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"service_id:%d",ts_eit_schedule_actual[i].service_id);
        group3->setText(0,tempbuf);
        QTreeWidgetItem *group4 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"version_number:%x",ts_eit_schedule_actual[i].version_number);
        group4->setText(0,tempbuf);
        QTreeWidgetItem *group5 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"section_number:%x",ts_eit_schedule_actual[i].section_number);
        group5->setText(0,tempbuf);
        QTreeWidgetItem *group6 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"last_section_number:%x",ts_eit_schedule_actual[i].last_section_number);
        group6->setText(0,tempbuf);
        QTreeWidgetItem *group7 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"transport_stream_id:%x",ts_eit_schedule_actual[i].transport_stream_id);
        group7->setText(0,tempbuf);
        QTreeWidgetItem *group8 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"original_network_id:%x",ts_eit_schedule_actual[i].original_network_id);
        group8->setText(0,tempbuf);
        QTreeWidgetItem *group9 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"segment_last_section_number:%x",ts_eit_schedule_actual[i].segment_last_section_number);
        group9->setText(0,tempbuf);
        QTreeWidgetItem *group10 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"last_table_id:%x",ts_eit_schedule_actual[i].last_table_id);
        group10->setText(0,tempbuf);
        for(int j = 0;j < ts_eit_schedule_actual[i].eit_loop.length();j++)
        {
            QTreeWidgetItem *item = new QTreeWidgetItem(group);
            sprintf(tempbuf,"event_descriptor%d",j+1);
            item->setText(0,tempbuf);
            QTreeWidgetItem *item1 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"event_id:%x",ts_eit_schedule_actual[i].eit_loop[j].event_id);
            item1->setText(0,tempbuf);
            QTreeWidgetItem *item2 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"UTC_time:%d/%d/%d %d:%d:%d",ts_eit_schedule_actual[i].eit_loop[j].UTC_time.Year
                                                        ,ts_eit_schedule_actual[i].eit_loop[j].UTC_time.Mouth
                                                        ,ts_eit_schedule_actual[i].eit_loop[j].UTC_time.Day
                                                        ,ts_eit_schedule_actual[i].eit_loop[j].UTC_time.Hour
                                                        ,ts_eit_schedule_actual[i].eit_loop[j].UTC_time.Min
                                                        ,ts_eit_schedule_actual[i].eit_loop[j].UTC_time.Sec);
            item2->setText(0,tempbuf);
            QTreeWidgetItem *item3 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"duration:%d:%d:%d",ts_eit_schedule_actual[i].eit_loop[j].duration_hour
                                               ,ts_eit_schedule_actual[i].eit_loop[j].duration_min
                                               ,ts_eit_schedule_actual[i].eit_loop[j].duration_sec);
            item3->setText(0,tempbuf);
            QTreeWidgetItem *item4 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"running_status:%x",ts_eit_schedule_actual[i].eit_loop[j].running_status);
            switch(ts_eit_schedule_actual[i].eit_loop[j].running_status)
            {
                case 0:
                    strcat(tempbuf,"(undeined)");
                    break;
                case 1:
                    strcat(tempbuf,"(not running)");
                    break;
                case 2:
                    strcat(tempbuf,"(starts in a few secoends)");
                    break;
                case 3:
                    strcat(tempbuf,"(pausing)");
                    break;
                case 4:
                    strcat(tempbuf,"(running)");
                    break;
                case 5:
                    strcat(tempbuf,"(service off-air)");
                    break;
                default:
                    strcat(tempbuf,"(reserved_future_use)");
                    break;
            }
            item4->setText(0,tempbuf);
            QTreeWidgetItem *item5 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"free_CA_mode:%x",ts_eit_schedule_actual[i].eit_loop[j].free_CA_mode);
            item5->setText(0,tempbuf);
            QTreeWidgetItem *item6 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"descriptors_loop_length:%x",ts_eit_schedule_actual[i].eit_loop[j].descriptors_loop_length);
            item6->setText(0,tempbuf);
            for(int k = 0;k < ts_eit_schedule_actual[i].eit_loop[j].short_eve.length();k++)
            {
                QTreeWidgetItem *title = new QTreeWidgetItem(item);
                sprintf(tempbuf,"short_event_descriptor%d",k+1);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_tag:%x",ts_eit_schedule_actual[i].eit_loop[j].short_eve[k].descriptor_tag);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_length:%x",ts_eit_schedule_actual[i].eit_loop[j].short_eve[k].descriptor_length);
                title2->setText(0,tempbuf);
                QTreeWidgetItem *title3 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"ISO_639_language_code:%c%c%c",ts_eit_schedule_actual[i].eit_loop[j].short_eve[k].ISO_639_language_code >> 16
                                                              ,(ts_eit_schedule_actual[i].eit_loop[j].short_eve[k].ISO_639_language_code >> 8)&0xff
                                                              ,ts_eit_schedule_actual[i].eit_loop[j].short_eve[k].ISO_639_language_code & 0xff);
                title3->setText(0,tempbuf);
                QTreeWidgetItem *title4 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"event_name_length:%x",ts_eit_schedule_actual[i].eit_loop[j].short_eve[k].event_name_length);
                title4->setText(0,tempbuf);
                QTreeWidgetItem *title5 = new QTreeWidgetItem(title);
                title5->setText(0,ts_eit_schedule_actual[i].eit_loop[j].short_eve[k].event_name);
                QTreeWidgetItem *title6 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"text_length:%x",ts_eit_schedule_actual[i].eit_loop[j].short_eve[k].text_length);
                title6->setText(0,tempbuf);
                QTreeWidgetItem *title7 = new QTreeWidgetItem(title);
                title7->setText(0,ts_eit_schedule_actual[i].eit_loop[j].short_eve[k].text_char);
            }
            for(int k = 0;k < ts_eit_schedule_actual[i].eit_loop[j].content_des.length();k++)
            {
                QTreeWidgetItem *title = new QTreeWidgetItem(item);
                sprintf(tempbuf,"content_descriptor%d",k+1);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_tag:%x",ts_eit_schedule_actual[i].eit_loop[j].content_des[k].descriptor_tag);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_length:%x",ts_eit_schedule_actual[i].eit_loop[j].content_des[k].descriptor_length);
                title2->setText(0,tempbuf);
                for(int m = 0;m < ts_eit_schedule_actual[i].eit_loop[j].content_des[k].cat_info.length();m++)
                {
                    QTreeWidgetItem *label = new QTreeWidgetItem(title);
                    label->setText(0,"category_information");
                    QTreeWidgetItem *label1 = new QTreeWidgetItem(label);
                    sprintf(tempbuf,"content_nibble_level_1:%x",ts_eit_schedule_actual[i].eit_loop[j].content_des[k].cat_info[m].content_nibble_level_1);
                    label1->setText(0,tempbuf);
                    QTreeWidgetItem *label2 = new QTreeWidgetItem(label);
                    sprintf(tempbuf,"content_nibble_level_2:%x",ts_eit_schedule_actual[i].eit_loop[j].content_des[k].cat_info[m].content_nibble_level_2);
                    label2->setText(0,tempbuf);
                    QTreeWidgetItem *label3 = new QTreeWidgetItem(label);
                    sprintf(tempbuf,"user_byte:%x",ts_eit_schedule_actual[i].eit_loop[j].content_des[k].cat_info[m].user_byte);
                    label3->setText(0,tempbuf);
                }
            }
            for(int k = 0;k < ts_eit_schedule_actual[i].eit_loop[j].componet_des.length();k++)
            {
                QTreeWidgetItem *title = new QTreeWidgetItem(item);
                sprintf(tempbuf,"component_descriptor%d",k+1);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_tag:%x",ts_eit_schedule_actual[i].eit_loop[j].componet_des[k].descriptor_tag);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_length:%x",ts_eit_schedule_actual[i].eit_loop[j].componet_des[k].descriptor_length);
                title2->setText(0,tempbuf);
                QTreeWidgetItem *title3 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"stream_content:%x",ts_eit_schedule_actual[i].eit_loop[j].componet_des[k].stream_content);
                title3->setText(0,tempbuf);
                QTreeWidgetItem *title4 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"component_type:%x",ts_eit_schedule_actual[i].eit_loop[j].componet_des[k].component_type);
                title4->setText(0,tempbuf);
                QTreeWidgetItem *title5 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"component_tag:%x",ts_eit_schedule_actual[i].eit_loop[j].componet_des[k].component_tag);
                title5->setText(0,tempbuf);
                QTreeWidgetItem *title6 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"ISO_639_language_code:%c%c%c",ts_eit_schedule_actual[i].eit_loop[j].componet_des[k].ISO_639_language_code >> 16
                                                              ,(ts_eit_schedule_actual[i].eit_loop[j].componet_des[k].ISO_639_language_code >> 8)&0xff
                                                              ,ts_eit_schedule_actual[i].eit_loop[j].componet_des[k].ISO_639_language_code & 0xff);
                title6->setText(0,tempbuf);
                QTreeWidgetItem *title7 = new QTreeWidgetItem(title);
                title7->setText(0,ts_eit_schedule_actual[i].eit_loop[j].componet_des[k].text_char);
            }
            for(int k = 0;k < ts_eit_schedule_actual[i].eit_loop[j].extended_eve_des.length();k++)
            {
                QTreeWidgetItem *title = new QTreeWidgetItem(item);
                sprintf(tempbuf,"extended_event_descriptor%d",k+1);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_tag:%x",ts_eit_schedule_actual[i].eit_loop[j].extended_eve_des[k].descriptor_tag);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_length:%x",ts_eit_schedule_actual[i].eit_loop[j].extended_eve_des[k].descriptor_length);
                title2->setText(0,tempbuf);
                QTreeWidgetItem *title3 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"ISO_639_language_code:%c%c%c",ts_eit_schedule_actual[i].eit_loop[j].extended_eve_des[k].ISO_639_language_code >> 16
                                                              ,(ts_eit_schedule_actual[i].eit_loop[j].extended_eve_des[k].ISO_639_language_code >> 8)&0xff
                                                              ,ts_eit_schedule_actual[i].eit_loop[j].extended_eve_des[k].ISO_639_language_code & 0xff);
                title3->setText(0,tempbuf);
                QTreeWidgetItem *title4 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_number:%x",ts_eit_schedule_actual[i].eit_loop[j].extended_eve_des[k].descriptor_number);
                title4->setText(0,tempbuf);
                QTreeWidgetItem *title5 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"last_descriptor_number:%x",ts_eit_schedule_actual[i].eit_loop[j].extended_eve_des[k].last_descriptor_number);
                title5->setText(0,tempbuf);
                QTreeWidgetItem *title6 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"length_of_items:%x",ts_eit_schedule_actual[i].eit_loop[j].extended_eve_des[k].length_of_items);
                title6->setText(0,tempbuf);
                for(int m = 0 ;m < ts_eit_schedule_actual[i].eit_loop[j].extended_eve_des[k].item.length();m++)
                {
                    QTreeWidgetItem *lable = new QTreeWidgetItem(title);
                    lable->setText(0,"item");
                    QTreeWidgetItem *lable1 = new QTreeWidgetItem(lable);
                    sprintf(tempbuf,"item_description_length:%x",ts_eit_schedule_actual[i].eit_loop[j].extended_eve_des[k].item[m].item_description_length);
                    lable1->setText(0,tempbuf);
                    QTreeWidgetItem *lable2 = new QTreeWidgetItem(lable);
                    lable2->setText(0,ts_eit_schedule_actual[i].eit_loop[j].extended_eve_des[k].item[m].item_description_char);
                    QTreeWidgetItem *lable3 = new QTreeWidgetItem(lable);
                    sprintf(tempbuf,"item_length:%x",ts_eit_schedule_actual[i].eit_loop[j].extended_eve_des[k].item[m].item_length);
                    lable3->setText(0,tempbuf);
                    QTreeWidgetItem *lable4 = new QTreeWidgetItem(lable);
                    lable4->setText(0,ts_eit_schedule_actual[i].eit_loop[j].extended_eve_des[k].item[m].item_char);
                }
                QTreeWidgetItem *title8 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"text_length:%x",ts_eit_schedule_actual[i].eit_loop[j].extended_eve_des[k].text_length);
                title8->setText(0,tempbuf);
                QTreeWidgetItem *title7 = new QTreeWidgetItem(title);
                title7->setText(0,ts_eit_schedule_actual[i].eit_loop[j].extended_eve_des[k].text_char);
            }
            for(int k = 0;k < ts_eit_schedule_actual[i].eit_loop[j].parental_rat_des.length();k++)
            {
                QTreeWidgetItem *title = new QTreeWidgetItem(item);
                sprintf(tempbuf,"parental_rating_descriptor%d",k+1);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_tag:%x",ts_eit_schedule_actual[i].eit_loop[j].parental_rat_des[k].descriptor_tag);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_length:%x",ts_eit_schedule_actual[i].eit_loop[j].parental_rat_des[k].descriptor_length);
                title2->setText(0,tempbuf);
                for(int m = 0; m < ts_eit_schedule_actual[i].eit_loop[j].parental_rat_des[k].ctrl_info.length();m++)
                {
                    QTreeWidgetItem *lable = new QTreeWidgetItem(title);
                    lable->setText(0,"contrl_information");
                    QTreeWidgetItem *lable1 = new QTreeWidgetItem(lable);
                    sprintf(tempbuf,"country_code:%c%c%c",ts_eit_schedule_actual[i].eit_loop[j].parental_rat_des[k].ctrl_info[m].country_code >> 16
                                                         ,(ts_eit_schedule_actual[i].eit_loop[j].parental_rat_des[k].ctrl_info[m].country_code >> 8)&0xff
                                                         ,ts_eit_schedule_actual[i].eit_loop[j].parental_rat_des[k].ctrl_info[m].country_code & 0xff);
                    lable1->setText(0,tempbuf);
                    QTreeWidgetItem *lable2 = new QTreeWidgetItem(lable);
                    sprintf(tempbuf,"rating:%x",ts_eit_schedule_actual[i].eit_loop[j].parental_rat_des[k].ctrl_info[m].rating);
                    lable2->setText(0,tempbuf);
                }
            }
        }
        QTreeWidgetItem *group11 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"CRC32:%x",ts_eit_schedule_actual[i].CRC_32);
        group11->setText(0,tempbuf);
        QTreeWidgetItem *group12 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"check_CRC32:%x",ts_eit_schedule_actual[i].check_CRC32);
        group12->setText(0,tempbuf);
    }
}
void Dialog::init_EIT_schedule_other()
{
    QTreeWidgetItem *EIT = new QTreeWidgetItem(ui->treeWidget);
    EIT->setText(0,"EIT_schedule_other_information");
    for(int i = 0;i < ts_eit_schedule_other.length();i++)
    {
        QTreeWidgetItem *group = new QTreeWidgetItem(EIT);
        sprintf(tempbuf,"section%d",i+1);
        group->setText(0,tempbuf);
        QTreeWidgetItem *group1 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"table_id:%x",ts_eit_schedule_other[i].table_id);
        group1->setText(0,tempbuf);
        QTreeWidgetItem *group2 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"section_length:%x",ts_eit_schedule_other[i].section_length);
        group2->setText(0,tempbuf);
        QTreeWidgetItem *group3 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"service_id:%d",ts_eit_schedule_other[i].service_id);
        group3->setText(0,tempbuf);
        QTreeWidgetItem *group4 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"version_number:%x",ts_eit_schedule_other[i].version_number);
        group4->setText(0,tempbuf);
        QTreeWidgetItem *group5 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"section_number:%x",ts_eit_schedule_other[i].section_number);
        group5->setText(0,tempbuf);
        QTreeWidgetItem *group6 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"last_section_number:%x",ts_eit_schedule_other[i].last_section_number);
        group6->setText(0,tempbuf);
        QTreeWidgetItem *group7 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"transport_stream_id:%x",ts_eit_schedule_other[i].transport_stream_id);
        group7->setText(0,tempbuf);
        QTreeWidgetItem *group8 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"original_network_id:%x",ts_eit_schedule_other[i].original_network_id);
        group8->setText(0,tempbuf);
        QTreeWidgetItem *group9 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"segment_last_section_number:%x",ts_eit_schedule_other[i].segment_last_section_number);
        group9->setText(0,tempbuf);
        QTreeWidgetItem *group10 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"last_table_id:%x",ts_eit_schedule_other[i].last_table_id);
        group10->setText(0,tempbuf);
        for(int j = 0;j < ts_eit_schedule_other[i].eit_loop.length();j++)
        {
            QTreeWidgetItem *item = new QTreeWidgetItem(group);
            sprintf(tempbuf,"event_descriptor%d",j+1);
            item->setText(0,tempbuf);
            QTreeWidgetItem *item1 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"event_id:%x",ts_eit_schedule_other[i].eit_loop[j].event_id);
            item1->setText(0,tempbuf);
            QTreeWidgetItem *item2 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"UTC_time:%d/%d/%d %d:%d:%d",ts_eit_schedule_other[i].eit_loop[j].UTC_time.Year
                                                        ,ts_eit_schedule_other[i].eit_loop[j].UTC_time.Mouth
                                                        ,ts_eit_schedule_other[i].eit_loop[j].UTC_time.Day
                                                        ,ts_eit_schedule_other[i].eit_loop[j].UTC_time.Hour
                                                        ,ts_eit_schedule_other[i].eit_loop[j].UTC_time.Min
                                                        ,ts_eit_schedule_other[i].eit_loop[j].UTC_time.Sec);
            item2->setText(0,tempbuf);
            QTreeWidgetItem *item3 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"duration:%d:%d:%d",ts_eit_schedule_other[i].eit_loop[j].duration_hour
                                               ,ts_eit_schedule_other[i].eit_loop[j].duration_min
                                               ,ts_eit_schedule_other[i].eit_loop[j].duration_sec);
            item3->setText(0,tempbuf);
            QTreeWidgetItem *item4 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"running_status:%x",ts_eit_schedule_other[i].eit_loop[j].running_status);
            switch(ts_eit_schedule_other[i].eit_loop[j].running_status)
            {
                case 0:
                    strcat(tempbuf,"(undeined)");
                    break;
                case 1:
                    strcat(tempbuf,"(not running)");
                    break;
                case 2:
                    strcat(tempbuf,"(starts in a few secoends)");
                    break;
                case 3:
                    strcat(tempbuf,"(pausing)");
                    break;
                case 4:
                    strcat(tempbuf,"(running)");
                    break;
                case 5:
                    strcat(tempbuf,"(service off-air)");
                    break;
                default:
                    strcat(tempbuf,"(reserved_future_use)");
                    break;
            }
            item4->setText(0,tempbuf);
            QTreeWidgetItem *item5 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"free_CA_mode:%x",ts_eit_schedule_other[i].eit_loop[j].free_CA_mode);
            item5->setText(0,tempbuf);
            QTreeWidgetItem *item6 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"descriptors_loop_length:%x",ts_eit_schedule_other[i].eit_loop[j].descriptors_loop_length);
            item6->setText(0,tempbuf);
            for(int k = 0;k < ts_eit_schedule_other[i].eit_loop[j].short_eve.length();k++)
            {
                QTreeWidgetItem *title = new QTreeWidgetItem(item);
                sprintf(tempbuf,"short_event_descriptor%d",k+1);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_tag:%x",ts_eit_schedule_other[i].eit_loop[j].short_eve[k].descriptor_tag);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_length:%x",ts_eit_schedule_other[i].eit_loop[j].short_eve[k].descriptor_length);
                title2->setText(0,tempbuf);
                QTreeWidgetItem *title3 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"ISO_639_language_code:%c%c%c",ts_eit_schedule_other[i].eit_loop[j].short_eve[k].ISO_639_language_code >> 16
                                                              ,(ts_eit_schedule_other[i].eit_loop[j].short_eve[k].ISO_639_language_code >> 8)&0xff
                                                              ,ts_eit_schedule_other[i].eit_loop[j].short_eve[k].ISO_639_language_code & 0xff);
                title3->setText(0,tempbuf);
                QTreeWidgetItem *title4 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"event_name_length:%x",ts_eit_schedule_other[i].eit_loop[j].short_eve[k].event_name_length);
                title4->setText(0,tempbuf);
                QTreeWidgetItem *title5 = new QTreeWidgetItem(title);
                title5->setText(0,ts_eit_schedule_other[i].eit_loop[j].short_eve[k].event_name);
                QTreeWidgetItem *title6 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"text_length:%x",ts_eit_schedule_other[i].eit_loop[j].short_eve[k].text_length);
                title6->setText(0,tempbuf);
                QTreeWidgetItem *title7 = new QTreeWidgetItem(title);
                title7->setText(0,ts_eit_schedule_other[i].eit_loop[j].short_eve[k].text_char);
            }
            for(int k = 0;k < ts_eit_schedule_other[i].eit_loop[j].content_des.length();k++)
            {
                QTreeWidgetItem *title = new QTreeWidgetItem(item);
                sprintf(tempbuf,"content_descriptor%d",k+1);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_tag:%x",ts_eit_schedule_other[i].eit_loop[j].content_des[k].descriptor_tag);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_length:%x",ts_eit_schedule_other[i].eit_loop[j].content_des[k].descriptor_length);
                title2->setText(0,tempbuf);
                for(int m = 0;m < ts_eit_schedule_other[i].eit_loop[j].content_des[k].cat_info.length();m++)
                {
                    QTreeWidgetItem *label = new QTreeWidgetItem(title);
                    label->setText(0,"category_information");
                    QTreeWidgetItem *label1 = new QTreeWidgetItem(label);
                    sprintf(tempbuf,"content_nibble_level_1:%x",ts_eit_schedule_other[i].eit_loop[j].content_des[k].cat_info[m].content_nibble_level_1);
                    label1->setText(0,tempbuf);
                    QTreeWidgetItem *label2 = new QTreeWidgetItem(label);
                    sprintf(tempbuf,"content_nibble_level_2:%x",ts_eit_schedule_other[i].eit_loop[j].content_des[k].cat_info[m].content_nibble_level_2);
                    label2->setText(0,tempbuf);
                    QTreeWidgetItem *label3 = new QTreeWidgetItem(label);
                    sprintf(tempbuf,"user_byte:%x",ts_eit_schedule_other[i].eit_loop[j].content_des[k].cat_info[m].user_byte);
                    label3->setText(0,tempbuf);
                }
            }
            for(int k = 0;k < ts_eit_schedule_other[i].eit_loop[j].componet_des.length();k++)
            {
                QTreeWidgetItem *title = new QTreeWidgetItem(item);
                sprintf(tempbuf,"component_descriptor%d",k+1);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_tag:%x",ts_eit_schedule_other[i].eit_loop[j].componet_des[k].descriptor_tag);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_length:%x",ts_eit_schedule_other[i].eit_loop[j].componet_des[k].descriptor_length);
                title2->setText(0,tempbuf);
                QTreeWidgetItem *title3 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"stream_content:%x",ts_eit_schedule_other[i].eit_loop[j].componet_des[k].stream_content);
                title3->setText(0,tempbuf);
                QTreeWidgetItem *title4 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"component_type:%x",ts_eit_schedule_other[i].eit_loop[j].componet_des[k].component_type);
                title4->setText(0,tempbuf);
                QTreeWidgetItem *title5 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"component_tag:%x",ts_eit_schedule_other[i].eit_loop[j].componet_des[k].component_tag);
                title5->setText(0,tempbuf);
                QTreeWidgetItem *title6 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"ISO_639_language_code:%c%c%c",ts_eit_schedule_other[i].eit_loop[j].componet_des[k].ISO_639_language_code >> 16
                                                              ,(ts_eit_schedule_other[i].eit_loop[j].componet_des[k].ISO_639_language_code >> 8)&0xff
                                                              ,ts_eit_schedule_other[i].eit_loop[j].componet_des[k].ISO_639_language_code & 0xff);
                title6->setText(0,tempbuf);
                QTreeWidgetItem *title7 = new QTreeWidgetItem(title);
                title7->setText(0,ts_eit_schedule_other[i].eit_loop[j].componet_des[k].text_char);
            }
            for(int k = 0;k < ts_eit_schedule_other[i].eit_loop[j].extended_eve_des.length();k++)
            {
                QTreeWidgetItem *title = new QTreeWidgetItem(item);
                sprintf(tempbuf,"extended_event_descriptor%d",k+1);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_tag:%x",ts_eit_schedule_other[i].eit_loop[j].extended_eve_des[k].descriptor_tag);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_length:%x",ts_eit_schedule_other[i].eit_loop[j].extended_eve_des[k].descriptor_length);
                title2->setText(0,tempbuf);
                QTreeWidgetItem *title3 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"ISO_639_language_code:%c%c%c",ts_eit_schedule_other[i].eit_loop[j].extended_eve_des[k].ISO_639_language_code >> 16
                                                              ,(ts_eit_schedule_other[i].eit_loop[j].extended_eve_des[k].ISO_639_language_code >> 8)&0xff
                                                              ,ts_eit_schedule_other[i].eit_loop[j].extended_eve_des[k].ISO_639_language_code & 0xff);
                title3->setText(0,tempbuf);
                QTreeWidgetItem *title4 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_number:%x",ts_eit_schedule_other[i].eit_loop[j].extended_eve_des[k].descriptor_number);
                title4->setText(0,tempbuf);
                QTreeWidgetItem *title5 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"last_descriptor_number:%x",ts_eit_schedule_other[i].eit_loop[j].extended_eve_des[k].last_descriptor_number);
                title5->setText(0,tempbuf);
                QTreeWidgetItem *title6 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"length_of_items:%x",ts_eit_schedule_other[i].eit_loop[j].extended_eve_des[k].length_of_items);
                title6->setText(0,tempbuf);
                for(int m = 0 ;m < ts_eit_schedule_other[i].eit_loop[j].extended_eve_des[k].item.length();m++)
                {
                    QTreeWidgetItem *lable = new QTreeWidgetItem(title);
                    lable->setText(0,"item");
                    QTreeWidgetItem *lable1 = new QTreeWidgetItem(lable);
                    sprintf(tempbuf,"item_description_length:%x",ts_eit_schedule_other[i].eit_loop[j].extended_eve_des[k].item[m].item_description_length);
                    lable1->setText(0,tempbuf);
                    QTreeWidgetItem *lable2 = new QTreeWidgetItem(lable);
                    lable2->setText(0,ts_eit_schedule_other[i].eit_loop[j].extended_eve_des[k].item[m].item_description_char);
                    QTreeWidgetItem *lable3 = new QTreeWidgetItem(lable);
                    sprintf(tempbuf,"item_length:%x",ts_eit_schedule_other[i].eit_loop[j].extended_eve_des[k].item[m].item_length);
                    lable3->setText(0,tempbuf);
                    QTreeWidgetItem *lable4 = new QTreeWidgetItem(lable);
                    lable4->setText(0,ts_eit_schedule_other[i].eit_loop[j].extended_eve_des[k].item[m].item_char);
                }
                QTreeWidgetItem *title8 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"text_length:%x",ts_eit_schedule_other[i].eit_loop[j].extended_eve_des[k].text_length);
                title8->setText(0,tempbuf);
                QTreeWidgetItem *title7 = new QTreeWidgetItem(title);
                title7->setText(0,ts_eit_schedule_other[i].eit_loop[j].extended_eve_des[k].text_char);
            }
            for(int k = 0;k < ts_eit_schedule_other[i].eit_loop[j].parental_rat_des.length();k++)
            {
                QTreeWidgetItem *title = new QTreeWidgetItem(item);
                sprintf(tempbuf,"parental_rating_descriptor%d",k+1);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_tag:%x",ts_eit_schedule_other[i].eit_loop[j].parental_rat_des[k].descriptor_tag);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(title);
                sprintf(tempbuf,"descriptor_length:%x",ts_eit_schedule_other[i].eit_loop[j].parental_rat_des[k].descriptor_length);
                title2->setText(0,tempbuf);
                for(int m = 0; m < ts_eit_schedule_other[i].eit_loop[j].parental_rat_des[k].ctrl_info.length();m++)
                {
                    QTreeWidgetItem *lable = new QTreeWidgetItem(title);
                    lable->setText(0,"contrl_information");
                    QTreeWidgetItem *lable1 = new QTreeWidgetItem(lable);
                    sprintf(tempbuf,"country_code:%c%c%c",ts_eit_schedule_other[i].eit_loop[j].parental_rat_des[k].ctrl_info[m].country_code >> 16
                                                         ,(ts_eit_schedule_other[i].eit_loop[j].parental_rat_des[k].ctrl_info[m].country_code >> 8)&0xff
                                                         ,ts_eit_schedule_other[i].eit_loop[j].parental_rat_des[k].ctrl_info[m].country_code & 0xff);
                    lable1->setText(0,tempbuf);
                    QTreeWidgetItem *lable2 = new QTreeWidgetItem(lable);
                    sprintf(tempbuf,"rating:%x",ts_eit_schedule_other[i].eit_loop[j].parental_rat_des[k].ctrl_info[m].rating);
                    lable2->setText(0,tempbuf);
                }
            }
        }
        QTreeWidgetItem *group11 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"CRC32:%x",ts_eit_schedule_other[i].CRC_32);
        group11->setText(0,tempbuf);
        QTreeWidgetItem *group12 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"check_CRC32:%x",ts_eit_schedule_other[i].check_CRC32);
        group12->setText(0,tempbuf);
    }
}
void Dialog::init_tree()
{
    ui->treeWidget->clear();
    if(ts_tot.length() > 0)
        init_TOT_tree();
    if(ts_tdt.length() > 0)
        init_TDT_tree();
    if(ts_sdt.length() > 0)
        init_SDT_tree();
    init_EIT_tree();
}
//void Dialog::on_pushButton_clicked()
//{
//    ui->textBrowser->clear();
//    mthread->abort();
//    thread->wait();
//    mthread->request_thread();
//}

//void Dialog::on_pushButton_2_clicked()
//{
//    ui->textBrowser->clear();
//    mthread->abort();
//    thread->wait();
//    mthread->request_thread1();
//}
