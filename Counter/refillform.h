#ifndef REFILLFORM_H
#define REFILLFORM_H

#include "Counter_global.h"
#include <QWidget>
#include <QDialog>

namespace Ui {
class RefillForm;
}

class COUNTER_EXPORT RefillForm : public QDialog
{
    Q_OBJECT
    QByteArray readed;
public:
    explicit RefillForm(QWidget *parent = nullptr);
    QByteArray& token() {return readed;}
    ~RefillForm();

private slots:
    void on_pushButton_clicked();
    void on_pushButton2_clicked();
    void on_pushButton3_clicked();

signals:
    void tokenReady(const QString& s);

private:
    Ui::RefillForm *ui;


};

#endif // REFILLFORM_H
