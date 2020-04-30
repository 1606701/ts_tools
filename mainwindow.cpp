#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <stdio.h>
#include <QMessageBox>
#include <QDebug>
#include <iostream>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <unistd.h>
#include "ui_dialog.h"
#include "ui_epg.h"
#include "basic_info.h"
using namespace std;
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
//    delete dialog;
//    delete epg;
//    delete b_info;
    delete []tempbuf;
    delete packet_CAT;
    delete packet_NIT;
    delete packet_PAT;
    delete packet_PMT;
    delete packet_SDT;
    delete packet_TDT;
    delete packet_TOT;
    delete packet_EIT;
}
void MainWindow::adjust_packet_header(TS_HEADER *packet,unsigned char *buffer)
{
    packet->sync_byte                 	 = buffer[0];
    packet->transport_error_indicator 	 = buffer[1] >> 7;
    packet->payload_unit_start_indicator = buffer[1] >> 6 & 0x1;
    packet->transport_priority           = buffer[1] >> 5 & 0x1;
    packet->PID 						 = (buffer[1] & 0x1F) << 8 | buffer[2];
    packet->transport_scrambling_control = buffer[3] >> 6 & 0x3;
    packet->adaptation_field_control     = buffer[3] >> 4 & 0x3;
    packet->continuity_counter           = buffer[3] & 0x0F;
    if(packet->sync_byte != 0x47)
    {
        //perror("sync error");
        return ;
    }
}

void MainWindow::get_all_NIT_buffer(TS_NIT *packet,unsigned char *buffer,unsigned int len)
{
    int i = 0;
    for(i = 0;i < 184;i++)
    {
        //printf("current_length=%x,len=%x\n",length,len);
        if(length_nit<len)
            long_buf_nit[length_nit++]=buffer[i];
        else
        {
            flag_get_nit = 0;
            check_flag_nit = 0;
            //printf("adjust big nit section\n********\n");
            //sleep(1);
            adjust_NIT_table(packet,(unsigned char *)long_buf_nit);
            delete []long_buf_nit;
            length_nit = 0 ;
            //printf("nit over\n");
            //check_flag = 0;
            break;
        }
    }
}
void MainWindow::get_all_EIT_buffer(TS_EIT *packet, unsigned char *buffer, unsigned int len)
{
    int i = 0;
    //printf("current_length=%x,len=%x\n",length,len);
    //printf("get other\n");
    for(i = 0;i < 184;i++)
    {
        //printf("current_length=%x,len=%x\n",length,len);
        if(length_eit<len)
            long_buf_eit[length_eit++]=buffer[i];
        else
        {
            flag_get_eit = 0;
            check_flag_eit = 0;
            //printf("adjust big eit section\n*******\n");
            //sleep(1);
            adjust_EIT_table(packet,(unsigned char *)long_buf_eit);
            delete []long_buf_eit;
            length_eit = 0 ;
            //printf("eit over\n");
            //check_flag = 0;
            break;
        }
    }
}
void MainWindow::get_all_SDT_buffer(TS_SDT *packet, unsigned char *buffer, unsigned int len)
{
    int i = 0;
    //printf("current_length=%x,len=%x\n",length,len);
    //printf("get other\n");
    for(i = 0;i < 184;i++)
    {
        //printf("current_length=%x,len=%x\n",length_sdt,len);
        if(length_sdt<len)
            long_buf_sdt[length_sdt++]=buffer[i];
        else
        {
            flag_get_sdt = 0;
            check_flag_sdt = 0;
            //printf("adjust big sdt section\n*********\n");
            //sleep(1);
            adjust_SDT_table(packet,(unsigned char *)long_buf_sdt);
            delete []long_buf_sdt;
            length_sdt = 0 ;
            //printf("sdt over\n");
            //check_flag = 0;
            break;
        }
    }
}
void MainWindow::get_all_PMT_buffer(TS_PMT *packet, unsigned char *buffer, unsigned int len,unsigned int PID)
{
    int i = 0;
    //printf("current_length=%x,len=%x\n",length,len);
    //printf("get other\n");
    for(i = 0;i < 184;i++)
    {
        //printf("current_length=%x,len=%x\n",length_sdt,len);
        if(length_pmt<len)
            long_buf_pmt[length_pmt++]=buffer[i];
        else
        {
            flag_get_pmt = 0;
            check_flag_pmt = 0;
            //printf("adjust big sdt section\n*********\n");
            //sleep(1);
            adjust_PMT_table(packet,(unsigned char *)long_buf_pmt,PID);
            delete []long_buf_pmt;
            length_pmt = 0 ;
            //printf("sdt over\n");
            //check_flag = 0;
            break;
        }
    }
}
void MainWindow::init_PAT_tree()
{
    QTreeWidgetItem *PAT = new QTreeWidgetItem(ui->treeWidget);
    PAT->setText(0,"PAT_information");
    for(int i = 0;i < ts_pat_program.length();i++)
    {
        QTreeWidgetItem *group = new QTreeWidgetItem(PAT);
        sprintf(tempbuf,"program%d(progarm_number=%d,PMT_PID=%x)",
                         i+1,ts_pat_program[i].program_number,ts_pat_program[i].program_map_PID);
        group->setText(0,tempbuf);
    }
    QTreeWidgetItem *group1 = new QTreeWidgetItem(PAT);
    sprintf(tempbuf,"CRC32=%x",packet_PAT->CRC_32);
    group1->setText(0,tempbuf);
    QTreeWidgetItem *group2 = new QTreeWidgetItem(PAT);
    sprintf(tempbuf,"check_CRC32=%x",packet_PAT->check_CRC32);
    group2->setText(0,tempbuf);
}
void MainWindow::init_PMT_tree()
{
    QTreeWidgetItem *PMT = new QTreeWidgetItem(ui->treeWidget);
    PMT->setText(0,"PMT_information");
    sort(ts_pmt.begin(),ts_pmt.end());
    ts_pmt.erase(unique(ts_pmt.begin(),ts_pmt.end()),ts_pmt.end());
    for(int i = 0;i < ts_pmt.length();i++)
    {
        QTreeWidgetItem *group = new QTreeWidgetItem(PMT);
        sprintf(tempbuf,"section:(program_id:%d,version_number:%x,section_number:%x)",
                         ts_pmt[i].program_number,ts_pmt[i].version_number,ts_pmt[i].section_number);
        group->setText(0,tempbuf);
        for(int j = 0;j < ts_pmt[i].ts_pmt_stream.length();j++)
        {
            QTreeWidgetItem *group1 = new QTreeWidgetItem(group);
            sprintf(tempbuf,"ES_PID:%x,stream_type:%x(%s)",ts_pmt[i].ts_pmt_stream[j].elementary_PID,
                                                         ts_pmt[i].ts_pmt_stream[j].stream_type,
                                                         ts_pmt[i].ts_pmt_stream[j].type_descriptor.toLatin1().data());
            group1->setText(0,tempbuf);
        }
        QTreeWidgetItem *group2 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"CRC32:%x",ts_pmt[i].CRC_32);
        group2->setText(0,tempbuf);
        QTreeWidgetItem *group3 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"check_CRC32:%x",ts_pmt[i].check_CRC32);
        group3->setText(0,tempbuf);
    }
}
void MainWindow::init_CAT_tree()
{
    QTreeWidgetItem *CAT = new QTreeWidgetItem(ui->treeWidget);
    CAT->setText(0,"CAT_information");
    for(int i = 0;i < ts_cat.length();i++)
    {
        QTreeWidgetItem *group = new QTreeWidgetItem(CAT);
        sprintf(tempbuf,"section:(table_id:%d,version_number:%x,section_number:%x)",
                         ts_cat[i].table_id,ts_pmt[i].version_number,ts_pmt[i].section_number);
        group->setText(0,tempbuf);
        QTreeWidgetItem *group1 = new QTreeWidgetItem(CAT);
        sprintf(tempbuf,"current_next_indicator:%x",ts_cat[i].current_next_indicator);
        group1->setText(0,tempbuf);
        QTreeWidgetItem *group2 = new QTreeWidgetItem(CAT);
        sprintf(tempbuf,"CRC32:%x",ts_cat[i].CRC_32);
        group2->setText(0,tempbuf);
        QTreeWidgetItem *group3 = new QTreeWidgetItem(CAT);
        sprintf(tempbuf,"check_CRC32:%x",ts_cat[i].check_CRC_32);
        group3->setText(0,tempbuf);
    }
}
void MainWindow::init_NIT_tree()
{
    QTreeWidgetItem *NIT = new QTreeWidgetItem(ui->treeWidget);
    NIT->setText(0,"NIT_information");
    for(int i = 0;i < ts_nit.length();i++)
    {
        QTreeWidgetItem *group = new QTreeWidgetItem(NIT);
        sprintf(tempbuf,"section:(table_id:%d,network_id:%x,version_number:%x,section_number:%x)",
                         ts_nit[i].table_id,ts_nit[i].network_id,ts_nit[i].version_number,ts_nit[i].section_number);
        group->setText(0,tempbuf);
        QTreeWidgetItem *group1 = new QTreeWidgetItem(group);
        group1->setText(0,"network_descriptor");
        for(int j = 0;j < ts_nit[i].nit_info_table_1.length();j++)
        {
            QTreeWidgetItem *Item1 = new QTreeWidgetItem(group1);
            sprintf(tempbuf,"descriptor_tag:%x",ts_nit[i].nit_info_table_1[j].descriptor_tag);
            Item1->setText(0,tempbuf);
            QTreeWidgetItem *Item2 = new QTreeWidgetItem(group1);
            sprintf(tempbuf,"descriptor_length:%x",ts_nit[i].nit_info_table_1[j].descriptor_length);
            Item2->setText(0,tempbuf);
            QTreeWidgetItem *Item3 = new QTreeWidgetItem(group1);
            sprintf(tempbuf,"netwrok_name:%s",ts_nit[i].nit_info_table_1[j].descriptor_char.toLatin1().data());
            Item3->setText(0,tempbuf);
        }
        for(int j = 0;j < ts_nit[i].nit_info_table_2.length();j++)
        {
            QTreeWidgetItem *group2 = new QTreeWidgetItem(group);
            sprintf(tempbuf,"Ts_stream_descriptor%d",j+1);
            group2->setText(0,tempbuf);
            QTreeWidgetItem *group3 = new QTreeWidgetItem(group2);
            group3->setText(0,"service_list_descriptor");
            for(int k = 0;k < ts_nit[i].nit_info_table_2[j].service_list.length();k++)
            {
                QTreeWidgetItem *Item1 = new QTreeWidgetItem(group3);
                sprintf(tempbuf,"descriptor_tag:%x",ts_nit[i].nit_info_table_2[j].service_list[k].descriptor_tag);
                Item1->setText(0,tempbuf);
                QTreeWidgetItem *Item2 = new QTreeWidgetItem(group3);
                sprintf(tempbuf,"descriptor_length:%x",ts_nit[i].nit_info_table_2[j].service_list[k].descriptor_length);
                Item2->setText(0,tempbuf);
                for(int m = 0;m < ts_nit[i].nit_info_table_2[j].service_list[k].nit_service.length();m++)
                {
                    QTreeWidgetItem *Item3 = new QTreeWidgetItem(group3);
                    sprintf(tempbuf,"Business_description%d",m+1);
                    Item3->setText(0,tempbuf);
                    QTreeWidgetItem *title = new QTreeWidgetItem(Item3);
                    sprintf(tempbuf,"service_id:%d",ts_nit[i].nit_info_table_2[j].service_list[k].nit_service[m].service_id);
                    title->setText(0,tempbuf);
                    QTreeWidgetItem *title1 = new QTreeWidgetItem(Item3);
                    sprintf(tempbuf,"service_type:%x",ts_nit[i].nit_info_table_2[j].service_list[k].nit_service[m].service_type);
                    title1->setText(0,tempbuf);
                }
            }
            QTreeWidgetItem *group4 = new QTreeWidgetItem(group2);
            group4->setText(0,"private_data_descriptor");
            for(int k = 0;k < ts_nit[i].nit_info_table_2[j].private_data.length();k++)
            {
                QTreeWidgetItem *Item1 = new QTreeWidgetItem(group4);
                sprintf(tempbuf,"descriptor_tag:%x",ts_nit[i].nit_info_table_2[j].private_data[k].descriptor_tag);
                Item1->setText(0,tempbuf);
                QTreeWidgetItem *Item2 = new QTreeWidgetItem(group4);
                sprintf(tempbuf,"descriptor_length:%x",ts_nit[i].nit_info_table_2[j].private_data[k].descriptor_length);
                Item2->setText(0,tempbuf);
                QTreeWidgetItem *Item3 = new QTreeWidgetItem(group4);
                sprintf(tempbuf,"private_data_specifier:%x",ts_nit[i].nit_info_table_2[j].private_data[k].private_data_specifier);
                Item3->setText(0,tempbuf);
            }
            QTreeWidgetItem *group5 = new QTreeWidgetItem(group2);
            group5->setText(0,"terrestrial_delivery_system_descriptor");
            for(int k = 0;k < ts_nit[i].nit_info_table_2[j].ter_delivery.length();k++)
            {
                QTreeWidgetItem *Item1 = new QTreeWidgetItem(group5);
                sprintf(tempbuf,"descriptor_tag:%x",ts_nit[i].nit_info_table_2[j].ter_delivery[k].descriptor_tag);
                Item1->setText(0,tempbuf);
                QTreeWidgetItem *Item2 = new QTreeWidgetItem(group5);
                sprintf(tempbuf,"descriptor_length:%x",ts_nit[i].nit_info_table_2[j].ter_delivery[k].descriptor_length);
                Item2->setText(0,tempbuf);
                QTreeWidgetItem *Item3 = new QTreeWidgetItem(group5);
                sprintf(tempbuf,"centre_frequency:%dHz",ts_nit[i].nit_info_table_2[j].ter_delivery[k].centre_frequency);
                Item3->setText(0,tempbuf);
                QTreeWidgetItem *Item4 = new QTreeWidgetItem(group5);
                switch(ts_nit[i].nit_info_table_2[j].ter_delivery[k].bandwidth)
                {
                    case 0:
                        sprintf(tempbuf,"bandwidth=8Mhz");
                        break;
                    case 1:
                        sprintf(tempbuf,"bandwidth=7Mhz");
                        break;
                    case 2:
                        sprintf(tempbuf,"bandwidth=6Mhz");
                        break;
                    case 3:
                        sprintf(tempbuf,"bandwidth=5Mhz");
                        break;
                    default:
                        sprintf(tempbuf,"Reserved for future use");
                        break;
                }
                Item4->setText(0,tempbuf);
                QTreeWidgetItem *Item5 = new QTreeWidgetItem(group5);
                sprintf(tempbuf,"constellation:%x",ts_nit[i].nit_info_table_2[j].ter_delivery[k].constellation);
                Item5->setText(0,tempbuf);
                QTreeWidgetItem *Item6 = new QTreeWidgetItem(group5);
                sprintf(tempbuf,"hierarchy_information:%x",ts_nit[i].nit_info_table_2[j].ter_delivery[k].hierarchy_information);
                Item6->setText(0,tempbuf);
                QTreeWidgetItem *Item7 = new QTreeWidgetItem(group5);
                sprintf(tempbuf,"code_rate_HP_stream:%x",ts_nit[i].nit_info_table_2[j].ter_delivery[k].code_rate_HP_stream);
                Item7->setText(0,tempbuf);
                QTreeWidgetItem *Item8 = new QTreeWidgetItem(group5);
                sprintf(tempbuf,"code_rate_LP_stream:%x",ts_nit[i].nit_info_table_2[j].ter_delivery[k].code_rate_LP_stream);
                Item8->setText(0,tempbuf);
                QTreeWidgetItem *Item9 = new QTreeWidgetItem(group5);
                sprintf(tempbuf,"guard_interval:%x",ts_nit[i].nit_info_table_2[j].ter_delivery[k].guard_interval);
                Item9->setText(0,tempbuf);
                QTreeWidgetItem *Item10 = new QTreeWidgetItem(group5);
                sprintf(tempbuf,"transmission_mode:%x",ts_nit[i].nit_info_table_2[j].ter_delivery[k].transmission_mode);
                Item10->setText(0,tempbuf);
                QTreeWidgetItem *Item11 = new QTreeWidgetItem(group5);
                sprintf(tempbuf,"other_frequency_flag:%x",ts_nit[i].nit_info_table_2[j].ter_delivery[k].other_frequency_flag);
                Item11->setText(0,tempbuf);
            }
            QTreeWidgetItem *group6 = new QTreeWidgetItem(group2);
            group6->setText(0,"cable_delivery_system_descriptor");
            for(int k = 0;k < ts_nit[i].nit_info_table_2[j].cal_delivery.length();k++)
            {
                QTreeWidgetItem *Item1 = new QTreeWidgetItem(group6);
                sprintf(tempbuf,"descriptor_tag:%x",ts_nit[i].nit_info_table_2[j].cal_delivery[k].descriptor_tag);
                Item1->setText(0,tempbuf);
                QTreeWidgetItem *Item2 = new QTreeWidgetItem(group6);
                sprintf(tempbuf,"descriptor_length:%x",ts_nit[i].nit_info_table_2[j].cal_delivery[k].descriptor_length);
                Item2->setText(0,tempbuf);
                QTreeWidgetItem *Item3 = new QTreeWidgetItem(group6);
                sprintf(tempbuf,"frequency:%dHz",ts_nit[i].nit_info_table_2[j].cal_delivery[k].frequency);
                Item3->setText(0,tempbuf);
                QTreeWidgetItem *Item4 = new QTreeWidgetItem(group6);
                sprintf(tempbuf,"FEC_outer:%x",ts_nit[i].nit_info_table_2[j].cal_delivery[k].FEC_outer);
                Item4->setText(0,tempbuf);
                QTreeWidgetItem *Item5 = new QTreeWidgetItem(group6);
                sprintf(tempbuf,"modulation:%x",ts_nit[i].nit_info_table_2[j].cal_delivery[k].modulation);
                Item5->setText(0,tempbuf);
                QTreeWidgetItem *Item6 = new QTreeWidgetItem(group6);
                sprintf(tempbuf,"symbol_rate:%x",ts_nit[i].nit_info_table_2[j].cal_delivery[k].symbol_rate);
                Item6->setText(0,tempbuf);
                QTreeWidgetItem *Item7 = new QTreeWidgetItem(group6);
                sprintf(tempbuf,"FEC_inner:%x",ts_nit[i].nit_info_table_2[j].cal_delivery[k].FEC_inner);
                Item7->setText(0,tempbuf);
            }
            QTreeWidgetItem *group7 = new QTreeWidgetItem(group2);
            group7->setText(0,"unknown_descriptor");
            for(int k = 0;k < ts_nit[i].nit_info_table_2[j].un_descr.length();k++)
            {
                QTreeWidgetItem *Item1 = new QTreeWidgetItem(group7);
                sprintf(tempbuf,"descriptor_tag:%x",ts_nit[i].nit_info_table_2[j].un_descr[k].descriptor_tag);
                Item1->setText(0,tempbuf);
                QTreeWidgetItem *Item2 = new QTreeWidgetItem(group7);
                sprintf(tempbuf,"descriptor_length:%x",ts_nit[i].nit_info_table_2[j].un_descr[k].descriptor_length);
                Item2->setText(0,tempbuf);
                QTreeWidgetItem *Item3 = new QTreeWidgetItem(group7);
                Item3->setText(0,"data:");
                for(int m = 0;m < ts_nit[i].nit_info_table_2[j].un_descr[k].data.length();m++)
                {
                    QTreeWidgetItem *title = new QTreeWidgetItem(Item3);
                    sprintf(tempbuf,"%x ",ts_nit[i].nit_info_table_2[j].un_descr[k].data[m]);
                    title->setText(0,tempbuf);
                }
            }
        }
        QTreeWidgetItem *group2 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"CRC32=%x",ts_nit[i].CRC_32);
        group2->setText(0,tempbuf);
        QTreeWidgetItem *group3 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"check_CRC32=%x",ts_nit[i].check_CRC32);
        group3->setText(0,tempbuf);
    }
}
void MainWindow::init_tree()
{
    if(ts_pat_program.length() > 0)
        init_PAT_tree();
    if(ts_pmt.length() > 0)
        init_PMT_tree();
    if(ts_cat.length() > 0)
        init_CAT_tree();
    if(ts_nit.length() > 0)
        init_NIT_tree();
}
void MainWindow::adjust_PAT_table(TS_PAT *packet,unsigned char *buffer)
{
    packet->table_id                    = buffer[0];
    packet->section_syntax_indicator    = buffer[1 ] >> 7;
    packet->zero                        = buffer[1] >> 6 & 0x1;
    packet->reserved_1                  = buffer[1] >> 4 & 0x3;
    packet->section_length              = (buffer[1] & 0x0F) << 8 | buffer[2];
    packet->transport_stream_id         = buffer[3] << 8 | buffer[4];
    packet->reserved_2                  = buffer[5] >> 6;
    packet->version_number              = buffer[5] >> 1 &  0x1F;
    packet->current_next_indicator      = (buffer[5] << 7) >> 7;
    packet->section_number              = buffer[6];
    packet->last_section_number         = buffer[7];
    packet->Is_write = 0;
    unsigned int len = 0;
    len = packet->section_length + 2;
    packet->CRC_32  = (buffer[len-3] & 0x000000FF) << 24
                        | (buffer[len-2] & 0x000000FF) << 16
                        | (buffer[len-1] & 0x000000FF) << 8
                        | (buffer[len] & 0x000000FF);
    unsigned int crc = 0xffffffff;

    int n = 0;
    for ( n = 0; n < packet->section_length - 12; n += 4 )
    {
        unsigned  int program_num = buffer[8  + n] << 8 | buffer[9  + n];
        packet->reserved_3 = buffer[10  + n] >> 5;
        packet->network_PID = 0x00;
        if ( program_num == 0x00)
        {
            packet->network_PID = (buffer[10  + n] & 0x1F) << 8 | buffer[11  + n];
        }
        else
        {
           TS_PAT_Program *PAT_program = new TS_PAT_Program;
           PAT_program->program_map_PID = (buffer[10  + n] & 0x1F) << 8 | buffer[11  + n];
           PAT_program->program_number = program_num;
           ts_pat_program.push_back(*PAT_program);
           if(packet->Is_write == 0)
           {
               PMT_PID.push_back(PAT_program->program_map_PID);
           }
           delete PAT_program;
        }
    }
    crc=crc32(crc,buffer,packet->section_length-1);
    packet->check_CRC32 = crc;
    packet->Is_write = 1;
}
void MainWindow::save_basic_info(char *buffer,unsigned int number)
{
    char *path_tempbuf = new char[200];
    const char *filename = "/home/china/Basic_info";
    ofstream fp;
    sprintf(path_tempbuf,"%s/%x",filename,number);
    //cout<<path_tempbuf<<endl;
    fp.open(path_tempbuf,ios::app);
    if(!fp)
    {
        cout<<"open file error"<<endl;
        usleep(1000000);
        return ;
    }
    fp<<buffer<<endl;
    fp.close();
    delete []path_tempbuf;
}
void category_type(char *buffer,unsigned int type)
{
    switch (type)
    {
        case 0x01:
        sprintf(buffer,"MPEG-1 Video");
        break;
        case 0x02:
        sprintf(buffer,"MPEG-2 Video");
        break;
        case 0x03:
        sprintf(buffer,"MPEG-1 Audio");
        break;
        case 0x04:
        sprintf(buffer,"MPEG-2 Audio");
        break;
        case 0x1b:
        sprintf(buffer,"H264");
        break;
        case 0xea:
        sprintf(buffer,"VC-1");
        break;
        case 0x0f:
        sprintf(buffer,"MPEG-2 ACC");
        break;
        case 0x11:
        sprintf(buffer,"MPEG-4 ACC");
        break;
        case 0x06:
        sprintf(buffer,"MPEG-2 private PES");
        break;
        case 0x90:
        sprintf(buffer,"PGS");
        break;
        case 0x91:
        sprintf(buffer,"IGS");
        break;
        case 0x92:
        sprintf(buffer,"Text Subtitle");
        break;
        default:
        sprintf(buffer,"unkonwn");
        break;
    }
}
void MainWindow::adjust_PMT_table(TS_PMT *packet,unsigned char *buffer,unsigned int PID)
{
    packet->table_id                         = buffer[0];
    packet->section_syntax_indicator         = buffer[1] >> 7;
    packet->zero                             = buffer[1] >> 6 & 0x01;
    packet->reserved_1                       = buffer[1] >> 4 & 0x03;
    packet->section_length                   = (buffer[1] & 0x0F) << 8 | buffer[2];
    packet->program_number                   = buffer[3] << 8 | buffer[4];
    packet->reserved_2                       = buffer[5] >> 6;
    packet->version_number                   = buffer[5] >> 1 & 0x1F;
    packet->current_next_indicator           = (buffer[5] << 7) >> 7;
    packet->section_number                   = buffer[6];
    packet->last_section_number              = buffer[7];
    packet->reserved_3                       = buffer[8] >> 5;
    packet->PCR_PID                          = ((buffer[8] << 8) | buffer[9]) & 0x1FFF;
    packet->reserved_4                       = buffer[10] >> 4;
    packet->program_info_length              = (buffer[10] & 0x0F) << 8 | buffer[11];

    if(packet->table_id != 0x02 && flag_get_pmt != 0)
    {
        get_all_PMT_buffer(packet,(unsigned char *)buffer,flag_get_pmt,PID);
        return ;
    }
    if(packet->section_length > 180 && packet->table_id == 0x02 && check_flag_pmt == 1)
    {
        int i = 0;
        flag_get_pmt = packet->section_length + 4;
        //printf("get 1\n");
        long_buf_pmt = new char[packet->section_length + 4];
        for(i = 0;i < 183;i++)
        {
            //printf("current_length=%x\n",length);
            long_buf_pmt[length_pmt++]=buffer[i];
        }
        return ;
    }
    else
    {
        if(packet->table_id != 0x02)
            return ;
    }
    unsigned int len = 0;
    len = packet->section_length + 2;
    packet->CRC_32  = (buffer[len-3] & 0x000000FF) << 24
                        | (buffer[len-2] & 0x000000FF) << 16
                        | (buffer[len-1] & 0x000000FF) << 8
                        | (buffer[len] & 0x000000FF);
    unsigned int crc = 0xffffffff;
    int pos = 12;
    basic_information *b_info = new basic_information;
    b_info->program_PID = PID;
    b_info->service_id = packet->program_number;
    if ( packet->program_info_length != 0 )
        pos += packet->program_info_length;
    for ( ; pos <= (packet->section_length + 2 ) -  4; )
    {
        TS_PMT_Stream *pmt_stream = new TS_PMT_Stream;
        pmt_stream->stream_type =  buffer[pos];
        packet->reserved_5  =  buffer[pos+1] >> 5;
        pmt_stream->elementary_PID = ((buffer[pos+1] << 8) | buffer[pos + 2]) & 0x1FFF;
        packet->reserved_6    =  buffer[pos + 3] >> 4;
        pmt_stream->ES_info_length =  (buffer[pos + 3] & 0x0F) << 8 | buffer[pos + 4];
        category_type(tempbuf,pmt_stream->stream_type);
        pmt_stream->type_descriptor = QString(QLatin1String(tempbuf));
        stream_info *s_info = new stream_info;
        s_info->PID = pmt_stream->elementary_PID;
        s_info->type = QString(tempbuf);
        b_info->stream_information.push_back(*s_info);
        delete s_info;
        pmt_stream->descriptor = 0x00;
        if (pmt_stream->ES_info_length != 0)
        {
           pmt_stream->descriptor = buffer[pos + 5];
           for( int len = 2; len <= pmt_stream->ES_info_length; len ++ )
           {
             pmt_stream->descriptor = pmt_stream->descriptor<< 8 | buffer[pos + 4 + len];
           }
           pos += pmt_stream->ES_info_length;
        }
        pos += 5;
        packet->ts_pmt_stream.push_back(*pmt_stream);
        delete pmt_stream;
    }
    for(int i = 0;i < ts_pmt.length();i++)
    {
        if(ts_pmt[i].program_number == packet->program_number)
            return ;
    }
    packet->PMT_conut += 1;
    b_info->PCR_id = packet->PCR_PID;
    b.PID_tree.push_back(*b_info);
    delete b_info;
    crc=crc32(crc,buffer,packet->section_length-1);
    packet->check_CRC32 = crc;
    ts_pmt.push_back(*packet);
    check_flag_pmt = 0;
}

void MainWindow::adjust_CAT_table(TS_CAT *packet,unsigned char *buffer)
{
    packet->table_id                    = buffer[0];
    packet->section_syntax_indicator    = buffer[1] >> 7;
    packet->zero                        = buffer[1] >> 6 & 0x1;
    packet->reserved_1                  = buffer[1] >> 4 & 0x3;
    packet->section_length              = (buffer[1] & 0x0F) << 8 | buffer[2];
    packet->reserved_2                  = (buffer[3] << 10) | (buffer[4] << 2) | (buffer[5] & 0xc0);
    packet->version_number              = (buffer[5] >> 1) & 0x1F;
    packet->current_next_indicator   	= buffer[5] & 0x01;
    packet->section_number              = buffer[6];
    packet->last_section_number 		= buffer[7];
    packet->Is_write = 1;
    unsigned int len = 0;
    len = packet->section_length + 2;
    packet->CRC_32  = (buffer[len-3] & 0x000000FF) << 24
                        | (buffer[len-2] & 0x000000FF) << 16
                        | (buffer[len-1] & 0x000000FF) << 8
                        | (buffer[len] & 0x000000FF);
    for(int k = 0;k < packet->section_length - 9;)
    {
        TS_CAT_info *ts_cat_info = new TS_CAT_info;
        ts_cat_info->descriptor_tag 		= buffer[8];
        ts_cat_info->descriptor_length 		= buffer[9];
        ts_cat_info->CA_system_ID 			= buffer[10] << 8 | buffer[11];
        ts_cat_info->reserved_3 			= buffer[12] >> 5;
        ts_cat_info->CA_PID 				= (buffer[12] & 0x1F) << 8 | buffer[13];
        int i = 0;
        for(i = 0;i < ts_cat_info->descriptor_length - 4;i++)
        {

        }
        k += ts_cat_info->descriptor_length + 1;
    }
    unsigned int crc = 0xffffffff;
    crc=crc32(crc,buffer,packet->section_length-1);
    packet->check_CRC_32 = crc;
    ts_cat.push_back(*packet);
}
void MainWindow::adjust_NIT_table(TS_NIT *packet, unsigned char *buffer)
{
    packet->table_id						= buffer[0];
    packet->section_syntax_indicator 		= buffer[1] >> 7;
    packet->reserved_future_use_1			= buffer[1] >> 6 & 0x01;
    packet->reserved_1 						= buffer[1] >> 4 & 0x03;
    packet->section_length 					= (buffer[1] & 0x0F) << 8 | buffer[2];
    packet->network_id 						= buffer[3] << 8 | buffer[4];
    packet->reserved_2 						= buffer[5] >> 6;
    packet->version_number					= buffer[5] >> 1 & 0x1F;
    packet->current_next_indicator 			= (buffer[5] << 7) >> 7;
    packet->section_number 					= buffer[6];
    packet->last_section_number				= buffer[7];
    packet->reserved_future_use_2 			= buffer[8] >> 4;
    packet->network_descriptors_length 		= (buffer[8] & 0x0F) << 8 | buffer[9];
    packet->nit_info_table_1.clear();
    packet->nit_info_table_2.clear();
    if(packet->table_id != 0x40 && flag_get_nit != 0)
    {
        get_all_NIT_buffer(packet,buffer,flag_get_nit);
        return ;
    }
    if(packet->section_length > 180 && packet->table_id == 0x40 && check_flag_nit == 1)
    {
        int i = 0;
        flag_get_nit = packet->section_length + 4;
        long_buf_nit = new char[packet->section_length + 4];
        for(i = 0;i < 183;i++)
        {
            long_buf_nit[length_nit++]=buffer[i];
        }
        return ;
    }
    else
    {
        if(packet->table_id != 0x40)
            return ;
    }
    int len = 0;
    len = packet->section_length + 2;
    packet->CRC_32  = (buffer[len-3] & 0x000000FF) << 24
                    | (buffer[len-2] & 0x000000FF) << 16
                    | (buffer[len-1] & 0x000000FF) << 8
                    | (buffer[len] & 0x000000FF);
    int pos = 10;
    int k = 0;
    unsigned int k1 = 0;
    int j =0;
    if ( packet->network_descriptors_length != 0 )
    {
        for(k = pos;k <= packet->network_descriptors_length;)
        {
            TS_NIT_info_1 nit_info_table;
            nit_info_table.descriptor_tag = buffer[k];
            nit_info_table.descriptor_length = buffer[k + 1];
            if (nit_info_table.descriptor_length != 0)
            {
                for(k1 = 0; k1 < nit_info_table.descriptor_length ; k1++)
                {
                    tempbuf[k1] = buffer[k + 2 + k1];
                }
                tempbuf[k1] ='\0';
                nit_info_table.descriptor_char = QString(QLatin1String(tempbuf));
                k1 = 0;
            }
            k += nit_info_table.descriptor_length + 2;
            packet->nit_info_table_1.push_back(nit_info_table);
        }
        pos = k;

    }
    packet->reserved_future_use_3 			= buffer[pos] >> 4;
    packet->transport_stream_loop_length	= (buffer[pos] & 0XF) << 8 | buffer[pos + 1];
    if (packet->transport_stream_loop_length != 0)
    {
        for(k = pos + 2;k <= packet->transport_stream_loop_length;)
        {
            TS_NIT_info_2 nit_info_table_2;
            nit_info_table_2.transport_stream_id = buffer[k] << 8 | buffer[k + 1];
            nit_info_table_2.original_network_id = buffer[k + 2] << 8 | buffer[k + 3];
            unsigned int transport_descriptors_length = (buffer[k + 4] & 0X0F) << 8 | buffer[k + 5];
            nit_info_table_2.transport_descriptors_length = transport_descriptors_length;
            for( k1 = 0;k1 < transport_descriptors_length ;)
            {
                unsigned char descriptor_tag;
                unsigned char descriptor_length;
                descriptor_tag 			 = buffer[k + 6 + k1];
                descriptor_length 		 = buffer[k + 7 + k1];
                if(descriptor_tag == 0x41)
                {
                    service_list_descriptor service_list;
                    service_list.descriptor_tag = descriptor_tag;
                    service_list.descriptor_length = descriptor_length;
                    for(j = 0;j < descriptor_length ;j+=3)
                    {
                        service_info ser_info;
                        ser_info.service_id = buffer[k + 8 + k1 + j] << 8 | buffer[k + 9 + k1 + j];
                        ser_info.service_type = buffer[k + 10 + k1 + j];
                        service_list.nit_service.push_back(ser_info);
                    }
                    nit_info_table_2.service_list.push_back(service_list);
                }
                else if(descriptor_tag == 0x5f)
                {
                    private_data_specifier_descriptor private_data;
                    private_data.descriptor_tag = descriptor_tag;
                    private_data.descriptor_length = descriptor_length;
                    private_data.private_data_specifier = buffer[k + 8 + k1 ] << 24 |
                                                          buffer[k + 9 + k1 ] << 16 |
                                                          buffer[k + 10 + k1] << 8  |
                                                          buffer[k + 11 + k1];
                    nit_info_table_2.private_data.push_back(private_data);
                }
                else if(descriptor_tag == 0x5a)
                {
                    terrestrial_delivery_system_descriptor t_delivery;
                    t_delivery.descriptor_tag = descriptor_tag;
                    t_delivery.descriptor_length = descriptor_length;
                    t_delivery.centre_frequency = buffer[k + 8 + k1 ] << 24 |
                                                  buffer[k + 9 + k1 ] << 16 |
                                                  buffer[k + 10 + k1] << 8  |
                                                  buffer[k + 11 + k1];
                    t_delivery.bandwidth = buffer[k + 12 + k1] >> 5;
                    t_delivery.constellation = buffer[k + 13 + k1] >> 6;
                    t_delivery.hierarchy_information = buffer[k + 13 + k1] & 0x38;
                    t_delivery.code_rate_HP_stream = buffer[k + 13 + k1] & 0x07;
                    t_delivery.code_rate_LP_stream = buffer[k + 14 + k1] & 0xe0;
                    t_delivery.guard_interval = buffer[k + 14 + k1] & 0x18;
                    t_delivery.transmission_mode = buffer[k + 14 + k1] & 0x06;
                    t_delivery.other_frequency_flag = buffer[k + 14 + k1] & 0x01;
                    nit_info_table_2.ter_delivery.push_back(t_delivery);
                }
                else if(descriptor_tag == 0x44)
                {
                    cable_delivery_system_descriptor c_delivery;
                    c_delivery.descriptor_tag = descriptor_tag;
                    c_delivery.descriptor_length = descriptor_length;
                    c_delivery.frequency = (buffer[k + 8 + k1 ] << 24 |
                                            buffer[k + 9 + k1 ] << 16 |
                                            buffer[k + 10 + k1] << 8  |
                                            buffer[k + 11 + k1]);
                    c_delivery.FEC_outer = buffer[k + 13 + k1] & 0x0F;
                    c_delivery.modulation = buffer[k + 14 + k1];
                    c_delivery.symbol_rate = buffer[k + 15 + k1] << 20
                                             | buffer[k + 16 + k1] << 12
                                             | buffer[k + 17 + k1] << 4
                                             | buffer[k + 18 + k1] >> 4;
                    c_delivery.FEC_inner = buffer[k + 18 + k1] & 0x0F;
                    nit_info_table_2.cal_delivery.push_back(c_delivery);
                }
                else if(descriptor_tag == 0x83)
                {
                    unknown_descriptor un_descr;
                    un_descr.descriptor_tag = descriptor_tag;
                    un_descr.descriptor_length = descriptor_length;
                    for(j = 0;j < descriptor_length;j++)
                        un_descr.data.push_back(buffer[k + 8 + k1 + j]);
                    nit_info_table_2.un_descr.push_back(un_descr);
                }
                k1 += (descriptor_length + 2);
            }
            k += (transport_descriptors_length + 6);
        packet->nit_info_table_2.push_back(nit_info_table_2);
        }
    }
    check_flag_nit = 0;
    unsigned int crc = 0xffffffff;
    crc = crc32(crc,buffer,packet->section_length - 1);
    packet->check_CRC32 = crc;
    packet->NIT_conut += 1;
    ts_nit.push_back(*packet);
    if(packet->NIT_conut == packet->last_section_number + 1)
        packet->Is_write = 1;
}
int MainWindow::mjdtod(int mjd)
{
    int y1,m1;
    y1=(int)(((float)mjd-15078.2)/365.25);
    m1=(int)(((float)mjd-14956.1-(int)((float)y1*365.25))/30.6001);
    return(mjd-14956-(int)((float)y1*365.25)-(int)((float)m1*30.6001));
}

int MainWindow::mjdtom(int mjd)
{
    int y1,m1,k;
    y1=(int)(((float)mjd-15078.2)/365.25);
    m1=(int)(((float)mjd-14956.1-(int)((float)y1*365.25))/30.6001);
    k=0; if((y1==14)||(y1==15)) k=1; return(m1-1-k*12);
}

int MainWindow::mjdtoy(int mjd)
{
    int y1,k;
    y1=(int)(((float)mjd-15078.2)/365.25);
    //m1=(int)(((float)mjd-14956.1-(int)((float)y1*365.25))/30.6001);
    k=0;
    if((y1==14)||(y1==15))
        k=1;
    return(y1+k);
}

void MainWindow::adjust_TDT_table(TS_TDT *packet,unsigned char *buffer)
{
    packet->table_id 					= buffer[0];
    packet->section_syntax_indicator	= buffer[1] >> 7;
    packet->reserved_future_use 		= (buffer[1] >> 6) & 0x01;
    packet->reserved 					= (buffer[1] >> 4) & 0x03;
    packet->section_length 				= (buffer[1] & 0x0F) << 8 | buffer[2];
    unsigned int MJD = (buffer[3] << 8) | buffer[4];
    unsigned int Y = mjdtoy(MJD);
    unsigned int M = mjdtom(MJD);
    unsigned int D = mjdtod(MJD);
    packet->UTC_time.Year = Y + 1900;
    packet->UTC_time.Mouth = M;
    packet->UTC_time.Day = D;
    packet->UTC_time.Hour = (buffer[5] >> 4)*10 + (buffer[5] & 0x0f);
    packet->UTC_time.Min = (buffer[6] >> 4)*10 + (buffer[6] & 0x0f);
    packet->UTC_time.Sec = (buffer[7] >> 4)*10 + (buffer[7] & 0x0f);
    d.ts_tdt.push_back(*packet);
}
void MainWindow::adjust_TOT_table(TS_TOT *packet,unsigned char *buffer)
{
    packet->table_id 					= buffer[0];
    if(packet->table_id != 0x73)
    {
        //perror("table_id error");
        return ;
    }
    packet->section_syntax_indicator	= buffer[1] >> 7;
    packet->reserved_future_use 		= (buffer[1] >> 6) & 0x01;
    packet->reserved_1 					= (buffer[1] >> 4) & 0x03;
    packet->section_length 				= (buffer[1] & 0x0F) << 8 | buffer[2];
    unsigned int MJD = buffer[3] << 8 | buffer[4];
    unsigned int Y = mjdtoy(MJD);
    unsigned int M = mjdtom(MJD);
    unsigned int D = mjdtod(MJD);
    offset_time.Year = Y+1900;
    offset_time.Mouth= M;
    offset_time.Day  = D;
    offset_time.Hour = (buffer[5] >> 4)*10 + (buffer[5] & 0x0F);
    offset_time.Min  = (buffer[6] >> 4)*10 + (buffer[6] & 0x0F);
    offset_time.Sec  = (buffer[7] >> 4)*10 + (buffer[7] & 0x0F);
    packet->UTC_time.Year = Y + 1900;
    packet->UTC_time.Mouth = M;
    packet->UTC_time.Day = D;
    packet->UTC_time.Hour = (buffer[5] >> 4)*10 + (buffer[5] & 0x0F);
    packet->UTC_time.Min = (buffer[6] >> 4)*10 + (buffer[6] & 0x0F);
    packet->UTC_time.Sec = (buffer[7] >> 4)*10 + (buffer[7] & 0x0F);
    packet->reserved_2  				= buffer[8] >> 4;
    packet->descriptor_loop_length 		= (buffer[8] & 0x0F) << 8 | buffer[9];
    int pos = 10;
    int i = 0,j = 0;
    for( i = 0;i < packet->descriptor_loop_length;)
    {
        local_time_offset_descriptor local_des;
        local_des.descriptor_tag = buffer[pos + i];
        local_des.descriptor_length = buffer[pos + i + 1];
        for( j = 0;j < buffer[pos + i + 1];)
        {
            time_descriptor time_des;
            time_des.country_code = buffer[pos + i + j + 2] << 16 |
                                    buffer[pos + i + j + 3] << 8  |
                                    buffer[pos + i + j + 4];
            time_des.country_region_id = buffer[pos + i + j + 5] >> 2;
            time_des.local_time_offset_polarity = buffer[pos + i + j + 5] & 0x01;
            time_des.local_time_offset = buffer[pos + i + j + 6] << 8 |
                                         buffer[pos + i + j + 7] ;

            unsigned char offset_time_hour = (buffer[pos + i + j + 6] >> 4)*10
                                           + (buffer[pos + i + j + 6] & 0x0F);
            unsigned char offset_time_min  = (buffer[pos + i + j + 7] >> 4)*10
                                           + (buffer[pos + i + j + 7] & 0x0F);
            if(time_des.local_time_offset_polarity == 1)
            {
                offset_time.Hour += offset_time_hour;
                offset_time.Min  += offset_time_min;
            }
            else
            {
                offset_time.Hour -= offset_time_hour;
                offset_time.Min  -= offset_time_min;
            }
            unsigned int MJD = buffer[pos + i + j + 8] << 8 | buffer[pos + i + j + 9];
            unsigned int Y = mjdtoy(MJD);
            unsigned int M = mjdtom(MJD);
            unsigned int D = mjdtod(MJD);
            time_des.time_of_change.Year = Y + 1900;
            time_des.time_of_change.Mouth = M;
            time_des.time_of_change.Day = D;
            time_des.time_of_change.Hour = (buffer[pos + i + j + 10] >> 4)*10
                                          +(buffer[pos + i + j + 10] & 0x0f);
            time_des.time_of_change.Min  = (buffer[pos + i + j + 11] >> 4)*10
                                          +(buffer[pos + i + j + 11] & 0x0f);
            time_des.time_of_change.Sec  = (buffer[pos + i + j + 12] >> 4)*10
                                          +(buffer[pos + i + j + 12] & 0x0f);
            time_des.next_time_offset =  buffer[pos + i + j + 13] << 8 |
                                            buffer[pos + i + j + 14];
            j+=13;
            local_des.time_desc.push_back(time_des);
        }
        i+=(j+2);
        packet->local_desc.push_back(local_des);
    }
    d.ts_tot.push_back(*packet);
    packet->Is_write = 1;
}
int MainWindow::Is_Right_time(Time time_current,Time time_offset,unsigned int duration)
{
    int div = 0;
    if(time_offset.Year>=time_current.Year &&
       time_offset.Mouth>=time_current.Mouth &&
       time_offset.Day>=time_current.Day)
    {
        if(time_offset.Hour >= time_current.Hour)
        {
            div += 3600*(time_offset.Hour - time_current.Hour);
            if(time_offset.Min >= time_current.Min)
            {
                div += 60*(time_offset.Min - time_current.Min);
            }
            else
            {
                div -= 60*(time_current.Min - time_offset.Min);
            }
            if(div <= duration && div >=0)
                return 1;
            else
                return 0;
        }
        else
            return 0;
    }
    else
        return 0;
}
void MainWindow::save_to_file(char *buffer, int position)
{
    char *path_tempbuf = new char[200];
    const char *filename = "/home/china/EIT_info";
    ofstream fp;
    sprintf(path_tempbuf,"%s/%d",filename,position);
    //cout<<path_tempbuf<<endl;
    fp.open(path_tempbuf,ios::app);
    if(!fp)
    {
        cout<<"open file error"<<endl;
        usleep(1000000);
        return ;
    }
    fp<<buffer<<endl;
    fp.close();
    delete []path_tempbuf;
}
void MainWindow::save_time_file(char *buffer)
{
    const char *filename = "/home/china/Time_info/Time";
    ofstream fp;
    fp.open(filename,ios::app);
    if(!fp)
    {
        cout<<"open file error"<<endl;
        usleep(1000000);
        return ;
    }
    fp<<buffer<<endl;
    fp.close();
}
void MainWindow::save_epg_file(char *buffer, unsigned int id)
{
    const char *filename = "/home/china/EPG_info";
    char *path_tempbuf = new char[100];
    sprintf(path_tempbuf,"%s/%d",filename,id);
    ofstream fp;
    fp.open(path_tempbuf,ios::app);
    if(!fp)
    {
        cout<<"open file error"<<endl;
        usleep(1000000);
        return ;
    }
    fp<<buffer<<endl;
    fp.close();
}
void MainWindow::category_choice(unsigned char level_1, unsigned char level_2,char *buf_category)
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
void MainWindow::adjust_EIT_table(TS_EIT *packet,unsigned char *buffer)
{
    int is_eit_flag = 0;
    packet->table_id                    = buffer[0];
    if(packet->table_id == 0x4E || packet->table_id == 0x4F ||
       ((packet->table_id >= 0x50) && (packet->table_id <= 0x5F)) ||
       ((packet->table_id >= 0x60) && (packet->table_id <= 0x6F)))
    {
        //printf("Is EIT\n");
        is_eit_flag=1;
    }
    else
    {
        if(flag_get_eit == 0)
            return ;
    }
     packet->section_syntax_indicator    = buffer[1] >> 7;
     packet->reserved_future_use_1       = buffer[1] >> 6 & 0x1;
     packet->reserved_1                  = buffer[1] >> 4 & 0x3;
     packet->section_length              = ((buffer[1] & 0x0F) << 8) | buffer[2];
     packet->service_id 				 = buffer[3] << 8 | buffer[4];
     packet->reserved_2 				 = buffer[5] >> 6;
     packet->version_number 			 = (buffer[5] & 0x3e) >> 1;
     packet->current_next_indicator 	 = buffer[5] & 0x01;
     packet->section_number 			 = buffer[6];
     packet->last_section_number 		 = buffer[7];
     packet->transport_stream_id 		 = buffer[8] << 8 | buffer[9];
     packet->original_network_id 		 = buffer[10] << 8 | buffer[11];
     packet->segment_last_section_number = buffer[12];
     packet->last_table_id 				 = buffer[13];
     if(flag_get_eit != 0)
     {
        get_all_EIT_buffer(packet,buffer,flag_get_eit);
        return ;
     }
     if(packet->section_length > 180 && check_flag_eit == 1 && is_eit_flag == 1)
     {
        int i = 0;
        flag_get_eit = packet->section_length + 4;
        long_buf_eit = new char[packet->section_length + 4];
        for(i = 0;i < 183;i++)
        {
            long_buf_eit[length_eit++]=buffer[i];
        }
        return ;
    }
    else
    {
        if(is_eit_flag != 1)
            return ;
    }
    EIT_number temp_eit_number;
    temp_eit_number.section_number = packet->section_number;
    temp_eit_number.version_number = packet->version_number;
    temp_eit_number.last_section_number = packet->last_section_number;
    temp_eit_number.section_length = packet->section_length;
    temp_eit_number.service_id = packet->service_id;
    for(QVector<EIT_number>::iterator it = eit_num.begin(); it != eit_num.end();it++)
    {
        if(it->section_number == temp_eit_number.section_number &&
           it->version_number == temp_eit_number.version_number &&
           it->last_section_number == temp_eit_number.last_section_number &&
           it->section_length == temp_eit_number.section_length &&
           it->service_id == temp_eit_number.service_id)
        {
            return ;
        }
    }
    eit_num.push_back(temp_eit_number);
    //e.service_id.push_back(packet->service_id);


    int len =packet->section_length + 2;
    packet->CRC_32  = (buffer[len-3] & 0x000000FF) << 24
                    | (buffer[len-2] & 0x000000FF) << 16
                    | (buffer[len-1] & 0x000000FF) << 8
                    | (buffer[len] & 0x000000FF);
    unsigned int crc = 0xffffffff;
    crc = crc32(crc,buffer,packet->section_length-1);
    packet->check_CRC32 = crc;
    unsigned int pos = 14;
    unsigned int k = 0;
    unsigned int k1 = 0;
    unsigned int i = 0,j = 0,m = 0,l = 0;
    for(k = 0;k < packet->section_length - 15;)
    {
        EIT_LOOP temp_loop;
        temp_loop.event_id                  = (buffer[pos + k] << 8) | (buffer[pos + k + 1]);
        temp_loop.MJD                       = buffer[pos + k + 2] << 8 | buffer[pos + k + 3];
        temp_loop.Hour                      = buffer[pos + k + 4];
        temp_loop.Min                       = buffer[pos + k + 5];
        temp_loop.Sec                       = buffer[pos + k + 6];
        temp_loop.duration_hour             = (buffer[pos + k + 7] >> 4)*10 + (buffer[pos + k + 7] & 0xf);
        temp_loop.duration_min              = (buffer[pos + k + 8] >> 4)*10 + (buffer[pos + k + 8] & 0xf);
        temp_loop.duration_sec              = (buffer[pos + k + 9] >> 4)*10 + (buffer[pos + k + 9] & 0xf);
        temp_loop.running_status            = buffer[pos + 10 + k] >> 5;
        temp_loop.free_CA_mode              = buffer[pos + k + 10] & 0x10;
        temp_loop.descriptors_loop_length   = (buffer[pos + 10 + k] & 0x0F) << 8 | buffer[pos + 11 + k];
        unsigned int Y = mjdtoy(temp_loop.MJD);
        unsigned int M = mjdtom(temp_loop.MJD);
        unsigned int D = mjdtod(temp_loop.MJD);
        unsigned int Hour = (temp_loop.Hour >> 4)*10 + (temp_loop.Hour & 0x0f);
        unsigned int Min  = (temp_loop.Min >> 4)*10 + (temp_loop.Min & 0x0f);
        unsigned int Sec  = (temp_loop.Sec >> 4)*10 + (temp_loop.Sec & 0x0f);
        temp_loop.UTC_time.Year = Y + 1900;
        temp_loop.UTC_time.Mouth = M;
        temp_loop.UTC_time.Day = D;
        temp_loop.UTC_time.Hour = Hour;
        temp_loop.UTC_time.Min = Min;
        temp_loop.UTC_time.Sec = Sec;
        Time server_time;
        server_time.Year = Y+1900;
        server_time.Mouth = M;
        server_time.Day = D;
        server_time.Hour = Hour;
        server_time.Min = Min;
        server_time.Sec = Sec;
//        sprintf(tempbuf,"start_time=%d/%d/%d,",1900 + Y,M,D);
//        save_to_file(tempbuf,dest);
//        save_epg_file(tempbuf,packet->service_id);
//        sprintf(tempbuf,"%d%d:%d%d:%d%d\n",temp_loop.Hour >> 4,
//                                  temp_loop.Hour & 0x0f,
//                                  temp_loop.Min >> 4,
//                                  temp_loop.Min & 0x0f,
//                                  temp_loop.Sec >> 4,
//                                  temp_loop.Sec & 0x0f);
//        save_to_file(tempbuf,dest);
//        save_epg_file(tempbuf,packet->service_id);
        Hour = (temp_loop.duration_hour >> 4)*10 + (temp_loop.duration_hour & 0x0f);
        Min  = (temp_loop.duration_min >> 4)*10 + (temp_loop.duration_min & 0x0f);
        Sec  = (temp_loop.duration_sec >> 4)*10 + (temp_loop.duration_sec & 0x0f);
        unsigned int duration = Hour*3600+Min*60+Sec;
        int dispaly_flag = 0;
        dispaly_flag = Is_Right_time(server_time,offset_time,duration);
        if(dispaly_flag)
        {

        }
        for(k1 = 0;k1 < temp_loop.descriptors_loop_length;)
        {
            unsigned char descriptor_tag = buffer[pos + k + k1 + 12];
            unsigned char descriptor_length = buffer[pos + k + k1 + 13];
            if(descriptor_tag == 0x4d)
            {
                short_event_descriptor short_eve_des;
                short_eve_des.descriptor_tag = descriptor_tag;
                short_eve_des.descriptor_length = descriptor_length;
                short_eve_des.ISO_639_language_code = (buffer[pos + k + 14 + k1] << 16) |
                                                      (buffer[pos + k + 15 + k1] << 8)  |
                                                      (buffer[pos + k + 16 + k1]);
                short_eve_des.event_name_length = buffer[pos + k + k1 + 17];
                for(i = 0;i < short_eve_des.event_name_length;i++)
                    tempbuf[i] = buffer[pos + k + k1 + 18 + i];
                tempbuf[i]='\0';
                short_eve_des.event_name = QString(QLatin1String(tempbuf));
                short_eve_des.text_length = buffer[pos + k + k1 + 19 + i - 1];
                for(j = 0;j < short_eve_des.text_length;j++)
                    tempbuf[j] = buffer[pos + k + k1 + 20 + i - 1 + j];
                tempbuf[i] = '\0';
                short_eve_des.text_char = QString(QLatin1String(tempbuf));
                temp_loop.short_eve.push_back(short_eve_des);
            }
            else if(descriptor_tag == 0x54)
            {
                content_descriptor content_des;
                content_des.descriptor_tag = descriptor_tag;
                content_des.descriptor_length = descriptor_length;
                for(i = 0;i < content_des.descriptor_length;i += 2 )
                {
                    category_info cate_info;
                    cate_info.content_nibble_level_1 = buffer[pos + k + 14 + k1 + i] >> 4;
                    cate_info.content_nibble_level_2 = buffer[pos + k + 14 + k1 + i] & 0x0F;
                    //category_choice(cate_info.content_nibble_level_1,cate_info.content_nibble_level_2,tempbuf);
                    cate_info.user_byte = buffer[pos + k + 15 + k1 + i];
                    content_des.cat_info.push_back(cate_info);
                }
                temp_loop.content_des.push_back(content_des);
            }
            else if(descriptor_tag == 0x50)
            {
                component_descriptor com_des;
                com_des.descriptor_tag = descriptor_tag;
                com_des.descriptor_length = descriptor_length;
                com_des.stream_content = buffer[pos + k + 14 + k1] & 0x0F;
                com_des.component_type = buffer[pos + k + 15 + k1];
                com_des.component_tag = buffer[pos + k + 16 + k1];
                com_des.ISO_639_language_code = buffer[pos + k + 17 + k1] << 16 |
                                                buffer[pos + k + 18 + k1] << 8  |
                                                buffer[pos + k + 19 + k1];
                for(i = 0; i < descriptor_length - 6;i++)
                    tempbuf[i] = buffer[pos + k + 20 + k1 + i];
                tempbuf[i]='\0';
                com_des.text_char = QString( QLatin1String(tempbuf));
                temp_loop.componet_des.push_back(com_des);
            }
            else if(descriptor_tag == 0x4e)
            {
                extended_event_descriptor extend_des;
                extend_des.descriptor_tag = descriptor_tag;
                extend_des.descriptor_length = descriptor_length;
                extend_des.descriptor_number = buffer[pos + k + 14 + k1] >> 4;
                extend_des.last_descriptor_number = buffer[pos + k + 14 + k1] & 0x0f;
                extend_des.ISO_639_language_code = buffer[pos + k + 15 + k1] << 16 |
                                                   buffer[pos + k + 16 + k1] << 8  |
                                                   buffer[pos + k + 17 + k1];
                extend_des.length_of_items = buffer[pos + k + 18 + k1];
                if(extend_des.length_of_items == 0)
                {
                    extend_des.text_length = buffer[pos + k + 19 + k1];
                    for(l = 0;l < buffer[pos + k + 19 + k1];l++)
                        tempbuf[l] = buffer[pos + k + 20 + k1 + l];
                    tempbuf[l]='\0';
                    extend_des.text_char = QString( QLatin1String(tempbuf));
                }
                else
                {
                    for(i = 0; i < extend_des.length_of_items ;i+=(j+m+2))
                    {
                        ITEM item;
                        item.item_description_length = buffer[pos + k + 19 + k1 + i];
                        for(j = 0;j <  item.item_description_length;j++)
                            tempbuf[j] = buffer[pos + k + 20 + k1 + i + j];
                        tempbuf[j] = '\0';
                        item.item_description_char = QString(QLatin1String(tempbuf));
                        item.item_length = buffer[pos + k + 21 + k1 + i + j -1];
                        for(m = 0;m < item.item_length ;m++)
                            tempbuf[m] = buffer[pos + k + 22 + k1 + i + j -1 + m];
                        tempbuf[m]='\0';
                        item.item_char = QString(QLatin1String(tempbuf));
                        extend_des.item.push_back(item);
                    }
                    extend_des.text_length= buffer[pos + k + 19 + k1 + i];
                    for(l = 0;l < buffer[pos + k + 19 + k1 + i];l++)
                        tempbuf[l] = buffer[pos + k + 20 + k1 + i  + l];
                    tempbuf[l] = '\0';
                    extend_des.text_char = QString(QLatin1String(tempbuf));
                }
                temp_loop.extended_eve_des.push_back(extend_des);
            }
            else if(descriptor_tag == 0x55)
            {
                parental_rating_descriptor par_rat;
                par_rat.descriptor_tag = descriptor_tag;
                par_rat.descriptor_length = descriptor_length;
                for(i = 0;i < par_rat.descriptor_length;i += 4)
                {
                    ctrl_information ctrl_info;
                    ctrl_info.country_code = buffer[pos + k + 14 + k1 + i] << 16 |
                                             buffer[pos + k + 15 + k1 + i] << 8  |
                                             buffer[pos + k + 16 + k1 + i];
                    ctrl_info.rating = buffer[pos + k + 17 + k1 + i];
                    par_rat.ctrl_info.push_back(ctrl_info);
                }
                temp_loop.parental_rat_des.push_back(par_rat);
            }
            k1 += (descriptor_length + 2);
        }
        k += temp_loop.descriptors_loop_length + 12;
        packet->eit_loop.push_back(temp_loop);
     }
    if(packet->table_id == 0x4e)
        d.ts_eit_pf_actual.push_back(*packet);
    else if(packet->table_id == 0x4f)
        d.ts_eit_pf_other.push_back(*packet);
    else if(packet->table_id >= 0x50 && packet->table_id <= 0x5f)
        d.ts_eit_schedule_actual.push_back(*packet);
    else if(packet->table_id >= 0x60 && packet->table_id <= 0x6f)
        d.ts_eit_schedule_other.push_back(*packet);
    packet->eit_loop.clear();
    check_flag_eit = 0;
}
void MainWindow::adjust_SDT_table(TS_SDT *packet, unsigned char *buffer)
{
    packet->table_id                    = buffer[0];
    packet->section_syntax_indicator    = buffer[1] >> 7;
    packet->reserved_future_use1        = buffer[1] >> 6 & 0x1;
    packet->reserved_1                  = buffer[1] >> 4 & 0x3;
    packet->section_length              = (buffer[1] & 0x0F) << 8 | buffer[2];
    packet->transport_stream_id         = buffer[3] << 8 | buffer[4];
    packet->reserved_2 				 	= buffer[5] >> 6;
    packet->version_number 			 	= (buffer[5] >> 1) & 0x1f;
    packet->current_next_indicator   	= buffer[5] & 0x01;
    packet->section_number 			 	= buffer[6];
    packet->last_section_number 		= buffer[7];
    packet->original_network_id 		= buffer[8] << 8 | buffer[9];
    packet->reserved_future_use2 		= buffer[10];
    if(packet->table_id != 0x42 && flag_get_sdt != 0)
    {
        get_all_SDT_buffer(packet,buffer,flag_get_sdt);
        return ;
    }
    if(packet->section_length > 180 && packet->table_id == 0x42 && check_flag_sdt == 1)
    {
        int i = 0;
        flag_get_sdt = packet->section_length + 4;
        long_buf_sdt = new char[packet->section_length + 4];
        for(i = 0;i < 183;i++)
        {
            long_buf_sdt[length_sdt++]=buffer[i];
        }
        return ;
    }
    else
    {
        if(packet->table_id != 0x42)
            return ;
    }
    int len =packet->section_length + 2;
    packet->CRC_32  = (buffer[len-3] & 0x000000FF) << 24
                        | (buffer[len-2] & 0x000000FF) << 16
                        | (buffer[len-1] & 0x000000FF) << 8
                        | (buffer[len] & 0x000000FF);
    unsigned int crc = 0xffffffff;
    crc = crc32(crc,buffer,packet->section_length-1);
    unsigned int pos = 11;
    unsigned int k = 0,k1 = 0;
    unsigned int i = 0,j = 0;
    for(k = 0;k < packet->section_length - 12;)
    {
        bussiness_descriptor buss_desc;
        buss_desc.service_id = buffer[pos + k] << 8 | buffer[pos + 1 + k];
        buss_desc.EIT_schedule_flag = buffer[pos + 2 + k] & 0x02;
        buss_desc.EIT_presnet_following_flag = buffer[pos + 2 + k] & 0x01;
        buss_desc.running_status = buffer[pos + 3 + k] >> 5;
        buss_desc.free_CA_mode = buffer[pos + 3 + k] & 0x10;
        buss_desc.descriptor_loop_length = (buffer[pos + 3 + k] & 0x0f) << 8 | buffer[pos + 4 + k];
        for(k1 = 0;k1 < buss_desc.descriptor_loop_length;)
        {
            unsigned char descriptor_tag = buffer[pos + 5 + k + k1];
            unsigned char descriptor_length = buffer[pos + 6 + k + k1];
            if(descriptor_tag == 0x48)
            {
                service_descriptor ser_des;
                ser_des.descriptor_tag = descriptor_tag;
                ser_des.descriptor_length = descriptor_length;
                ser_des.service_type = buffer[pos + 7 + k + k1];
                ser_des.service_provider_name_length = buffer[pos + 8 + k + k1];
                for(i = 0;i < ser_des.service_provider_name_length;i++)
                    tempbuf[i] = buffer[pos + 9 + k + k1 + i];
                tempbuf[i]='\0';
                ser_des.service_provider_name = QString(QLatin1String(tempbuf));
                ser_des.service_name_length = buffer[pos + 10 + k + k1 + i -1];
                for(j = 0;j < ser_des.service_name_length;j++)
                    tempbuf[j] = buffer[pos + 11 + k + k1 + i - 1 + j];
                tempbuf[j] = '\0';
                ser_des.service_name = QString(QLatin1String(tempbuf));
                buss_desc.ser_desc.push_back(ser_des);
            }
            else if(descriptor_tag == 0x5f)
            {
                private_data_specifier_descriptor priva_data;
                priva_data.descriptor_tag = descriptor_tag;
                priva_data.descriptor_length = descriptor_length;
                priva_data.private_data_specifier = buffer[pos + 7 + k + k1 + i] << 24 |
                                                    buffer[pos + 7 + k + k1 + i] << 16 |
                                                    buffer[pos + 7 + k + k1 + i] << 8  |
                                                    buffer[pos + 7 + k + k1 + i];
                buss_desc.pri_data.push_back(priva_data);
            }
            else
            {
                unknown_descriptor un_des;
                un_des.descriptor_tag = descriptor_tag;
                un_des.descriptor_length = descriptor_length;
                for(i = 0;i < un_des.descriptor_length;i++)
                    un_des.data.push_back(buffer[pos + 7 + k + k1 + i]);
                buss_desc.un_desc.push_back(un_des);
            }
            k1 += descriptor_length + 2;
        }
        k += buss_desc.descriptor_loop_length + 5;
        packet->bus_desc.push_back(buss_desc);
    }
    check_flag_sdt = 0;
    packet->check_CRC32 = crc;
    packet->SDT_conut += 1;
    d.ts_sdt.push_back(*packet);
    if(packet->SDT_conut == packet->last_section_number + 1)
        packet->Is_write = 1;
}
void MainWindow::process_packet(unsigned char *buf)
{
    unsigned int flag = 0;
    TS_HEADER *header = new TS_HEADER;
    adjust_packet_header(header,buf);
    if(header->payload_unit_start_indicator == 1)
    {
        flag = 1;
        if(header->PID == 0x10)
            check_flag_nit = 1;
        else if(header->PID == 0x11)
            check_flag_sdt = 1;
        else if(header->PID == 0x12)
            check_flag_eit = 1;
    }
    else
    {
        if(header->PID == 0x10 && check_flag_nit == 1)
            flag = 0;
        else if(header->PID == 0x11 && check_flag_sdt == 1)
            flag = 0;
        else if(header->PID == 0x12 && check_flag_eit == 1)
            flag = 0;
        else
        {
            if(header->PID == 0x10 || header->PID == 0x11 || header->PID == 0x12)
                return ;
            else
                flag = 0;
        }
    }
    if(header->PID == 0x00)
    {
        if(packet_PAT->Is_write == 1)
            return ;
        if(flag)
            adjust_PAT_table(packet_PAT,buf+5);
        else
            adjust_PAT_table(packet_PAT,buf+4);
    }
    else if(header->PID == 0x01)
    {
        if(packet_CAT->Is_write == 1)
            return ;
        if(flag)
            adjust_CAT_table(packet_CAT,(unsigned char *)buf+5);
        else
            adjust_CAT_table(packet_CAT,(unsigned char *)buf+4);
    }
    else if(header->PID == 0x14)
    {
        if((buf+5)[0] == 0x70)
        {
            if(packet_TDT->Is_write == 1)
                return ;
            adjust_TDT_table(packet_TDT,(unsigned char *)buf+5);
        }
        else if((buf+5)[0] == 0x73)
        {
            if(packet_TOT->Is_write == 1)
                return ;
            adjust_TOT_table(packet_TOT,(unsigned char *)buf+5);
        }
    }
    else if(header->PID == 0x10)
    {
        if(packet_NIT->Is_write == 1)
            return ;
        if(flag)
            adjust_NIT_table(packet_NIT,(unsigned char *)buf+5);
        else
            adjust_NIT_table(packet_NIT,(unsigned char *)buf+4);
        if(length_nit == 0)
            check_flag_nit = 0;
    }
    else if(header->PID == 0x12)
    { 
        if(flag)
            adjust_EIT_table(packet_EIT,(unsigned char *)buf+5);
        else
            adjust_EIT_table(packet_EIT,(unsigned char *)buf+4);
        if(length_eit == 0)
            check_flag_eit = 0;
    }
    else if(header->PID == 0x11)
    {
        if(packet_SDT->Is_write == 1)
            return ;
        if(flag)
            adjust_SDT_table(packet_SDT,(unsigned char *)buf+5);
        else
            adjust_SDT_table(packet_SDT,(unsigned char *)buf+4);
        if(length_sdt == 0)
            check_flag_sdt = 0;
    }
    else if(header->PID)
    {
        for(int i = 0;i < PMT_PID.length();i++)
        {
            if(header->PID == PMT_PID[i])
            {
                //PMT_PID[i] = 0;
                check_flag_pmt = 1;
                if(packet_PMT->PMT_conut == PMT_PID.length())
                    return ;
                if(flag)
                    adjust_PMT_table(packet_PMT,(unsigned char *)buf+5,header->PID);
                else
                    adjust_PMT_table(packet_PMT,(unsigned char *)buf+4,header->PID);
                if(length_pmt == 0)
                    check_flag_pmt = 0;
            }
        }
    }
    delete header;
}
void MainWindow::process_first(unsigned char *buf)
{
    unsigned int flag = 0;
    TS_HEADER *header = new TS_HEADER;
    adjust_packet_header(header,buf);
    if(header->payload_unit_start_indicator == 1)
        flag = 1;
    else
        flag = 0;
    if(header->PID == 0x00)
    {
        if(packet_PAT->Is_write == 1)
            return ;
        if(flag)
            adjust_PAT_table(packet_PAT,(unsigned char *)buf+5);
        else
            adjust_PAT_table(packet_PAT,(unsigned char *)buf+4);
    }
    delete header;
}
unsigned int MainWindow::crc32(unsigned int crc, unsigned char *buffer, unsigned int size)
{
    unsigned int i;
        for (i = 0; i < size; i++)
        {
            crc = (crc_table[((crc >> 24) & 0xff) ^ buffer[i]] ^ (crc << 8)) & 0xffffffff;
        }
        return (crc);
}
void MainWindow::clear_cache()
{
    ts_pat_program.clear();
    ts_cat.clear();
    ts_nit.clear();
    ts_pmt.clear();
    d.ts_sdt.clear();
    d.ts_tdt.clear();
    d.ts_tot.clear();
    d.ts_eit_pf_other.clear();
    d.ts_eit_pf_actual.clear();
    d.ts_eit_schedule_other.clear();
    d.ts_eit_schedule_actual.clear();
}
void MainWindow::clear_all()
{
    ui->treeWidget->clear();
    packet_PMT->PMT_conut = 0;
    packet_CAT->Is_write = 0;
    packet_PAT->Is_write = 0;
    packet_NIT->Is_write = 0;
    packet_NIT->NIT_conut = 0;
    packet_SDT->Is_write = 0;
    packet_SDT->SDT_conut = 0;
    packet_TDT->Is_write = 0;
    packet_TOT->Is_write = 0;
    offset_time.Year = 0;
    offset_time.Mouth = 0;
    offset_time.Day = 0;
    offset_time.Hour = 0;
    offset_time.Min = 0;
    offset_time.Sec = 0;
    clear_cache();
}
void MainWindow::on_pushButton_clicked()
{

    clear_all();
    char *buf = new char[188];
    QString filename;
    int filesize = 0;
    filename = QFileDialog::getOpenFileName(this,tr("file"),"",tr("text(*.ts)"));
    QByteArray ba = filename.toLatin1();
    char *Tsfilename = ba.data();
    fstream fp;
    fp.open(Tsfilename,ios::in);
    if(!fp.is_open())
    {
        qDebug()<<"open failed";
    }
    fp.seekg(0,ios::end);
    filesize = fp.tellg();
    int first_size = filesize;
    fp.seekg(0,ios::beg);
    while(first_size > 0)
    {
        fp.read(buf,188);
        first_size -= 188;
        if(buf[0]==0x47)
        {
            process_first((unsigned char *)buf);
        }
        else
        {
            for(int i = 0;i < 188;i++)
            {
                if(buf[i] == 0x47)
                {
                    fp.clear();
                    fp.seekg(i,ios::beg);
                    cout<<"move once,offest="<<i<<endl;
                    break;
                }
            }
        }
    }
    fp.close();
    qDebug()<<"first read over"<<endl;

    fp.open(Tsfilename,ios::in);
    fp.seekg(0,ios::beg);
    while(filesize > 0)
    {
        fp.read(buf,188);
        filesize -= 188;
        if(buf[0]==0x47)
        {
            process_packet((unsigned char *)buf);
        }
        else
        {
            for(int i = 0;i < 188;i++)
            {
                if(buf[i] == 0x47)
                {
                    fp.clear();
                    fp.seekg(i,ios::beg);
                    cout<<"move once,offest="<<i<<endl;
                    break;
                }
            }
        }
    }
    fp.close();
    qDebug()<<"TS OVER"<<endl;
    init_tree();
    delete []buf;
}

void MainWindow::on_pushButton_3_clicked()
{

    d.setWindowTitle("SI");
    d.init_tree();
    d.exec();
}

void MainWindow::on_pushButton_4_clicked()
{
    e.ts_eit_pf_actual = d.ts_eit_pf_actual;
    e.ts_eit_schedule_actual = d.ts_eit_schedule_actual;
    e.init_tree();
    e.setWindowTitle("EPG");
    e.exec();
}

void MainWindow::on_pushButton_2_clicked()
{
    b.setWindowTitle("Basic_information");
    b.init_tree();
    b.exec();
}
