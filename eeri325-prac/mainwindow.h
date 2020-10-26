#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDirIterator>
#include <QSound>
#include <QAudioDecoder>
#include <QAudioDeviceInfo>
#include <QtGlobal>

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
    QAudioDecoder *decoder;
    QVector <double> orgSoundSignal;

    public slots:
    void updateSoundFiles();
    void playOrgSound();
    void decodeOrgSound();
    void readBuffer();
    void plotOrgTimePlot();

private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
