#ifndef SECTION_DATA_H
#define SECTION_DATA_H

#include <QDialog>
#include <QTreeWidgetItem>
#include <fstream>
#include <database.h>

typedef struct section_info
{
    unsigned int PID            :13;
    QString type;
    unsigned continuity_counter :4;
}section_info;

namespace Ui {
class section_data;
}

class section_data : public QDialog
{
    Q_OBJECT

public:
    explicit section_data(QWidget *parent = nullptr);
    ~section_data();
    void init_tree();
    char *tempbuf = new char[1000];
    QVector<section_info> Section_tree;

    database d;
    QVector<QString> data;


private:
    Ui::section_data *ui;
private slots:
    void slotDoubleClickItem(QTreeWidgetItem* item,int count);
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

};

#endif // SECTION_DATA_H
