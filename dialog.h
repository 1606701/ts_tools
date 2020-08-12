#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QThread>
#include <QTreeWidget>

typedef struct Time
{
    unsigned int Year;
    unsigned int Mouth;
    unsigned int Day;
    unsigned int Hour;
    unsigned int Min;
    unsigned int Sec;
}Time;
typedef struct time_descriptor
{
    unsigned country_code               :24;
    unsigned country_region_id          :6;
    unsigned local_time_offset_polarity :1;
    unsigned local_time_offset          :16;
    Time time_of_change;
    unsigned next_time_offset           :16;
}time_descriptor;
typedef struct local_time_offset_descriptor
{
    unsigned descriptor_tag         :8;
    unsigned descriptor_length      :8;
    QVector<time_descriptor> time_desc;
}local_time_offset_descriptor;
typedef struct TS_TOT
{
    unsigned table_id 					: 8;
    unsigned section_syntax_indicator	: 1;
    unsigned reserved_future_use 		: 1;
    unsigned reserved_1 				: 2;
    unsigned section_length 			: 12;
    unsigned reserved_2 				: 4;
    Time UTC_time;
    unsigned descriptor_loop_length		: 12;
    unsigned Is_write                   : 1;
    QVector<local_time_offset_descriptor> local_desc;
}TS_TOT;

typedef struct TS_TDT
{
    unsigned table_id 					: 8;
    unsigned section_syntax_indicator 	: 1;
    unsigned reserved_future_use 		: 1;
    unsigned reserved 					: 2;
    unsigned section_length 			: 12;
    Time UTC_time;
    unsigned Is_write                   : 1;
}TS_TDT;
typedef struct service_descriptor
{
    unsigned descriptor_tag                 :8;
    unsigned descriptor_length              :8;
    unsigned service_type                   :8;
    unsigned service_provider_name_length   :8;
    QString service_provider_name;
    unsigned service_name_length            :8;
    QString service_name;
}service_descriptor;
typedef struct private_data_specifier_descriptor
{
    unsigned descriptor_tag         :8;
    unsigned descriptor_length      :8;
    unsigned private_data_specifier :32;
}private_data_specifier_descriptor;
typedef struct unknown_descriptor
{
    unsigned descriptor_tag         :8;
    unsigned descriptor_length      :8;
    QVector<QString> data;
}unknown_descriptor;
typedef struct bussiness_descriptor
{
    unsigned service_id                :16;
    unsigned EIT_schedule_flag          :1;
    unsigned EIT_presnet_following_flag :1;
    unsigned running_status             :3;
    unsigned free_CA_mode               :1;
    unsigned descriptor_loop_length     :12;
    QVector<service_descriptor> ser_desc;
    QVector<private_data_specifier_descriptor> pri_data;
    QVector<unknown_descriptor> un_desc;
}bussiness_descriptor;
typedef struct TS_SDT
{
    unsigned table_id 					: 8;
    unsigned section_syntax_indicator   : 1;
    unsigned reserved_future_use1 	 	: 1;
    unsigned reserved_1 					: 2;
    unsigned section_length				: 12;
    unsigned transport_stream_id        : 16;
    unsigned reserved_2 					: 2;
    unsigned version_number             : 5;
    unsigned current_next_indicator     : 1;
    unsigned section_number             : 8;
    unsigned last_section_number        : 8;
    unsigned original_network_id 		: 16;
    QVector<bussiness_descriptor> bus_desc;
    unsigned reserved_future_use2 		: 8;
    unsigned CRC_32						: 32;
    unsigned check_CRC32                : 32;
    unsigned Is_write                   : 1;
    unsigned SDT_count_actual           : 8;
    unsigned SDT_count_other            : 8;
    unsigned SDT_actual                 : 1;
    unsigned SDT_other                  : 1;
}TS_SDT;
typedef struct short_event_descriptor
{
    unsigned descriptor_tag         :8;
    unsigned descriptor_length      :8;
    unsigned ISO_639_language_code  :24;
    unsigned event_name_length      :8;
    QString event_name;
    unsigned text_length            :8;
    QString text_char;
}short_event_descriptor;
typedef struct category_info
{
    unsigned content_nibble_level_1 :4;
    unsigned content_nibble_level_2 :4;
    unsigned user_byte              :8;
}category_info;
typedef struct content_descriptor
{
    unsigned descriptor_tag         :8;
    unsigned descriptor_length      :8;
    QVector<category_info> cat_info;
}content_descriptor;

typedef struct component_descriptor
{
    unsigned descriptor_tag         :8;
    unsigned descriptor_length      :8;
    unsigned stream_content         :4;
    unsigned component_type         :4;
    unsigned component_tag          :8;
    unsigned ISO_639_language_code  :24;
    QString text_char;
}component_descriptor;
typedef struct ITEM
{
    unsigned item_description_length :8;
    QString item_description_char;
    unsigned item_length             :8;
    QString item_char;
}ITEM;
typedef struct  extended_event_descriptor
{
    unsigned descriptor_tag         :8;
    unsigned descriptor_length      :8;
    unsigned descriptor_number      :4;
    unsigned last_descriptor_number :4;
    unsigned ISO_639_language_code  :24;
    unsigned length_of_items        :8;
    QVector<ITEM> item;
    unsigned text_length            :8;
    QString text_char;
}extended_event_descriptor;
typedef struct ctrl_information
{
    unsigned country_code   :24;
    unsigned rating         :8;
}ctrl_information;
typedef struct parental_rating_descriptor
{
    unsigned descriptor_tag         :8;
    unsigned descriptor_length      :8;
    QVector<ctrl_information> ctrl_info;
}parental_rating_descriptor;
typedef struct EIT_LOOP
{
    unsigned event_id                   : 16;
    unsigned MJD                        : 16;
    unsigned Hour                       : 8;
    unsigned Min                        : 8;
    unsigned Sec                        : 8;
    Time UTC_time;
    unsigned duration_hour              : 8;
    unsigned duration_min               : 8;
    unsigned duration_sec               : 8;
    unsigned running_status             : 3;
    unsigned free_CA_mode               : 1;
    unsigned descriptors_loop_length    : 12;
    QVector<short_event_descriptor> short_eve;//短描述
    QVector<content_descriptor> content_des; //内容描述
    QVector<component_descriptor> componet_des;//组成描述
    QVector<extended_event_descriptor> extended_eve_des; //扩展事件描述
    QVector<parental_rating_descriptor> parental_rat_des; //父母评级控制
}EIT_LOOP;

typedef struct TS_EIT
{
    unsigned table_id 					: 8;
    unsigned section_syntax_indicator 	: 1;
    unsigned reserved_future_use_1		: 1;
    unsigned reserved_1                 : 2;
    unsigned section_length				: 12;
    unsigned service_id 				: 16;
    unsigned reserved_2 				: 2;
    unsigned version_number 			: 5;
    unsigned current_next_indicator 	: 1;
    unsigned section_number 			: 8;
    unsigned last_section_number 		: 8;
    unsigned transport_stream_id 		: 16;
    unsigned original_network_id 		: 16;
    unsigned segment_last_section_number: 8;
    unsigned last_table_id 				: 8;
    QVector<EIT_LOOP> eit_loop;
    unsigned CRC_32 					: 32;
    unsigned check_CRC32                : 32;
}TS_EIT;
typedef struct bouquet_name_descriptor
{
    unsigned descriptor_tag         :8;
    unsigned descriptor_length      :8;
    QString data;
}bouquet_name_descriptor;
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

typedef struct transport_stream_descriptor
{
    unsigned transport_stram_id             : 16;
    unsigned original_network_id            : 16;
    unsigned transport_descriptors_length   : 12;
    QVector<service_list_descriptor> ser_desc;
}transport_stream_descriptor;

typedef struct TS_BAT
{
    unsigned table_id 					: 8;
    unsigned section_syntax_indicator   : 1;
    unsigned section_length				: 12;
    unsigned bouquet_id                 : 16;
    unsigned version_number             : 5;
    unsigned current_next_indicator     : 1;
    unsigned section_number             : 8;
    unsigned last_section_number        : 8;
    unsigned bouquet_descriptors_length :12;
    QVector<bouquet_name_descriptor> bou_desc;
    unsigned transport_stream_loop_length :12;
    QVector<transport_stream_descriptor> tran_desc;
    unsigned CRC_32 					: 32;
    unsigned check_CRC32                : 32;
}TS_BAT;

namespace Ui {
class Dialog;
}

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

//private:
    Ui::Dialog *ui;
    char *tempbuf = new char[100];
    QVector<TS_TOT> ts_tot;
    QVector<TS_TDT> ts_tdt;
    QVector<TS_SDT> ts_sdt_actual;
    QVector<TS_SDT> ts_sdt_other;
    QVector<TS_BAT> ts_bat;
    QVector<TS_EIT> ts_eit_pf_actual;
    QVector<TS_EIT> ts_eit_pf_other;
    QVector<TS_EIT> ts_eit_schedule_actual;
    QVector<TS_EIT> ts_eit_schedule_other;
    void init_tree();
    void init_TOT_tree();
    void init_SDT_tree();
    void init_EIT_tree();
    void init_BAT_tree();

    void init_TDT_tree();
    void init_SDT_actual_tree();
    void init_SDT_other_tree();

    void init_EIT_PF_actual();
    void init_EIT_PF_other();;
    void init_EIT_schedule_actual();
    void init_EIT_schedule_other();

};

#endif // DIALOG_H
