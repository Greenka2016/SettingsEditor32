#include "searchforconnecteddevices.h"
#include "connectionprotocol.h"
#include "powersource.h"
#include "thermalcamera.h"
#include "rotarytable.h"
#include "product.h"
#include "QSerialPortInfo"
#include "QSerialPort"
#include "tuple"


void SearchForConnectedDevices::startSeachingForConnectedDevices()
{
    TCPSettings TCPSettings;
    UDPSettings UDPSettings;
    ModbusSettings ModbusSettings;
    SerialPortSettings SerialPortSettings;
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        SerialPortSettings.SerialPortCOMPorts.append(info.portName());
    }
    MilStdSettings MilStdSettings;
    for (int i = 1; i <= 30; ++i)
    {
        MilStdSettings.MilStdDeviceIDs.append(i);
    }

    for (int i = 0; i <= 31; ++i)
    {
        MilStdSettings.MilStdChannelIDs.append(i);
    }

    QList<QVariant> settings;

    settings.append(QVariant::fromValue(TCPSettings));
    settings.append(QVariant::fromValue(UDPSettings));
    settings.append(QVariant::fromValue(ModbusSettings));
    settings.append(QVariant::fromValue(SerialPortSettings));
    settings.append(QVariant::fromValue(MilStdSettings));


    searchForPowerSource(settings);

    searchForProduct(settings);

    searchForThermalCamera(settings);

    searchForRotaryTable(settings);
}

void SearchForConnectedDevices::searchForPowerSource(QList<QVariant> settingsList)
{
    connectionProtocol connectionProtocol;

    QList<QVariant> suitableSettingsList = iteratingThroughTheSettings(settingsList);
    if (suitableSettingsList.at(0).canConvert<suitableSettings>())
    {
        suitableSettings suitableSettingsStruct = suitableSettingsList.at(0).value<suitableSettings>();
        foreach (QVariant TCPPairTuple, suitableSettingsStruct.suitableTCPSettings)
        {
            std::tuple<QString, int> TCPPair = TCPPairTuple.value<std::tuple<QString, int>>();
            auto TCPIP = std::get<0>(TCPPair);
            auto TCPPort = std::get<1>(TCPPair);
        }
    }
}

void SearchForConnectedDevices::searchForProduct(QList<QVariant> settingsList)
{
    connectionProtocol connectionProtocol;

    QList<QVariant> suitableSettings = iteratingThroughTheSettings(settingsList);

}

void SearchForConnectedDevices::searchForThermalCamera(QList<QVariant> settingsList)
{
    connectionProtocol connectionProtocol;

    QList<QVariant> suitableSettings = iteratingThroughTheSettings(settingsList);
}

void SearchForConnectedDevices::searchForRotaryTable(QList<QVariant> settingsList)
{
    connectionProtocol connectionProtocol;

    QList<QVariant> suitableSettings = iteratingThroughTheSettings(settingsList);
}

QList<QVariant> SearchForConnectedDevices::iteratingThroughTheSettings(QList<QVariant> settings)
{
    connectionProtocol connectionProtocol;
    suitableSettings suitableSettingsStruct;
    QList<QVariant> suitableSettings;

    QList<QVariant> suitableTCPSettings;
    QVariant TCP = settings.at(0);
    if (TCP.canConvert<TCPSettings>())
    {
        TCPSettings TCPstruct = TCP.value<TCPSettings>();
        foreach (QString TCPIP, TCPstruct.TCPIP)
        {
            foreach (int TCPPort, TCPstruct.TCPPorts)
            {
                if (connectionProtocol.checkTCPConnection(TCPIP, TCPPort) == true)
                {
                    std::tuple<QString, int> TCPPair(TCPIP, TCPPort);
                    suitableTCPSettings.append(QVariant::fromValue(TCPPair));
                }
            }
        }
    }


    QList<QVariant> suitableUDPSettings;
    QVariant UDP = settings.at(1);
    if (UDP.canConvert<UDPSettings>())
    {
        UDPSettings UDPstruct = UDP.value<UDPSettings>();
        foreach (QString UDPIP, UDPstruct.UDPIP)
        {
            foreach (int UDPPort, UDPstruct.UDPPorts)
            {
                if (connectionProtocol.checkUDPConnection(UDPIP, UDPPort) == true)
                {
                    std::tuple<QString, int> UDPPair(UDPIP, UDPPort);
                    suitableUDPSettings.append(QVariant::fromValue(UDPPair));
                }
            }
        }
    }

    QList<QVariant> suitableModbusSettings;
    QVariant Modbus = settings.at(2);
    if (Modbus.canConvert<ModbusSettings>())
    {
        ModbusSettings Modbusstruct = Modbus.value<ModbusSettings>();
        foreach (QString ModbusIP, Modbusstruct.ModbusIP)
        {
            foreach (int ModbusPort, Modbusstruct.ModbusPorts)
            {
                if (connectionProtocol.checkModbusConnection(ModbusIP, ModbusPort) == true)
                {
                    std::tuple<QString, int> ModbusPair(ModbusIP, ModbusPort);
                    suitableModbusSettings.append(QVariant::fromValue(ModbusPair));
                }
            }
        }
    }

    QList<QVariant> suitableSerialPortSettings;
    QVariant SerialPort = settings.at(3);
    if (SerialPort.canConvert<SerialPortSettings>())
    {
        SerialPortSettings SerialPortstruct = SerialPort.value<SerialPortSettings>();
        foreach (QString SerialPortCOMPort, SerialPortstruct.SerialPortCOMPorts)
        {
            foreach (int SerialPortBaudRate, SerialPortstruct.SerialPortBaudRate)
            {
                foreach (QSerialPort::Parity SerialPortParity, SerialPortstruct.SerialPortParity)
                {
                    foreach (QSerialPort::DataBits serialPortDataBit, SerialPortstruct.serialPortDataBits)
                    {
                        foreach (QSerialPort::StopBits serialPortStopBit, SerialPortstruct.serialPortStopBits)
                        {
                            if (connectionProtocol.checkSerialPortConnection(SerialPortCOMPort, SerialPortBaudRate, SerialPortParity,
                                                                             serialPortDataBit, serialPortStopBit) == true)
                            {
                                std::tuple<QString, int, QSerialPort::Parity, QSerialPort::DataBits, QSerialPort::StopBits> SerialPortFive
                                        (SerialPortCOMPort, SerialPortBaudRate, SerialPortParity, serialPortDataBit, serialPortStopBit);
                                suitableSerialPortSettings.append(QVariant::fromValue(SerialPortFive));
                            }
                        }
                    }
                }
            }
        }
    }

    QList<QVariant> suitableMilStdSettings;
    QVariant MilStd = settings.at(4);
    if (MilStd.canConvert<MilStdSettings>())
    {
        MilStdSettings MilStdstruct = MilStd.value<MilStdSettings>();
        foreach (int MilStdDeviceID, MilStdstruct.MilStdDeviceIDs)
        {
            foreach (int MilStdChannelID, MilStdstruct.MilStdChannelIDs)
            {
                foreach (QString MilStdMode, MilStdstruct.MilStdModes)
                {
                    if (connectionProtocol.checkMilStdConnection(MilStdMode, MilStdDeviceID, MilStdChannelID) == true)
                    {
                        std::tuple<int, int, QString> MilStdTrio(MilStdDeviceID, MilStdChannelID, MilStdMode);
                        suitableMilStdSettings.append(QVariant::fromValue(MilStdTrio));
                    }
                }
            }
        }
    }

    suitableSettingsStruct.suitableTCPSettings = suitableTCPSettings;
    suitableSettingsStruct.suitableUDPSettings = suitableUDPSettings;
    suitableSettingsStruct.suitableModbusSettings = suitableModbusSettings;
    suitableSettingsStruct.suitableSerialPortSettings = suitableSerialPortSettings;
    suitableSettingsStruct.suitableMilStdSettings = suitableMilStdSettings;

    suitableSettings.append(QVariant::fromValue(suitableSettingsStruct));
    return suitableSettings;
}
