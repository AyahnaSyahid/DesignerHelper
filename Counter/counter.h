#ifndef COUNTER_H
#define COUNTER_H

#include "Counter_global.h"

#include <QObject>
#include <QtSql>
#include <QVariantMap>

class COUNTER_EXPORT Counter : public QObject
{
    Q_OBJECT
    QSqlDatabase database;
    static QDate exp;
    QString err;
    QSettings *cSettings;

public:
    explicit Counter(QObject *parent = nullptr);
    const QDate& expire() const;
    qint64 avail();
    qint64 totalCounter();
    qint64 bonus();
    QString instT();
    bool canAcceptRequest();
    const QString& errMessage();

public slots:
    bool refill(const QString& data);
    void updateAvail(int many);
    void updateCounter(int many);
    void setBonus(int many);

signals:
    void availUpdated(int many);
    void counterUpdated(int many);
    void bonusUpdated(int many);
    void refillFailed(const QString& s);
    void refillSuccess(int availAdd, int bonusAdd);
};

#endif // COUNTER_H
