#include "dialogdetailcounter.h"

#include "counter.h"
#include "ui_dialogdetailcounter.h"
DialogDetailCounter::DialogDetailCounter(Counter *c, QWidget *parent)
    : QDialog(parent), ctr(c), ui(new Ui::DialogDetailCounter) {
  ui->setupUi(this);
  if (ctr) {
    ui->labelTotal->setText(QString("%L1").arg(ctr->totalCounter()));
    ui->labelCounter->setText(QString("%L1").arg(ctr->avail()));
    ui->labelBonus->setText(QString("%L1").arg(ctr->bonus()));
    ui->labelTs->setText(QString("%1").arg(ctr->instT()));
  }
}

DialogDetailCounter::~DialogDetailCounter() { delete ui; }
