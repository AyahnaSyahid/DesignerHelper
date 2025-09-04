#ifndef GAMMAWORKER_H
#define GAMMAWORKER_H

#include <QObject>

class GammaWorker : public QObject {
    Q_OBJECT
    QImage *target;
    double gamma;
public:
    GammaWorker(QObject* parent = nullptr, QImage *img = nullptr, double gm = 0);
    void doGamma();;

signals:
    void done();
};

#endif // GAMMAWORKER_H
