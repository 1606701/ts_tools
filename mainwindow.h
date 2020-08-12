
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "dialog.h"
#include "epg.h"
#include "basic_info.h"
#include "section_data.h"
#include <QDirIterator>
#include "my_thread.h"
#include <QThreadPool>
#include "task.h"
#include <QMap>
#include <QFuture>
#include <QtConcurrent>
#include "database.h"

typedef struct data_list
{
    unsigned PID                        :13;
    QList<QString>  data;
}data_list;
typedef struct TS_HEADER
{
    unsigned sync_byte                  : 8; //长度为8，同步字节
    unsigned transport_error_indicator  : 1; //传输错误标识
    unsigned payload_unit_start_indicator : 1; //负载单元开始标识
    unsigned transport_priority         : 1; //优先传输标识
    unsigned PID                        : 13; //PID号
    unsigned transport_scrambling_control : 2; //传输扰乱控制
    unsigned adaptation_field_control   : 2; //自适应区域控制
    unsigned continuity_counter         : 4; //连续计数器
} TS_HEADER;


typedef struct TS_PAT
{
    unsigned table_id                    : 8; //固定为0x00 ，标志是该表是PAT
    unsigned section_syntax_indicator    : 1; //段语法标志位，固定为1
    unsigned zero                        : 1; //0
    unsigned reserved_1                  : 2; // 保留位
    unsigned section_length              : 12; //表示这个字节后面有用的字节数，包括CRC32
    unsigned transport_stream_id         : 16; //该传输流的ID，区别于一个网络中其它多路复用的流
    unsigned reserved_2                  : 2;// 保留位
    unsigned version_number              : 5; //范围0-31，表示PAT的版本号
    unsigned current_next_indicator      : 1; //发送的PAT是当前有效还是下一个PAT有效
    unsigned section_number              : 8; //分段的号码。PAT可能分为多段传输，第一段为00，以后每个分段加1，最多可能有256个分段
    unsigned last_section_number         : 8;  //最后一个分段的号码
    unsigned reserved_3                  : 3; // 保留位
    unsigned network_PID                 : 13; //网络信息表（NIT）的PID,节目号为0时对应的PID为network_PID
    unsigned CRC_32                      : 32;  //CRC32校验码
    unsigned check_CRC32                 : 32;
    unsigned Is_write                    : 1;
} TS_PAT;
typedef struct TS_CAT_info
{
    unsigned descriptor_tag	 			: 8;
    unsigned descriptor_length 			: 8;
    unsigned CA_system_ID 				: 16;
    unsigned reserved_3 				: 3;
    unsigned CA_PID 					: 13;
}TS_CAT_info;

typedef struct stream_descriptor
{
    unsigned descriptor_tag         : 8;
    unsigned descriptor_length      : 8;
    QString data;
}stream_descriptor;

typedef struct stream_identifier_descriptor //0x52
{
    unsigned descriptor_tag         : 8;
    unsigned descriptor_length      : 8;
    unsigned component              : 8;
}stream_identifier_descriptor;

typedef struct vedio_stream_descriptor //0x2
{
    unsigned descriptor_tag               : 8;
    unsigned descriptor_length            : 8;
    unsigned multiple_frame_rate_flag     : 1;
    unsigned frame_rate_code              : 4;
    unsigned MPEG_1_only_flag             : 1;
    unsigned constrained_parameter_flag   : 1;
    unsigned still_picture_flag           : 1;
    unsigned profile_and_level_indication : 8;
    unsigned chroma_format                : 2;
    unsigned frame_rate_extension         : 1;
    unsigned reserved                     : 5;
}vedio_stream_descriptor;

typedef struct data_stream_alignment_descriptor //0x6
{
    unsigned descriptor_tag         : 8;
    unsigned descriptor_length      : 8;
    unsigned alignment_type         : 8;
}data_stream_alignment_descriptor;

typedef struct maximum_bitrate_descriptor //0xe
{
    unsigned descriptor_tag         : 8;
    unsigned descriptor_length      : 8;
    unsigned reserved               : 2;
    unsigned maximum_bitrate        : 22;
}maximum_bitrate_descriptor;

typedef struct multiplex_buffer_utilization_descriptor //0xc
{
    unsigned descriptor_tag         : 8;
    unsigned descriptor_length      : 8;
    unsigned bound_valid_flag       : 1;
    unsigned LTW_offset_lower_bound : 15;
    unsigned reserved               : 1;
    unsigned LTW_offset_upper_bound : 14;
}multiplex_buffer_utilization_descriptor;

typedef struct system_clock_descriptor //0xb
{
    unsigned descriptor_tag                     : 8;
    unsigned descriptor_length                  : 8;
    unsigned external_clock_reference_indicator : 1;
    unsigned clock_accuracy_integer             : 6;
    unsigned clock_accuracy_exponent            : 3;
}system_clock_descriptor;

typedef struct  smoothing_buffer_descriptor //0x10
{
    unsigned descriptor_tag                     : 8;
    unsigned descriptor_length                  : 8;
    unsigned sb_leak_rate                       : 22;
    unsigned sb_size                            : 22;
}smoothing_buffer_descriptor;

typedef struct audio_stream_descriptor
{
    unsigned descriptor_tag                     : 8;
    unsigned descriptor_length                  : 8;
    unsigned free_format_flag                   : 1;
    unsigned ID                                 : 1;
    unsigned layer                              : 2;
    unsigned variable_rate_audio_indicator      : 1;
}audio_stream_descriptor;

typedef struct video_stream_descriptor
{
    unsigned descriptor_tag                     : 8;
    unsigned descriptor_length                  : 8;
    unsigned multiple_frame_rate_flag           : 1;
    unsigned frame_rate_code                    : 4;
    unsigned MPEG_1_only_flag                   : 1;
    unsigned constrained_parameter_flag         : 1;
    unsigned still_picture_flag                 : 1;
}video_stream_descriptor;

typedef struct language_node
{
    unsigned ISO_639_language_code :24;
    unsigned audio_type            :8;
}language_node;
typedef struct ISO_639_language_descriptor
{
    unsigned descriptor_tag         : 8;
    unsigned descriptor_length      : 8;
    QVector<language_node> lang_code;
}ISO_639_language_descriptor;

typedef struct TS_PMT_Stream
{
    unsigned stream_type    	  	: 8; //指示特定PID的节目元素包的类型。该处PID由elementary PID指定
    unsigned elementary_PID  		: 13; //该域指示TS包的PID值。这些TS包含有相关的节目元素
    unsigned ES_info_length  		: 12; //前两位bit为00。该域指示跟随其后的描述相关节目元素的byte数
    QString type_descriptor;
    //QVector<stream_descriptor> stream_desc;

    QVector<TS_CAT_info> ca_desc;
    QVector<stream_identifier_descriptor> stream_desc;
    QVector<video_stream_descriptor> vide_desc;
    QVector<multiplex_buffer_utilization_descriptor> mul_desc;
    QVector<maximum_bitrate_descriptor> maxi_desc;
    QVector<system_clock_descriptor> sys_cloc_desc;
    QVector<smoothing_buffer_descriptor> smoo_desc;
    QVector<audio_stream_descriptor> audi_desc;
    QVector<ISO_639_language_descriptor> ISO_desc;
    QVector<data_stream_alignment_descriptor> data_stream_desc;

}TS_PMT_Stream;
typedef struct TS_PMT
{
    unsigned table_id                   : 8; //固定为0x02, 表示PMT表
    unsigned section_syntax_indicator   : 1; //固定为0x01
    unsigned zero                       : 1; //0x01
    unsigned reserved_1                 : 2; //0x03
    unsigned section_length 			: 12;//首先两位bit置为00，它指示段的byte数，由段长度域开始，包含CRC
    unsigned program_number             : 16;// 指出该节目对应于可应用的Program map PID
    unsigned reserved_2                 : 2; //0x03
    unsigned version_number             : 5; //指出TS流中Program map section的版本号
    unsigned current_next_indicator  	: 1; //当该位置1时，当前传送的Program map section可用
     //当该位置0时，指示当前传送的Program map section不可用，下一个TS流的Program map section有效
    unsigned section_number            	: 8; //固定为0x00
    unsigned last_section_number      	: 8; //固定为0x00
    unsigned reserved_3               	: 3; //0x07
    unsigned PCR_PID                   	: 13; //指明TS包的PID值，该TS包含有PCR域，
      //该PCR值对应于由节目号指定的对应节目，如果对于私有数据流的节目定义与PCR无关，这个域的值将为0x1FFF。
    unsigned reserved_4            		: 4;  //预留为0x0F
    unsigned program_info_length  		: 12; //前两位bit为00。该域指出跟随其后对节目信息的描述的byte数。

    QVector<TS_CAT_info> ca_desc;
    QVector<stream_identifier_descriptor> stream_desc;
    QVector<multiplex_buffer_utilization_descriptor> mul_desc;
    QVector<maximum_bitrate_descriptor> maxi_desc;
    QVector<system_clock_descriptor> sys_cloc_desc;
    QVector<smoothing_buffer_descriptor> smoo_desc;
    QVector<audio_stream_descriptor> audi_desc;
    QVector<ISO_639_language_descriptor> ISO_desc;

    QVector<TS_PMT_Stream> ts_pmt_stream;
    unsigned reserved_5                	: 3; //0x07
    unsigned reserved_6                	: 4; //0x0F
    unsigned CRC_32                    	: 32;
    unsigned check_CRC32                : 32;
    unsigned PMT_conut                  : 8;
    bool operator <(const TS_PMT& rhs) const
    {
        if(program_number != rhs.program_number)
            return program_number < rhs.program_number;
        else
            return section_length < rhs.section_length;
    }
    bool operator ==(const TS_PMT& rhs) const
    {
        return program_number == rhs.program_number;
    }
}TS_PMT;

typedef struct TS_CAT
{
    unsigned table_id 				  	: 8;
    unsigned section_syntax_indicator 	: 1;
    unsigned zero						: 1;
    unsigned reserved_1 				: 2;
    unsigned section_length 			: 12;
    unsigned reserved_2 				: 18;
    unsigned version_number 			: 5;
    unsigned current_next_indicator 	: 1;
    unsigned section_number 			: 8;
    unsigned last_section_number 		: 8;
    QVector<TS_CAT_info> ca_desc;
    unsigned CRC_32						: 32;
    unsigned check_CRC_32				: 32;
    unsigned Is_write                   : 1;
}TS_CAT;

typedef struct TS_NIT_info_1
{
    unsigned descriptor_tag		  	  		: 8;
    unsigned descriptor_length  			: 8;
    QString descriptor_char;
}TS_NIT_info_1;
//typedef struct service_info
//{
//    unsigned service_id             :16;
//    unsigned service_type           :8;
//}service_info;
//typedef struct service_list_descriptor
//{
//    unsigned descriptor_tag         :8;
//    unsigned descriptor_length      :8;
//    QVector<service_info> nit_service;
//}service_list_descriptor;

typedef struct terrestrial_delivery_system_descriptor
{
    unsigned descriptor_tag         :8;
    unsigned descriptor_length      :8;
    unsigned centre_frequency       :32;
    unsigned bandwidth              :3;
    unsigned constellation          :2;
    unsigned hierarchy_information  :3;
    unsigned code_rate_HP_stream    :3;
    unsigned code_rate_LP_stream    :3;
    unsigned guard_interval         :2;
    unsigned transmission_mode      :2;
    unsigned other_frequency_flag   :1;
}terrestrial_delivery_system_descriptor;
typedef struct cable_delivery_system_descriptor
{
    unsigned descriptor_tag         :8;
    unsigned descriptor_length      :8;
    unsigned frequency              :32;
    unsigned FEC_outer              :4;
    unsigned modulation             :8;
    unsigned symbol_rate            :28;
    unsigned FEC_inner              :4;
}cable_delivery_system_descriptor;

typedef struct satellite_delivery_system_descriptor
{
    unsigned descriptor_tag                 :8;
    unsigned descriptor_length              :8;
    unsigned frequency                      :32;
    unsigned orbital_position               :16;
    unsigned west_east_flag                 :1;
    unsigned polarization                   :2;
    unsigned modulation                     :5;
    unsigned symbol_rate                    :28;
    unsigned FEC_inner                      :4;
}satellite_delivery_system_descriptor;
typedef struct TS_NIT_info_2
{
    unsigned transport_stream_id			: 16;
    unsigned original_network_id			: 16;
    unsigned reserved_future_use 			: 4;
    unsigned transport_descriptors_length	: 12;
    QVector<service_list_descriptor> service_list;
    QVector<private_data_specifier_descriptor> private_data;
    QVector<terrestrial_delivery_system_descriptor> ter_delivery;
    QVector<cable_delivery_system_descriptor> cal_delivery;
    QVector<satellite_delivery_system_descriptor> sat_delivery;
    QVector<unknown_descriptor> un_descr;
}TS_NIT_info_2;

typedef struct TS_NIT
{
    unsigned table_id  					: 8;
    unsigned section_syntax_indicator   : 1;
    unsigned reserved_future_use_1		: 1;
    unsigned reserved_1                 : 2;
    unsigned section_length				: 12;
    unsigned network_id 				: 16;
    unsigned reserved_2					: 2;
    unsigned version_number				: 5;
    unsigned current_next_indicator		: 1;
    unsigned section_number 			: 8;
    unsigned last_section_number		: 8;
    unsigned reserved_future_use_2  	: 4;
    unsigned network_descriptors_length	: 12;
    QVector<TS_NIT_info_1> nit_info_table_1;
    unsigned reserved_future_use_3 		: 4;
    unsigned transport_stream_loop_length	: 12;
    QVector<TS_NIT_info_2> nit_info_table_2;
    unsigned CRC_32                     : 32;  //CRC32校验码
    unsigned check_CRC32                : 32;
    unsigned Is_write                   : 1;
    unsigned NIT_conut                  : 8;
}TS_NIT;





typedef struct EIT_number
{
    unsigned version_number 			: 5;
    unsigned section_number 			: 8;
    unsigned last_section_number        : 8;
    unsigned section_length				: 12;
    unsigned service_id 				: 16;
}EIT_number;
typedef struct SDT_number
{
    unsigned version_number 			: 5;
    unsigned section_number 			: 8;
    unsigned last_section_number        : 8;
    unsigned section_length				: 12;
    unsigned table_id                   : 8;
}SDT_number;
typedef struct BAT_number
{
    unsigned version_number 			: 5;
    unsigned section_number 			: 8;
    unsigned last_section_number        : 8;
    unsigned section_length				: 12;
    unsigned table_id                   : 8;
}BAT_number;
typedef struct TS_PAT_Program
{
    unsigned program_number   : 16;//节目号
    unsigned program_map_PID  : 13;
    //节目映射表的pid，节目号大于0时，每个节目对应一个
}TS_PAT_Program;


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    char *tempbuf = new char[1000];
    int pat_flag = 0;
    unsigned int length_nit = 0;
    unsigned int length_eit = 0;
    unsigned int length_sdt = 0;
    unsigned int length_pmt = 0;
    char *long_buf_nit=NULL;
    char *long_buf_eit=NULL;
    char *long_buf_sdt=NULL;
    char *long_buf_pmt=NULL;
    unsigned int flag_get_nit = 0;
    unsigned int flag_get_eit = 0;
    unsigned int flag_get_sdt = 0;
    unsigned int flag_get_pmt = 0;
    unsigned int flag_start = 0;
    unsigned int flag_start_nit = 0;
    unsigned int check_section_flag = 0;
    int check_flag_nit = 0;
    int check_flag_eit = 0;
    int check_flag_sdt = 0;
    int check_flag_pmt = 0;
    Time offset_time;

    Dialog d;
    EPG e;
    basic_info b;
    section_data s;

    TS_PAT *packet_PAT = new TS_PAT;
    TS_PMT *packet_PMT = new TS_PMT;
    TS_CAT *packet_CAT = new TS_CAT;
    TS_NIT *packet_NIT = new TS_NIT;
    TS_SDT *packet_SDT = new TS_SDT;
    TS_BAT *packet_BAT = new TS_BAT;
    TS_TOT *packet_TOT = new TS_TOT;
    TS_TDT *packet_TDT = new TS_TDT;
    TS_EIT *packet_EIT = new TS_EIT;

    QVector<EIT_number> eit_num;
    QVector<SDT_number> sdt_num;
    QVector<BAT_number> bat_num;

    QVector<unsigned int> PMT_PID;
    QVector<TS_PAT_Program> ts_pat_program;
    QVector<TS_PMT> ts_pmt;
    QVector<TS_CAT> ts_cat;
    QVector<TS_NIT> ts_nit;

    QThreadPool *Q_pool;
    Task *task;
    //database my_db;
    //QMultiMap<int,QString> my_data;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_2_clicked();

    void on_pushButton_6_clicked();

private:
    void process_packet(unsigned char *buf);
    void process_first(unsigned char *buf);
    void init_tree();
    void init_PAT_tree();
    void init_PMT_tree();
    void init_CAT_tree();
    void init_NIT_tree();
    void adjust_packet_header(TS_HEADER *packet,unsigned char *buffer);
    void adjust_PAT_table(TS_PAT *packet,unsigned char *buffer);
    void adjust_PMT_table(TS_PMT *packet,unsigned char *buffer,unsigned int PID);
    void adjust_NIT_table(TS_NIT *packet,unsigned char *buffer);
    void adjust_CAT_table(TS_CAT *packet,unsigned char *buffer);

    void adjust_TDT_table(TS_TDT *packet,unsigned char *buffer);
    void adjust_TOT_table(TS_TOT *packet,unsigned char *buffer);
    void adjust_EIT_table(TS_EIT *packet,unsigned char *buffer);
    void adjust_SDT_table(TS_SDT *packet,unsigned char *buffer);
    void adjust_BAT_table(TS_BAT *packet,unsigned char *buffer);

    void get_all_NIT_buffer(TS_NIT *packet,unsigned char *buffer,unsigned int len);
    void get_all_EIT_buffer(TS_EIT *packet,unsigned char *buffer,unsigned int len);
    void get_all_SDT_buffer(TS_SDT *packet,unsigned char *buffer,unsigned int len);
    void get_all_PMT_buffer(TS_PMT *packet,unsigned char *buffer,unsigned int len,unsigned int PID);

    unsigned int crc32(unsigned int crc,unsigned char *buffer, unsigned int size);
    void write_to_text(char *buffer,int position);
    void save_to_file(char *buffer,int position);
    void save_time_file(char *buffer);
    void save_epg_file(char *buffer,unsigned int id);
    void category_choice(unsigned char level_1,unsigned char level_2,char *buf_category);
    void save_basic_info(char *buffer,unsigned int number);
    int Is_Right_time(Time time_current,Time time_offset,unsigned int duration);
    void clear_all();
    void clear_cache();
    int mjdtod(int mjd);
    int mjdtom(int mjd);
    int mjdtoy(int mjd);
    void get_section_data(unsigned int pid);
    //Q_INVOKABLE void get_right_data(QString msg,int id);
private:
    Ui::MainWindow *ui;
signals:
    void stop();
public slots:
    void over();

};
#endif // MAINWINDOW_H
