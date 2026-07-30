#pragma once

#include <QDialog>

namespace Ui {
class CreateSizeDialog;
}

class CreateSizeDialog : public QDialog {
 public:
  struct SizeDef {
    QString tipe, name;
    qreal width, height;
  };

  enum Mode { Kertas, Polaroid };
  CreateSizeDialog(Mode m, QWidget* parent = nullptr);
  ~CreateSizeDialog();

  SizeDef getSizeData() const;
  bool checkInput() const;

 public slots:
  void accept() override;

 private:
  Ui::CreateSizeDialog* ui;
  Mode _mode;
};

Q_DECLARE_METATYPE(CreateSizeDialog::Mode);