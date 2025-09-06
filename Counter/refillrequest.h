#ifndef REFILLREQUEST_H
#define REFILLREQUEST_H

#include <QDialog>

namespace Ui {
class RefillRequest;
}

class RefillRequest : public QDialog {
  Q_OBJECT
 public:
  explicit RefillRequest(QWidget *parent = nullptr);
  ~RefillRequest();

 private slots:
  void on_pushButton_clicked();

 private:
  QString lineContent();
  Ui::RefillRequest *ui;
};

#endif  // REFILLREQUEST_H
