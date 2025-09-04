#include "polaroidmakerwidget.h"
#include "ui_polaroidmakerwidget.h"
#include "dialogdetailcounter.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QDirIterator>
#include <QMessageBox>
#include "sizetemplatedialog.h"
#include "gammaeditordialog.h"
#include "savedialog.h"
#include "refillform.h"

PolaroidMakerNS::SizeFListsTable PolaroidMakerWidget::s_table {};

PolaroidMakerWidget::PolaroidMakerWidget(QWidget *parent) :
    QWidget(parent),
    stm(new SizeTemplateModel(this, &PolaroidMakerWidget::s_table)),
    ui(new Ui::PolaroidMakerWidget)
{
    ui->setupUi(this);
    cropColor = QColor(Qt::gray);
    bgColor = QColor(Qt::white);
    markColor = QColor(Qt::gray);

    proxUK = new QSortFilterProxyModel(this);
    proxUP = new QSortFilterProxyModel(this);

    proxUK->setSourceModel(stm);
    proxUP->setSourceModel(stm);

    proxUK->setFilterFixedString("Kertas");
    proxUP->setFilterFixedString("Polaroid");
    proxUK->setFilterKeyColumn(0);
    proxUP->setFilterKeyColumn(0);

    ui->kTemplate->setModel(proxUK);
    ui->kTemplate->setModelColumn(1);
    ui->pTemplate->setModel(proxUP);
    ui->pTemplate->setModelColumn(1);

    polModel = maker.polaroidModel();

    connect(polModel, &QAbstractItemModel::dataChanged, this, &PolaroidMakerWidget::refreshCounter);
    connect(polModel, &QAbstractItemModel::modelReset, this, &PolaroidMakerWidget::refreshCounter);
    connect(polModel, &QAbstractItemModel::rowsRemoved, this, &PolaroidMakerWidget::refreshCounter);
    connect(polModel, &QAbstractItemModel::rowsInserted, this, &PolaroidMakerWidget::refreshCounter);
    connect(this, &PolaroidMakerWidget::ukuranKertasChanged, this, &PolaroidMakerWidget::refreshCounter);
    connect(this, &PolaroidMakerWidget::ukuranPolaroidChanged, this, &PolaroidMakerWidget::refreshCounter);

    ui->listView->setModel(polModel);
    ui->listView->setIconSize(QSize(128, 128));
    ui->listView->setUniformItemSizes(true);
    ui->listView->setResizeMode(ui->listView->Adjust);
    ui->delPol->setEnabled(false);
    ui->propPol->setEnabled(false);
    ui->propPol->hide();
    ui->eGamma->setEnabled(false);
    ui->restPol->setEnabled(false);
    ui->statL->setText("");

    connect(ui->listView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &PolaroidMakerWidget::deleteToggler);
    connect(this, &PolaroidMakerWidget::ukuranKertasChanged, this,
            [=](){this->setUkuranKertas(
                    QSizeF(ui->kWSize->value(),
                           ui->kHSize->value()));});
    connect(this, &PolaroidMakerWidget::ukuranPolaroidChanged, this,
            [=](){this->setUkuranPolaroid(
                    QSizeF(ui->pWSize->value(),
                           ui->pHSize->value()));});
    ui->progBar->setHidden(true);
    auto act = new QAction("Counter", this);
    this->addAction(act);
    act->setObjectName("actionLihatCounter");
    connect(act, &QAction::triggered, this, &PolaroidMakerWidget::lihatCounter);
    loadConfiguration();
}

PolaroidMakerWidget::~PolaroidMakerWidget()
{
    delete ui;
    delete stm;
    delete polModel;
    delete proxUK;
    delete proxUP;
}

void PolaroidMakerWidget::loadUkuranKertasList()
{

}

void PolaroidMakerWidget::loadUkuranPolaroidList()
{

}

void PolaroidMakerWidget::refreshCounter()
{
    QString fmt("Total Polaroid : %1, Total Halaman : %2");
    fmt = fmt.arg(maker.polaroidCount()).arg(maker.pageToCreate());
    ui->statL->setText(fmt);
}

void PolaroidMakerWidget::deleteToggler()
{
    bool hasSelected(ui->listView->selectionModel()->hasSelection());
    ui->delPol->setEnabled(hasSelected);
    ui->propPol->setEnabled(hasSelected);
    ui->eGamma->setEnabled(hasSelected);
    bool hasItem(ui->listView->model()->rowCount());
    ui->restPol->setEnabled(hasItem);
}

void PolaroidMakerWidget::loadConfiguration()
{
    QString pt = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    if(!QFile::exists(pt)) {
        QDir dr;
        dr.mkpath(pt);
    }
    stm->setDataFile( pt + "/uklist.txt");
}

void PolaroidMakerWidget::on_kTemplate_activated(const QString &arg1)
{
    auto sz = s_table.get("Kertas", arg1);
    ui->kWSize->setValue(sz.width());
    ui->kHSize->setValue(sz.height());
}

void PolaroidMakerWidget::on_pTemplate_activated(const QString &arg1)
{
    auto pair = s_table.get("Polaroid", arg1);
    ui->pWSize->setValue(pair.width());
    ui->pHSize->setValue(pair.height());
}

void PolaroidMakerWidget::on_kWSize_valueChanged(double arg1)
{
    Q_UNUSED(arg1)
    emit ukuranKertasChanged();
}

void PolaroidMakerWidget::on_kHSize_valueChanged(double arg1)
{
    Q_UNUSED(arg1)
    emit ukuranKertasChanged();
}

void PolaroidMakerWidget::on_pWSize_valueChanged(double arg1)
{
    Q_UNUSED(arg1)
    emit ukuranPolaroidChanged();
}

void PolaroidMakerWidget::on_pHSize_valueChanged(double arg1)
{
    Q_UNUSED(arg1)
    emit ukuranPolaroidChanged();
}

void PolaroidMakerWidget::on_colTButton_clicked()
{
    auto nc = QColorDialog::getColor(markColor, this, "Pilih warna teks");
    if(nc.isValid()) {
        if(nc != markColor) {
            ui->colLPrev->setStyleSheet(QString("background-color: %1").arg(nc.name(QColor::HexRgb)));
            markColor = nc;
        }
    }
}

void PolaroidMakerWidget::on_colCButton_clicked()
{
    auto nc = QColorDialog::getColor(cropColor, this, "Pilih warna cropmark");
    if(nc.isValid()) {
        if(nc != cropColor) {
            ui->colCPrev->setStyleSheet(QString("background-color: %1").arg(nc.name(QColor::HexRgb)));
            cropColor = nc;
        }
    }
}

void PolaroidMakerWidget::on_colBButton_clicked()
{
    auto nc = QColorDialog::getColor(bgColor, this, "Pilih warna background");
    if(nc.isValid())
        if(nc != bgColor) {
            ui->colBPrev->setStyleSheet(QString("background-color: %1").arg(nc.name(QColor::HexRgb)));
            bgColor = nc;
        }
}

void PolaroidMakerWidget::on_pushButton_clicked()
{
    SizeTemplateDialog dlg(this, stm);
    dlg.setWindowTitle("Tambahkan Ukuran");
    dlg.exec();
}

void PolaroidMakerWidget::on_delPol_clicked()
{
    auto rows = ui->listView->selectionModel()->selectedRows();
    for(auto r : qAsConst(rows)) {
        polModel->removeRow(r.row());
//        qDebug() << "delPol Clicked" << " R =" << r;
    }
    ui->listView->selectionModel()->clear();
}

void PolaroidMakerWidget::on_restPol_clicked()
{
    qobject_cast<PolaroidListModel*>(polModel)->clearData();
    ui->restPol->setDisabled(true);
}

void PolaroidMakerWidget::on_addFold_clicked()
{
    auto dir = QFileDialog::getExistingDirectory(this, "Pilih folder", "");
    if(dir.isEmpty())
        return;
//    qDebug() << dir;
    QDirIterator dit(dir, QStringList() << "*.png" << "*.jpeg" << "*.webp" << "*.jpg", QDir::Files | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    QStringList files;
    while(dit.hasNext()) {
        auto dn(dit.next());
//        qDebug() << dn;
        files << dn;
    }
    if(files.empty())
        return;
    PolaroidListModel *plm = static_cast<PolaroidListModel*>(polModel);
    plm->insertImages(files);
}

void PolaroidMakerWidget::on_addFile_clicked()
{
    auto fn = QFileDialog::getOpenFileNames(this, "Pilih beberapa gambar", "", "Gambar (*.png *.jpg *.jpeg *.webp)");
    if(fn.isEmpty())
        return;
    PolaroidListModel *plm = static_cast<PolaroidListModel*>(polModel);
    plm->insertImages(fn);
}

void PolaroidMakerWidget::on_propPol_clicked()
{
    auto smod = ui->listView->selectionModel();
    auto mod = static_cast<PolaroidListModel*>(polModel);
    if(!smod->hasSelection())
        return;
    auto &pol = mod->getPolaroid(smod->selectedRows()[0].row());
    Q_UNUSED(pol)
}

void PolaroidMakerWidget::on_aGamma_editingFinished()
{
    if(ui->aGamma->value() > 1.0) {
        return;
    }
    auto mod = static_cast<PolaroidListModel*>(polModel);
    mod->setGamma(ui->aGamma->value());
}

void PolaroidMakerWidget::setUkuranKertas(const QSizeF &sf)
{
    maker.setPageSize(sf);
}

void PolaroidMakerWidget::setUkuranPolaroid(const QSizeF &sp)
{
    maker.setPolaroidSize(sp);
}

void PolaroidMakerWidget::on_eGamma_clicked()
{
    if(!ui->listView->selectionModel()->hasSelection())
        return;
    Polaroid &pl = static_cast<PolaroidListModel*>(polModel)->getPolaroid(ui->listView->selectionModel()->selectedRows()[0].row());
    GammaEditorDialog ged(this, &pl);
    ged.setWindowTitle("Gamma Preview");
    auto ok = ged.exec();
    if(ok == ged.Accepted) {
        ui->aGamma->setValue(1.0 - ged.currentGamma());
        auto mod = static_cast<PolaroidListModel*>(polModel);
        mod->setGamma(ui->aGamma->value());
    }
}

void PolaroidMakerWidget::on_doButton_clicked()
{
    if(maker.pageToCreate() < 1) {
        return;
    }
    Counter *ctr = new Counter();
    connect(ctr, &Counter::refillSuccess, this, &PolaroidMakerWidget::refillSuccessHandler);
    connect(ctr, &Counter::refillFailed, this, &PolaroidMakerWidget::refillFailedHandler);

    if(!ctr->canAcceptRequest()) {
        QMessageBox tokenHabis(this);
        tokenHabis.setInformativeText("Peringatan Token habis");
        tokenHabis.setText("Silahkan isi token page anda agar dapat menggunakan apllikasi kembali");
        tokenHabis.addButton(QMessageBox::Open);
        tokenHabis.addButton(QMessageBox::Ok);
        tokenHabis.setButtonText(QMessageBox::Open, "Isi token");
        if(tokenHabis.exec() == QMessageBox::Open)
        {
            RefillForm rf(this);
            connect(&rf, &RefillForm::tokenReady, ctr, &Counter::refill);
            connect(ctr, &Counter::refillSuccess, &rf, &RefillForm::accept);
            rf.exec();
        }
        delete ctr;
        return;
    }
    delete ctr;
    // File save dialog
    maker.setDpi(ui->outDpi->value());
    maker.setGamma(ui->aGamma->value());
    maker.setPageSize(QSizeF(ui->kWSize->value(), ui->kHSize->value()));
    maker.setPolaroidSize(QSizeF(ui->pWSize->value(), ui->pHSize->value()));
    maker.setPageNumbering(ui->pNumCheck->isChecked());
    maker.setPageMarking(true);
    maker.setPageMarkingText(ui->markText->text());
    maker.setPageMarkingColor(markColor);
    maker.setPageMarkingFont(ui->fontComboBox->currentFont());
    PolaroidListModel *pm = static_cast<PolaroidListModel*>(polModel);
    // Apply crop, bg, etc
    for(auto pth = 0; pth < maker.polaroidCount(); ++pth)
    {
        auto &pol = pm->getPolaroid(pth);
        pol.setBackgroudColor(bgColor);
        pol.setCropmarkColor(cropColor);
        pol.setCropmarkSize(ui->cropSize->value());
        pol.setCropmarkWeight(ui->cropWeight->value());
    }

    SaveDialog sg(this, &maker);
    sg.setWindowTitle("Menyimpan File");
    auto acc = sg.exec();
    if(acc == sg.Accepted) {
        static_cast<PolaroidListModel*>(polModel)->clearData();
        emit operationSuccess();
    }
}

void PolaroidMakerWidget::refillFailedHandler(const QString &msg)
{
    QMessageBox::warning(this, "Pegisian gagal", msg);
}

void PolaroidMakerWidget::refillSuccessHandler()
{
    QMessageBox msg;
    msg.setWindowTitle("Refill sukses");
    uint avail=0, bonus=0;
    Counter *ctr = this->findChild<Counter*>();
    if(ctr) {
        avail = ctr->avail();
        bonus = ctr->bonus();
    } else {
        ctr = new Counter(this);
        avail = ctr->avail();
        bonus = ctr->bonus();
        delete ctr;
    }
    msg.setText(QString("Counter saat ini :%1\nBonus counter : %2").arg(avail).arg(bonus));
    msg.exec();
}

void PolaroidMakerWidget::lihatCounter()
{
    Counter *ctr = this->findChild<Counter*>();
    if(!ctr)
    {
        ctr = new Counter(this);
    }
    DialogDetailCounter ddc(ctr, this);
    ddc.exec();
    ctr->deleteLater();
}

