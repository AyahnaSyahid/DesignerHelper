#include "counter.h"

#include <QApplication>
#include <QDate>
#include <QFile>
#include <QRegularExpression>
#include <QStorageInfo>

QDate Counter::exp(2026, 2, 28);

const int preReleaseCounter = 100;

Counter::Counter(QObject *parent)
    : QObject(parent) {
  cSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope,
                    qApp->organizationName(), qApp->applicationName(), this);
  
  // Perbaikan typo: "initilaized" -> "initialized"
  if (!cSettings->value("counter/initialized").isValid()) {
    cSettings->setValue("counter/initialized", QDateTime::currentDateTime());
  } else {
    cSettings->setValue("counter/lastAccessTime", QDateTime::currentDateTime());
  }
  
  // Perbaikan case-sensitivity: "InstallId" vs "installId"
  if (!cSettings->value("counter/installId").isValid()) {
    installId = QUuid::createUuid().toString(QUuid::WithoutBraces).toUpper();
    cSettings->setValue("counter/installId", installId);
  } else {
    installId = cSettings->value("counter/installId").toString();
  }
  
  // Perbaikan: database() method, bukan property
  QSqlDatabase db = QSqlDatabase::database("PolaroidCounter", true);
  
  if (!db.isValid()) { // belum ada koneksi
    db = QSqlDatabase::addDatabase("QSQLITE", "PolaroidCounter");
    QFileInfo cfgPath(cSettings->fileName());
    QString databaseName = cfgPath.absolutePath() + "/" + qApp->applicationName() + ".xmld";
    db.setDatabaseName(databaseName);
  }
  
  if (!db.open()) {
    if (db.lastError().isValid()) {
      emit databaseError(db.lastError().text());
    } else {
      emit databaseError("DATABASE ERROR : 667678");
    }
  } else {
    // Check table dan wrap dalam transaction
    if (db.tables().indexOf("counter") == -1) {
      db.transaction();
      
      QSqlQuery q(db);
      bool ok = q.exec("CREATE TABLE counter (key TEXT PRIMARY KEY, val TEXT NOT NULL) WITHOUT ROWID;");
      
      if (ok) {
        q.prepare("INSERT INTO counter (key, val) VALUES ('installId', :installId)");
        q.bindValue(":installId", installId);
        ok = q.exec();
      }
      
      if (ok) {
        ok = q.exec("INSERT INTO counter (key, val) VALUES ('avail', '0')");
      }
      
      if (ok) {
        ok = q.exec("INSERT INTO counter (key, val) VALUES ('bonus', '0')");
      }
      
      // Commit atau rollback berdasarkan hasil
      if (ok) {
        db.commit();
      } else {
        db.rollback();
        emit databaseError("Failed to initialize counter table: " + q.lastError().text());
      }
    }
  }
  
  cSettings->sync();
}

bool Counter::refill(const QString &data) {
  QString ba = QByteArray::fromBase64(data.toLocal8Bit());
  /*
   * (BASE64DECODED data)
   * "installId$$installTime=2022-04-16 21:33:12$$avail=1000$$bonus=25"
   */
  QRegularExpression regex(
      R"--(^[A-Za-z0-9^-]{36}\$\$installTime=20[2-9]\d-(?:0[1-9]|1[0-2])-(?:0[1-9]|[1-2]\d|3[0-1]) (?:0[0-9]|1[0-9]|2[0-3])(?::(?:0[1-9]|[1-5]\d))+\$\$avail=\d+\$\$bonus=\d+)--");
  auto regmatch = regex.match(ba);
  if (!regmatch.hasMatch()) {
    emit refillFailed("Data Korupt 01025");
    return false;
  }
  QStringList pairs = ba.split("$$");
  if (pairs.count() < 4) {
    emit refillFailed("Data Korupt 02235");
    return false;
  }
  QString iId, installT, availT, bonusT;
  iId = pairs[0];
  installT = pairs[1].split('=')[1];
  availT = pairs[2].split('=')[1];
  bonusT = pairs[3].split('=')[1];
  if (iId != installId) {
    emit refillFailed("Maaf, Install ID tidak cocok");
    return false;
  }
  if (installT < instT()) {
    emit refillFailed("Maaf, token ini mungkin sudah dipakai sebelumnya");
    return false;
  }

  qint64 currentAvailable = avail();
  qint64 availValue = availT.toLongLong();
  qint64 bonusValue = bonusT.toLongLong();

  updateAvail(availV);
  setBonus(bonusV);

  if (avail() != befa) {
    if (!database.isOpen()) database.open();
    QSqlQuery q;
    q.exec(
        QString("UPDATE counter SET val = '%1' WHERE key ='installTime'")
            .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")));
    emit refillSuccess(availT.toInt(), bonusT.toInt());
    return true;
  } else {
    emit refillFailed("Kegagalan tidak di ketahui");
  }
  return false;
}

const QDate &Counter::expire() const { return exp; }

qint64 Counter::avail() {
  QSqlQuery q("SELECT val FROM counter WHERE key = 'avail'");
  q.next();
  return q.value(0).toLongLong();
}

qint64 Counter::totalCounter() {
  if (!database.isOpen()) database.open();
  QSqlQuery q("SELECT val FROM counter WHERE key = 'totalCounter'");
  // qDebug() << q.lastError();
  q.next();
  return q.value(0).toLongLong();
}

qint64 Counter::bonus() {
  if (!database.isOpen()) database.open();
  QSqlQuery q("SELECT val FROM counter WHERE key = 'bonus'");
  // qDebug() << q.lastError();
  q.next();
  return q.value(0).toLongLong();
}

QString Counter::instT() {
  if (!database.isOpen()) database.open();
  QSqlQuery q("SELECT val FROM counter WHERE key = 'installTime'");
  // qDebug() << q.lastError();
  q.next();
  return q.value(0).toString();
}

bool Counter::canAcceptRequest() { return avail() + bonus() > 0; }

const QString &Counter::errMessage() { return err; }

void Counter::updateAvail(int many) {
  int av = avail() + many;
  if (!database.isOpen()) database.open();
  QSqlQuery q;
  q.prepare("UPDATE counter SET val = ? WHERE key = 'avail'");
  q.bindValue(0, av);
  if (q.exec()) emit availUpdated(many);
  database.close();
}

void Counter::updateCounter(int many) {
  int co = totalCounter() + many;
  if (!database.isOpen()) database.open();
  QSqlQuery q;
  q.prepare("UPDATE counter SET val = ? WHERE key = 'totalCounter'");
  q.bindValue(0, co);
  if (q.exec()) emit counterUpdated(many);
  database.close();
}

void Counter::setBonus(int many) {
  int bo = many;
  if (!database.isOpen()) database.open();
  QSqlQuery q;
  q.prepare("UPDATE counter SET val = ? WHERE key = 'bonus'");
  q.bindValue(0, bo);
  if (q.exec()) emit bonusUpdated(many);
  database.close();
}