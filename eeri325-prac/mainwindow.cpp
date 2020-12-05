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
    //connect(ui->orgCmbInput, SIGNAL(currentIndexChanged(QString)), this, SLOT(decodeOrgSound()));
    connect(ui->orgBtnPlot, SIGNAL(clicked()), this, SLOT(plotOrgTimePlot()));
    connect(ui->btnTransform, SIGNAL(clicked()), this, SLOT(process2d()));
    connect(ui->filBtnplot, SIGNAL(clicked()), this, SLOT(plotFilteredSignal()));
    connect(ui->filFFTcmb, SIGNAL(currentIndexChanged(int)), this, SLOT(filfft()));

    //image stuffs
    InputImage = new QImage("res/original.jpg");
    InputImage->convertTo(QImage::Format_Grayscale16);
    qDebug() << InputImage->format();
    qDebug() << InputImage->width();
    qDebug() << InputImage->height();
    qDebug() << InputImage->isGrayscale();

//    QRgb color = qRgb(122, 122, 122);
//
//    for(int i = InputImage->width()/2; i < InputImage->width(); i++)
//        for(int j = InputImage->height()/2; j < InputImage->height(); j++)
//        {
//            InputImage->setPixel(i,j,color);
//        }
//
//    vecInputImage  = saveImage(*InputImage);
//    qDebug() << vecInputImage.size();
//    qDebug() << vecInputImage[0].size();
//    *InputImage = setImage(vecInputImage);
//
//    ui->picOriginal->setPixmap(QPixmap::fromImage(*InputImage));
//
    decodeOrgSound();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete orgSound;
}

vector<vector<int>> MainWindow::saveImage(QImage &Image)
{
    //loop through image
    vector<vector<int>> res(Image.width());

    for(int i=0; i<Image.width(); i++){
        res[i] = vector<int>(Image.height());
        for(int j=0; j<Image.height(); j++)
        {
            res[i][j] = Image.pixelColor(i, j).blue();
        }
    }
    return res;
}

QImage MainWindow::setImage(vector<vector<int> > &ImgVec)
{
    QImage Img(ImgVec.size(), ImgVec[0].size(), QImage::Format_Grayscale16);
    QRgb color = qRgb(122, 122, 122);
    for(int i=0; i<ImgVec.size(); i++){
        for(int j=0; j<ImgVec[i].size(); j++)
        {
            color = qRgb(ImgVec[i][j], ImgVec[i][j], ImgVec[i][j]);
            Img.setPixel(i,j,color);
        }
    }
    return Img;
}

vector<complex<double> > MainWindow::toComplex(QVector<double> &signal)
{
    vector<complex<double>> output(signal.size(), 0);
    for (int i=0; i<signal.size() ; i++) {
        output[i] = {signal[i], 0.0};
    }
    return output;
}

void MainWindow::process2d()
{
   ftw_run();
}

void MainWindow::processImage()
{
    qDebug() << "hi";
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
    QFileInfo source("res/africa-toto.wav");
    if (source.exists())
    {
        //QFileInfo source(ui->orgCmbInput->currentText());
        qDebug() << (source.absoluteFilePath());

        QAudioFormat desiredFormat;
        desiredFormat.setChannelCount(1);
        desiredFormat.setCodec("audio/x-wav");
        desiredFormat.setSampleType(QAudioFormat::SignedInt);
        desiredFormat.setSampleRate((int)sampleFreq);
        desiredFormat.setSampleSize(16);

        decoder = new QAudioDecoder(this);
        decoder->setAudioFormat(desiredFormat);
        decoder->setSourceFilename(source.absoluteFilePath());

        qDebug() << "Decoding Input File";
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
    qDebug() << "Decoded stuff";
}

void MainWindow::plotOrgTimePlot()
{
    ui->orgBtnPlot->setText("Computing FFT");
    ui->orgBtnPlot->setEnabled(false);

    ui->orgPlotTime->setInteraction(QCP::iRangeDrag, true);
    ui->orgPlotTime->setInteraction(QCP::iRangeZoom, true);
    //ui->orgPlotTime->axisRect()->setRangeZoomAxes(Qt::Vertical | Qt::Horizontal);

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

    testfft(); //Find FreqResponse of orginal Signal
}

void MainWindow::plotOrgMagSpec()
{
    ui->orgPlotFreq->clearItems();
    ui->orgPlotFreq->clearGraphs();
    ui->orgPlotFreq->replot();

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

void MainWindow::plotOrgPhaseSpec()
{
    ui->orgPlotFreq->clearItems();
    ui->orgPlotFreq->clearGraphs();
    ui->orgPlotFreq->replot();

    QVector<double> phase = myPhaseSpectrum(bins);

    //plot
    ui->orgPlotFreq->setInteraction(QCP::iRangeDrag, true);
    ui->orgPlotFreq->setInteraction(QCP::iRangeZoom, true);

    QVector<double> f; // initialize frequency axis
    double freqResolution = (double)sampleFreq/(double)bins.size();
    qDebug() << "Resolution: " << freqResolution;

    double max = 0;
    for (int k = 0; k <phase.size()/2; k++){
        f.append(k*freqResolution);
        if (phase[k]>max)
            max = phase[k];
    }

    // create graph and assign data to it:
    ui->orgPlotFreq->addGraph();
    ui->orgPlotFreq->graph(0)->setData(f, phase);
    // give the axes some labels:
    ui->orgPlotFreq->xAxis->setLabel("Frequency [Hz]");
    ui->orgPlotFreq->yAxis->setLabel("Phase [rad/s]");
    // set axes ranges, so we see all data:
    ui->orgPlotFreq->xAxis->setRange(0, f[f.size()-1]);
    ui->orgPlotFreq->yAxis->setRange(0, max);
    ui->orgPlotFreq->replot();

}

void MainWindow::plotFilteredSignal()
{
    ui->filBtnplot->setText("Computing DFT of filterd sound");
    ui->filBtnplot->setEnabled(false);
    qDebug() << ui->filCmb->currentIndex();

    //determine which filter to use
    switch (ui->filCmb->currentIndex()) {
        case -1: 	filteredSoundSignal = My4kLPF(orgSoundSignal);
                    break;
        case 0:		filteredSoundSignal = My4kLPF(orgSoundSignal);
                    break;

        case 1:		filteredSoundSignal = My4kHPF(orgSoundSignal);
                    break;
    }

    ui->FilSound->setInteraction(QCP::iRangeDrag, true);
    ui->FilSound->setInteraction(QCP::iRangeZoom, true);

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

    //find fft of filtered signal
    cmpfilteredSignal = toComplex(filteredSoundSignal);
    filteredfft = MyFFT(cmpfilteredSignal);

    switch (ui->filFFTcmb->currentIndex()) {
        case 0:	plotFilteredMag();
                break;
        case 1:	plotFilteredPhase();
                break;
        case 2:	plotInv();
                break;
        case 3:	plotInv();
                break;

    }

    ui->filBtnplot->setText("Done");
    ui->filBtnplot->setEnabled(true);
}

void MainWindow::plotFilteredMag()
{
    ui->FreqFilSound->clearItems();
    ui->FreqFilSound->clearGraphs();
    ui->FreqFilSound->replot();

    ui->filBtnplot->setText("Plotting");
    ui->filBtnplot->setEnabled(true);

    qDebug() << "FFT Filtered Mag";
    QVector<double> mag = myMagSpectrum(filteredfft);
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
    double freqResolution = (double)sampleFreq/(double)filteredfft.size();
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

    ui->filBtnplot->setText("Plot");
    ui->filBtnplot->setEnabled(true);

}

void MainWindow::plotFilteredPhase()
{
    ui->FreqFilSound->clearItems();
    ui->FreqFilSound->clearGraphs();
    ui->FreqFilSound->replot();

    QVector<double> phase = myPhaseSpectrum(filteredfft);

    //plot
    ui->FreqFilSound->setInteraction(QCP::iRangeDrag, true);
    ui->FreqFilSound->setInteraction(QCP::iRangeZoom, true);

    QVector<double> f; // initialize frequency axis
    double freqResolution = (double)sampleFreq/(double)filteredfft.size();
    qDebug() << "Resolution: " << freqResolution;

    double max = 0;
    for (int k = 0; k <phase.size()/2; k++){
        f.append(k*freqResolution);
        if (phase[k]>max)
            max = phase[k];
    }

    // create graph and assign data to it:
    ui->FreqFilSound->addGraph();
    ui->FreqFilSound->graph(0)->setData(f, phase);
    // give the axes some labels:
    ui->FreqFilSound->xAxis->setLabel("Frequency [Hz]");
    ui->FreqFilSound->yAxis->setLabel("Phase [rad/s]");
    // set axes ranges, so we see all data:
    ui->FreqFilSound->xAxis->setRange(0, f[f.size()-1]);
    ui->FreqFilSound->yAxis->setRange(0, max);
    ui->FreqFilSound->replot();

}

void MainWindow::plotInv()
{
    qDebug("Inverse please");
    ui->orgPlotFreq->clearItems();
    ui->orgPlotFreq->clearGraphs();
    ui->orgPlotFreq->replot();

    invfft = MyInvFFT(filteredfft);

    QVector<double> invRl(invfft.size());

    //plot
    ui->orgPlotFreq->setInteraction(QCP::iRangeDrag, true);
    ui->orgPlotFreq->setInteraction(QCP::iRangeZoom, true);

    QVector<double> t; // initialize frequency axis

    double max = 0;
    for (int k = 0; k < filteredfft.size(); k++){
        t.append(k*(1.0/sampleFreq));

        //take real parts
        invRl[k] = real(invfft[k]);

        if (invRl[k]>max)
            max = invRl[k];
    }

    // create graph and assign data to it:
    ui->orgPlotFreq->addGraph();
    ui->orgPlotFreq->graph(0)->setData(t, invRl);
    // give the axes some labels:
    ui->orgPlotFreq->xAxis->setLabel("Time [s]");
    ui->orgPlotFreq->yAxis->setLabel("Magnitude");
    // set axes ranges, so we see all data:
    ui->orgPlotFreq->xAxis->setRange(0, t[t.size()-1]);
    ui->orgPlotFreq->yAxis->setRange(-max, max);
    ui->orgPlotFreq->replot();
}

void MainWindow::testfft()
{
    ui->orgBtnPlot->setText("Complete");
    ui->orgBtnPlot->setEnabled(false);
    cmpOrgSoundSignal = toComplex(orgSoundSignal);
    bins = MyFFT(cmpOrgSoundSignal); //calculate pure dft
    qDebug() << ui->orgCmbFourier->currentIndex();

    switch (ui->orgCmbFourier->currentIndex()) {
        case -1: 	plotOrgMagSpec();
                    break;
        case 0: 	plotOrgPhaseSpec();
                    break;
        case 1: 	plotOrgMagSpec();
                    break;
    }


    ui->orgBtnPlot->setText("Plot");
    ui->orgBtnPlot->setEnabled(true);
}

void MainWindow::filfft()
{
    qDebug() << "HiQ";
    qDebug() << ui->filFFTcmb->currentIndex();
    switch (ui->filFFTcmb->currentIndex()) {
        case 0:	plotFilteredMag();
                break;
        case 1:	plotFilteredPhase();
                break;
        case 2:	plotInv();
                break;
    }

}
