#ifndef SIZETEMPLATEDIALOG_H
#define SIZETEMPLATEDIALOG_H

#include <QDialog>
#include <QAbstractTableModel>
#include <QItemSelection>
#include "polaroidmakerns.h"
namespace Ui {
class SizeTemplateDialog;
}

class SizeTemplateDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SizeTemplateDialog(QWidget *parent = nullptr, QAbstractItemModel *md=nullptr);
    ~SizeTemplateDialog();

private slots:
    void on_baru_clicked();
    void on_hapus_clicked();
    void table_selectedItem_changed(const QItemSelection& s, const QItemSelection& d);

private:
    Ui::SizeTemplateDialog *ui;
    QAbstractItemModel *sizeModel;
};

#endif // SIZETEMPLATEDIALOG_H
