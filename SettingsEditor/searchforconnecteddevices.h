#ifndef SEARCHFORCONNECTEDDEVICES_H
#define SEARCHFORCONNECTEDDEVICES_H
#include <QString>
#include <QVariant>
#include <connectionprotocol.h>


class SearchForConnectedDevices
{
    Q_OBJECT

    struct deviceData
    {
        QString type;
        QList<QVariant> settings;
    };

    struct suitableSettings
    {
        QList<QVariant> suitableTCPSettings;
        QList<QVariant> suitableUDPSettings;
        QList<QVariant> suitableModbusSettings;
        QList<QVariant> suitableSerialPortSettings;
        QList<QVariant> suitableMilStdSettings;
    };

    struct TCPSettings
    {
        QString connectionMethod = "TCP";
        QList<QString> TCPIP = {"192.168.0.1", "173.194.219.100", "8.8.8.8"};
        QList<int> TCPPorts = {0, 20, 21, 22, 23, 25, 110, 143, 80, 443, 194};
    };

    struct UDPSettings
    {
        QString connectionMethod = "UDP";
        QList<QString> UDPIP = {"192.168.0.1", "224.0.0.1", "127.0.0.1"};
        QList<int> UDPPorts = {67, 68, 88, 161, 162};
    };

    struct ModbusSettings
    {
        QString connectionMethod = "Modbus";
        QList<QString> ModbusIP = {"192.168.0.100", "54.13.0.1"};
        QList<int> ModbusPorts = {502};
    };

    struct SerialPortSettings
    {
        QString connectionMethod = "SerialPort";
        QList<QString> SerialPortCOMPorts;

        QList<int> SerialPortBaudRate = {1200, 2400, 4800, 9600, 14400, 19200, 38400, 56000, 57600,
                                         115200, 128000, 230400, 256000, 460800, 921600, 1250000};

        QList<QSerialPort::Parity> SerialPortParity = {QSerialPort::NoParity, QSerialPort::EvenParity, QSerialPort::OddParity,
                                                       QSerialPort::SpaceParity, QSerialPort::MarkParity};

        QList<QSerialPort::DataBits> serialPortDataBits = {QSerialPort::Data5, QSerialPort::Data6, QSerialPort::Data7, QSerialPort::Data8};

        QList<QSerialPort::StopBits> serialPortStopBits = {QSerialPort::OneStop, QSerialPort::OneAndHalfStop, QSerialPort::TwoStop};
    };

    struct MilStdSettings
    {
        QString connectionMethod = "MilStd";
        QList<int> MilStdDeviceIDs;

        QList<int> MilStdChannelIDs;

        QList<QString> MilStdModes = {"Command Mode", "Data Mode", "BC-RT"};
    };


public:
    void startSeachingForConnectedDevices();

signals:
    bool deviceDetected(const deviceData& data);

private:
    void searchForPowerSource(QList<QVariant> PowerSourceSettingsList);
    void searchForProduct(QList<QVariant> ProductSourceSettingsList);
    void searchForThermalCamera(QList<QVariant> ThermalCameraSettingsList);
    void searchForRotaryTable(QList<QVariant> RotaryTableSettingsList);
    QList<QVariant> iteratingThroughTheSettings(QList<QVariant> settings);

    bool isDataValid(const QByteArray& data);

    void sendData(const QByteArray& data);

    deviceData interpretData(const QByteArray& data);
};
#endif // SEARCHFORCONNECTEDDEVICES_H
