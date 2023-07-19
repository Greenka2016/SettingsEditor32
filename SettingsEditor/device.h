#ifndef DEVICE_H
#define DEVICE_H
#include <QString>
#include <QListWidget>
#include "channel.h"

class Device
{
public:
    explicit Device(QString serialNumber, QString productType, QString channel, QListWidget* listOfNetworkConnections, QMap<QString, QVariant> settingsMap);

    QString getSerialNumber();
    QString getProductType();
    QListWidget& getListOfNetworkConnections();
    QMap<QString, QVariant> getSettingsMap();
    QString getChannel();


protected:
    void setDeviceData(QString serialNumber, QString productType);
    void setNetworkConnections(QListWidget* listOfNetworkConnections);
    void setNetworkConnectionsSettings(QMap<QString, QVariant> settingsMap);
    void setChannel(QString channel);


private:
    QString serialNumber;
    QString productType;
    QString console;
    QListWidget listOfNetworkConnections;
    QMap<QString, QVariant> settingsMap;
};

#endif // DEVICE_H
