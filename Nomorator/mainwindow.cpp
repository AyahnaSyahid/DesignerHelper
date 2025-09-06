#include "mainwindow.h"

#include <QFileDialog>

#include "src/ncontroller.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_controller(new Controller(this)) {
  ui->setupUi(this);
  ui->nstart->setMax(ui->nstop);
  ui->nstop->setMin(ui->nstart);

  ui->nstart->setValue(m_controller->start());
  ui->nstop->setValue(m_controller->stop());
  ui->bd->setValue(m_controller->bind());
  ui->col->setValue(m_controller->column());
  ui->row->setValue(m_controller->row());
  ui->cpad->setText(QString(m_controller->padChar()));
  ui->npad->setValue(m_controller->padCount());
  ui->sfx->setText(m_controller->sufix().c_str());
  ui->prf->setText(m_controller->prefix().c_str());
  ui->stext->setText(
      m_controller->getNomorator()->getPage(0).data[0][0].c_str());
  ui->nposx->setValue(m_controller->startPos().x());
  ui->nposy->setValue(m_controller->startPos().y());
  ui->nofx->setValue(m_controller->offsets().width());
  ui->nofy->setValue(m_controller->offsets().height());
  ui->dupx->setValue(m_controller->dupOffset().width());
  ui->dupy->setValue(m_controller->dupOffset().height());
  if (m_controller->dupOffset().isEmpty()) {
    ui->pdup->setChecked(false);
    ui->dupx->setDisabled(true);
    ui->dupy->setDisabled(true);
  }
  ui->verticalLayout_2->insertWidget(0, m_controller->getView());
  delete ui->tg;
  // Connection
  pageSizeUpdate();
  setupConnection();
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::setupConnection() {
  connect(m_controller, &Controller::parameterChanged, this,
          &MainWindow::interpretNomoratorChanges);
  connect(ui->nstart, SIGNAL(valueChanged(int)), m_controller,
          SLOT(setStart(int)));
  connect(ui->nstop, SIGNAL(valueChanged(int)), m_controller,
          SLOT(setStop(int)));
  connect(ui->bd, SIGNAL(valueChanged(int)), m_controller, SLOT(setBind(int)));
  connect(ui->col, SIGNAL(valueChanged(int)), m_controller,
          SLOT(setColumn(int)));
  connect(ui->row, SIGNAL(valueChanged(int)), m_controller, SLOT(setRow(int)));
  connect(ui->cpad, SIGNAL(textChanged(QString)), m_controller,
          SLOT(setPad(QString)));
  connect(ui->npad, SIGNAL(valueChanged(int)), m_controller, SLOT(setPad(int)));
  connect(ui->prf, SIGNAL(textChanged(QString)), m_controller,
          SLOT(setPrefix(QString)));
  connect(ui->sfx, SIGNAL(textChanged(QString)), m_controller,
          SLOT(setSufix(QString)));
  connect(ui->pwidth, SIGNAL(valueChanged(double)), this,
          SLOT(pageSizeUpdate()));
  connect(ui->pheight, SIGNAL(valueChanged(double)), this,
          SLOT(pageSizeUpdate()));
  connect(ui->nposx, SIGNAL(valueChanged(double)), this,
          SLOT(startPosUpdate()));
  connect(ui->nposy, SIGNAL(valueChanged(double)), this,
          SLOT(startPosUpdate()));
  connect(ui->dupx, SIGNAL(valueChanged(double)), this,
          SLOT(duplicationUpdate()));
  connect(ui->dupy, SIGNAL(valueChanged(double)), this,
          SLOT(duplicationUpdate()));
  connect(ui->nofx, SIGNAL(valueChanged(double)), this,
          SLOT(offsetSizeUpdate()));
  connect(ui->nofy, SIGNAL(valueChanged(double)), this,
          SLOT(offsetSizeUpdate()));
  connect(ui->pdup, &QRadioButton::toggled, this, [=](bool tg) {
    if (!tg) {
      ui->dupx->setValue(0);
      ui->dupy->setValue(0);
    }
  });
  connect(m_controller->getView(), &NomoratorView::startPosMoved, this,
          [=](QPointF f) {
            blockSignals(true);
            ui->nposx->setValue(f.x());
            ui->nposy->setValue(f.y());
            blockSignals(false);
          });
}

void MainWindow::interpretNomoratorChanges(const QString &key,
                                           const QVariant &var) {
  qDebug() << key << " changed (" << var << ")";
  if (key == "prefixStr" || key == "suffixStr" || key == "padChar" ||
      key == "padCount")
    ui->stext->setText(
        m_controller->getNomorator()->getPage(0).data[0][0].c_str());
}

void MainWindow::pageSizeUpdate() {
  QSizeF ps(ui->pwidth->value(), ui->pheight->value());
  m_controller->setPageSize(ps);
}

void MainWindow::startPosUpdate() {
  QPointF np(ui->nposx->value(), ui->nposy->value());
  m_controller->setStartPos(np);
}

void MainWindow::duplicationUpdate() {
  if (ui->pdup->isEnabled()) {
    QSizeF dups(ui->dupx->value(), ui->dupy->value());
    m_controller->setDupOffset(dups);
  }
}

void MainWindow::offsetSizeUpdate() {
  QSizeF nofs(ui->nofx->value(), ui->nofy->value());
  m_controller->setOffsets(nofs);
}

void MainWindow::on_browsebg_clicked() {
  auto fn = QFileDialog::getOpenFileName(this, "Pilih Background(Master)",
                                         QDir::homePath(),
                                         "Image File ( *.jpg *.png *.jpeg)");
  if (!fn.isEmpty()) {
    ui->imfname->setText(fn);
    m_controller->getView()->setBgImage(fn);
  }
}

void MainWindow::on_actionHapus_Background_triggered() {
  m_controller->getView()->clearBg();
}
