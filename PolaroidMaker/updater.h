#ifndef updater_h
#define updater_h

#include <QObject>

class Updater : public QObject
{
    Q_OBJECT

public :
    Updater(QObject* parent = nullptr) : QObject(parent) { }
    ~Updater() {}
    void emitPageUpdate(int c, int a=0);
    void emitPolaroidUpdate(int c, int a=0);

signals:
    void pageUpdate(int c, int a);
    void polaroidUpdate(int c, int a);
};

#endif