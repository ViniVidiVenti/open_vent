#include "portcomms.h"

portComms::portComms(QObject *parent) : QObject(parent)
{
    outValues.resize(3);  //set up Serial data output vector
    serial = new QSerialPort;
    isStringComplete = true; //for handling partial string reads
    connect(serial, &QSerialPort::readyRead,this, &portComms::getData); //update output vector, emit signal and update plot
}

//function for initialising the port parameters and opening the port for data handling
void portComms::initPort()
{
    serial->setPortName("COM12");  //update this to reflect actual hardware port
    serial->setBaudRate(QSerialPort::Baud115200);  //baud rate = 115200. update as needed
    serial->setParity(QSerialPort::NoParity); //parity = none. update as needed
    serial->setFlowControl(QSerialPort::NoFlowControl); //flow control = none. update as needed
    serial->setDataBits(QSerialPort::Data8); //data bits = 8. update as needed
    serial->setStopBits(QSerialPort::OneStop); //stop bits = 1. update as needed
    if(serial->open(QIODevice::ReadOnly)) //open serial port
    {
        serial->clear(QSerialPort::AllDirections); //flush port
    }
    else
    {
        emit errorComms();  //if port cannot be opened.
    }
}

void portComms::getData()
{
  if (serial->bytesAvailable()>0) {
          // read request
          quint64 dataLength = serial->readLine(buf, sizeof(buf));  //read serial input data line by line
          if(dataLength>0)
          {
              if(isStringComplete)
              {
                msg = QLatin1String(buf); //convert to QString. if previous data was complete, re-assign fresh data
              }
              else
              {
                  msg += QLatin1String(buf); //convert to QString. if previous data was incomplete, append fresh data
              }
              if(msg.contains("P") && msg.contains("\n")) //check if complete string was received
              {
                isStringComplete = true;
                QStringList valuesList = msg.split("\t"); //split by tab delimiters
                //update output values vectors
                outValues.replace(0, valuesList.at(0).mid(1+valuesList.at(0).indexOf("P")).toDouble());
                outValues.replace(1, valuesList.at(1).mid(1+valuesList.at(1).indexOf("F")).toDouble());
                outValues.replace(2, valuesList.at(2).mid(1+valuesList.at(2).indexOf("V")).toDouble());
                emit readComplete(); //emit signal for MainWindow to process new received data
              }
              else
              {
                  isStringComplete = false;
              }
          }
          else
          {
              emit errorComms();
          }
       } else {
              emit errorComms();
      }
}
