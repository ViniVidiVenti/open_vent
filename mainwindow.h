#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QVector>
#include <QColor>
#include <QPen>
#include <QDateTime>
#include <QList>
#include <QSignalMapper>
#include <QTimer>
#include "portcomms.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


public slots:
    void init_plots();
    void init_vectors();
    void updatePlot();
    void errorHandle();
    void updateDisplayValues();
    void updateDateTime();
   // QString receiveMsg();

//signals:
//    void newPlotUpdate();

private slots:
    void initDisplay();
    
private:
    Ui::MainWindow *ui;
    QVector<double> pressureVector;
    QVector<double> flowVector;
    QVector<double> volumeVector;
    QVector<double> timeVector;
    QTimer *timeTimer;
    QDateTime current;
    portComms *dataPort;
    int nData;
    int timeWindow;
    int samplingRate;
    int pressurePlotMax;
    int flowPlotMax;
    int volumePlotMax;
};

#endif // MAINWINDOW_H
