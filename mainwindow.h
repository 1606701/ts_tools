#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "dialog.h"
#include "epg.h"
#include "basic_info.h"


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

typedef struct TS_PMT_Stream
{
    unsigned stream_type    	  	: 8; //指示特定PID的节目元素包的类型。该处PID由elementary PID指定
    unsigned elementary_PID  		: 13; //该域指示TS包的PID值。这些TS包含有相关的节目元素
    unsigned ES_info_length  		: 12; //前两位bit为00。该域指示跟随其后的描述相关节目元素的byte数
    QString type_descriptor;
    unsigned int descriptor;
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


typedef struct TS_CAT_info
{
    unsigned descriptor_tag	 			: 8;
    unsigned descriptor_length 			: 8;
    unsigned CA_system_ID 				: 16;
    unsigned reserved_3 				: 3;
    unsigned CA_PID 					: 13;
}TS_CAT_info;
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
typedef struct service_info
{
    unsigned service_id             :16;
    unsigned service_type           :8;
}service_info;
typedef struct service_list_descriptor
{
    unsigned descriptor_tag         :8;
    unsigned descriptor_length      :8;
    QVector<service_info> nit_service;
}service_list_descriptor;

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
    int check_flag_nit = 0;
    int check_flag_eit = 0;
    int check_flag_sdt = 0;
    int check_flag_pmt = 0;
    Time offset_time;

    Dialog d;
    EPG e;
    basic_info b;

    TS_PAT *packet_PAT = new TS_PAT;
    TS_PMT *packet_PMT = new TS_PMT;
    TS_CAT *packet_CAT = new TS_CAT;
    TS_NIT *packet_NIT = new TS_NIT;
    TS_SDT *packet_SDT = new TS_SDT;
    TS_TOT *packet_TOT = new TS_TOT;
    TS_TDT *packet_TDT = new TS_TDT;
    TS_EIT *packet_EIT = new TS_EIT;

    QVector<EIT_number> eit_num;
    QVector<unsigned int> PMT_PID;
    QVector<TS_PAT_Program> ts_pat_program;
    QVector<TS_PMT> ts_pmt;
    QVector<TS_CAT> ts_cat;
    QVector<TS_NIT> ts_nit;
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();

    void on_pushButton_2_clicked();

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
private:
    Ui::MainWindow *ui;

//    Dialog *dialog;
//    EPG *epg;
//    basic_info *b_info;
};
#endif // MAINWINDOW_H
