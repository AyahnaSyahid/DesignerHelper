#ifndef POLAROIDPROPDIALOG_H
#define POLAROIDPROPDIALOG_H

#include <QDialog>

namespace Ui {
class polaroidpropdialog;
}

class polaroidpropdialog : public QDialog {
  Q_OBJECT

 public:
  explicit polaroidpropdialog(QWidget *parent = nullptr);
  ~polaroidpropdialog();

 private:
  Ui::polaroidpropdialog *ui;
};

#endif  // POLAROIDPROPDIALOG_H
