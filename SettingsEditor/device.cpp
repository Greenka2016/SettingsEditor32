#include "device.h"

Device::Device(QString serialNumber, QString productType, QString channel, QListWidget* listOfNetworkConnections, QMap<QString, QVariant> settingsMap)
{
    setDeviceData(serialNumber, productType);
    setNetworkConnections(listOfNetworkConnections);
    setNetworkConnectionsSettings(settingsMap);
    setChannel(channel);
}

void Device::setDeviceData(QString serialNumber, QString productType)
{
    this->serialNumber = serialNumber;
    this->productType = productType;
}

void Device::setNetworkConnections(QListWidget* listOfNetworkConnections)
{
    for (int i = 0; i < listOfNetworkConnections->count(); ++i)
    {
        QListWidgetItem* item = listOfNetworkConnections->item(i);
        this->listOfNetworkConnections.addItem(item);
    }
}

void Device::setNetworkConnectionsSettings(QMap<QString, QVariant> settingsMap)
{
    this->settingsMap = settingsMap;
}

void Device::setChannel(QString channel)
{
    this->console = channel;
}

QString Device::getSerialNumber()
{
    return serialNumber;
}

QString Device::getProductType()
{
    return productType;
}

QListWidget& Device::getListOfNetworkConnections()
{
    return listOfNetworkConnections;
}

QMap<QString, QVariant> Device::getSettingsMap()
{
    return settingsMap;
}

QString Device::getChannel()
{
    return console;
}
