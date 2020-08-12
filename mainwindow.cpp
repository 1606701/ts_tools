#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QFileDialog>
#include <stdio.h>
#include <QMessageBox>
#include <QTextCodec>
#include <QDebug>
#include <iostream>
#include <fstream>
#include <cassert>
#include <cstdlib>
#include <unistd.h>
#include "ui_dialog.h"
#include "ui_epg.h"
#include "basic_info.h"
#include "my_thread.h"
#include <QThreadPool>

using namespace std;
static unsigned int crc_table[256] = {
  0x00000000, 0x04c11db7, 0x09823b6e, 0x0d4326d9,
  0x130476dc, 0x17c56b6b, 0x1a864db2, 0x1e475005,
  0x2608edb8, 0x22c9f00f, 0x2f8ad6d6, 0x2b4bcb61,
  0x350c9b64, 0x31cd86d3, 0x3c8ea00a, 0x384fbdbd,
  0x4c11db70, 0x48d0c6c7, 0x4593e01e, 0x4152fda9,
  0x5f15adac, 0x5bd4b01b, 0x569796c2, 0x52568b75,
  0x6a1936c8, 0x6ed82b7f, 0x639b0da6, 0x675a1011,
  0x791d4014, 0x7ddc5da3, 0x709f7b7a, 0x745e66cd,
  0x9823b6e0, 0x9ce2ab57, 0x91a18d8e, 0x95609039,
  0x8b27c03c, 0x8fe6dd8b, 0x82a5fb52, 0x8664e6e5,
  0xbe2b5b58, 0xbaea46ef, 0xb7a96036, 0xb3687d81,
  0xad2f2d84, 0xa9ee3033, 0xa4ad16ea, 0xa06c0b5d,
  0xd4326d90, 0xd0f37027, 0xddb056fe, 0xd9714b49,
  0xc7361b4c, 0xc3f706fb, 0xceb42022, 0xca753d95,
  0xf23a8028, 0xf6fb9d9f, 0xfbb8bb46, 0xff79a6f1,
  0xe13ef6f4, 0xe5ffeb43, 0xe8bccd9a, 0xec7dd02d,
  0x34867077, 0x30476dc0, 0x3d044b19, 0x39c556ae,
  0x278206ab, 0x23431b1c, 0x2e003dc5, 0x2ac12072,
  0x128e9dcf, 0x164f8078, 0x1b0ca6a1, 0x1fcdbb16,
  0x018aeb13, 0x054bf6a4, 0x0808d07d, 0x0cc9cdca,
  0x7897ab07, 0x7c56b6b0, 0x71159069, 0x75d48dde,
  0x6b93dddb, 0x6f52c06c, 0x6211e6b5, 0x66d0fb02,
  0x5e9f46bf, 0x5a5e5b08, 0x571d7dd1, 0x53dc6066,
  0x4d9b3063, 0x495a2dd4, 0x44190b0d, 0x40d816ba,
  0xaca5c697, 0xa864db20, 0xa527fdf9, 0xa1e6e04e,
  0xbfa1b04b, 0xbb60adfc, 0xb6238b25, 0xb2e29692,
  0x8aad2b2f, 0x8e6c3698, 0x832f1041, 0x87ee0df6,
  0x99a95df3, 0x9d684044, 0x902b669d, 0x94ea7b2a,
  0xe0b41de7, 0xe4750050, 0xe9362689, 0xedf73b3e,
  0xf3b06b3b, 0xf771768c, 0xfa325055, 0xfef34de2,
  0xc6bcf05f, 0xc27dede8, 0xcf3ecb31, 0xcbffd686,
  0xd5b88683, 0xd1799b34, 0xdc3abded, 0xd8fba05a,
  0x690ce0ee, 0x6dcdfd59, 0x608edb80, 0x644fc637,
  0x7a089632, 0x7ec98b85, 0x738aad5c, 0x774bb0eb,
  0x4f040d56, 0x4bc510e1, 0x46863638, 0x42472b8f,
  0x5c007b8a, 0x58c1663d, 0x558240e4, 0x51435d53,
  0x251d3b9e, 0x21dc2629, 0x2c9f00f0, 0x285e1d47,
  0x36194d42, 0x32d850f5, 0x3f9b762c, 0x3b5a6b9b,
  0x0315d626, 0x07d4cb91, 0x0a97ed48, 0x0e56f0ff,
  0x1011a0fa, 0x14d0bd4d, 0x19939b94, 0x1d528623,
  0xf12f560e, 0xf5ee4bb9, 0xf8ad6d60, 0xfc6c70d7,
  0xe22b20d2, 0xe6ea3d65, 0xeba91bbc, 0xef68060b,
  0xd727bbb6, 0xd3e6a601, 0xdea580d8, 0xda649d6f,
  0xc423cd6a, 0xc0e2d0dd, 0xcda1f604, 0xc960ebb3,
  0xbd3e8d7e, 0xb9ff90c9, 0xb4bcb610, 0xb07daba7,
  0xae3afba2, 0xaafbe615, 0xa7b8c0cc, 0xa379dd7b,
  0x9b3660c6, 0x9ff77d71, 0x92b45ba8, 0x9675461f,
  0x8832161a, 0x8cf30bad, 0x81b02d74, 0x857130c3,
  0x5d8a9099, 0x594b8d2e, 0x5408abf7, 0x50c9b640,
  0x4e8ee645, 0x4a4ffbf2, 0x470cdd2b, 0x43cdc09c,
  0x7b827d21, 0x7f436096, 0x7200464f, 0x76c15bf8,
  0x68860bfd, 0x6c47164a, 0x61043093, 0x65c52d24,
  0x119b4be9, 0x155a565e, 0x18197087, 0x1cd86d30,
  0x029f3d35, 0x065e2082, 0x0b1d065b, 0x0fdc1bec,
  0x3793a651, 0x3352bbe6, 0x3e119d3f, 0x3ad08088,
  0x2497d08d, 0x2056cd3a, 0x2d15ebe3, 0x29d4f654,
  0xc5a92679, 0xc1683bce, 0xcc2b1d17, 0xc8ea00a0,
  0xd6ad50a5, 0xd26c4d12, 0xdf2f6bcb, 0xdbee767c,
  0xe3a1cbc1, 0xe760d676, 0xea23f0af, 0xeee2ed18,
  0xf0a5bd1d, 0xf464a0aa, 0xf9278673, 0xfde69bc4,
  0x89b8fd09, 0x8d79e0be, 0x803ac667, 0x84fbdbd0,
  0x9abc8bd5, 0x9e7d9662, 0x933eb0bb, 0x97ffad0c,
  0xafb010b1, 0xab710d06, 0xa6322bdf, 0xa2f33668,
  0xbcb4666d, 0xb8757bda, 0xb5365d03, 0xb1f740b4
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete []tempbuf;
    delete packet_CAT;
    delete packet_NIT;
    delete packet_PAT;
    delete packet_PMT;
    delete packet_SDT;
    delete packet_TDT;
    delete packet_TOT;
    delete packet_EIT;
    delete Q_pool;
    delete task;
}

void MainWindow::get_section_data(unsigned int pid)
{
    section_info *sec_info = new section_info;
    sec_info->PID = pid;
    switch (sec_info->PID) {
        case 0x00:sec_info->type = "PAT";break;
        case 0x01:sec_info->type = "CAT";break;
        case 0x10:sec_info->type = "NIT";break;
        case 0x11:sec_info->type = "SDT/BAT";break;
        case 0x12:sec_info->type = "EIT";break;
        case 0x14:sec_info->type = "TDT/TOT";break;
        default:
            sec_info->type = "unkonwn";
            for(int i = 0;i < PMT_PID.length();i++)
            {
                if(pid == PMT_PID[i])
                {
                    sec_info->type = "PMT";
                    break;
                }
            }
            for(int i = 0;i < b.PID_tree.length();i++)
            {
                for(int j = 0;j < b.PID_tree[i].stream_information.length();j++)
                {
                    if(pid == b.PID_tree[i].stream_information[j].PID)
                    {
                        sec_info->type = b.PID_tree[i].stream_information[j].type;
                        break;
                    }
                }
            }
            break;
    }
    for(QVector<section_info>::iterator it = s.Section_tree.begin(); it != s.Section_tree.end();it++)
    {
        if(sec_info->PID == it->PID)
        {
            return ;
        }
    }
    s.Section_tree.push_back(*sec_info);
    delete sec_info;
}

/*
* function descriptor:analyze section header information
* input:
*   TS_HEADER *packet:save header information
*   unsigned char *buffer:read from ts stream
* output:null
* return:void
*/
void MainWindow::adjust_packet_header(TS_HEADER *packet,unsigned char *buffer)
{
    packet->sync_byte                 	 = buffer[0];
    if(packet->sync_byte != 0x47)
    {
        return ;
    }
    packet->transport_error_indicator 	 = buffer[1] >> 7;
    packet->payload_unit_start_indicator = buffer[1] >> 6 & 0x1;
    packet->transport_priority           = buffer[1] >> 5 & 0x1;
    packet->PID 						 = (buffer[1] & 0x1F) << 8 | buffer[2];
    packet->transport_scrambling_control = buffer[3] >> 6 & 0x3;
    packet->adaptation_field_control     = buffer[3] >> 4 & 0x3;
    packet->continuity_counter           = buffer[3] & 0x0F;
    if(check_section_flag)
    {
        //start_sig(packet->PID,packet->continuity_counter,buffer);
        get_section_data(packet->PID);
    }
}

/*
* function descriptor:When the length of the nit section table is greater than 188,
*                     get the remaining bytes.
* input:
*   TS_NIT *packet:Used to store the last parsed nit section table information
*   unsigned char *buffer:The rest of the nit information
*   unsigned int len:The length of the entire nit section
* output:null
* return:void
*/
void MainWindow::get_all_NIT_buffer(TS_NIT *packet,unsigned char *buffer,unsigned int len)
{
    for(int i = 0;i < 184;i++)
    {
        if(length_nit<len)
            long_buf_nit[length_nit++]=buffer[i];
        else
        {
            flag_get_nit = 0;
            check_flag_nit = 0;
            adjust_NIT_table(packet,(unsigned char *)long_buf_nit);
            delete []long_buf_nit;
            length_nit = 0 ;
            break;
        }
    }
}

/*
* function descriptor:When the length of the eit section table is greater than 188,
*                     get the remaining bytes.
* input:
*   TS_NIT *packet:Used to store the last parsed eit section table information
*   unsigned char *buffer:The rest of the eit information
*   unsigned int len:The length of the entire eit section
* output:null
* return:void
*/
void MainWindow::get_all_EIT_buffer(TS_EIT *packet, unsigned char *buffer, unsigned int len)
{
    for(int i = 0;i < 184;i++)
    {
        if(length_eit<len)
            long_buf_eit[length_eit++]=buffer[i];
        else
        {
            flag_get_eit = 0;
            check_flag_eit = 0;
            adjust_EIT_table(packet,(unsigned char *)long_buf_eit);
            delete []long_buf_eit;
            length_eit = 0 ;
            break;
        }
    }
}

/*
* function descriptor:When the length of the sdt section table is greater than 188,
*                     get the remaining bytes.
* input:
*   TS_NIT *packet:Used to store the last parsed sdt section table information
*   unsigned char *buffer:The rest of the sdt information
*   unsigned int len:The length of the entire sdt section
* output:null
* return:void
*/
void MainWindow::get_all_SDT_buffer(TS_SDT *packet, unsigned char *buffer, unsigned int len)
{
    for(int i = 0;i < 184;i++)
    {
        if(length_sdt<len)
            long_buf_sdt[length_sdt++]=buffer[i];
        else
        {
            flag_get_sdt = 0;
            check_flag_sdt = 0;
            adjust_SDT_table(packet,(unsigned char *)long_buf_sdt);
            delete []long_buf_sdt;
            length_sdt = 0 ;
            break;
        }
    }
}

/*
* function descriptor:When the length of the pmt section table is greater than 188,
*                     get the remaining bytes.
* input:
*   TS_NIT *packet:Used to store the last parsed pmt section table information
*   unsigned char *buffer:The rest of the pmt information
*   unsigned int len:The length of the entire pmt section
* output:null
* return:void
*/
void MainWindow::get_all_PMT_buffer(TS_PMT *packet, unsigned char *buffer, unsigned int len,unsigned int PID)
{
    //printf("current_length=%x,len=%x\n",length,len);
    //printf("get other\n");
    for(int i = 0;i < 184;i++)
    {
        //printf("current_length=%x,len=%x\n",length_sdt,len);
        if(length_pmt<len)
            long_buf_pmt[length_pmt++]=buffer[i];
        else
        {
            flag_get_pmt = 0;
            check_flag_pmt = 0;
            //printf("adjust big sdt section\n*********\n");
            adjust_PMT_table(packet,(unsigned char *)long_buf_pmt,PID);
            delete []long_buf_pmt;
            length_pmt = 0 ;
            //printf("sdt over\n");
            //check_flag = 0;
            break;
        }
    }
}

/*
* function descriptor:Initializes the pat information tree
* input:null
* output:null
* return:void
*/
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

/*
* function descriptor:Initializes the pmt information tree
* input:null
* output:null
* return:void
*/
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
        QTreeWidgetItem *group1 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"table_id:%x",ts_pmt[i].table_id);
        group1->setText(0,tempbuf);
        QTreeWidgetItem *group2 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"section_syntax_indicator:%x",ts_pmt[i].section_syntax_indicator);
        group2->setText(0,tempbuf);
        QTreeWidgetItem *group3 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"section_length:%x",ts_pmt[i].section_length);
        group3->setText(0,tempbuf);
        QTreeWidgetItem *group4 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"program_number:%x",ts_pmt[i].program_number);
        group4->setText(0,tempbuf);
        QTreeWidgetItem *group5 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"version_number:%x",ts_pmt[i].version_number);
        group5->setText(0,tempbuf);
        QTreeWidgetItem *group6 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"current_next_indicator:%x",ts_pmt[i].current_next_indicator);
        group6->setText(0,tempbuf);
        QTreeWidgetItem *group7 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"section_number:%x",ts_pmt[i].section_number);
        group7->setText(0,tempbuf);
        QTreeWidgetItem *group8 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"last_section_number:%x",ts_pmt[i].last_section_number);
        group8->setText(0,tempbuf);
        QTreeWidgetItem *group9 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"PCR_PID:%x",ts_pmt[i].PCR_PID);
        group9->setText(0,tempbuf);
        QTreeWidgetItem *group10 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"program_info_length:%x",ts_pmt[i].program_info_length);
        group10->setText(0,tempbuf);
        QTreeWidgetItem *group11 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"program_info_descriptors");
        group11->setText(0,tempbuf);
        for(int j = 0;j < ts_pmt[i].ca_desc.length();j++)
        {
            QTreeWidgetItem *Item = new QTreeWidgetItem(group11);
            sprintf(tempbuf,"CA_descriptor%d",j+1);
            Item->setText(0,tempbuf);
            QTreeWidgetItem *title = new QTreeWidgetItem(Item);
            sprintf(tempbuf,"descriptor_tag:%x",ts_pmt[i].ca_desc[j].descriptor_tag);
            title->setText(0,tempbuf);
            QTreeWidgetItem *title1 = new QTreeWidgetItem(Item);
            sprintf(tempbuf,"descriptor_length:%x",ts_pmt[i].ca_desc[j].descriptor_length);
            title1->setText(0,tempbuf);
            QTreeWidgetItem *title2 = new QTreeWidgetItem(Item);
            sprintf(tempbuf,"CA_system_ID:%x",ts_pmt[i].ca_desc[j].CA_system_ID);
            title2->setText(0,tempbuf);
            QTreeWidgetItem *title3 = new QTreeWidgetItem(Item);
            sprintf(tempbuf,"CA_PID:%x",ts_pmt[i].ca_desc[j].CA_PID);
            title3->setText(0,tempbuf);
        }
        for(int j = 0;j < ts_pmt[i].maxi_desc.length();j++)
        {
            QTreeWidgetItem *Item = new QTreeWidgetItem(group11);
            sprintf(tempbuf,"maximum_bitrate_descriptor%d",j+1);
            Item->setText(0,tempbuf);
            QTreeWidgetItem *title = new QTreeWidgetItem(Item);
            sprintf(tempbuf,"descriptor_tag:%x",ts_pmt[i].maxi_desc[j].descriptor_tag);
            title->setText(0,tempbuf);
            QTreeWidgetItem *title1 = new QTreeWidgetItem(Item);
            sprintf(tempbuf,"descriptor_length:%x",ts_pmt[i].maxi_desc[j].descriptor_length);
            title1->setText(0,tempbuf);
            QTreeWidgetItem *title2 = new QTreeWidgetItem(Item);
            sprintf(tempbuf,"maximum_bitrate:%x",ts_pmt[i].maxi_desc[j].maximum_bitrate);
            title2->setText(0,tempbuf);
        }
        for(int j = 0;j < ts_pmt[i].mul_desc.length();j++)
        {
            QTreeWidgetItem *Item = new QTreeWidgetItem(group11);
            sprintf(tempbuf,"multiplex_buffer_utilization_descriptor%d",j+1);
            Item->setText(0,tempbuf);
            QTreeWidgetItem *title = new QTreeWidgetItem(Item);
            sprintf(tempbuf,"descriptor_tag:%x",ts_pmt[i].mul_desc[j].descriptor_tag);
            title->setText(0,tempbuf);
            QTreeWidgetItem *title1 = new QTreeWidgetItem(Item);
            sprintf(tempbuf,"descriptor_length:%x",ts_pmt[i].mul_desc[j].descriptor_length);
            title1->setText(0,tempbuf);
            QTreeWidgetItem *title2 = new QTreeWidgetItem(Item);
            sprintf(tempbuf,"bound_valid_flag:%x",ts_pmt[i].mul_desc[j].bound_valid_flag);
            title2->setText(0,tempbuf);
            QTreeWidgetItem *title3 = new QTreeWidgetItem(Item);
            sprintf(tempbuf,"LTW_offset_lower_bound:%x",ts_pmt[i].mul_desc[j].LTW_offset_lower_bound);
            title3->setText(0,tempbuf);
            QTreeWidgetItem *title4 = new QTreeWidgetItem(Item);
            sprintf(tempbuf,"LTW_offset_upper_bound:%x",ts_pmt[i].mul_desc[j].LTW_offset_upper_bound);
            title4->setText(0,tempbuf);
        }
        for(int j = 0;j < ts_pmt[i].sys_cloc_desc.length();j++)
        {
            QTreeWidgetItem *Item = new QTreeWidgetItem(group11);
            sprintf(tempbuf,"system_clock_descriptor%d",j+1);
            Item->setText(0,tempbuf);
            QTreeWidgetItem *title = new QTreeWidgetItem(Item);
            sprintf(tempbuf,"descriptor_tag:%x",ts_pmt[i].sys_cloc_desc[j].descriptor_tag);
            title->setText(0,tempbuf);
            QTreeWidgetItem *title1 = new QTreeWidgetItem(Item);
            sprintf(tempbuf,"descriptor_length:%x",ts_pmt[i].sys_cloc_desc[j].descriptor_length);
            title1->setText(0,tempbuf);
            QTreeWidgetItem *title2 = new QTreeWidgetItem(Item);
            sprintf(tempbuf,"external_clock_reference_indicator:%x",ts_pmt[i].sys_cloc_desc[j].external_clock_reference_indicator);
            title2->setText(0,tempbuf);
            QTreeWidgetItem *title3 = new QTreeWidgetItem(Item);
            sprintf(tempbuf,"clock_accuracy_integer:%x",ts_pmt[i].sys_cloc_desc[j].clock_accuracy_integer);
            title3->setText(0,tempbuf);
            QTreeWidgetItem *title4 = new QTreeWidgetItem(Item);
            sprintf(tempbuf,"clock_accuracy_exponent:%x",ts_pmt[i].sys_cloc_desc[j].clock_accuracy_exponent);
            title4->setText(0,tempbuf);
        }
        for(int j = 0;j < ts_pmt[i].smoo_desc.length();j++)
        {
            QTreeWidgetItem *Item = new QTreeWidgetItem(group11);
            sprintf(tempbuf,"smoothing_buffer_descriptor%d",j+1);
            Item->setText(0,tempbuf);
            QTreeWidgetItem *title = new QTreeWidgetItem(Item);
            sprintf(tempbuf,"descriptor_tag:%x",ts_pmt[i].smoo_desc[j].descriptor_tag);
            title->setText(0,tempbuf);
            QTreeWidgetItem *title1 = new QTreeWidgetItem(Item);
            sprintf(tempbuf,"descriptor_length:%x",ts_pmt[i].smoo_desc[j].descriptor_length);
            title1->setText(0,tempbuf);
            QTreeWidgetItem *title2 = new QTreeWidgetItem(Item);
            sprintf(tempbuf,"sb_leak_rate:%d",ts_pmt[i].smoo_desc[j].sb_leak_rate);
            title2->setText(0,tempbuf);
            QTreeWidgetItem *title3 = new QTreeWidgetItem(Item);
            sprintf(tempbuf,"sb_size:%d",ts_pmt[i].smoo_desc[j].sb_size);
            title3->setText(0,tempbuf);
        }

        for(int j = 0;j < ts_pmt[i].ts_pmt_stream.length();j++)
        {
            QTreeWidgetItem *stream = new QTreeWidgetItem(group);
            sprintf(tempbuf,"Unit_stream%d",j + 1);
            stream->setText(0,tempbuf);
            QTreeWidgetItem *item = new QTreeWidgetItem(stream);
            sprintf(tempbuf,"ES_PID:%x,stream_type:%x(%s),Es_Info_Len:%x",ts_pmt[i].ts_pmt_stream[j].elementary_PID,
                                                                          ts_pmt[i].ts_pmt_stream[j].stream_type,
                                                                          ts_pmt[i].ts_pmt_stream[j].type_descriptor.toLatin1().data(),
                                                                          ts_pmt[i].ts_pmt_stream[j].ES_info_length);
            item->setText(0,tempbuf);
            QTreeWidgetItem *item1 = new QTreeWidgetItem(stream);
            sprintf(tempbuf,"ES_Correlation_Descriptors");
            item1->setText(0,tempbuf);
           for(int k = 0;k < ts_pmt[i].ts_pmt_stream[j].ca_desc.length();k++)
            {
                QTreeWidgetItem *Item = new QTreeWidgetItem(item1);
                sprintf(tempbuf,"CA_descriptor%d",k+1);
                Item->setText(0,tempbuf);
                QTreeWidgetItem *title = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"descriptor_tag:%x",ts_pmt[i].ts_pmt_stream[j].ca_desc[k].descriptor_tag);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"descriptor_length:%x",ts_pmt[i].ts_pmt_stream[j].ca_desc[k].descriptor_length);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"CA_system_ID:%x",ts_pmt[i].ts_pmt_stream[j].ca_desc[k].CA_system_ID);
                title2->setText(0,tempbuf);
                QTreeWidgetItem *title3 = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"CA_PID:%x",ts_pmt[i].ts_pmt_stream[j].ca_desc[k].CA_PID);
                title3->setText(0,tempbuf);
            }
           for(int k = 0;k < ts_pmt[i].ts_pmt_stream[j].stream_desc.length();k++)
           {
               QTreeWidgetItem *Item = new QTreeWidgetItem(item1);
               sprintf(tempbuf,"stream_identifier_descriptor%d",k+1);
               Item->setText(0,tempbuf);
               QTreeWidgetItem *title = new QTreeWidgetItem(Item);
               sprintf(tempbuf,"descriptor_tag:%x",ts_pmt[i].ts_pmt_stream[j].stream_desc[k].descriptor_tag);
               title->setText(0,tempbuf);
               QTreeWidgetItem *title1 = new QTreeWidgetItem(Item);
               sprintf(tempbuf,"descriptor_length:%x",ts_pmt[i].ts_pmt_stream[j].stream_desc[k].descriptor_length);
               title1->setText(0,tempbuf);
               QTreeWidgetItem *title2 = new QTreeWidgetItem(Item);
               sprintf(tempbuf,"component:%x",ts_pmt[i].ts_pmt_stream[j].stream_desc[k].component);
               title2->setText(0,tempbuf);
           }
           for(int k = 0;k < ts_pmt[i].ts_pmt_stream[j].audi_desc.length();k++)
           {
               QTreeWidgetItem *Item = new QTreeWidgetItem(item1);
               sprintf(tempbuf,"audio_stream_descriptor%d",k+1);
               Item->setText(0,tempbuf);
               QTreeWidgetItem *title = new QTreeWidgetItem(Item);
               sprintf(tempbuf,"descriptor_tag:%x",ts_pmt[i].ts_pmt_stream[j].audi_desc[k].descriptor_tag);
               title->setText(0,tempbuf);
               QTreeWidgetItem *title1 = new QTreeWidgetItem(Item);
               sprintf(tempbuf,"descriptor_length:%x",ts_pmt[i].ts_pmt_stream[j].audi_desc[k].descriptor_length);
               title1->setText(0,tempbuf);
               QTreeWidgetItem *title2 = new QTreeWidgetItem(Item);
               sprintf(tempbuf,"free_format_flag:%x",ts_pmt[i].ts_pmt_stream[j].audi_desc[k].free_format_flag);
               title2->setText(0,tempbuf);
               QTreeWidgetItem *title3 = new QTreeWidgetItem(Item);
               sprintf(tempbuf,"ID:%x",ts_pmt[i].ts_pmt_stream[j].audi_desc[k].ID);
               title3->setText(0,tempbuf);
               QTreeWidgetItem *title4 = new QTreeWidgetItem(Item);
               sprintf(tempbuf,"layer:%x",ts_pmt[i].ts_pmt_stream[j].audi_desc[k].layer);
               title4->setText(0,tempbuf);
               QTreeWidgetItem *title5 = new QTreeWidgetItem(Item);
               sprintf(tempbuf,"variable_rate_audio_indicator:%x",ts_pmt[i].ts_pmt_stream[j].audi_desc[k].variable_rate_audio_indicator);
               title5->setText(0,tempbuf);
           }
           for(int k = 0;k < ts_pmt[i].ts_pmt_stream[j].ISO_desc.length();k++)
           {
               QTreeWidgetItem *Item = new QTreeWidgetItem(item1);
               sprintf(tempbuf,"ISO_639_language_descriptor%d",k+1);
               Item->setText(0,tempbuf);
               QTreeWidgetItem *title = new QTreeWidgetItem(Item);
               sprintf(tempbuf,"descriptor_tag:%x",ts_pmt[i].ts_pmt_stream[j].ISO_desc[k].descriptor_tag);
               title->setText(0,tempbuf);
               QTreeWidgetItem *title1 = new QTreeWidgetItem(Item);
               sprintf(tempbuf,"descriptor_length:%x",ts_pmt[i].ts_pmt_stream[j].ISO_desc[k].descriptor_length);
               title1->setText(0,tempbuf);
               for(int l = 0;l < ts_pmt[i].ts_pmt_stream[j].ISO_desc[k].lang_code.length();l++)
               {
                   QTreeWidgetItem *node = new QTreeWidgetItem(Item);
                   sprintf(tempbuf,"language_node%d",l);
                   node->setText(0,tempbuf);
                   QTreeWidgetItem *node1 = new QTreeWidgetItem(node);
                   sprintf(tempbuf,"ISO_639_language_code:%c%c%c",(ts_pmt[i].ts_pmt_stream[j].ISO_desc[k].lang_code[l].ISO_639_language_code >> 16)
                                                                 ,(ts_pmt[i].ts_pmt_stream[j].ISO_desc[k].lang_code[l].ISO_639_language_code >> 8) & 0xFF
                                                                 ,ts_pmt[i].ts_pmt_stream[j].ISO_desc[k].lang_code[l].ISO_639_language_code & 0xFF);
                   node1->setText(0,tempbuf);
                   QTreeWidgetItem *node2 = new QTreeWidgetItem(node);
                   sprintf(tempbuf,"audio_type:%x",ts_pmt[i].ts_pmt_stream[j].ISO_desc[k].lang_code[l].audio_type);
                   node2->setText(0,tempbuf);
               }
           }
            for(int k = 0;k < ts_pmt[i].ts_pmt_stream[j].vide_desc.length();k++)
            {
                QTreeWidgetItem *Item = new QTreeWidgetItem(item1);
                sprintf(tempbuf,"video_stream_descriptor%d",k+1);
                Item->setText(0,tempbuf);
                QTreeWidgetItem *title = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"descriptor_tag:%x",ts_pmt[i].ts_pmt_stream[j].vide_desc[k].descriptor_tag);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"descriptor_length:%x",ts_pmt[i].ts_pmt_stream[j].vide_desc[k].descriptor_length);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"multiple_frame_rate_flag:%x",ts_pmt[i].ts_pmt_stream[j].vide_desc[k].multiple_frame_rate_flag);
                title2->setText(0,tempbuf);
                QTreeWidgetItem *title3 = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"frame_rate_code:%x",ts_pmt[i].ts_pmt_stream[j].vide_desc[k].frame_rate_code);
                title3->setText(0,tempbuf);
                QTreeWidgetItem *title4 = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"MPEG_1_only_flag:%x",ts_pmt[i].ts_pmt_stream[j].vide_desc[k].MPEG_1_only_flag);
                title4->setText(0,tempbuf);
                QTreeWidgetItem *title5 = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"constrained_parameter_flag:%x",ts_pmt[i].ts_pmt_stream[j].vide_desc[k].constrained_parameter_flag);
                title5->setText(0,tempbuf);
                QTreeWidgetItem *title6 = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"still_picture_flag:%x",ts_pmt[i].ts_pmt_stream[j].vide_desc[k].still_picture_flag);
                title6->setText(0,tempbuf);
            }
           for(int k = 0;k < ts_pmt[i].ts_pmt_stream[j].maxi_desc.length();k++)
            {
                QTreeWidgetItem *Item = new QTreeWidgetItem(item1);
                sprintf(tempbuf,"maximum_bitrate_descriptor%d",k+1);
                Item->setText(0,tempbuf);
                QTreeWidgetItem *title = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"descriptor_tag:%x",ts_pmt[i].ts_pmt_stream[j].maxi_desc[k].descriptor_tag);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"descriptor_length:%x",ts_pmt[i].ts_pmt_stream[j].maxi_desc[k].descriptor_length);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"maximum_bitrate:%x",ts_pmt[i].ts_pmt_stream[j].maxi_desc[k].maximum_bitrate);
                title2->setText(0,tempbuf);
            }
            for(int k = 0;k < ts_pmt[i].ts_pmt_stream[j].mul_desc.length();k++)
            {
                QTreeWidgetItem *Item = new QTreeWidgetItem(item1);
                sprintf(tempbuf,"multiplex_buffer_utilization_descriptor%d",k+1);
                Item->setText(0,tempbuf);
                QTreeWidgetItem *title = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"descriptor_tag:%x",ts_pmt[i].ts_pmt_stream[j].mul_desc[k].descriptor_tag);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"descriptor_length:%x",ts_pmt[i].ts_pmt_stream[j].mul_desc[k].descriptor_length);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"bound_valid_flag:%x",ts_pmt[i].ts_pmt_stream[j].mul_desc[k].bound_valid_flag);
                title2->setText(0,tempbuf);
                QTreeWidgetItem *title3 = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"LTW_offset_lower_bound:%x",ts_pmt[i].ts_pmt_stream[j].mul_desc[k].LTW_offset_lower_bound);
                title3->setText(0,tempbuf);
                QTreeWidgetItem *title4 = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"LTW_offset_upper_bound:%x",ts_pmt[i].ts_pmt_stream[j].mul_desc[k].LTW_offset_upper_bound);
                title4->setText(0,tempbuf);
            }
            for(int k = 0;k < ts_pmt[i].ts_pmt_stream[j].sys_cloc_desc.length();k++)
            {
                QTreeWidgetItem *Item = new QTreeWidgetItem(item1);
                sprintf(tempbuf,"system_clock_descriptor%d",k+1);
                Item->setText(0,tempbuf);
                QTreeWidgetItem *title = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"descriptor_tag:%x",ts_pmt[i].ts_pmt_stream[j].sys_cloc_desc[k].descriptor_tag);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"descriptor_length:%x",ts_pmt[i].ts_pmt_stream[j].sys_cloc_desc[k].descriptor_length);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"external_clock_reference_indicator:%x",ts_pmt[i].ts_pmt_stream[j].sys_cloc_desc[k].external_clock_reference_indicator);
                title2->setText(0,tempbuf);
                QTreeWidgetItem *title3 = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"clock_accuracy_integer:%x",ts_pmt[i].ts_pmt_stream[j].sys_cloc_desc[k].clock_accuracy_integer);
                title3->setText(0,tempbuf);
                QTreeWidgetItem *title4 = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"clock_accuracy_exponent:%x",ts_pmt[i].ts_pmt_stream[j].sys_cloc_desc[k].clock_accuracy_exponent);
                title4->setText(0,tempbuf);
            }
            for(int k = 0;k < ts_pmt[i].ts_pmt_stream[j].smoo_desc.length();k++)
            {
                QTreeWidgetItem *Item = new QTreeWidgetItem(item1);
                sprintf(tempbuf,"smoothing_buffer_descriptor%d",k+1);
                Item->setText(0,tempbuf);
                QTreeWidgetItem *title = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"descriptor_tag:%x",ts_pmt[i].ts_pmt_stream[j].smoo_desc[k].descriptor_tag);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"descriptor_length:%x",ts_pmt[i].ts_pmt_stream[j].smoo_desc[k].descriptor_length);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"sb_leak_rate:%d",ts_pmt[i].ts_pmt_stream[j].smoo_desc[k].sb_leak_rate);
                title2->setText(0,tempbuf);
                QTreeWidgetItem *title3 = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"sb_size:%d",ts_pmt[i].ts_pmt_stream[j].smoo_desc[k].sb_size);
                title3->setText(0,tempbuf);
            }
            for(int k = 0;k < ts_pmt[i].ts_pmt_stream[j].data_stream_desc.length();k++)
            {
                QTreeWidgetItem *Item = new QTreeWidgetItem(item1);
                sprintf(tempbuf,"data_stream_alignment_descriptor%d",k+1);
                Item->setText(0,tempbuf);
                QTreeWidgetItem *title = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"descriptor_tag:%x",ts_pmt[i].ts_pmt_stream[j].data_stream_desc[k].descriptor_tag);
                title->setText(0,tempbuf);
                QTreeWidgetItem *title1 = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"descriptor_length:%x",ts_pmt[i].ts_pmt_stream[j].data_stream_desc[k].descriptor_length);
                title1->setText(0,tempbuf);
                QTreeWidgetItem *title2 = new QTreeWidgetItem(Item);
                sprintf(tempbuf,"alignment_type:%x",ts_pmt[i].ts_pmt_stream[j].data_stream_desc[k].alignment_type);
                title2->setText(0,tempbuf);
            }
        }
        QTreeWidgetItem *group12 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"CRC32:%x",ts_pmt[i].CRC_32);
        group12->setText(0,tempbuf);
        QTreeWidgetItem *group13 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"check_CRC32:%x",ts_pmt[i].check_CRC32);
        group13->setText(0,tempbuf);
    }
}

/*
* function descriptor:Initializes the cat information tree
* input:null
* output:null
* return:void
*/
void MainWindow::init_CAT_tree()
{
    QTreeWidgetItem *CAT = new QTreeWidgetItem(ui->treeWidget);
    CAT->setText(0,"CAT_information");
    for(int i = 0;i < ts_cat.length();i++)
    {
        QTreeWidgetItem *group = new QTreeWidgetItem(CAT);
        sprintf(tempbuf,"section:(table_id:%d,version_number:%x,section_number:%x)",
                         ts_cat[i].table_id,ts_cat[i].version_number,ts_cat[i].section_number);
        group->setText(0,tempbuf);
        QTreeWidgetItem *group1 = new QTreeWidgetItem(CAT);
        sprintf(tempbuf,"table_id:%x",ts_cat[i].table_id);
        group1->setText(0,tempbuf);
        QTreeWidgetItem *group2 = new QTreeWidgetItem(CAT);
        sprintf(tempbuf,"section_synatx_indicator:%x",ts_cat[i].section_syntax_indicator);
        group2->setText(0,tempbuf);
        QTreeWidgetItem *group3 = new QTreeWidgetItem(CAT);
        sprintf(tempbuf,"section_length:%x",ts_cat[i].section_length);
        group3->setText(0,tempbuf);
        QTreeWidgetItem *group4 = new QTreeWidgetItem(CAT);
        sprintf(tempbuf,"current_next_indicator:%x",ts_cat[i].current_next_indicator);
        group4->setText(0,tempbuf);
        QTreeWidgetItem *group5 = new QTreeWidgetItem(CAT);
        sprintf(tempbuf,"section_number:%x",ts_cat[i].section_number);
        group5->setText(0,tempbuf);
        QTreeWidgetItem *group6 = new QTreeWidgetItem(CAT);
        sprintf(tempbuf,"last_section_number:%x",ts_cat[i].last_section_number);
        group6->setText(0,tempbuf);
        QTreeWidgetItem *group7 = new QTreeWidgetItem(CAT);
        sprintf(tempbuf,"CA_descriptor");
        group7->setText(0,tempbuf);
        for(int j = 0;j < ts_cat[i].ca_desc.length();j++)
        {
            QTreeWidgetItem *item = new QTreeWidgetItem(group7);
            sprintf(tempbuf,"CA_descriptor%d",j+1);
            item->setText(0,tempbuf);
            QTreeWidgetItem *title = new QTreeWidgetItem(item);
            sprintf(tempbuf,"descriptor_tag:%x",ts_cat[i].ca_desc[j].descriptor_tag);
            title->setText(0,tempbuf);
            QTreeWidgetItem *title1 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"descriptor_length:%x",ts_cat[i].ca_desc[j].descriptor_length);
            title1->setText(0,tempbuf);
            QTreeWidgetItem *title2 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"CA_system_ID:%x",ts_cat[i].ca_desc[j].CA_system_ID);
            title2->setText(0,tempbuf);
            QTreeWidgetItem *title3 = new QTreeWidgetItem(item);
            sprintf(tempbuf,"CA_PID:%x",ts_cat[i].ca_desc[j].CA_PID);
            title3->setText(0,tempbuf);
        }
        QTreeWidgetItem *group8 = new QTreeWidgetItem(CAT);
        sprintf(tempbuf,"CRC32:%x",ts_cat[i].CRC_32);
        group8->setText(0,tempbuf);
        QTreeWidgetItem *group9 = new QTreeWidgetItem(CAT);
        sprintf(tempbuf,"check_CRC32:%x",ts_cat[i].check_CRC_32);
        group9->setText(0,tempbuf);
    }
}

/*
* function descriptor:Initializes the nit information tree
* input:null
* output:null
* return:void
*/
void MainWindow::init_NIT_tree()
{
    QTreeWidgetItem *NIT = new QTreeWidgetItem(ui->treeWidget);
    NIT->setText(0,"NIT_information");
    for(int i = 0;i < ts_nit.length();i++)
    {
        QTreeWidgetItem *group = new QTreeWidgetItem(NIT);
        sprintf(tempbuf,"section:(table_id:%x,network_id:%x,version_number:%x,section_number:%x)",
                         ts_nit[i].table_id,ts_nit[i].network_id,ts_nit[i].version_number,ts_nit[i].section_number);
        group->setText(0,tempbuf);
        QTreeWidgetItem *low = new QTreeWidgetItem(group);
        sprintf(tempbuf,"table_id:%x",ts_nit[i].table_id);
        low->setText(0,tempbuf);
        QTreeWidgetItem *low1 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"section_syntax_indicator:%x",ts_nit[i].section_syntax_indicator);
        low1->setText(0,tempbuf);
        QTreeWidgetItem *low2 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"section_length:%x",ts_nit[i].section_length);
        low2->setText(0,tempbuf);
        QTreeWidgetItem *low3 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"network_id:%x",ts_nit[i].network_id);
        low3->setText(0,tempbuf);
        QTreeWidgetItem *low4 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"version_number:%x",ts_nit[i].version_number);
        low4->setText(0,tempbuf);
        QTreeWidgetItem *low5 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"current_next_indicator:%x",ts_nit[i].current_next_indicator);
        low5->setText(0,tempbuf);
        QTreeWidgetItem *low6 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"section_number:%x",ts_nit[i].section_number);
        low6->setText(0,tempbuf);
        QTreeWidgetItem *low7 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"last_section_number:%x",ts_nit[i].last_section_number);
        low7->setText(0,tempbuf);
        QTreeWidgetItem *low8 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"network_descriptors_length:%x",ts_nit[i].network_descriptors_length);
        low8->setText(0,tempbuf);

        QTreeWidgetItem *desc = new QTreeWidgetItem(group);
        desc->setText(0,"descriptors");
        for(int j = 0;j < ts_nit[i].nit_info_table_1.length();j++)
        {
            if(ts_nit[i].nit_info_table_1[j].descriptor_tag == 0x40)
            {
                QTreeWidgetItem *group1 = new QTreeWidgetItem(desc);
                group1->setText(0,"network_descriptor");
                QTreeWidgetItem *Item1 = new QTreeWidgetItem(group1);
                sprintf(tempbuf,"descriptor_tag:%x",ts_nit[i].nit_info_table_1[j].descriptor_tag);
                Item1->setText(0,tempbuf);
                QTreeWidgetItem *Item2 = new QTreeWidgetItem(group1);
                sprintf(tempbuf,"descriptor_length:%x",ts_nit[i].nit_info_table_1[j].descriptor_length);
                Item2->setText(0,tempbuf);
                QTreeWidgetItem *Item3 = new QTreeWidgetItem(group1);
                Item3->setText(0,ts_nit[i].nit_info_table_1[j].descriptor_char);
            }
            else
            {
                QTreeWidgetItem *unknow = new QTreeWidgetItem(desc);
                unknow->setText(0,"unknow_descriptor");
                QTreeWidgetItem *Item1 = new QTreeWidgetItem(unknow);
                sprintf(tempbuf,"descriptor_tag:%x",ts_nit[i].nit_info_table_1[j].descriptor_tag);
                Item1->setText(0,tempbuf);
                QTreeWidgetItem *Item2 = new QTreeWidgetItem(unknow);
                sprintf(tempbuf,"descriptor_length:%x",ts_nit[i].nit_info_table_1[j].descriptor_length);
                Item2->setText(0,tempbuf);
                QTreeWidgetItem *Item3 = new QTreeWidgetItem(unknow);
                Item3->setText(0,ts_nit[i].nit_info_table_1[j].descriptor_char);
            }
        }
        QTreeWidgetItem *low9 = new QTreeWidgetItem(group);
        sprintf(tempbuf,"transport_stream_loop_length:%x",ts_nit[i].transport_stream_loop_length);
        low9->setText(0,tempbuf);
        for(int j = 0;j < ts_nit[i].nit_info_table_2.length();j++)
        {
            QTreeWidgetItem *group2 = new QTreeWidgetItem(group);
            sprintf(tempbuf,"Ts_stream_descriptor%d",j+1);
            group2->setText(0,tempbuf);
            QTreeWidgetItem *row = new QTreeWidgetItem(group2);
            sprintf(tempbuf,"transport_stream_id:%x",ts_nit[i].nit_info_table_2[j].transport_stream_id);
            row->setText(0,tempbuf);
            QTreeWidgetItem *row1 = new QTreeWidgetItem(group2);
            sprintf(tempbuf,"original_network_id:%x",ts_nit[i].nit_info_table_2[j].original_network_id);
            row1->setText(0,tempbuf);
            QTreeWidgetItem *row2 = new QTreeWidgetItem(group2);
            sprintf(tempbuf,"transport_descriptors_length:%x",ts_nit[i].nit_info_table_2[j].transport_descriptors_length);
            row2->setText(0,tempbuf);
            for(int k = 0;k < ts_nit[i].nit_info_table_2[j].service_list.length();k++)
            {
                QTreeWidgetItem *group3 = new QTreeWidgetItem(group2);
                group3->setText(0,"service_list_descriptor");

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

            for(int k = 0;k < ts_nit[i].nit_info_table_2[j].private_data.length();k++)
            {
                QTreeWidgetItem *group4 = new QTreeWidgetItem(group2);
                group4->setText(0,"private_data_descriptor");

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

            for(int k = 0;k < ts_nit[i].nit_info_table_2[j].ter_delivery.length();k++)
            {
                QTreeWidgetItem *group5 = new QTreeWidgetItem(group2);
                group5->setText(0,"terrestrial_delivery_system_descriptor");

                QTreeWidgetItem *Item1 = new QTreeWidgetItem(group5);
                sprintf(tempbuf,"descriptor_tag:%x",ts_nit[i].nit_info_table_2[j].ter_delivery[k].descriptor_tag);
                Item1->setText(0,tempbuf);
                QTreeWidgetItem *Item2 = new QTreeWidgetItem(group5);
                sprintf(tempbuf,"descriptor_length:%x",ts_nit[i].nit_info_table_2[j].ter_delivery[k].descriptor_length);
                Item2->setText(0,tempbuf);
                QTreeWidgetItem *Item3 = new QTreeWidgetItem(group5);
                sprintf(tempbuf,"centre_frequency:%d0Hz",ts_nit[i].nit_info_table_2[j].ter_delivery[k].centre_frequency);
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

            for(int k = 0;k < ts_nit[i].nit_info_table_2[j].cal_delivery.length();k++)
            {
                QTreeWidgetItem *group6 = new QTreeWidgetItem(group2);
                group6->setText(0,"cable_delivery_system_descriptor");

                QTreeWidgetItem *Item1 = new QTreeWidgetItem(group6);
                sprintf(tempbuf,"descriptor_tag:%x",ts_nit[i].nit_info_table_2[j].cal_delivery[k].descriptor_tag);
                Item1->setText(0,tempbuf);
                QTreeWidgetItem *Item2 = new QTreeWidgetItem(group6);
                sprintf(tempbuf,"descriptor_length:%x",ts_nit[i].nit_info_table_2[j].cal_delivery[k].descriptor_length);
                Item2->setText(0,tempbuf);
                QTreeWidgetItem *Item3 = new QTreeWidgetItem(group6);
                sprintf(tempbuf,"frequency:%x%x%x%x,%x%x%x%xMHz",ts_nit[i].nit_info_table_2[j].cal_delivery[k].frequency >> 28
                                                                ,ts_nit[i].nit_info_table_2[j].cal_delivery[k].frequency >> 24 & 0x0f
                                                                ,ts_nit[i].nit_info_table_2[j].cal_delivery[k].frequency >> 20 & 0x0f
                                                                ,ts_nit[i].nit_info_table_2[j].cal_delivery[k].frequency >> 16 & 0x0f
                                                                ,ts_nit[i].nit_info_table_2[j].cal_delivery[k].frequency >> 12 & 0x0f
                                                                ,ts_nit[i].nit_info_table_2[j].cal_delivery[k].frequency >> 8 & 0x0f
                                                                ,ts_nit[i].nit_info_table_2[j].cal_delivery[k].frequency >> 4 & 0x0f
                                                                ,ts_nit[i].nit_info_table_2[j].cal_delivery[k].frequency & 0x0f);
                Item3->setText(0,tempbuf);
                QTreeWidgetItem *Item4 = new QTreeWidgetItem(group6);
                sprintf(tempbuf,"FEC_outer:%x",ts_nit[i].nit_info_table_2[j].cal_delivery[k].FEC_outer);
                Item4->setText(0,tempbuf);
                QTreeWidgetItem *Item5 = new QTreeWidgetItem(group6);
                sprintf(tempbuf,"modulation:%x",ts_nit[i].nit_info_table_2[j].cal_delivery[k].modulation);
                Item5->setText(0,tempbuf);
                QTreeWidgetItem *Item6 = new QTreeWidgetItem(group6);
                sprintf(tempbuf,"symbol_rate:%x%x%x.%x%x%x%x Msymbol/s",ts_nit[i].nit_info_table_2[j].cal_delivery[k].symbol_rate >> 24 & 0x0f
                                                             ,ts_nit[i].nit_info_table_2[j].cal_delivery[k].symbol_rate >> 20 & 0x0f
                                                             ,ts_nit[i].nit_info_table_2[j].cal_delivery[k].symbol_rate >> 16 & 0x0f
                                                             ,ts_nit[i].nit_info_table_2[j].cal_delivery[k].symbol_rate >> 12 & 0x0f
                                                             ,ts_nit[i].nit_info_table_2[j].cal_delivery[k].symbol_rate >> 8 & 0x0f
                                                             ,ts_nit[i].nit_info_table_2[j].cal_delivery[k].symbol_rate >> 4 & 0x0f
                                                             ,ts_nit[i].nit_info_table_2[j].cal_delivery[k].symbol_rate & 0x0f);
                Item6->setText(0,tempbuf);
                QTreeWidgetItem *Item7 = new QTreeWidgetItem(group6);
                sprintf(tempbuf,"FEC_inner:%x",ts_nit[i].nit_info_table_2[j].cal_delivery[k].FEC_inner);
                Item7->setText(0,tempbuf);
            }

            for(int k = 0;k < ts_nit[i].nit_info_table_2[j].un_descr.length();k++)
            {
                QTreeWidgetItem *group7 = new QTreeWidgetItem(group2);
                group7->setText(0,"unknown_descriptor");

                QTreeWidgetItem *Item1 = new QTreeWidgetItem(group7);
                sprintf(tempbuf,"descriptor_tag:%x",ts_nit[i].nit_info_table_2[j].un_descr[k].descriptor_tag);
                Item1->setText(0,tempbuf);
                QTreeWidgetItem *Item2 = new QTreeWidgetItem(group7);
                sprintf(tempbuf,"descriptor_length:%x",ts_nit[i].nit_info_table_2[j].un_descr[k].descriptor_length);
                Item2->setText(0,tempbuf);
                QTreeWidgetItem *Item3 = new QTreeWidgetItem(group7);
                Item3->setText(0,"data:");
                memset(tempbuf,0,strlen(tempbuf));
                QTreeWidgetItem *title = new QTreeWidgetItem(Item3);
                for(int m = 0;m < ts_nit[i].nit_info_table_2[j].un_descr[k].data.length();m++)
                {     
                    title->setText(0,ts_nit[i].nit_info_table_2[j].un_descr[k].data[m]);
                }
            }

            for(int k = 0;k < ts_nit[i].nit_info_table_2[j].sat_delivery.length();k++)
            {
                QTreeWidgetItem *group8 = new QTreeWidgetItem(group2);
                group8->setText(0,"Satellite_delivery_system_descriptor");

                QTreeWidgetItem *Item1 = new QTreeWidgetItem(group8);
                sprintf(tempbuf,"descriptor_tag:%x",ts_nit[i].nit_info_table_2[j].sat_delivery[k].descriptor_tag);
                Item1->setText(0,tempbuf);
                QTreeWidgetItem *Item2 = new QTreeWidgetItem(group8);
                sprintf(tempbuf,"descriptor_length:%x",ts_nit[i].nit_info_table_2[j].sat_delivery[k].descriptor_length);
                Item2->setText(0,tempbuf);
                QTreeWidgetItem *Item3 = new QTreeWidgetItem(group8);
                sprintf(tempbuf,"frequency:%x%x%x.%x%x%x%x%xGHz",ts_nit[i].nit_info_table_2[j].sat_delivery[k].frequency >> 28
                                                                ,ts_nit[i].nit_info_table_2[j].sat_delivery[k].frequency >> 24 & 0x0f
                                                                ,ts_nit[i].nit_info_table_2[j].sat_delivery[k].frequency >> 20 & 0x0f
                                                                ,ts_nit[i].nit_info_table_2[j].sat_delivery[k].frequency >> 16 & 0x0f
                                                                ,ts_nit[i].nit_info_table_2[j].sat_delivery[k].frequency >> 12 & 0x0f
                                                                ,ts_nit[i].nit_info_table_2[j].sat_delivery[k].frequency >> 8 & 0x0f
                                                                ,ts_nit[i].nit_info_table_2[j].sat_delivery[k].frequency >> 4 & 0x0f
                                                                ,ts_nit[i].nit_info_table_2[j].sat_delivery[k].frequency & 0x0f);
                Item3->setText(0,tempbuf);
                QTreeWidgetItem *Item4 = new QTreeWidgetItem(group8);
                sprintf(tempbuf,"orbital_position:%x%x%x.%x`",ts_nit[i].nit_info_table_2[j].sat_delivery[k].orbital_position >> 12 & 0x0f
                                                             ,ts_nit[i].nit_info_table_2[j].sat_delivery[k].orbital_position >> 8 & 0x0f
                                                             ,ts_nit[i].nit_info_table_2[j].sat_delivery[k].orbital_position >> 4 & 0x0f
                                                             ,ts_nit[i].nit_info_table_2[j].sat_delivery[k].orbital_position & 0x0f);
                Item4->setText(0,tempbuf);
                QTreeWidgetItem *Item5 = new QTreeWidgetItem(group8);
                sprintf(tempbuf,"west_east_flag:%x",ts_nit[i].nit_info_table_2[j].sat_delivery[k].west_east_flag);
                Item5->setText(0,tempbuf);
                QTreeWidgetItem *Item6 = new QTreeWidgetItem(group8);
                sprintf(tempbuf,"polarization:%x",ts_nit[i].nit_info_table_2[j].sat_delivery[k].polarization);
                Item6->setText(0,tempbuf);
                QTreeWidgetItem *Item7 = new QTreeWidgetItem(group8);
                sprintf(tempbuf,"modulation:%x",ts_nit[i].nit_info_table_2[j].sat_delivery[k].modulation);
                Item7->setText(0,tempbuf);
                QTreeWidgetItem *Item8 = new QTreeWidgetItem(group8);
                sprintf(tempbuf,"symbol_rate:%x%x%x.%x%x%x%xMsymbol/s",ts_nit[i].nit_info_table_2[j].sat_delivery[k].symbol_rate >> 24 & 0x0f
                                                             ,ts_nit[i].nit_info_table_2[j].sat_delivery[k].symbol_rate >> 20 & 0x0f
                                                             ,ts_nit[i].nit_info_table_2[j].sat_delivery[k].symbol_rate >> 16 & 0x0f
                                                             ,ts_nit[i].nit_info_table_2[j].sat_delivery[k].symbol_rate >> 12 & 0x0f
                                                             ,ts_nit[i].nit_info_table_2[j].sat_delivery[k].symbol_rate >> 8 & 0x0f
                                                             ,ts_nit[i].nit_info_table_2[j].sat_delivery[k].symbol_rate >> 4 & 0x0f
                                                             ,ts_nit[i].nit_info_table_2[j].sat_delivery[k].symbol_rate & 0x0f);
                Item8->setText(0,tempbuf);
                QTreeWidgetItem *Item9 = new QTreeWidgetItem(group8);
                sprintf(tempbuf,"FEC_inner:%x",ts_nit[i].nit_info_table_2[j].sat_delivery[k].FEC_inner);
                Item9->setText(0,tempbuf);
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

/*
* function descriptor:Initializes the PSI information tree
* input:null
* output:null
* return:void
*/
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

/*
* function descriptor:Parsing table pat
* input:
*   TS_PAT *packet:Used to store the last parsed pat section table information
*   unsigned char *buffer:The byte content read
* output:null
* return:void
*/
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
/*
* function descriptor:Determine the type based on the parameter type
* input:
*   char *buffer:Store the type by text
*   unsigned int type:the type of stream
* output:null
* return:void
*/
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

/*
* function descriptor:Parsing table pmt
* input:
*   TS_PMT *packet:Used to store the last parsed pmt section table information
*   unsigned char *buffer:The byte content read
* output:null
* return:void
*/
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

        flag_get_pmt = packet->section_length + 4;
        //printf("get 1\n");
        long_buf_pmt = new char[packet->section_length + 4];
        for(int i = 0;i < 183;i++)
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

    packet->ca_desc.clear();
    packet->mul_desc.clear();
    packet->maxi_desc.clear();
    packet->smoo_desc.clear();
    packet->sys_cloc_desc.clear();

    if ( packet->program_info_length != 0 )
    {
        for(int i = 0;i < packet->program_info_length;)
        {
            if(buffer[pos + i] == 0x09)
            {
                TS_CAT_info *ca_des = new TS_CAT_info;
                ca_des->descriptor_tag    = buffer[pos + i];
                ca_des->descriptor_length = buffer[pos + 1 + i];
                ca_des->CA_system_ID 	  = buffer[pos + 2 + i] << 8 | buffer[pos + 3 + i];
                ca_des->reserved_3 		  = buffer[pos + 4 + i] >> 5;
                ca_des->CA_PID 		      = (buffer[pos + 4 + i] & 0x1F) << 8 | buffer[pos + 5 + i];
                packet->ca_desc.push_back(*ca_des);
                delete ca_des;
            }
            else if(buffer[pos + i] == 0xc)
            {
                multiplex_buffer_utilization_descriptor *mul_des = new multiplex_buffer_utilization_descriptor;
                mul_des->descriptor_tag         = buffer[pos + i];
                mul_des->descriptor_length      = buffer[pos + 1 + i];
                mul_des->bound_valid_flag       = buffer[pos + 2 + i] >> 7;
                mul_des->LTW_offset_lower_bound = (buffer[pos + 2 + i] & 0x7f) << 8 | buffer[pos + 3 + i];
                mul_des->reserved               = buffer[pos + 4 + i] >> 7;
                mul_des->LTW_offset_upper_bound = (buffer[pos + 4 + i] & 0x7f) << 8 | buffer[pos + 5 + i];
                packet->mul_desc.push_back(*mul_des);
                delete mul_des;
            }
            else if(buffer[pos + i] == 0xe)
            {
                maximum_bitrate_descriptor *max_des = new maximum_bitrate_descriptor;
                max_des->descriptor_tag     = buffer[pos + i];
                max_des->descriptor_length  = buffer[pos + 1 + i];
                max_des->maximum_bitrate    = (buffer[pos + 2 + i] & 0x3f) << 16
                                             | buffer[pos + 3 + i] << 8
                                             | buffer[pos + 4 + i];
                packet->maxi_desc.push_back(*max_des);
                delete max_des;
            }
            else if(buffer[pos + i] == 0xb)
            {
                system_clock_descriptor *sys_des = new system_clock_descriptor;
                sys_des->descriptor_tag                     = buffer[pos + i];
                sys_des->descriptor_length                  = buffer[pos + 1 + i];
                sys_des->external_clock_reference_indicator = buffer[pos + 2 + i] >> 7;
                sys_des->clock_accuracy_integer             = buffer[pos + 2 + i] & 0x3f;
                sys_des->clock_accuracy_exponent            = buffer[pos + 3 + i] >> 5;
                packet->sys_cloc_desc.push_back(*sys_des);
                delete sys_des;
            }
            else if(buffer[pos + i] == 0x10)
            {
                smoothing_buffer_descriptor *smoo_des = new smoothing_buffer_descriptor;
                smoo_des->descriptor_tag                     = buffer[pos + i];
                smoo_des->descriptor_length                  = buffer[pos + 1 + i];
                smoo_des->sb_leak_rate                       = (buffer[pos + 2 + i] & 0x3f) << 16 |
                                                               buffer[pos + 3 + i] << 8 |
                                                               buffer[pos + 4 + i];
                smoo_des->sb_size                            = (buffer[pos + 5 + i] & 0x3f) << 16 |
                                                               buffer[pos + 6 + i] << 8 |
                                                               buffer[pos + 7 + i];
                packet->smoo_desc.push_back(*smoo_des);
                delete smoo_des;
            }
            else if(buffer[pos + i] == 0x3)
            {
                audio_stream_descriptor *aud = new audio_stream_descriptor;
                aud->descriptor_tag                          = buffer[pos + i];
                aud->descriptor_length                       = buffer[pos + i + 1];
                aud->free_format_flag                        = buffer[pos + i + 2] >> 7;
                aud->ID                                      = (buffer[pos + i + 2] >> 6) & 0x1;
                aud->layer                                   = (buffer[pos + i + 2] >> 4) & 0x3;
                aud->variable_rate_audio_indicator           = buffer[pos + i + 2] & 0x08;
                packet->audi_desc.push_back(*aud);
                delete aud;
            }
            else if(buffer[pos + i] == 0xa)
            {
                ISO_639_language_descriptor *lang_des = new ISO_639_language_descriptor;
                lang_des->descriptor_tag              = buffer[pos + i];
                lang_des->descriptor_length           = buffer[pos + i + 1];
                for(int j = 0;j < lang_des->descriptor_length;)
                {
                    language_node *node = new language_node;
                    node->ISO_639_language_code = buffer[pos + i + 2 + j] << 16 |
                                                  buffer[pos + i + 3 + j] << 8  |
                                                  buffer[pos + i + 4 + j];
                    node->audio_type            = buffer[pos + i + 5 + j];
                    lang_des->lang_code.push_back(*node);
                    delete node;
                    j += 4;
                }
                packet->ISO_desc.push_back(*lang_des);
                delete lang_des;
            }
            else if(buffer[pos + i] == 0x52)
            {
                stream_identifier_descriptor *stream_des = new stream_identifier_descriptor;
                stream_des->descriptor_tag      = buffer[pos + i];
                stream_des->descriptor_length   = buffer[pos + i + 1];
                stream_des->component           = buffer[pos + i + 2];
                packet->stream_desc.push_back(*stream_des);
                delete stream_des;
            }
            i += buffer[pos + 1 + i] + 2;
        }
        pos += packet->program_info_length;
    }
    packet->ts_pmt_stream.clear();
    for ( ; pos < packet->section_length  -  4; )
    {
        TS_PMT_Stream *pmt_stream = new TS_PMT_Stream;
        pmt_stream->stream_type =  buffer[pos];
        //packet->reserved_5  =  buffer[pos+1] >> 5;
        pmt_stream->elementary_PID = ((buffer[pos + 1] << 8) | buffer[pos + 2]) & 0x1FFF;
        //packet->reserved_6    =  buffer[pos + 3] >> 4;
        pmt_stream->ES_info_length =  (buffer[pos + 3] & 0x0F) << 8 | buffer[pos + 4];
        category_type(tempbuf,pmt_stream->stream_type);
        pmt_stream->type_descriptor = QString(QLatin1String(tempbuf));
        stream_info *s_info = new stream_info;
        s_info->PID = pmt_stream->elementary_PID;
        s_info->type = QString(tempbuf);
        b_info->stream_information.push_back(*s_info);
        delete s_info;
        //pmt_stream->descriptor = 0x00;
        int temp = 0;
        while (temp < pmt_stream->ES_info_length)
        {
            if(buffer[pos + 5 + temp] == 0x09)
            {
                TS_CAT_info *ca_des = new TS_CAT_info;
                ca_des->descriptor_tag    = buffer[pos + 5 + temp];
                ca_des->descriptor_length = buffer[pos + 6 + temp];
                ca_des->CA_system_ID 	  = buffer[pos + 7 + temp] << 8 | buffer[pos + 8 + temp];
                ca_des->reserved_3 		  = buffer[pos + 9 + temp] >> 5;
                ca_des->CA_PID 		      = (buffer[pos + 9 + temp] & 0x1F) << 8 | buffer[pos + 10 + temp];
                pmt_stream->ca_desc.push_back(*ca_des);
                delete ca_des;
            }
            else if(buffer[pos + 5 + temp] == 0xc)
            {
                multiplex_buffer_utilization_descriptor *mul_des = new multiplex_buffer_utilization_descriptor;
                mul_des->descriptor_tag         = buffer[pos + 5 + temp];
                mul_des->descriptor_length      = buffer[pos + 6 + temp];
                mul_des->bound_valid_flag       = buffer[pos + 7 + temp] >> 7;
                mul_des->LTW_offset_lower_bound = (buffer[pos + 7 + temp] & 0x7f) << 8 | buffer[pos + 8 + temp];
                mul_des->reserved               = buffer[pos + 9 + temp] >> 7;
                mul_des->LTW_offset_upper_bound = (buffer[pos + 9 + temp] & 0x7f) << 8 | buffer[pos + 10 + temp];
                pmt_stream->mul_desc.push_back(*mul_des);
                delete mul_des;
            }
            else if(buffer[pos + 5 + temp] == 0xe)
            {
                maximum_bitrate_descriptor *max_des = new maximum_bitrate_descriptor;
                max_des->descriptor_tag     = buffer[pos + 5 + temp];
                max_des->descriptor_length  = buffer[pos + 6 + temp];
                max_des->maximum_bitrate    = (buffer[pos + 7 + temp] & 0x3f) << 16
                                             | buffer[pos + 8 + temp] << 8
                                             | buffer[pos + 9 + temp];
                pmt_stream->maxi_desc.push_back(*max_des);
                delete max_des;
            }
            else if(buffer[pos + 5 + temp] == 0xb)
            {
                system_clock_descriptor *sys_des = new system_clock_descriptor;
                sys_des->descriptor_tag                     = buffer[pos + 5 + temp];
                sys_des->descriptor_length                  = buffer[pos + 6 + temp];
                sys_des->external_clock_reference_indicator = buffer[pos + 7 + temp] >> 7;
                sys_des->clock_accuracy_integer             = buffer[pos + 7 + temp] & 0x3f;
                sys_des->clock_accuracy_exponent            = buffer[pos + 8 + temp] >> 5;
                pmt_stream->sys_cloc_desc.push_back(*sys_des);
                delete sys_des;
            }
            else if(buffer[pos + 5 + temp] == 0x10)
            {
                smoothing_buffer_descriptor *smoo_des = new smoothing_buffer_descriptor;
                smoo_des->descriptor_tag                     = buffer[pos + 5 + temp];
                smoo_des->descriptor_length                  = buffer[pos + 6 + temp];
                smoo_des->sb_leak_rate                       = (buffer[pos + 7 + temp] & 0x3f) << 16 |
                                                               buffer[pos + 8 + temp] << 8 |
                                                               buffer[pos + 9 + temp];
                smoo_des->sb_size                            = (buffer[pos + 10 + temp] & 0x3f) << 16 |
                                                               buffer[pos + 11 + temp] << 8 |
                                                               buffer[pos + 12 + temp];
                pmt_stream->smoo_desc.push_back(*smoo_des);
                delete smoo_des;
            }
            else if(buffer[pos + 5 + temp] == 0x3)
            {
                audio_stream_descriptor *aud = new audio_stream_descriptor;
                aud->descriptor_tag                          = buffer[pos + temp + 5];
                aud->descriptor_length                       = buffer[pos + temp + 6];
                aud->free_format_flag                        = buffer[pos + temp + 7] >> 7;
                aud->ID                                      = (buffer[pos + temp + 7] >> 6) & 0x1;
                aud->layer                                   = (buffer[pos + temp + 7] >> 4) & 0x3;
                aud->variable_rate_audio_indicator           = buffer[pos + temp + 7] & 0x08;
                pmt_stream->audi_desc.push_back(*aud);
                delete aud;
            }
            else if(buffer[pos + 5 + temp] == 0xa)
            {
                ISO_639_language_descriptor *lang_des = new ISO_639_language_descriptor;
                lang_des->descriptor_tag              = buffer[pos + temp + 5];
                lang_des->descriptor_length           = buffer[pos + temp + 6];
                for(int j = 0;j < lang_des->descriptor_length;)
                {
                    language_node *node = new language_node;
                    node->ISO_639_language_code = buffer[pos + temp + 7 + j] << 24 |
                                                  buffer[pos + temp + 8 + j] << 16 |
                                                  buffer[pos + temp + 9 + j] << 8  |
                                                  buffer[pos + temp + 10 + j];
                    node->audio_type            = buffer[pos + temp + 11 + j];
                    lang_des->lang_code.push_back(*node);
                    delete node;
                    j += 5;
                }
                pmt_stream->ISO_desc.push_back(*lang_des);
                delete lang_des;
            }
            else if(buffer[pos + 5 + temp] == 0x52)
            {
                stream_identifier_descriptor *stream_des = new stream_identifier_descriptor;
                stream_des->descriptor_tag      = buffer[pos + temp + 5];
                stream_des->descriptor_length   = buffer[pos + temp + 6];
                stream_des->component           = buffer[pos + temp + 7];
                pmt_stream->stream_desc.push_back(*stream_des);
                delete stream_des;
            }
           else if(buffer[pos + 5 + temp] == 0x6)
            {
                data_stream_alignment_descriptor *data_des = new data_stream_alignment_descriptor;
                data_des->descriptor_tag        = buffer[pos + temp + 5];
                data_des->descriptor_length     = buffer[pos + temp + 6];
                data_des->alignment_type        = buffer[pos + temp + 7];
                pmt_stream->data_stream_desc.push_back(*data_des);
                delete data_des;
            }
            else if (buffer[pos + 5 +temp] == 0x2)
            {
                video_stream_descriptor *vid_des = new video_stream_descriptor;
                vid_des->descriptor_tag             = buffer[pos + temp + 5];
                vid_des->descriptor_length          = buffer[pos + temp + 6];
                vid_des->multiple_frame_rate_flag   = buffer[pos + temp + 7] >> 7;
                vid_des->frame_rate_code            = (buffer[pos + temp + 7] >> 3) & 0xF;
                vid_des->MPEG_1_only_flag           = (buffer[pos + temp + 7] >> 2) & 0x1;
                vid_des->constrained_parameter_flag = (buffer[pos + temp + 7] >> 1) & 0x1;
                vid_des->still_picture_flag         = buffer[pos + temp + 7] & 0x01;
                pmt_stream->vide_desc.push_back(*vid_des);
                delete vid_des;
            }
            temp += buffer[pos + 6 + temp] + 2;
        }
        pos += pmt_stream->ES_info_length;
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

/*
* function descriptor:Parsing table cat
* input:
*   TS_CAT *packet:Used to store the last parsed cat section table information
*   unsigned char *buffer:The byte content read
* output:null
* return:void
*/
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
        ts_cat_info->descriptor_tag 		= buffer[8 + k];
        ts_cat_info->descriptor_length 		= buffer[9 + k];
        ts_cat_info->CA_system_ID 			= buffer[10 + k] << 8 | buffer[11 + k];
        ts_cat_info->reserved_3 			= buffer[12 + k] >> 5;
        ts_cat_info->CA_PID 				= (buffer[12 + k] & 0x1F) << 8 | buffer[13 + k];
        k += ts_cat_info->descriptor_length + 2;
        packet->ca_desc.push_back(*ts_cat_info);
    }
    unsigned int crc = 0xffffffff;
    crc=crc32(crc,buffer,packet->section_length-1);
    packet->check_CRC_32 = crc;
    ts_cat.push_back(*packet);
}

/*
* function descriptor:Parsing table nit
* input:
*   TS_NIT *packet:Used to store the last parsed nit section table information
*   unsigned char *buffer:The byte content read
* output:null
* return:void
*/
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
        flag_get_nit = packet->section_length + 4;
        long_buf_nit = new char[packet->section_length + 4];
        for(int i = 0;i < 183;i++)
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
        for(k = pos;k < pos+packet->network_descriptors_length;)
        {

            TS_NIT_info_1 nit_info_table;
            nit_info_table.descriptor_tag = buffer[k];
            nit_info_table.descriptor_length = buffer[k + 1];
            if (nit_info_table.descriptor_length != 0)
            {
                QString temp;
                for(k1 = 0; k1 < nit_info_table.descriptor_length ; k1++)
                {
                    sprintf(tempbuf,"%02x ",buffer[k + 2 + k1]);
                    temp += tempbuf;
                }
                nit_info_table.descriptor_char = temp;
                k1 = 0;
            }
            k += nit_info_table.descriptor_length + 2;
            packet->nit_info_table_1.push_back(nit_info_table);

        }
        pos = k;
    }
    packet->reserved_future_use_3 			= buffer[pos] >> 4;
    packet->transport_stream_loop_length	= (buffer[pos] & 0x0F) << 8 | buffer[pos + 1];
    if (packet->transport_stream_loop_length != 0)
    {
        for(k = pos + 2;k < packet->transport_stream_loop_length+ pos + 2;)
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
                else if(descriptor_tag == 0x43)
                {
                    satellite_delivery_system_descriptor s_delivery;
                    s_delivery.descriptor_tag = descriptor_tag;
                    s_delivery.descriptor_length = descriptor_length;
                    s_delivery.frequency = buffer[k + 8 + k1 ] << 24 |
                                           buffer[k + 9 + k1 ] << 16 |
                                           buffer[k + 10 + k1] << 8  |
                                           buffer[k + 11 + k1];
                    s_delivery.orbital_position = buffer[k + 12 + k1] << 8 |
                                                  buffer[k + 13 + k1];
                    s_delivery.west_east_flag = buffer[k + 14 + k1] >> 7;
                    s_delivery.polarization = buffer[k + 14 + k1] >> 5 & 0x03;
                    s_delivery.modulation = buffer[k + 14 + k1] & 0x1F;
                    s_delivery.symbol_rate = buffer[k + 15 + k1] << 20 |
                                             buffer[k + 16 + k1] << 12 |
                                             buffer[k + 17 + k1] << 4 |
                                             buffer[k + 18 + k1] >> 4;
                    s_delivery.FEC_inner = buffer[k + 18 + k1] & 0x0f;
                    nit_info_table_2.sat_delivery.push_back(s_delivery);
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
                    QString temp;
                    for(j = 0;j < descriptor_length;j++)
                    {
                        sprintf(tempbuf,"%02x ",buffer[k + 8 + k1 + j]);
                        temp += tempbuf;
                    }
                    un_descr.data.push_back(temp);
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

/*
* function descriptor:Parsing table tdt
* input:
*   TS_TDT *packet:Used to store the last parsed tdt section table information
*   unsigned char *buffer:The byte content read
* output:null
* return:void
*/
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

/*
* function descriptor:Parsing table tot
* input:
*   TS_TOT *packet:Used to store the last parsed tot section table information
*   unsigned char *buffer:The byte content read
* output:null
* return:void
*/
void MainWindow::adjust_TOT_table(TS_TOT *packet,unsigned char *buffer)
{
    packet->table_id 					= buffer[0];
    if(packet->table_id != 0x73)
    {
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

/*
* function descriptor:Parsing table eit
* input:
*   TS_EIT *packet:Used to store the last parsed eit section table information
*   unsigned char *buffer:The byte content read
* output:null
* return:void
*/
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
    int k = 0;
    unsigned int k1 = 0;
    int i = 0,j = 0,m = 0,l = 0;
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
//        Time server_time;
//        server_time.Year = Y+1900;
//        server_time.Mouth = M;
//        server_time.Day = D;
//        server_time.Hour = Hour;
//        server_time.Min = Min;
//        server_time.Sec = Sec;
        Hour = (temp_loop.duration_hour >> 4)*10 + (temp_loop.duration_hour & 0x0f);
        Min  = (temp_loop.duration_min >> 4)*10 + (temp_loop.duration_min & 0x0f);
        Sec  = (temp_loop.duration_sec >> 4)*10 + (temp_loop.duration_sec & 0x0f);
        //unsigned int duration = Hour*3600+Min*60+Sec;
//        int dispaly_flag = 0;
//        dispaly_flag = Is_Right_time(server_time,offset_time,duration);
//        if(dispaly_flag)
//        {

//        }
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
/*
* function descriptor:Parsing table bat
* input:
*   TS_BAT *packet:Used to store the last parsed bat section table information
*   unsigned char *buffer:The byte content read
* output:null
* return:void
*/
void MainWindow::adjust_BAT_table(TS_BAT *packet, unsigned char *buffer)
{
    packet->table_id                    = buffer[0];
    packet->section_syntax_indicator    = buffer[1] >> 7;
    packet->section_length              = (buffer[1] & 0x0F) << 8 | buffer[2];
    packet->bouquet_id                  = buffer[3] << 8 | buffer[4];
    packet->version_number              = (buffer[5] >> 1) & 0x1f;
    packet->current_next_indicator   	= buffer[5] & 0x01;
    packet->section_number 			 	= buffer[6];
    packet->last_section_number 		= buffer[7];
    packet->bouquet_descriptors_length 	= (buffer[8] & 0x0F) << 8 | buffer[9];
    packet->bou_desc.clear();
    packet->tran_desc.clear();
    int len = 0;
    len = packet->section_length + 2;
    packet->CRC_32  = (buffer[len-3] & 0x000000FF) << 24
                    | (buffer[len-2] & 0x000000FF) << 16
                    | (buffer[len-1] & 0x000000FF) << 8
                    | (buffer[len] & 0x000000FF);
    int pos = 10;
    for(int i = 0;i < packet->bouquet_descriptors_length;)
    {
        bouquet_name_descriptor *bou_des = new bouquet_name_descriptor;
        bou_des->descriptor_tag         = buffer[pos + i];
        bou_des->descriptor_length      = buffer[pos + i + 1];
        QString temp;
        for(int j = 0;j < bou_des->descriptor_length;j++)
        {
            sprintf(tempbuf,"%02x ",buffer[pos + i + 2 + j]);
            temp += tempbuf;
        }
        bou_des->data = temp;
        i += bou_des->descriptor_length + 2;
        packet->bou_desc.push_back(*bou_des);
        delete bou_des;
    }
    pos += packet->bouquet_descriptors_length;
    packet->transport_stream_loop_length = (buffer[pos] & 0x0F) << 8 | buffer[pos + 1];
    pos += 2;
    for(int i = 0;i < packet->transport_stream_loop_length;)
    {
        transport_stream_descriptor *tra_des = new transport_stream_descriptor;
        tra_des->transport_stram_id             = buffer[pos + i] << 8 | buffer[pos + i + 1];
        tra_des->original_network_id            = buffer[pos + i + 2] << 8 | buffer[pos + i + 3];
        tra_des->transport_descriptors_length   = (buffer[pos + i + 4] & 0x0F)<< 8 | buffer[pos + i + 5];
        for(int j = 0;j < tra_des->transport_descriptors_length;)
        {
            service_list_descriptor *ser_list_des = new service_list_descriptor;
            ser_list_des->descriptor_tag    = buffer[pos + i + 6 + j];
            ser_list_des->descriptor_length = buffer[pos + i + 7 + j];
            for(int l = 0;l < ser_list_des->descriptor_length;)
            {
                service_info *ser_info = new service_info;
                ser_info->service_id    = buffer[pos + i + 8 + j + l] << 8 | buffer[pos + i + 9 + j + l];
                ser_info->service_type  = buffer[pos + i + 10 + j + l];
                ser_list_des->nit_service.push_back(*ser_info);
                delete ser_info;
                l += 3;
            }
            tra_des->ser_desc.push_back(*ser_list_des);
            j += ser_list_des->descriptor_length + 2;
            delete ser_list_des;
        }
        packet->tran_desc.push_back(*tra_des);
        i += tra_des->transport_descriptors_length + 6;
        delete tra_des;
    }
    unsigned int crc = 0xffffffff;
    crc = crc32(crc,buffer,packet->section_length - 1);
    packet->check_CRC32 = crc;
    d.ts_bat.push_back(*packet);
}
/*
* function descriptor:Parsing table sdt
* input:
*   TS_SDT *packet:Used to store the last parsed sdt section table information
*   unsigned char *buffer:The byte content read
* output:null
* return:void
*/
void MainWindow::adjust_SDT_table(TS_SDT *packet, unsigned char *buffer)
{
    packet->table_id                    = buffer[0];
    packet->section_syntax_indicator    = buffer[1] >> 7;
    //packet->reserved_future_use1        = buffer[1] >> 6 & 0x1;
    //packet->reserved_1                  = buffer[1] >> 4 & 0x3;
    packet->section_length              = (buffer[1] & 0x0F) << 8 | buffer[2];
    packet->transport_stream_id         = buffer[3] << 8 | buffer[4];
    //packet->reserved_2 				 	= buffer[5] >> 6;
    packet->version_number 			 	= (buffer[5] >> 1) & 0x1f;
    packet->current_next_indicator   	= buffer[5] & 0x01;
    packet->section_number 			 	= buffer[6];
    packet->last_section_number 		= buffer[7];
    packet->original_network_id 		= buffer[8] << 8 | buffer[9];
    //packet->reserved_future_use2 		= buffer[10];
    packet->bus_desc.clear();
    if((packet->table_id != 0x42 && packet->table_id != 0x46 && packet->table_id != 0x4a)  && flag_get_sdt != 0)
    {
        get_all_SDT_buffer(packet,buffer,flag_get_sdt);
        return ;
    }
    if(packet->section_length > 180 && (packet->table_id == 0x42||packet->table_id == 0x46 || packet->table_id == 0x4a) && check_flag_sdt == 1)
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
        if(packet->table_id != 0x42 && packet->table_id != 0x46 && packet->table_id != 0x4a)
            return ;
    }
    if(packet->table_id == 0x4a)
    {
        BAT_number temp_bat_number;
        temp_bat_number.table_id = packet->table_id;
        temp_bat_number.section_length = packet->section_length;
        temp_bat_number.section_number = packet->section_number;
        temp_bat_number.version_number = packet->version_number;
        temp_bat_number.last_section_number = packet->last_section_number;
        for(QVector<BAT_number>::iterator it = bat_num.begin(); it != bat_num.end();it++)
        {
            if(temp_bat_number.table_id == it->table_id &&
               temp_bat_number.section_length == it->section_length &&
               temp_bat_number.section_number == it->section_number &&
               temp_bat_number.version_number == it->version_number &&
               temp_bat_number.last_section_number == it->last_section_number)
                return ;
        }
        bat_num.push_back(temp_bat_number);
        adjust_BAT_table(packet_BAT,buffer);
    }

    SDT_number temp_sdt_number;
    temp_sdt_number.table_id = packet->table_id;
    temp_sdt_number.section_length = packet->section_length;
    temp_sdt_number.section_number = packet->section_number;
    temp_sdt_number.version_number = packet->version_number;
    temp_sdt_number.last_section_number = packet->last_section_number;
    for(QVector<SDT_number>::iterator it = sdt_num.begin(); it != sdt_num.end();it++)
    {
        if(temp_sdt_number.table_id == it->table_id &&
           temp_sdt_number.section_length == it->section_length &&
           temp_sdt_number.section_number == it->section_number &&
           temp_sdt_number.version_number == it->version_number &&
           temp_sdt_number.last_section_number == it->last_section_number)
            return ;
    }
    sdt_num.push_back(temp_sdt_number);


    int len =packet->section_length + 2;
    packet->CRC_32  = (buffer[len-3] & 0x000000FF) << 24
                        | (buffer[len-2] & 0x000000FF) << 16
                        | (buffer[len-1] & 0x000000FF) << 8
                        | (buffer[len] & 0x000000FF);
    unsigned int crc = 0xffffffff;
    crc = crc32(crc,buffer,packet->section_length-1);
    unsigned int pos = 11;
    int k = 0,k1 = 0;
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
                QString temp;
                for(i = 0;i < un_des.descriptor_length;i++)
                {
                    sprintf(tempbuf,"%02x ",buffer[pos + 7 + k + k1 + i]);
                    temp += tempbuf;
                }
                un_des.data.push_back(temp);
                buss_desc.un_desc.push_back(un_des);
            }
            k1 += descriptor_length + 2;
        }
        k += buss_desc.descriptor_loop_length + 5;
        packet->bus_desc.push_back(buss_desc);
    }
    check_flag_sdt = 0;
    packet->check_CRC32 = crc;
    if(packet->table_id == 0x42)
    {
        d.ts_sdt_actual.push_back(*packet);
        packet->SDT_count_actual += 1;
    }
    if(packet->table_id == 0x46)
    {
        d.ts_sdt_other.push_back(*packet);
        packet->SDT_count_other += 1;
    }
    if(packet->SDT_count_actual == packet->last_section_number + 1)
        packet->SDT_actual = 1;
    if(packet->SDT_count_other == packet->last_section_number + 1)
        packet->SDT_other = 1;
    if(packet->SDT_actual == 1 && packet->SDT_other == 1)
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

void clearTempFiles(const QString &temp_path)
{
    QDir Dir(temp_path);
    if(Dir.isEmpty())
    {
        //std::cout << "临时文件文件夹" << temp_path.toLatin1().data() << "为空"<<endl;
        return ;
    }
    // 第三个参数是QDir的过滤参数，这三个表示收集所有文件和目录，且不包含"."和".."目录。
    // 因为只需要遍历第一层即可，所以第四个参数填QDirIterator::NoIteratorFlags
    QDirIterator DirsIterator(temp_path, QDir::Files | QDir::AllDirs | QDir::NoDotAndDotDot, QDirIterator::NoIteratorFlags);
    while(DirsIterator.hasNext())
    {
        if (!Dir.remove(DirsIterator.next())) // 删除文件操作如果返回否，那它就是目录
        {
            QDir(DirsIterator.filePath()).removeRecursively(); // 删除目录本身以及它下属所有的文件及目录
        }
    }
}

void MainWindow::clear_cache()
{
    ts_pat_program.clear();
    ts_cat.clear();
    ts_nit.clear();
    ts_pmt.clear();
    d.ts_sdt_actual.clear();
    d.ts_sdt_other.clear();
    d.ts_tdt.clear();
    d.ts_tot.clear();
    d.ts_eit_pf_other.clear();
    d.ts_eit_pf_actual.clear();
    d.ts_eit_schedule_other.clear();
    d.ts_eit_schedule_actual.clear();
    b.PID_tree.clear();
    e.epg_info.clear();
    e.category_info.clear();
    e.ts_eit_pf_actual.clear();
    e.ts_eit_schedule_actual.clear();
    s.Section_tree.clear();
    clearTempFiles(".//qtDb.db");
    check_section_flag = 0;
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
    packet_SDT->SDT_count_actual = 0;
    packet_SDT->SDT_count_other = 0;
    packet_SDT->SDT_actual = 0;
    packet_SDT->SDT_other  = 0;
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
void MainWindow::over()
{
    ui->pushButton->setEnabled(true);
}
void MainWindow::on_pushButton_clicked()
{
    ui->pushButton->setEnabled(false);
    clear_all();
    Q_pool = new QThreadPool();
    Q_pool->setMaxThreadCount(1);
    //my_db.createConnection();
    //QThreadPool::setMaxThreadCount(5);
    unsigned char *buf = new unsigned char[188];
    QString filename;
    int filesize = 0;
    filename = QFileDialog::getOpenFileName(this,tr("file"),"",tr("ts(*.ts);;mts(*.mts);;all(*)"));
    Task *task = new Task(filename);
    connect(this,SIGNAL(stop()),task,SLOT(setCancel()));
    connect(task,SIGNAL(load_over()),this,SLOT(over()));
    Q_pool->globalInstance()->start(task);
    ui->label->setText(filename);
    //fstream fp;
    QFile fp(filename);
    if(!fp.open(QIODevice::ReadOnly))
       qDebug()<<"open falid";
    else
       qDebug()<<"open success";
    QTextStream stream(&fp);
    filesize = fp.size();
    int first_size = filesize;
    while(first_size > 0)
    {
        fp.read((char *)buf,188);
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
                    fp.seek(i);
                    cout<<"move once,offest="<<i<<endl;
                    break;
                }
            }
        }
        memset(buf,0,188);
    }
    qDebug()<<"first read over"<<endl;
    check_section_flag = 1;
    fp.seek(0);
    while(filesize > 0)
    {
        fp.read((char *)buf,188);
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
                    fp.seek(i);
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
    QFuture<void> res = QtConcurrent::run(&d,&Dialog::init_tree);
    //d.init_tree();
    d.exec();
}

void MainWindow::on_pushButton_4_clicked()
{
    e.ts_eit_pf_actual = d.ts_eit_pf_actual;
    e.ts_eit_schedule_actual = d.ts_eit_schedule_actual;
    //e.init_tree();
    QFuture<void> res = QtConcurrent::run(&e,&EPG::init_tree);
    e.setWindowTitle("EPG");
    e.exec();
}

void MainWindow::on_pushButton_2_clicked()
{
    b.setWindowTitle("Basic_information");
    //b.init_tree();
    QFuture<void> res = QtConcurrent::run(&b,&basic_info::init_tree);
    b.exec();
}

void MainWindow::on_pushButton_5_clicked()
{
    s.setWindowTitle("Section_data");
    //s.my_data = &my_data;
    s.init_tree();
    //QFuture<void> res = QtConcurrent::run(&s,&section_data::init_tree);
    s.exec();
}

void MainWindow::on_pushButton_6_clicked()
{
    emit stop();
    ui->pushButton->setEnabled(true);
}
