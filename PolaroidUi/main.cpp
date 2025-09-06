#include <QAction>
#include <QApplication>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QSettings>
#include <QTimer>

#define APP_VERSION 1.6

#include "polaroidmakerwidget.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  a.setOrganizationName("BlackCircle");
  a.setApplicationName("PolaroidMaker");
  a.setApplicationVersion(QString("%1").arg(APP_VERSION));
  QSettings deepFreezeUsersDelete(
      "HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432Node\\Faronics\\Deep Freeze 6",
      QSettings::NativeFormat);
  QString state = deepFreezeUsersDelete.value("DF Status", "").toString();
  if (state == "Frozen") {
    QTimer tm;
    tm.setSingleShot(true);
    tm.setInterval(20);
    tm.connect(&tm, &QTimer::timeout, &a, &QApplication::quit);
    tm.start();
    return a.exec();
  }

  QMainWindow mw;
  PolaroidMakerWidget pw;
  auto mb = mw.menuBar();
  auto mm = mb->addMenu("Lihat");
  mm->addActions(pw.actions());
  auto ab = mb->addAction("Tentang Qt");
  ab->connect(ab, &QAction::triggered, &a, &QApplication::aboutQt);
  mw.setCentralWidget(&pw);
  mw.show();
  return a.exec();
}
