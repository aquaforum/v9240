#include "widget.h"
#include "ui_widget.h"
#include <QDebug>
#include <QTimer>
#include <QThread>

#include <v9240.h>

const QStringList fieldName  ={
    "Voltage",
    "Amperage",
    "Frequency",
    "Power",
    "Reactive"
};


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    auto &table = *ui->tableWidget;

    table.setColumnCount(2);
    table.setRowCount(5);
    for(size_t i = 0 ; i < 5; ++i)
    {
        table.setItem(i,0,new QTableWidgetItem(fieldName[i]));
        table.setItem(i,1,new QTableWidgetItem("-"));
    }

    chip  = new V9240(this);
    chip->open("/dev/ttyUSB1");
    chip->start();

    auto *tmr = new QTimer(this);
    connect(tmr,&QTimer::timeout,this,&Widget::update_data);
    tmr->setInterval(250);
    tmr->start();

}


Widget::~Widget()
{
    delete ui;
}

void Widget::update_data()
{
    auto &table = *ui->tableWidget;

    table.blockSignals(true);
    for(int i = 0 ; i < 5 ; ++i)
    {
        table.item(i,1)->setText(QString::number((*chip)[static_cast<V9240::parameter>(i)],'f',2));
    }

    table.blockSignals(false);
}


