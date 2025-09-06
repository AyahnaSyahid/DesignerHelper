#ifndef POLAROIDMAKER_H
#define POLAROIDMAKER_H
#include <QObject>
#include <QProgressBar>
#include <QSizeF>
#include <QVariantMap>

#include "PolaroidMaker_global.h"
#include "polaroid.h"
#include "polaroidlistmodel.h"

class Updater;
class POLAROIDMAKER_EXPORT PolaroidMaker {
  QVariantMap settings;
  QSizeF pageSize{330.0, 480.0};
  bool pageNumbering = false;
  bool pageMarking = false;
  QFont pageMarkingFont;
  QString pageMarkingText;
  QColor pageMarkingColor;
  int p_dpi = 254;
  QSizeF p_polsize{63.6, 95.0};
  QPair<int, int> getLayoutDimension();
  float gamma = 1;
  void (*updateCB)() = nullptr;
  void (*progressUpdateCB)(const int &page, const int &polIndex) = nullptr;
  void notifyUpdate();
  void progressUpdate(const int &page, const int &polIndex);
  QString m_failReason;
  Updater *updater;

 public:
  static QVariantMap defaultSettings;
  void setUpdateCB(void (*ucb)());
  void setProgressUpdateCB(void (*pucb)(const int &p, const int &pol));
  void createConnection(QProgressBar *polp, QProgressBar *pagep);
  PolaroidMaker();
  ~PolaroidMaker();
  int pageToCreate();
  inline int polaroidCount() { return polModel->rowCount(); }
  inline const QString &failReason() const { return m_failReason; }
  inline void setPageNumbering(bool pn = true) { pageNumbering = pn; }
  inline void setPageMarking(bool pn = true) { pageMarking = pn; }
  inline void setPageMarkingFont(const QFont &fn) { pageMarkingFont = fn; }
  inline void setPageMarkingText(const QString &tx) { pageMarkingText = tx; }
  inline void setPageMarkingColor(const QColor &cl) { pageMarkingColor = cl; }

  void setGamma(const float &gm);
  void setPageSize(const QSizeF &sf);
  void setPolaroidSize(const QSizeF &ps);
  void setDpi(int dpi);
  void addPolaroid(const QString &pic);
  void removePolaroid(const int &ind);
  bool writeToFile(const QString &fname);
  QAbstractItemModel *polaroidModel();

 private:
  PolaroidListModel *polModel;
};

#endif  // POLAROIDMAKER_H
