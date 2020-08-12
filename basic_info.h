#ifndef BASIC_INFO_H
#define BASIC_INFO_H

#include <QDialog>
#include <QTreeWidget>
typedef struct stream_info
{
    unsigned int PID;
    QString type;
}stream_info;
typedef struct basic_information
{
    unsigned int program_PID;
    unsigned int service_id;
    QVector<stream_info> stream_information;
    unsigned int PCR_id;
}basic_information;
namespace Ui {
class basic_info;
}

class basic_info : public QDialog
{
    Q_OBJECT

public:
    explicit basic_info(QWidget *parent = nullptr);
    ~basic_info();
    void init_tree();
    char *tempbuf = new char[100];
    QVector<basic_information> PID_tree;
private slots:

private:
    Ui::basic_info *ui;
};

#endif // BASIC_INFO_H
