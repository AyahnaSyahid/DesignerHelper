#include "createsizedialog.h"

#include "ui_createsizedialog.h"
#include <QMessageBox>

CreateSizeDialog::CreateSizeDialog(Mode m, QWidget* parent)
    : QDialog(parent), _mode(m), ui(new Ui::CreateSizeDialog) {
  ui->setupUi(this);
  this->setWindowTitle(this->windowTitle() +
                       (_mode == Kertas ? " Kertas" : " Polaroid"));
  ui->label->setText(m == Kertas ? "Ukuran Kertas" : "Ukuran Polaroid");
}

CreateSizeDialog::~CreateSizeDialog() { delete ui; }

CreateSizeDialog::SizeDef CreateSizeDialog::getSizeData() const {
  QString tipe = _mode == Kertas ? "Kertas" : "Polaroid";
  return SizeDef{.tipe = tipe,
                 .name = ui->lineEdit->text(),
                 .width = ui->widthDps->value(),
                 .height = ui->heightDps->value()};
}

bool CreateSizeDialog::checkInput() const 
{
  return (ui->lineEdit->text().count() > 3) && (ui->widthDps->value() > 0) && (ui->heightDps->value() > 0);
}

void CreateSizeDialog::accept() {
    if (checkInput()) {
        QDialog::accept();
        return ;
    }
    QMessageBox::warning(this, "Periksa masukan", "Pastikan nama ukuran lebih dari 3 karakter, Panjang > 0 dant Tinggi > 0");
}