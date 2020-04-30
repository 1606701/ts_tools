#ifndef EPG_H
#define EPG_H

#include <QDialog>
#include <fstream>
#include <QVector>
#include "dialog.h"
typedef struct event_information
{
    unsigned event_id                   : 16;
    Time UTC_time;
    unsigned duration_hour              : 8;
    unsigned duration_min               : 8;
    unsigned duration_sec               : 8;
    unsigned ISO_639_language_code      :24;
    QString event_name;
    QString category;
    QVector<ctrl_information> ctrl_info;
    QString text_char;
    bool operator <(const event_information& rhs) const
    {
        if(event_id != rhs.event_id)
            return event_id < rhs.event_id;
        return event_name == event_name;
    }
    bool operator ==(const event_information& rhs) const
    {
        return event_id == rhs.event_id;
    }
}event_information;
typedef struct EPG_information
{
    unsigned service_id 				: 16;
    QVector<event_information> event_info;
}EPG_information;

typedef struct CATEGORY_information
{
    QString category_info;
    QVector<event_information> event_info;
}CATEGORY_information;

namespace Ui {
class EPG;
}

class EPG : public QDialog
{
    Q_OBJECT
    char *tempbuf = new char[1000];
public:
    explicit EPG(QWidget *parent = nullptr);
    ~EPG();


    QVector<EPG_information> epg_info;
    QVector<CATEGORY_information> category_info;
    QVector<TS_EIT> ts_eit_pf_actual;
    QVector<TS_EIT> ts_eit_schedule_actual;
    void init_tree();
    void init_epg_info_pf();
    void init_epg_info_schedule();
    void init_epg_info_service_id();
    void init_epg_info_category();
    void category_choice_epg(unsigned int level_1, unsigned int level_2,char *buf_category);
    //bool sort_service_id(EPG_information a,EPG_information b);
private slots:
//    void on_pushButton_clicked();

//    void on_pushButton_3_clicked();

//    void on_pushButton_4_clicked();

private:
    Ui::EPG *ui;
};

#endif // EPG_H
