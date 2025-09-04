#ifndef GAMMAEDITORDIALOG_H
#define GAMMAEDITORDIALOG_H
#include "polaroid.h"
#include <QDialog>
#include <QPixmap>

namespace Ui {
class GammaEditorDialog;
}

class GammaEditorDialog : public QDialog
{
    Q_OBJECT
    Polaroid *mPol = nullptr;
    double initGamma=0;
    QImage before;
    QImage after;

public:
    explicit GammaEditorDialog(QWidget *parent = nullptr, Polaroid *pol=nullptr);
    double currentGamma();
    void setPolaroid(Polaroid *pol);
    void setInitialGamma(double g);
    ~GammaEditorDialog();

private slots:
    void updateToolTiptext();
    void updateAfter();
    void on_gslide_valueChanged(int value);

private:
    Ui::GammaEditorDialog *ui;
};

#endif // GAMMAEDITORDIALOG_H
