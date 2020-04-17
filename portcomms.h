#ifndef PORTCOMMS_H
#define PORTCOMMS_H

#include <QObject>
#include <QSerialPort>

class portComms : public QObject
{
    Q_OBJECT
public:
    explicit portComms(QObject *parent = nullptr);
    void initPort();
    void getData();
    QVector<double> outValues;
    QSerialPort *serial;

signals:
    void errorComms();
    void readComplete();

private:
    char buf[32];
    bool isStringComplete;
    QString msg;
};

#endif // PORTCOMMS_H
