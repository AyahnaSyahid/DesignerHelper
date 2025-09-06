#ifndef POLAROIDMAKERWIDGET_H
#define POLAROIDMAKERWIDGET_H

#include <QMap>
#include <QSortFilterProxyModel>
#include <QWidget>

#include "PolaroidMaker_global.h"
#include "polaroidmaker.h"
#include "polaroidmakerns.h"
#include "sizetemplatemodel.h"
namespace Ui {
class PolaroidMakerWidget;
}

class Counter;
class POLAROIDMAKER_EXPORT PolaroidMakerWidget : public QWidget {
  Q_OBJECT
  SizeTemplateModel *stm;
  QSortFilterProxyModel *proxUK;
  QSortFilterProxyModel *proxUP;
  QAbstractItemModel *polModel;
  PolaroidMaker maker;

 public:
  static PolaroidMakerNS::SizeFListsTable s_table;
  explicit PolaroidMakerWidget(QWidget *parent = nullptr);
  ~PolaroidMakerWidget();
  void saveUiState();

 public slots:
  void refreshCounter();

 private slots:
  void loadUkuranKertasList();
  void loadUkuranPolaroidList();
  void deleteToggler();
  void loadConfiguration();
  void on_kTemplate_activated(const QString &arg1);
  void on_pTemplate_activated(const QString &arg1);
  void on_kWSize_valueChanged(double arg1);
  void on_kHSize_valueChanged(double arg1);
  void on_pWSize_valueChanged(double arg1);
  void on_pHSize_valueChanged(double arg1);
  void on_colTButton_clicked();
  void on_colCButton_clicked();
  void on_colBButton_clicked();
  void on_pushButton_clicked();
  void on_delPol_clicked();
  void on_restPol_clicked();
  void on_addFold_clicked();
  void on_addFile_clicked();
  void on_propPol_clicked();
  void on_aGamma_editingFinished();
  void setUkuranKertas(const QSizeF &sf);
  void setUkuranPolaroid(const QSizeF &sp);
  void on_eGamma_clicked();
  void on_doButton_clicked();
  void refillFailedHandler(const QString &msg);
  void refillSuccessHandler(int av, int bn);
  void lihatCounter();
  void openRefillDialog();

 private:
  Ui::PolaroidMakerWidget *ui;
  QColor cropColor;
  QColor bgColor;
  QColor markColor;
  Counter *ctr;

 signals:
  void ukuranKertasChanged();
  void ukuranPolaroidChanged();
  void operationSuccess();
};

#endif  // POLAROIDMAKERWIDGET_H
