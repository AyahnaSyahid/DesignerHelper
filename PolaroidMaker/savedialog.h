#ifndef SAVEDIALOG_H
#define SAVEDIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include "polaroidmaker.h"

namespace Ui {
class SaveDialog;
}

class SaveDialog : public QDialog
{
    Q_OBJECT
    PolaroidMaker *maker;
public:
    explicit SaveDialog(QWidget *parent = nullptr, PolaroidMaker *mr=nullptr);

    ~SaveDialog();

private slots:
    void on_openSave_clicked();
    void on_buttonBox_clicked(QAbstractButton *button);
    void acceptAndClose();

private:
    Ui::SaveDialog *ui;
};

#endif // SAVEDIALOG_H
