#include "polaroidpropdialog.h"
#include "ui_polaroidpropdialog.h"

polaroidpropdialog::polaroidpropdialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::polaroidpropdialog)
{
    ui->setupUi(this);
}

polaroidpropdialog::~polaroidpropdialog()
{
    delete ui;
}
