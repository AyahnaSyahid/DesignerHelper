#include "sizetemplatedialog.h"
#include "ui_sizetemplatedialog.h"
#include "sizetemplatemodel.h"

SizeTemplateDialog::SizeTemplateDialog(QWidget *parent, QAbstractItemModel *md) :
    QDialog(parent),
    ui(new Ui::SizeTemplateDialog)
{
    ui->setupUi(this);
    ui->tabelUkuran->setModel(md);
    ui->hapus->setDisabled(true);
    ui->ubah->hide();
    connect(ui->tabelUkuran->selectionModel(), &QItemSelectionModel::selectionChanged, this, &SizeTemplateDialog::table_selectedItem_changed);
}

SizeTemplateDialog::~SizeTemplateDialog()
{
    delete ui;
}

void SizeTemplateDialog::on_baru_clicked()
{
    ui->tabelUkuran->model()->insertRow(1);
}

void SizeTemplateDialog::on_hapus_clicked()
{
    if(ui->tabelUkuran->selectionModel()->hasSelection()) {
        ui->tabelUkuran->model()->removeRow(ui->tabelUkuran->selectionModel()->selectedIndexes()[0].row());
    }
}

void SizeTemplateDialog::table_selectedItem_changed(const QItemSelection &s, const QItemSelection &d)
{
    Q_UNUSED(d)
    if(s.count())
        ui->hapus->setEnabled(true);
    else
        ui->hapus->setDisabled(true);
}


