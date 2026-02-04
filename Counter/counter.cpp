#include "counter.h"

#include <QApplication>
#include <QDate>
#include <QFile>
#include <QRegularExpression>
#include <QStorageInfo>

QDate Counter::exp(2026, 2, 28);
QDate Counter::dies(2026, 8, 31);

const int preReleaseCounter = 200;

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
  
  cSettings->sync();
  
  // Perbaikan: database() method, bukan property
  QSqlDatabase db = QSqlDatabase::database("PolaroidCounter", true);
  
  if (!db.isValid()) { // belum ada koneksi
    db = QSqlDatabase::addDatabase("QSQLITE", "PolaroidCounter");
    QFileInfo cfgPath(cSettings->fileName());
    QString databaseName = cfgPath.absolutePath() + "/" + qApp->applicationName() + ".xmld";
    db.setDatabaseName(databaseName);
  }
  
  qDebug() << "Database Path :" << db.databaseName();
  
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
        q.prepare("INSERT INTO counter (key, val) VALUES ('avail', :avail)");
        if (QDate::currentDate() <= exp) {
          q.bindValue(":avail", preReleaseCounter);
        } else {
          q.bindValue(":avail", '0');
        }
        ok = q.exec();
      }
      
      if (ok) {
        ok = q.exec("INSERT INTO counter (key, val) VALUES ('bonus', '0')");
      }
      
      if (ok) {
        ok = q.exec("INSERT INTO counter (key, val) VALUES ('totalCounter', '0')");
      }
      
      if (ok) {
        ok = q.exec("INSERT INTO counter (key, val) VALUES ('installTime', datetime('now', 'localtime'))");
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
}

bool Counter::refill(const QString &data) {
  QString ba = QByteArray::fromBase64(data.toLocal8Bit());
  /*
   * (BASE64DECODED data)
   * "installTime=2022-04-16 21:33:12$$avail=1000$$bonus=25" ------- (V1)
   * "installId$$installTime=2022-04-16 21:33:12$$avail=1000$$bonus=25" ------- (V2)
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
  
  qDebug() << iId  << installT << availT << bonusT ;
  
  if (iId != installId) {
    emit refillFailed("Maaf, Install ID tidak cocok");
    return false;
  }
  auto lastRefill = QDateTime::fromString(instT(), Qt::ISODate);
  auto tokenRefill = QDateTime::fromString(installT, Qt::ISODate);
  
  qDebug() << "last refill" << lastRefill;
  qDebug() << "token refill" << tokenRefill;
  
  if ( tokenRefill <= lastRefill ) {
    emit refillFailed("Maaf, token ini sudah dipakai sebelumnya");
    return false;
  }
  
  auto db = QSqlDatabase::database("PolaroidCounter");
  if(!db.isValid()) {
    emit refillFailed("Error : 267829 Invalid Database Connection");
    return false;
  }
  
  db.transaction();
  
  QSqlQuery q(db);
  
  
  q.prepare("UPDATE counter SET val = :instt WHERE [key] = 'installTime'");
  q.bindValue(":instt", installT);
  
  bool ok = q.exec();
  
  if (ok) {
    q.prepare("UPDATE counter SET val = CAST ( CAST(val AS INTEGER) + :avail AS TEXT) WHERE [key] = 'avail'");
    q.bindValue(":avail", availT.toInt());
    ok = q.exec();
  }
  
  if (ok && (bonusT.toInt() > 0)) {
    q.prepare("UPDATE counter SET val = CAST ( CAST (val AS INTEGER) + :bons AS TEXT) WHERE [key] = 'bonus'");
    q.bindValue(":bons", bonusT.toInt());
    ok = q.exec();
  }
  
  if (ok) {
    db.commit();
    emit availUpdated(availT.toInt());
    if (bonusT.toInt() > 0)
      emit bonusUpdated(bonusT.toInt());
    emit refillSuccess(availT.toInt(), bonusT.toInt());
    return true;
  }
  
  emit refillFailed(q.lastError().text());
  emit databaseError(q.lastError().text());
  db.rollback();
  return false;
}

const QDate &Counter::expire() const { return exp; }

qint64 Counter::avail() {
  qDebug() << "avail called";
  auto db = QSqlDatabase::database("PolaroidCounter", true);
  if(! ( db.isValid() && db.isOpen()) ) {
    return 0;
  }
  QSqlQuery q("SELECT CAST(val AS INTEGER) FROM counter WHERE [key] = 'avail'", db);
  q.next();
  return q.value(0).toLongLong();
}

qint64 Counter::totalCounter() {
  qDebug() << "totalCounter called";
  auto db = QSqlDatabase::database("PolaroidCounter", true);
  if(! ( db.isValid() && db.isOpen()) ) {
    return 0;
  }
  QSqlQuery q("SELECT CAST(val AS INTEGER) FROM counter WHERE [key] = 'totalCounter'", db);
  q.next();
  return q.value(0).toLongLong();
}

qint64 Counter::bonus() {
  qDebug() << "bonus called";
  
  auto db = QSqlDatabase::database("PolaroidCounter", true);
  if(! ( db.isValid() && db.isOpen()) ) {
    return 0;
  }
  QSqlQuery q("SELECT val FROM counter WHERE [key] = 'bonus'", db);
  q.next();
  return q.value(0).toLongLong();
}

QString Counter::instT() {
  qDebug() << "instT called";
  auto db = QSqlDatabase::database("PolaroidCounter", true);
  if(! ( db.isValid() && db.isOpen()) ) {
    return 0;
  }
  QSqlQuery q("SELECT val FROM counter WHERE [key] = 'installTime'", db);
  q.next();
  return q.value(0).toString();
}

bool Counter::canAcceptRequest() {
  if (QDate::currentDate() < Counter::dies) {
    return avail() + bonus() > 0;
  }
  err = "Aplikasi memerlukan update";
  return false;
}

const QString &Counter::errMessage() { return err; }

void Counter::updateCounter(int many) {
  auto db = QSqlDatabase::database("PolaroidCounter", true);
  if(! ( db.isValid() && db.isOpen()) ) {
    return;
  }
  QSqlQuery q(db);
  q.prepare("UPDATE counter SET val = CAST( CAST( val AS INTEGER ) + :many ) AS TEXT WHERE key = 'totalCounter'");
  q.bindValue(":many", many);
  if (q.exec()) emit counterUpdated(many);
}

// keep for compatibility
void Counter::setBonus(int many) {
  int bo = many;
  auto db = QSqlDatabase::database("PolaroidCounter", true);
  if(! ( db.isValid() && db.isOpen()) ) {
    return;
  }
  QSqlQuery q(db);
  q.prepare("UPDATE counter SET val = :bons WHERE key = 'bonus'");
  q.bindValue(":bons", bo);
  if (q.exec()) emit bonusUpdated(many);
}

void Counter::decreaseCounter(int many) {
  auto db = QSqlDatabase::database("PolaroidCounter", true);
  int c_bonus = bonus() - many;
  
  if (!db.transaction()) {
    qWarning() << "Failed to start transaction";
    return;
  }
  
  QSqlQuery q(db);
  
  if (c_bonus > -1) {
    // set bonus
    q.prepare("UPDATE counter SET val = :cbons WHERE [key] = 'bonus'");
    q.bindValue(":cbons", c_bonus);
    
    bool bsets = q.exec();
    
    if (bsets) {
      q.prepare("UPDATE counter SET val = CAST ( CAST (val AS INTEGER) + :cnt AS TEXT ) WHERE [key] = 'totalCounter'");
      q.bindValue(":cnt", many);
      bsets = q.exec(); 
    }
    
    if (bsets) {
      if (db.commit()) {
        emit bonusUpdated(c_bonus);
        return;
      }
    }
    
    db.rollback();
    qWarning() << "Failed to update bonus:" << q.lastError();
    return;
  }
  
  // c_bonus <= -1, kurangi dari available
  q.prepare("UPDATE counter SET val = 0 WHERE [key] = 'bonus'");
  bool exok = q.exec();
  
  if (exok) {
    q.prepare("UPDATE counter SET val = CAST(CAST(val AS INTEGER) + :cbons AS TEXT ) WHERE [key] = 'avail'");
    q.bindValue(":cbons", c_bonus);
    exok = q.exec();
  }
  
  if (exok) {
    q.prepare("UPDATE counter SET val = CAST ( CAST (val AS INTEGER) + :cnt AS TEXT ) WHERE [key] = 'totalCounter'");
    q.bindValue(":cnt", many);
    exok = q.exec();
  }
  
  if (exok && db.commit()) {
    emit bonusUpdated(0);
    emit availUpdated(avail());
    emit counterUpdated(many);
  } else {
    db.rollback();
    qWarning() << "Failed to update counters:" << q.lastError();
  }
}