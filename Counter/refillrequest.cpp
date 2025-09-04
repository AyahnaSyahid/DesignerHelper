#include "refillrequest.h"
#include "ui_refillrequest.h"
#include "counter.h"

#include <QClipboard>
#include <QHash>

RefillRequest::RefillRequest(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RefillRequest)
{
    ui->setupUi(this);
    Counter *ct = new Counter;
    QByteArray ba(ct->instT().toLocal8Bit());
    ui->plainTextEdit->setPlainText(ba.toBase64(ba.OmitTrailingEquals));
    ct->deleteLater();
}

RefillRequest::~RefillRequest()
{
    delete ui;
}

void RefillRequest::on_pushButton_clicked()
{
    QClipboard *clip = qApp->clipboard();
    clip->setText(lineContent());
}

QString RefillRequest::lineContent()
{
    return ui->plainTextEdit->toPlainText();
}
