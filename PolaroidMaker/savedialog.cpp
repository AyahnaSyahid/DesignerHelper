#include "savedialog.h"
#include "ui_savedialog.h"
#include "counter.h"
#include "exportworker.h"
#include <QThread>
#include <QFileDialog>
#include <QDateTime>
#include <QMessageBox>

QProgressBar *polpgr = nullptr;
QProgressBar *pagpgr = nullptr;

void progressUpdateMaker(const int& page, const int& polindex) {
    if(pagpgr) {
        pagpgr->setValue(page);
    }
    if(polpgr) {
        polpgr->setValue(polindex);
    }
}

SaveDialog::SaveDialog(QWidget *parent, PolaroidMaker *mr) :
    QDialog(parent),
    maker(mr),
    ui(new Ui::SaveDialog)
{
    ui->setupUi(this);
    ui->pageProgress->setMaximum(maker->pageToCreate());
    ui->pageProgress->hide();
    ui->polaroidProgress->setMaximum(maker->polaroidCount());
    ui->polaroidProgress->hide();
    ui->tPolLabel->setText(ui->tPolLabel->text().arg(maker->polaroidCount()));
    ui->tPageLabel->setText(ui->tPageLabel->text().arg(maker->pageToCreate()));
    polpgr = ui->polaroidProgress;
    pagpgr = ui->pageProgress;
    // maker->setProgressUpdateCB(&progressUpdateMaker);
    maker->createConnection(ui->polaroidProgress, ui->pageProgress);
}

SaveDialog::~SaveDialog()
{
    delete ui;
}

void SaveDialog::on_openSave_clicked()
{
    auto selName = QDateTime::currentDateTime().toString("yyyy-MM-dd HH_mm_ss") + ".pdf";
    auto fn = QFileDialog::getSaveFileName(this,
                                           "Simpan sebagai",
                                           selName,
                                           "PDF (*.pdf)");
    if(fn.isEmpty())
        QMessageBox::warning(this, "Dibatalkan", "Anda harus memilih tempat untuk menyimpan file");
    else {
        ui->filePath->setText(fn);
    }
}


void SaveDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    if(button != ui->buttonBox->button(QDialogButtonBox::Ok)) {
        return reject();
    }
    if(ui->filePath->text().isEmpty()) {
        on_openSave_clicked();
        return;
    }
    if(button == ui->buttonBox->button(QDialogButtonBox::Ok)) {
        ui->pageProgress->setHidden(false);
        ui->polaroidProgress->setHidden(false);
        ExportWorker *ew = new ExportWorker(maker, ui->filePath->text());
        QThread *et = new QThread();
        ew->moveToThread(et);
        connect(et, &QThread::started, ew, &ExportWorker::expol);
        connect(ew, &ExportWorker::exportDone, et, &QThread::quit);
        connect(ew, &ExportWorker::exportDone, this, &SaveDialog::acceptAndClose);
        connect(et, &QThread::finished, et, &QThread::deleteLater);
        connect(ew, &ExportWorker::exportDone, ew, &ExportWorker::deleteLater);
        et->start();
    }
}


void SaveDialog::acceptAndClose()
{
    if(!maker->failReason().isEmpty()) {
        QMessageBox::information(this, "Export gagal", maker->failReason());
        emit accepted();
        accept();
        return;
    }
    QMessageBox::information(this, "Info", "Export berhasil");
    Counter *ctr = new Counter();
    auto many = maker->pageToCreate();
    auto hasBonus = ctr->bonus();
    if(many > hasBonus) {
        ctr->setBonus(0);
        ctr->updateAvail(-(many - hasBonus));
        ctr->updateCounter(many);
    } else if(many == hasBonus) {
        ctr->setBonus(0);
        ctr->updateCounter(many);
    } else {
        ctr->updateAvail(-many);
        ctr->updateCounter(many);
    }
    delete ctr;
    emit accepted();
    accept();
}

