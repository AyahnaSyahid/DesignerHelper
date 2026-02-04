#ifndef COUNTER_H
#define COUNTER_H

#include <QObject>
#include <QVariantMap>
#include <QtSql>

#include "Counter_global.h"

#ifdef _WIN32
  #include <windows.h>
#endif
  
class COUNTER_EXPORT Counter : public QObject {
  Q_OBJECT

  static QDate exp;
  static QDate dies;

  QString err;
  QSettings *cSettings;
  QString installId;

 public:
  explicit Counter(QObject *parent = nullptr);
  const QDate &expire() const;
  qint64 avail();
  qint64 totalCounter();
  qint64 bonus();
  QString instT();
  bool canAcceptRequest();
  const QString &errMessage();
  inline const QString &getInstallId() const { return installId; }

 public slots:
  bool refill(const QString &data);
  void updateCounter(int many);
  void setBonus(int many);
  void decreaseCounter(int many);

 signals:
  void availUpdated(int many);
  void counterUpdated(int many);
  void bonusUpdated(int many);
  void refillFailed(const QString &s);
  void refillSuccess(int availAdd, int bonusAdd);
  void databaseError(const QString& es);
};

#endif  // COUNTER_H
