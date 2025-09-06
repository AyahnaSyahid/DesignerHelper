#ifndef DIALOGDETAILCOUNTER_H
#define DIALOGDETAILCOUNTER_H
#include <QDialog>

#include "Counter_global.h"

namespace Ui {
class DialogDetailCounter;
}

class Counter;
class COUNTER_EXPORT DialogDetailCounter : public QDialog {
  Q_OBJECT
  Counter *ctr;

 public:
  explicit DialogDetailCounter(Counter *c = nullptr, QWidget *parent = nullptr);
  ~DialogDetailCounter();

 private:
  Ui::DialogDetailCounter *ui;
};

#endif  // DIALOGDETAILCOUNTER_H
