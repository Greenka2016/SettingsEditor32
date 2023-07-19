#ifndef CONNECTIONPROTOCOL_H
#define CONNECTIONPROTOCOL_H
#include <QString>
#include <QSerialPort>

class connectionProtocol
{

public:
    bool checkTCPConnection(const QString& IP, int port);
    bool checkUDPConnection(const QString& IP, int port);
    bool checkSerialPortConnection(const QString& portName, int baudRate, QSerialPort::Parity parity,
                                   QSerialPort::DataBits dataBits, QSerialPort::StopBits stopBits);
    bool checkMilStdConnection(const QString& mode, int deviceNumber, int channelNumber);
    bool checkModbusConnection(const QString& IP, int port);

};
#endif // CONNECTIONPROTOCOL_H
