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

void MainWindow::MakePlot()
{
    // generate some data:
    QVector<double> x(101), y(101); // initialize with entries 0..100
    for (int i=0; i<101; ++i)
    {
      x[i] = i/50.0 - 1; // x goes from -1 to 1
      y[i] = x[i]*x[i]; // let's plot a quadratic function
    }
    // create graph and assign data to it:
    ui->orgPlotTime->addGraph();
    ui->orgPlotTime->graph(0)->setData(x, y);
    // give the axes some labels:
    ui->orgPlotTime->xAxis->setLabel("time");
    ui->orgPlotTime->yAxis->setLabel("Magnitude");
    // set axes ranges, so we see all data:
    ui->orgPlotTime->xAxis->setRange(-1, 1);
    ui->orgPlotTime->yAxis->setRange(0, 1);
    ui->orgPlotTime->replot();
}
