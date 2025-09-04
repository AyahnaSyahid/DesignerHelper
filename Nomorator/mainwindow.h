#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class Controller;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    Controller *m_controller;
    void setupConnection();
    void interpretNomoratorChanges(const QString& key, const QVariant& var);

private slots:
    void pageSizeUpdate();
    void startPosUpdate();
    void duplicationUpdate();
    void offsetSizeUpdate();
    void on_browsebg_clicked();
    void on_actionHapus_Background_triggered();
};
#endif // MAINWINDOW_H
