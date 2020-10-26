#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDirIterator>
#include <QSound>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QSound *orgSound;

    public slots:
    void updateSoundFiles();
    void playOrgSound();
    void MakePlot();

private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
