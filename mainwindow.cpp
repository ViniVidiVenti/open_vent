#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSerialPort>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->mainStack->setCurrentIndex(0);
    timeTimer = new QTimer;
    ui->statusLabel->setText("Status: Normal Operation"); //default message. Error messages will update this label
    nData=0;
    timeWindow = 60; //window for the time axis in the plots. presently at 60 seconds. update as needed
    samplingRate = 50; //sampling rate. needed for sizing plot data vectors. presently at 50 Hz. update as needed
    pressurePlotMax = 60; //upper limit of the pressure plot. presently at 60cm H2O. update as needed
    flowPlotMax = 100; //upper limit of the flow plot. presently at 60l/min. update as needed
    volumePlotMax = 600; //upper limit of the volume plot. presently at 100ml. update as needed
    init_vectors();
    init_plots();
    dataPort = new portComms;
    connect(dataPort, &portComms::readComplete, this, &MainWindow::updatePlot); //updatePlot slot called when new serial data received
    connect(dataPort, &portComms::errorComms, this, &MainWindow::errorHandle); //errorHandle slot called when error signal received
    connect(timeTimer, &QTimer::timeout, this, &MainWindow::updateDateTime);
    timeTimer->start(1000);
    dataPort->initPort(); //call for initializing serial port
}

//function for initializing all the widgets on the front screen
void MainWindow::initDisplay()
{

}


//function for initialising the pressure, flow and volume vectors for plotting.
void MainWindow::init_vectors()
{
    pressureVector.resize(timeWindow*samplingRate); //resize pressure vector to accomodate values in timeWindow
    pressureVector.fill(0); //initialize vector
    flowVector.resize(timeWindow*samplingRate); //resize flow vector to accomodate values in timeWindow
    flowVector.fill(0); //initialize vector
    volumeVector.resize(timeWindow*samplingRate); //resize volume vector to accomodate values in timeWindow
    volumeVector.fill(0); //initialize vector
    timeVector.resize(timeWindow*samplingRate); //resize time vector to accomodate values in timeWindow
    timeVector.fill(0); //initialize vector
    for(int i=0; i<timeWindow*samplingRate; i++)
    {
        double timeValue = (double)i;
        timeVector.replace(i,timeWindow- (timeValue/samplingRate)); //the time vector is inverted in order to allow the graph to scroll left to right while allowing more efficient vector manipulation.
    }
}


//function for initializing the plots, setting the axis ranges, the graphs colors and assigning values to the graphs
void MainWindow::init_plots()
{
    //set axes for pressure plot
    ui->pressurePlot->addGraph();
    ui->pressurePlot->graph(0)->setPen(QPen(QColor(Qt::red)));
    ui->pressurePlot->xAxis->setRangeLower(0);
    ui->pressurePlot->xAxis->setRangeUpper(timeWindow);
    ui->pressurePlot->xAxis->setTickLabels(false);
    ui->pressurePlot->xAxis->grid()->setVisible(false);
    ui->pressurePlot->yAxis->setRangeLower(0);
    ui->pressurePlot->yAxis->setRangeUpper(pressurePlotMax);
    ui->pressurePlot->yAxis->setTickLabels(false);
    ui->pressurePlot->yAxis->setSubTicks(false);
    ui->pressurePlot->yAxis->setLabel("Pressure");
    ui->pressurePlot->yAxis->grid()->setVisible(false);
    ui->maxPLabel->setText(QString::number(pressurePlotMax));
    ui->pressurePlot->lower();

    //set axes for flow plot
    ui->flowPlot->addGraph();
    ui->flowPlot->graph(0)->setPen(QPen(QColor(Qt::blue)));
    ui->flowPlot->xAxis->setRangeLower(0);
    ui->flowPlot->xAxis->setRangeUpper(timeWindow);
    ui->flowPlot->xAxis->setTickLabels(false);
    ui->flowPlot->xAxis->grid()->setVisible(false);
    ui->flowPlot->yAxis->setRangeLower(0);
    ui->flowPlot->yAxis->setRangeUpper(flowPlotMax);
    ui->flowPlot->yAxis->setTickLabels(false);
    ui->flowPlot->yAxis->setSubTicks(false);
    ui->flowPlot->yAxis->setLabel("Flow");
    ui->flowPlot->yAxis->grid()->setVisible(false);
    ui->maxFlabel->setText(QString::number(flowPlotMax));
    ui->flowPlot->lower();

    //set axes for volume plot
    ui->volumePlot->addGraph();
    ui->volumePlot->graph(0)->setPen(QPen(QColor(Qt::gray)));
    ui->volumePlot->xAxis->setRangeLower(0);
    ui->volumePlot->xAxis->setRangeUpper(timeWindow);
    ui->volumePlot->xAxis->setTickLabels(false);
    ui->volumePlot->xAxis->grid()->setVisible(false);
    ui->volumePlot->yAxis->setRangeLower(0);
    ui->volumePlot->yAxis->setRangeUpper(volumePlotMax);
    ui->volumePlot->yAxis->setTickLabels(false);
    ui->volumePlot->yAxis->setSubTicks(false);
    ui->volumePlot->yAxis->setLabel("Volume");
    ui->volumePlot->yAxis->grid()->setVisible(false);
    ui->maxVLabel->setText(QString::number(volumePlotMax));
    ui->volumePlot->lower();

    //set data for the plots
    ui->pressurePlot->graph(0)->setData(timeVector,pressureVector);
    ui->pressurePlot->replot();
    ui->flowPlot->graph(0)->setData(timeVector,flowVector);
    ui->flowPlot->replot();
    ui->volumePlot->graph(0)->setData(timeVector,volumeVector);
    ui->volumePlot->replot();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//function for updating the plot when new data values are received on the serial port
void MainWindow::updatePlot()
{
    nData+=1;
    pressureVector.remove(0);
    pressureVector.append(dataPort->outValues.at(0)); //push new value into the end of the vector and push out the first value from the vector
    flowVector.remove(0);
    flowVector.append(dataPort->outValues.at(1)); //push new value into the end of the vector and push out the first value from the vector
    volumeVector.remove(0);
    volumeVector.append(dataPort->outValues.at(2)); //push new value into the end of the vector and push out the first value from the vector
    updateDisplayValues();
    if(nData==5)
    {
        //update plot only at a frequency of sampling rate/5 to reduce load on the processor
        ui->pressurePlot->graph(0)->setData(timeVector,pressureVector); //reassign values to pressure graph
        ui->pressurePlot->replot(); //redraw
        ui->flowPlot->graph(0)->setData(timeVector,flowVector); //reassign values to flow graph
        ui->flowPlot->replot(); //redraw
        ui->volumePlot->graph(0)->setData(timeVector,volumeVector); //reassign values to volume graph
        ui->volumePlot->replot(); //redraw
        nData=0;
    }
}

//function for updating the values on the display when new data values are received on the serial port
void MainWindow::updateDisplayValues()
{
    ui->pressureDisplay->setText(QString::number(dataPort->outValues.at(0)));
    ui->volumeDisplay->setText(QString::number(dataPort->outValues.at(1)));
}

//function for updating the status label when portComms throws a error signal
void MainWindow::errorHandle()
{
    ui->statusLabel->setText("Status: Communication Error");
}

void MainWindow::updateDateTime()
{
    current = QDateTime::currentDateTime();
    QString dateText = "Date: ";
    dateText.append(current.date().toString("dd.MM.yyyy"));
    ui->dateLabel->setText(dateText);
    QString timeText = "Time: ";
    timeText.append(current.time().toString("hh:mm:ss"));
    ui->timeLabel->setText(timeText);
}
