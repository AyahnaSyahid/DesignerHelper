#include "gammaeditordialog.h"
#include "ui_gammaeditordialog.h"
#include "gammaworker.h"
#include <QPointer>
#include <QThread>

namespace GME {
    QPointer<QThread> gthread;
}

GammaEditorDialog::GammaEditorDialog(QWidget *parent, Polaroid *pol) :
    QDialog(parent),
    mPol(pol),
    ui(new Ui::GammaEditorDialog)
{
    ui->setupUi(this);
    ui->imAft->setFixedSize(QSize(201, 302));
    ui->imBef->setFixedSize(QSize(201, 302));
    before = mPol->getImage().scaled(ui->imBef->width(), ui->imBef->height(), Qt::KeepAspectRatio);
    after = before;
    auto gval = pol->gammaValue();
    if(gval != 1.0) {
        Polaroid::applyGamma(&after, gval);
    }
    auto fx = QPixmap::fromImage(before);
    auto af = QPixmap::fromImage(after);
    ui->gslide->setMaximum(1000);
    ui->gslide->setMinimum(0);
    ui->gslide->setTickInterval(100);
    ui->gslide->setValue(1000 - (pol->gammaValue() * 1000));
    ui->imBef->setPixmap(fx);
    ui->imAft->setPixmap(af);
}

double GammaEditorDialog::currentGamma()
{
    return ui->gslide->value() / 1000.0;
}

void GammaEditorDialog::setInitialGamma(double g)
{
    initGamma = g;
}

GammaEditorDialog::~GammaEditorDialog()
{
    delete ui;
}

void GammaEditorDialog::on_gslide_valueChanged(int position)
{
    updateToolTiptext();
    if(position != initGamma * 1000) {
        if(GME::gthread) {
            return;
        } else {
            after = before;
            GME::gthread = new QThread();
            GammaWorker *gw = new GammaWorker(nullptr, &after, (ui->gslide->maximum() - ui->gslide->value()) / 1000.0);
            connect(GME::gthread, &QThread::started, gw, &GammaWorker::doGamma);
            connect(gw, &GammaWorker::done, this, [=](){this->updateAfter();});
            connect(gw, &GammaWorker::done, GME::gthread, &QThread::quit);
            connect(gw, &GammaWorker::done, gw, &GammaWorker::deleteLater);
            connect(GME::gthread, &QThread::finished, GME::gthread, &QThread::deleteLater);
            gw->moveToThread(GME::gthread);
            GME::gthread->start();
        }
    }
}

void GammaEditorDialog::updateToolTiptext()
{
    auto val = ui->gslide->value();
    ui->gslide->setToolTip(QString("%1").arg((ui->gslide->maximum() - val) / 1000.0));
}

void GammaEditorDialog::updateAfter()
{
    ui->imAft->setPixmap(QPixmap::fromImage(after));
}



