#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //Connect signals and slots
    connect(ui->orgBtnUpdate, SIGNAL(clicked()), this, SLOT(updateSoundFiles()));
    connect(ui->orgBtnPlay, SIGNAL(clicked()), this, SLOT(playOrgSound()));
    connect(ui->orgCmbInput, SIGNAL(currentIndexChanged(QString)), this, SLOT(decodeOrgSound()));
    connect(ui->orgBtnPlot, SIGNAL(clicked()), this, SLOT(plotOrgTimePlot()));

}

MainWindow::~MainWindow()
{
    delete ui;
    delete orgSound;
}

void MainWindow::updateSoundFiles()
{
    QDirIterator it("./res", QDirIterator::Subdirectories);
    ui->orgCmbInput->clear();
    while (it.hasNext()) {
        it.next();
        if (it.fileName().contains("wav", Qt::CaseInsensitive)){
            ui->orgCmbInput->addItem(it.filePath());
        }
    }
}

void MainWindow::playOrgSound()
{
    if (orgSound != nullptr)
    {
        //check if still playing then stop
        if (orgSound->isFinished() == false)
        {
            orgSound->stop();
            ui->orgBtnPlay->setText("Play");
        }
        else    //play if not playing
        {
            if (QFile::exists(ui->orgCmbInput->currentText())){
                orgSound->play(ui->orgCmbInput->currentText());
                ui->orgBtnPlay->setText("Stop");
            } else {
                ui->orgBtnPlay->setText("Play");
                qDebug() << "playOrgSound: File does not exist.";
            }
        }

    } else {    //instantiate object if not already.
        if (QFile::exists(ui->orgCmbInput->currentText())){
            orgSound = new QSound(ui->orgCmbInput->currentText());
            orgSound->play();
            ui->orgBtnPlay->setText("Stop");
        } else {
            ui->orgBtnPlay->setText("Play");
            qDebug() << "playOrgSound: File does not exist.";
        }
    }
}

void MainWindow::decodeOrgSound()
{
    if (QFile::exists(ui->orgCmbInput->currentText())){
        QAudioFormat desiredFormat;
        desiredFormat.setChannelCount(2);
        desiredFormat.setCodec("audio/x-wav");
        desiredFormat.setSampleType(QAudioFormat::SignedInt);
        desiredFormat.setSampleRate(44100);
        desiredFormat.setSampleSize(16);

        decoder = new QAudioDecoder(this);
        decoder->setAudioFormat(desiredFormat);
        decoder->setSourceFilename("/home/adam/drive/Projects/eeri325-prac/build-eeri325-prac-Desktop-Debug/res/500sine.wav");

        connect(decoder, SIGNAL(bufferReady()), this, SLOT(readBuffer()));
        decoder->start();
    }
    else {
        qDebug() << "plotOrgSound: File does not exist";
    }
}

void MainWindow::readBuffer()
{
    QAudioBuffer buffer = decoder->read();
    const qint16 *data = buffer.data<qint16>();
    for (int k = 0; k < buffer.sampleCount(); k++)
    {
           orgSoundSignal.append((double)(*(data+k)));
    }
    qDebug() << "Buffer Increased to size " << orgSoundSignal.size();
}

void MainWindow::plotOrgTimePlot()
{
    ui->orgPlotTime->setInteraction(QCP::iRangeDrag, true);
    ui->orgPlotTime->setInteraction(QCP::iRangeZoom, true);
    //ui->orgPlotTime->axisRect()->setRangeZoomAxes(Qt::Vertical | Qt::Horizontal);
    // generate some data:
    QVector<double> x; // initialize entries

    double max = 0;
    for (int k = 0; k < orgSoundSignal.size(); k++) {
        x.append(k);
        if (orgSoundSignal[k] > max)
            max = orgSoundSignal[k];
    }

    // create graph and assign data to it:
    ui->orgPlotTime->addGraph();
    ui->orgPlotTime->graph(0)->setData(x, orgSoundSignal);
    // give the axes some labels:
    ui->orgPlotTime->xAxis->setLabel("time");
    ui->orgPlotTime->yAxis->setLabel("Magnitude");
    // set axes ranges, so we see all data:
    ui->orgPlotTime->xAxis->setRange(0, x.size());
    ui->orgPlotTime->yAxis->setRange(-max, max);
    ui->orgPlotTime->replot();
}
