#include "refillform.h"
#include "refillrequest.h"
#include "ui_refillform.h"
#include <QFileDialog>
#include <QMessageBox>


RefillForm::RefillForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RefillForm)
{
    ui->setupUi(this);
    ui->plainTextEdit->setReadOnly(true);
}

RefillForm::~RefillForm()
{
    delete ui;
}

void RefillForm::on_pushButton_clicked()
{
    auto ch = QFileDialog::getOpenFileName(nullptr, "Isi Token");
    if(ch.isEmpty()){
        QMessageBox::warning(this, "Gagal", "Operasi dibatalkan");
        this->close();
        return;
    }
    ui->lineEdit->setText(ch);
    QFile tokf(ch);
    tokf.open(QFile::ReadOnly);
    readed = tokf.readAll();
    tokf.close();
    ui->plainTextEdit->setPlainText(readed.data());
}


void RefillForm::on_pushButton2_clicked()
{
    if(ui->plainTextEdit->toPlainText().isEmpty()) {
        QMessageBox::warning(this, "Peringatan", "Silahkan pilih file token");
        return;
    }
    emit tokenReady(QString(readed));
}


void RefillForm::on_pushButton3_clicked()
{
    RefillRequest rr;
    rr.exec();
}

