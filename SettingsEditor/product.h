#ifndef PRODUCT_H
#define PRODUCT_H
#include "device.h"
#include "channel.h"

class Product : public Device
{
public:
    using Device::Device;

private:
    QString console;
    QString telemetry;
    QString combat;
};

#endif // PRODUCT_H
