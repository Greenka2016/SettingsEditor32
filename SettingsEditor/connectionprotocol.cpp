#include "connectionprotocol.h"
#include <QTcpSocket>
#include <QUdpSocket>
#include <QSerialPort>
#include <QModbusTcpClient>

bool connectionProtocol::checkTCPConnection(const QString& IP, int port)
{
    // Create socket
    QTcpSocket socket;

    // Set connection
    socket.connectToHost(IP, port);

    if (socket.waitForConnected(5000))
    {
        socket.disconnectFromHost();
        return true;
    } else
    {
        qDebug() << "Ошибка подключения:" << socket.errorString();
        return false;
    }
}

bool connectionProtocol::checkUDPConnection(const QString& IP, int port)
{
    // Create socket
    QUdpSocket socket;

    QByteArray requestData = "Connection test";
    QHostAddress hostAddress(IP);

    socket.writeDatagram(requestData, hostAddress, port);

    if (socket.waitForReadyRead(5000))
    {
        QByteArray responseData;
        responseData.resize(socket.pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;

        socket.readDatagram(responseData.data(), responseData.size(), &sender, &senderPort);

        return true;
    } else
    {
        qDebug() << "Ошибка подключения:" << socket.errorString();
        return false;
    }
}

bool connectionProtocol::checkSerialPortConnection(const QString& portName, int baudRate, QSerialPort::Parity parity,
                                           QSerialPort::DataBits dataBits, QSerialPort::StopBits stopBits)
{
    QSerialPort serialPort;
    serialPort.setPortName(portName);
    serialPort.setBaudRate(baudRate);
    serialPort.setParity(parity);
    serialPort.setDataBits(dataBits);
    serialPort.setStopBits(stopBits);

    if (serialPort.open(QIODevice::ReadWrite))
    {
        serialPort.close();
        return true;
    } else
    {
        qDebug() << "Ошибка подключения:" << serialPort.errorString();
        return false;
    }
}

bool connectionProtocol::checkMilStdConnection(const QString& mode, int deviceNumber, int channelNumber)
{

}

bool connectionProtocol::checkModbusConnection(const QString& IP, int port)
{
    QModbusTcpClient modbusClient;
    modbusClient.setConnectionParameter(QModbusDevice::NetworkPortParameter, port); // Set port
    modbusClient.setConnectionParameter(QModbusDevice::NetworkPortParameter, IP); // Set IP

    if (modbusClient.connectDevice())
    {
        modbusClient.disconnectDevice();
        return true;
    } else
    {
        qDebug() << "Ошибка подключения:" << modbusClient.errorString();
        return false;
    }
}
