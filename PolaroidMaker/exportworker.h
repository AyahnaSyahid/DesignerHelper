#ifndef EXPORTWORKER_H
#define EXPORTWORKER_H
#include "polaroidmaker.h"
#include <QObject>

class ExportWorker : public QObject {
    Q_OBJECT
    PolaroidMaker *maker;
    QString outFile;

public :
    explicit ExportWorker(PolaroidMaker *mak, const QString& ofile) : maker(mak), outFile(ofile) {}

public slots:
    void expol();

signals:
    void exportDone();
    void exportFail(const QString &reason);
};

#endif // EXPORTWORKER_H
