#include "counter.h"
#include <QApplication>
#include <QDate>
#include <QFile>
#include <QStorageInfo>
#include <QCryptographicHash>

QDate Counter::exp(2022, 9, 30);
const int preReleaseCounter = 100;

Counter::Counter(QObject *parent) : QObject(parent), database(QSqlDatabase::database())
{
    cSettings = new QSettings(QSettings::IniFormat, QSettings::UserScope, qApp->organizationName(), qApp->applicationName(), this);
    if(!cSettings->value("counter/initialized").isValid()) {
        cSettings->setValue("counter/initilaized", QDateTime::currentDateTime());
    } else {
        cSettings->setValue("counter/lastAccessTime", QDateTime::currentDateTime());
    }
    cSettings->sync();
    QFileInfo cfgPath(cSettings->fileName());
    QString databaseName =  cfgPath.absolutePath() + "/" + qAppName() + ".xmld";
    auto dbExists = QFile::exists(databaseName);

    if(!database.isValid()) {
        database = QSqlDatabase::addDatabase("QSQLITE");
        database.setDatabaseName(databaseName);
        database.open();
    }

    if(!database.isOpen())
        database.open();

    if(!dbExists){
        QSqlQuery query;
        query.exec("CREATE TABLE counter (key TEXT UNIQUE ON CONFLICT REPLACE, val TEXT)");
        query.exec(QString("INSERT INTO counter VALUES ('installTime', '%1')").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")));
        query.exec(QString("INSERT INTO counter VALUES ('avail', %1)").arg(exp < QDate::currentDate() ? 0 : preReleaseCounter));
        query.exec(QString("INSERT INTO counter VALUES ('totalCounter', 0)"));
        query.exec(QString("INSERT INTO counter VALUES ('bonus', %1)").arg(exp < QDate::currentDate() ? 0 : 25));
        cSettings->setValue("counter/dataBaseCreated", QDateTime::currentDateTime());
    }
}

bool Counter::refill(const QString &data)
{
    QString ba = QByteArray::fromBase64(data.toLocal8Bit());
    /*
     * (BASE64DECODED data)
     * "installTime=2022-04-16 21:33:12$$avail=1000$$bonus=25"
     */
    QStringList pairs = ba.split("$$");
    if(pairs.count() < 3){
        emit refillFailed("Data Korupt");
        return false;
    }
    QString installT, availT, bonusT;
    installT = pairs[0].split('=')[1];
    availT = pairs[1].split('=')[1];
    bonusT = pairs[2].split('=')[1];
    if(installT < instT()){
        emit refillFailed("Maaf, token ini mungkin sudah dipakai sebelumnya");
        return false;
    }

    qint64 befa = avail();
    qint64 availV = availT.toLongLong();
    qint64 bonusV = bonusT.toLongLong();

    updateAvail(availV);
    setBonus(bonusV);

    if(avail() != befa) {
        if(!database.isOpen())
            database.open();
        QSqlQuery q;
        q.exec(QString("UPDATE counter SET val = '%1' WHERE key ='installTime'").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss")));
        emit refillSuccess();
    }
    else
        emit refillFailed("Kegagalan tidak di ketahui");
    return true;
}

const QDate &Counter::expire() const
{
    return exp;
}

qint64 Counter::avail()
{
    if(!database.isOpen())
        database.open();
    QSqlQuery q("SELECT val FROM counter WHERE key = 'avail'");
    // qDebug() << q.lastError();
    q.next();
    return q.value(0).toLongLong();
}

qint64 Counter::totalCounter()
{
    if(!database.isOpen())
        database.open();
    QSqlQuery q("SELECT val FROM counter WHERE key = 'totalCounter'");
    // qDebug() << q.lastError();
    q.next();
    return q.value(0).toLongLong();
}

qint64 Counter::bonus()
{
    if(!database.isOpen())
        database.open();
    QSqlQuery q("SELECT val FROM counter WHERE key = 'bonus'");
    // qDebug() << q.lastError();
    q.next();
    return q.value(0).toLongLong();
}

QString Counter::instT()
{
    if(!database.isOpen())
        database.open();
    QSqlQuery q("SELECT val FROM counter WHERE key = 'installTime'");
    // qDebug() << q.lastError();
    q.next();
    return q.value(0).toString();
}

bool Counter::canAcceptRequest()
{
    return avail() + bonus() > 0;
}

const QString &Counter::errMessage()
{
    return err;
}

void Counter::updateAvail(int many)
{
    int av = avail() + many;
    if(!database.isOpen())
        database.open();
    QSqlQuery q;
    q.prepare("UPDATE counter SET val = ? WHERE key = 'avail'");
    q.bindValue(0, av);
    if(q.exec())
        emit availUpdated(many);
    database.close();
}

void Counter::updateCounter(int many)
{
    int co = totalCounter() + many;
    if(!database.isOpen())
        database.open();
    QSqlQuery q;
    q.prepare("UPDATE counter SET val = ? WHERE key = 'totalCounter'");
    q.bindValue(0, co);
    if(q.exec())
        emit counterUpdated(many);
    database.close();
}

void Counter::setBonus(int many)
{
    int bo = many;
    if(!database.isOpen())
        database.open();
    QSqlQuery q;
    q.prepare("UPDATE counter SET val = ? WHERE key = 'bonus'");
    q.bindValue(0, bo);
    if(q.exec())
        emit bonusUpdated(many);
    database.close();
}
