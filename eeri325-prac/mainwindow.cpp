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

    //image stuffs
    InputImage = new QImage("res/original.jpg");
    InputImage->convertTo(QImage::Format_Grayscale16);
    qDebug() << InputImage->format();
    qDebug() << InputImage->width();
    qDebug() << InputImage->height();
    qDebug() << InputImage->isGrayscale();
    QPoint p(3000, 4000);
    QRgba64 color = (1, 1, 1);

    for(int i = InputImage->width()/2; i < InputImage->width(); i++)
        for(int j = InputImage->height()/2; j < InputImage->height(); j++)
        {
            InputImage->setPixel(i,j,color);
        }
    qDebug() << InputImage->pixelColor(p);


    ui->picOriginal->setPixmap(QPixmap::fromImage(*InputImage));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete orgSound;
}

//convert real vector to complex vector
vector<complex<double>> MainWindow::toComplex(QVector<double> &signal)
{
    vector<complex<double>> output(signal.size(), 0);
    for (int i=0; i<signal.size() ; i++) {
        output[i] = {signal[i], 0.0};
    }
    return output;
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
    sampleFreq = 22050;
    if (QFile::exists(ui->orgCmbInput->currentText()))
    {
        QFileInfo source(ui->orgCmbInput->currentText());
        qDebug() << (source.absoluteFilePath());

        QAudioFormat desiredFormat;
        desiredFormat.setChannelCount(2);
        desiredFormat.setCodec("audio/x-wav");
        desiredFormat.setSampleType(QAudioFormat::SignedInt);
        desiredFormat.setSampleRate((int)sampleFreq);
        desiredFormat.setSampleSize(16);

        decoder = new QAudioDecoder(this);
        decoder->setAudioFormat(desiredFormat);
        decoder->setSourceFilename(source.absoluteFilePath());

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
    ui->orgBtnPlot->setText("Plotting wav");
    ui->orgBtnPlot->setEnabled(false);
    ui->orgPlotTime->setInteraction(QCP::iRangeDrag, true);
    ui->orgPlotTime->setInteraction(QCP::iRangeZoom, true);
    //ui->orgPlotTime->axisRect()->setRangeZoomAxes(Qt::Vertical | Qt::Horizontal);
    // generate some data:
    QVector<double> t; // initialize entries

    double period = 1/sampleFreq;
    double max = 0;
    for (int k = 0; k < orgSoundSignal.size(); k++) {
        t.append(k*period);
        if (orgSoundSignal[k] > max)
            max = orgSoundSignal[k];
    }

    // create graph and assign data to it:
    ui->orgPlotTime->addGraph();
    ui->orgPlotTime->graph(0)->setData(t, orgSoundSignal);
    // give the axes some labels:
    ui->orgPlotTime->xAxis->setLabel("time[s]");
    ui->orgPlotTime->yAxis->setLabel("Magnitude");
    // set axes ranges, so we see all data:
    ui->orgPlotTime->xAxis->setRange(0, t.last());
    ui->orgPlotTime->yAxis->setRange(-max, max);
    ui->orgPlotTime->replot();

    testfft();
    plotFilteredSignal();
}

void MainWindow::plotFilteredSignal()
{
    ui->FilSound->setInteraction(QCP::iRangeDrag, true);
    ui->FilSound->setInteraction(QCP::iRangeZoom, true);

    filteredSoundSignal = My4kLPF(orgSoundSignal);
    QVector<double> t; // initialize entries

    double period = 1/sampleFreq;
    double max = 0;
    for (int k = 0; k < filteredSoundSignal.size(); k++) {
        t.append(k*period);
        if (filteredSoundSignal[k] > max)
            max = filteredSoundSignal[k];
    }

    // create graph and assign data to it:
    ui->FilSound->addGraph();
    ui->FilSound->graph(0)->setData(t, filteredSoundSignal);
    // give the axes some labels:
    ui->FilSound->xAxis->setLabel("time[s]");
    ui->FilSound->yAxis->setLabel("Magnitude");
    // set axes ranges, so we see all data:
    ui->FilSound->xAxis->setRange(0, t.last());
    ui->FilSound->yAxis->setRange(-max, max);
    ui->FilSound->replot();

    filfft();
}

void MainWindow::testfft()
{
    //test signal parameters
    cmpOrgSoundSignal = toComplex(orgSoundSignal);

    qDebug() << "FFT";
    vector<complex<double>> bins = MyFFT(cmpOrgSoundSignal); //calculate pure dft
    QVector<double> mag = myMagSpectrum(bins);
    //clean up signal
    for (int i=0; i<mag.size() ; i++)
    {
        mag[i] = mag[i]*2.0/double(mag.size());
    }

    //plot
    ui->orgPlotFreq->setInteraction(QCP::iRangeDrag, true);
    ui->orgPlotFreq->setInteraction(QCP::iRangeZoom, true);
    //ui->orgPlotFreq->axisRect()->setRangeZoomAxes(Qt::Vertical | Qt::Horizontal);
    // generate some data:
    QVector<double> f; // initialize frequency axis
    double freqResolution = (double)sampleFreq/(double)bins.size();
    qDebug() << "Resolution: " << freqResolution;

    double max = 0;
    for (int k = 0; k <mag.size()/2; k++){
        f.append(k*freqResolution);
        if (mag[k]>max)
            max = mag[k];
    }

    // create graph and assign data to it:
    ui->orgPlotFreq->addGraph();
    ui->orgPlotFreq->graph(0)->setData(f, mag);
    // give the axes some labels:
    ui->orgPlotFreq->xAxis->setLabel("Frequency [Hz]");
    ui->orgPlotFreq->yAxis->setLabel("Magnitude");
    // set axes ranges, so we see all data:
    ui->orgPlotFreq->xAxis->setRange(0, f[f.size()-1]);
    ui->orgPlotFreq->yAxis->setRange(0, max);
    ui->orgPlotFreq->replot();
}

void MainWindow::filfft()
{
    //test signal parameters
    cmpfilteredSignal = toComplex(filteredSoundSignal);

    qDebug() << "FFT";
    vector<complex<double>> bins = MyFFT(cmpfilteredSignal); //calculate pure dft
    QVector<double> mag = myMagSpectrum(bins);
    //clean up signal
    for (int i=0; i<mag.size() ; i++)
    {
        mag[i] = mag[i]*2.0/double(mag.size());
    }

    //plot
    ui->FreqFilSound->setInteraction(QCP::iRangeDrag, true);
    ui->FreqFilSound->setInteraction(QCP::iRangeZoom, true);
    //ui->FreqFilSound->axisRect()->setRangeZoomAxes(Qt::Vertical | Qt::Horizontal);
    // generate some data:
    QVector<double> f; // initialize frequency axis
    double freqResolution = (double)sampleFreq/(double)bins.size();
    qDebug() << "Resolution: " << freqResolution;

    double max = 0;
    for (int k = 0; k <mag.size()/2; k++){
        f.append(k*freqResolution);
        if (mag[k]>max)
            max = mag[k];
    }

    // create graph and assign data to it:
    ui->FreqFilSound->addGraph();
    ui->FreqFilSound->graph(0)->setData(f, mag);
    // give the axes some labels:
    ui->FreqFilSound->xAxis->setLabel("Frequency [Hz]");
    ui->FreqFilSound->yAxis->setLabel("Magnitude");
    // set axes ranges, so we see all data:
    ui->FreqFilSound->xAxis->setRange(0, f[f.size()-1]);
    ui->FreqFilSound->yAxis->setRange(0, max);
    ui->FreqFilSound->replot();

}

//    complex<double> sample;
//    for (int i = 0; i < 32; i++){
//        sample = complex<double>(sin(1*i*(M_PI/4)), 0.0);
//        samples.push_back(sample);
//        qDebug() << real(samples[i]) << "+j(" << imag(samples[i]) << "), ";
//    }
