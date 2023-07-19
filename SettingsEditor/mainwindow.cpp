#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "tree.h"
#include <connectionprotocol.h>
#include "product.h"
#include <QCoreApplication>
#include <QDebug>
#include <QMessageBox>
#include <QSerialPortInfo>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->TCPPortLineEdit->setValidator(new QIntValidator);
    ui->UDPPortLineEdit->setValidator(new QIntValidator);
    ui->ModBusPortLineEdit->setValidator(new QIntValidator);
    ui->milSTDChannelNumberLineEdit->setValidator(new QIntValidator);
    ui->milSTDDeviceNumberLineEdit->setValidator(new QIntValidator);

    connect(ui->serialNumberLineEdit, SIGNAL(textChanged(QString)), this, SLOT(MainWindow::on_serialNumberLineEdit_textChanged()));

    ui->boxProductType->addItem("Изделие");
    ui->boxProductType->addItem("Поворотный стол");
    ui->boxProductType->addItem("Термокамера");
    ui->boxProductType->addItem("Источник питания");

    connect(ui->boxProductType, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_boxProductType_currentIndexChanged);

    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts())
    {
        ui->boxCOMPort->addItem(info.portName());
    }

    ui->boxBaudRate->addItem("1200");
    ui->boxBaudRate->addItem("2400");
    ui->boxBaudRate->addItem("4800");
    ui->boxBaudRate->addItem("9600");
    ui->boxBaudRate->addItem("14400");
    ui->boxBaudRate->addItem("19200");
    ui->boxBaudRate->addItem("38400");
    ui->boxBaudRate->addItem("56000");
    ui->boxBaudRate->addItem("57600");
    ui->boxBaudRate->addItem("115200");
    ui->boxBaudRate->addItem("128000");
    ui->boxBaudRate->addItem("230400");
    ui->boxBaudRate->addItem("256000");
    ui->boxBaudRate->addItem("460800");
    ui->boxBaudRate->addItem("921600");
    ui->boxBaudRate->addItem("1250000");

    ui->boxParity->addItem("Нет");
    ui->boxParity->addItem("Чётный");
    ui->boxParity->addItem("Нечётный");
    ui->boxParity->addItem("Пробел");
    ui->boxParity->addItem("Маркер");

    ui->boxDataBits->addItem("5");
    ui->boxDataBits->addItem("6");
    ui->boxDataBits->addItem("7");
    ui->boxDataBits->addItem("8");

    ui->boxStopBits->addItem("1");
    ui->boxStopBits->addItem("1,5");
    ui->boxStopBits->addItem("2");

    Tree tree(ui->settingsTree);
    tree.setSettingsTree(ui->settingsTree);

    tree.loadTreeFromRegistry(tree.getSettingsTree());

    int topLevelItemCount = tree.getSettingsTree()->topLevelItemCount();
    for (int i = 0; i < topLevelItemCount; ++i)
    {
        QString item = tree.getSettingsTree()->topLevelItem(i)->data(0, 0).toString();
        if (ui->serialNumberComboBox->findText(item) == -1)
        {
            ui->serialNumberComboBox->addItem(item);
        }

        loadSettingsFromRegistry(item);
    }

    connect(ui->serialNumberComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_serialNumberComboBox_currentIndexChanged);

    connect(ui->boxChannelOfDevice, QOverload<int>::of(&QComboBox::activated), this, &MainWindow::on_boxChannelOfDevice_activated);

    connect(ui->TCPIPComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_TCPIPComboBox_currentIndexChanged);

    connect(ui->TCPPortComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_TCPPortComboBox_currentIndexChanged);

    connect(ui->UDPIPComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_UDPIPComboBox_currentIndexChanged);

    connect(ui->UDPPortComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_UDPPortComboBox_currentIndexChanged);

    connect(ui->ModBusIPComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_ModBusIPComboBox_currentIndexChanged);

    connect(ui->ModBusPortComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_ModBusPortComboBox_currentIndexChanged);

    connect(ui->milSTDChannelNumberComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_milSTDChannelNumberComboBox_currentIndexChanged);

    connect(ui->milSTDDeviceNumberComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_milSTDDeviceNumberComboBox_currentIndexChanged);

    connect(ui->milSTDModeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_milSTDModeComboBox_currentIndexChanged);

    QPushButton *saveSettingsButton = ui->saveSettingsButton;

    connect(saveSettingsButton, &QPushButton::clicked, [tree, this]()
    {
        on_saveSettingsButton_clicked(tree);
    });
    QPushButton *connectionTestButton = ui->connectionTestButton;

    connect( connectionTestButton, SIGNAL(clicked()), this, SLOT( on_connectionTestButton_clicked()));
}


void MainWindow::on_serialNumberComboBox_currentIndexChanged(int index)
{
    ui->serialNumberLineEdit->clear();
    QString selectedSerialNumber = ui->serialNumberComboBox->itemText(index);

    ui->serialNumberLineEdit->setText(selectedSerialNumber);

    loadSettingsFromRegistry(selectedSerialNumber);
}

void MainWindow::on_boxProductType_currentIndexChanged(int index)
{
    ui->boxChannelOfDevice->clear();
    QString selectedText = ui->boxProductType->itemText(index);

    if (selectedText == "Поворотный стол" || selectedText == "Термокамера" || selectedText == "Источник питания")
    {
        ui->boxChannelOfDevice->addItem("1 канал");
    } else if (selectedText == "Изделие")
    {
        ui->boxChannelOfDevice->addItem("Консоль");
        ui->boxChannelOfDevice->addItem("Телеметрия");
        ui->boxChannelOfDevice->addItem("Боевой");
    }

    QString selectedSerialNumber = ui->serialNumberLineEdit->text();

    int topLevelItemCount = ui->settingsTree->topLevelItemCount();
    for (int i = 0; i < topLevelItemCount; ++i)
    {
        QTreeWidgetItem* item = ui->settingsTree->topLevelItem(i);
        if (item->text(0) == selectedSerialNumber)
        {
            loadSettingsFromRegistry(selectedSerialNumber);
        }
    }
}

void MainWindow::on_boxChannelOfDevice_activated(int index)
{
    QString selectedSerialNumber = ui->serialNumberLineEdit->text();

    int topLevelItemCount = ui->settingsTree->topLevelItemCount();
    for (int i = 0; i < topLevelItemCount; ++i)
    {
        QTreeWidgetItem* item = ui->settingsTree->topLevelItem(i);
        if (item->text(0) == selectedSerialNumber)
        {
            loadSettingsFromRegistry(selectedSerialNumber);
        }
    }
}

void MainWindow::loadSettingsFromRegistry(QString selectedSerialNumber)
{
    QTreeWidget* settingsTree = ui->settingsTree;
    QTreeWidgetItem* serialNumberItem = findItemByValue(settingsTree, selectedSerialNumber);
    QTreeWidgetItem*  productTypeItem = nullptr;
    QString currentProductType = ui->boxProductType->currentText();
    for (int i = 0; i < serialNumberItem->childCount(); ++i)
    {
        if (serialNumberItem->child(i)->text(0) == currentProductType)
        {
            productTypeItem = serialNumberItem->child(i);
            break;
        }
        else resetSettings();
    }

    if (productTypeItem != nullptr)
    {
        QTreeWidgetItem*  channelItem = nullptr;
        QString currentChannel = ui->boxChannelOfDevice->currentText();
        for (int i = 0; i < productTypeItem->childCount(); ++i)
        {
            if (productTypeItem->child(i)->text(0) == currentChannel)
            {
                channelItem = productTypeItem->child(i);

                QList<QTreeWidgetItem*> settings = getChildItems(channelItem);
                insertSettings(settings);
                break;
            }
            else resetSettings();
        }
    }
}

void MainWindow::insertSettings(QList<QTreeWidgetItem*> settings)
{
    for (int i = 0; i < settings.count(); ++i)
    {
        QString settingName = "TCPIP";
        if (settings.at(i)->text(0) == settingName)
        {
            if (ui->TCPIPComboBox->findText(settings.at(i)->child(0)->text(0)) == -1)
            {
                ui->TCPIPComboBox->addItem(settings.at(i)->child(0)->text(0));
            }
            ui->TCPIPComboBox->setCurrentText(settings.at(i)->child(0)->text(0));
            ui->TCPIPLineEdit->setText(settings.at(i)->child(0)->text(0));
            break;
        }
    }


    for (int i = 0; i < settings.count(); ++i)
    {
        QString settingName = "TCPPort";
        if (settings.at(i)->text(0) == settingName)
        {
            if (ui->TCPPortComboBox->findText(settings.at(i)->child(0)->text(0)) == -1)
            {
                ui->TCPPortComboBox->addItem(settings.at(i)->child(0)->text(0));
            }
            ui->TCPPortComboBox->setCurrentText(settings.at(i)->child(0)->text(0));
            ui->TCPPortLineEdit->setText(settings.at(i)->child(0)->text(0));
            break;
        }
    }


    for (int i = 0; i < settings.count(); ++i)
    {
        QString settingName = "UDPIP";
        if (settings.at(i)->text(0) == settingName)
        {
            if (ui->UDPIPComboBox->findText(settings.at(i)->child(0)->text(0)) == -1)
            {
                ui->UDPIPComboBox->addItem(settings.at(i)->child(0)->text(0));
            }
            ui->UDPIPComboBox->setCurrentText(settings.at(i)->child(0)->text(0));
            ui->UDPIPLineEdit->setText(settings.at(i)->child(0)->text(0));
            break;
        }
    }


    for (int i = 0; i < settings.count(); ++i)
    {
        QString settingName = "UDPPort";
        if (settings.at(i)->text(0) == settingName)
        {
            if (ui->UDPPortComboBox->findText(settings.at(i)->child(0)->text(0)) == -1)
            {
                ui->UDPPortComboBox->addItem(settings.at(i)->child(0)->text(0));
            }
            ui->UDPPortComboBox->setCurrentText(settings.at(i)->child(0)->text(0));
            ui->UDPPortLineEdit->setText(settings.at(i)->child(0)->text(0));
            break;
        }
    }


    for (int i = 0; i < settings.count(); ++i)
    {
        QString settingName = "ModBusIP";
        if (settings.at(i)->text(0) == settingName)
        {
            if (ui->ModBusIPComboBox->findText(settings.at(i)->child(0)->text(0)) == -1)
            {
                ui->ModBusIPComboBox->addItem(settings.at(i)->child(0)->text(0));
            }
            ui->ModBusIPComboBox->setCurrentText(settings.at(i)->child(0)->text(0));
            ui->ModBusIPLineEdit->setText(settings.at(i)->child(0)->text(0));
            break;
        }
    }


    for (int i = 0; i < settings.count(); ++i)
    {
        QString settingName = "ModBusPort";
        if (settings.at(i)->text(0) == settingName)
        {
            if (ui->ModBusPortComboBox->findText(settings.at(i)->child(0)->text(0)) == -1)
            {
                ui->ModBusPortComboBox->addItem(settings.at(i)->child(0)->text(0));
            }
            ui->ModBusPortComboBox->setCurrentText(settings.at(i)->child(0)->text(0));
            ui->ModBusPortLineEdit->setText(settings.at(i)->child(0)->text(0));
            break;
        }
    }


    for (int i = 0; i < settings.count(); ++i)
    {
        QString settingName = "MilStdMode";
        if (settings.at(i)->text(0) == settingName)
        {
            if (ui->milSTDModeComboBox->findText(settings.at(i)->child(0)->text(0)) == -1)
            {
                ui->milSTDModeComboBox->addItem(settings.at(i)->child(0)->text(0));
            }
            ui->milSTDModeComboBox->setCurrentText(settings.at(i)->child(0)->text(0));
            ui->milSTDModeLineEdit->setText(settings.at(i)->child(0)->text(0));
            break;
        }
    }

    for (int i = 0; i < settings.count(); ++i)
    {
        QString settingName = "MilStdDeviceNumber";
        if (settings.at(i)->text(0) == settingName)
        {
            if (ui->milSTDDeviceNumberComboBox->findText(settings.at(i)->child(0)->text(0)) == -1)
            {
                ui->milSTDDeviceNumberComboBox->addItem(settings.at(i)->child(0)->text(0));
            }
            ui->milSTDDeviceNumberComboBox->setCurrentText(settings.at(i)->child(0)->text(0));
            ui->milSTDDeviceNumberLineEdit->setText(settings.at(i)->child(0)->text(0));
            break;
        }
    }


    for (int i = 0; i < settings.count(); ++i)
    {
        QString settingName = "MilStdChannelNumber";
        if (settings.at(i)->text(0) == settingName)
        {
            if (ui->milSTDChannelNumberComboBox->findText(settings.at(i)->child(0)->text(0)) == -1)
            {
                ui->milSTDChannelNumberComboBox->addItem(settings.at(i)->child(0)->text(0));
            }
            ui->milSTDChannelNumberComboBox->setCurrentText(settings.at(i)->child(0)->text(0));
            ui->milSTDChannelNumberLineEdit->setText(settings.at(i)->child(0)->text(0));
            break;
        }
    }


    for (int i = 0; i < settings.count(); ++i)
    {
        QString settingName = "SerialPortCOMPort";
        if (settings.at(i)->text(0) == settingName)
        {
            if (settings.at(i)->child(0)->text(0) == "COM1")
            {
                ui->boxCOMPort->setCurrentText("COM1");
                break;
            } else if (settings.at(i)->child(0)->text(0) == "COM6")
            {
                ui->boxCOMPort->setCurrentText("COM6");
                break;
            } else if (settings.at(i)->child(0)->text(0) == "COM5")
            {
                ui->boxCOMPort->setCurrentText("COM5");
                break;
            } else if (settings.at(i)->child(0)->text(0) == "COM4")
            {
                ui->boxCOMPort->setCurrentText("COM4");
                break;
            } else if (settings.at(i)->child(0)->text(0) == "COM2")
            {
                ui->boxCOMPort->setCurrentText("COM2");
                break;
            } else if (settings.at(i)->child(0)->text(0) == "COM18")
            {
                ui->boxCOMPort->setCurrentText("COM18");
                break;
            } else if (settings.at(i)->child(0)->text(0) == "COM8")
            {
                ui->boxCOMPort->setCurrentText("COM8");
                break;
            } else if (settings.at(i)->child(0)->text(0) == "COM10")
            {
                ui->boxCOMPort->setCurrentText("COM10");
                break;
            } else if (settings.at(i)->child(0)->text(0) == "COM12")
            {
                ui->boxCOMPort->setCurrentText("COM12");
                break;
            } else if (settings.at(i)->child(0)->text(0) == "COM19")
            {
                ui->boxCOMPort->setCurrentText("COM19");
                break;
            } else if (settings.at(i)->child(0)->text(0) == "COM11")
            {
                ui->boxCOMPort->setCurrentText("COM11");
                break;
            } else if (settings.at(i)->child(0)->text(0) == "COM9")
            {
                ui->boxCOMPort->setCurrentText("COM9");
                break;
            } else if (settings.at(i)->child(0)->text(0) == "COM13")
            {
                ui->boxCOMPort->setCurrentText("COM13");
                break;
            } else {
                ui->boxCOMPort->setCurrentText("COM1");
                break;
            }
        }
    }


    for (int i = 0; i < settings.count(); ++i)
    {
        QString settingName = "SerialPortBaudRate";
        if (settings.at(i)->text(0) == settingName)
        {
            if (settings.at(i)->child(0)->text(0) == "1200")
            {
                ui->boxBaudRate->setCurrentText("1200");
                break;
            } else if (settings.at(i)->child(0)->text(0) == "2400")
            {
                ui->boxBaudRate->setCurrentText("2400");
                break;
            } else if (settings.at(i)->child(0)->text(0) == "4800")
            {
                ui->boxBaudRate->setCurrentText("4800");
                break;
            } else if (settings.at(i)->child(0)->text(0) == "9600")
            {
                ui->boxBaudRate->setCurrentText("9600");
                break;
            } else if (settings.at(i)->child(0)->text(0) == "14400")
            {
                ui->boxBaudRate->setCurrentText("14400");
                break;
            } else if (settings.at(i)->child(0)->text(0) == "19200")
            {
                ui->boxBaudRate->setCurrentText("19200");
                break;
            } else if (settings.at(i)->child(0)->text(0) == "38400")
            {
                ui->boxBaudRate->setCurrentText("38400");
                break;
            } else if (settings.at(i)->child(0)->text(0) == "56000")
            {
                ui->boxBaudRate->setCurrentText("56000");
                break;
            } else if (settings.at(i)->child(0)->text(0) == "57600")
            {
                ui->boxBaudRate->setCurrentText("57600");
                break;
            } else if (settings.at(i)->child(0)->text(0) == "115200")
            {
                ui->boxBaudRate->setCurrentText("115200");
                break;
            } else if (settings.at(i)->child(0)->text(0) == "230400")
            {
                ui->boxBaudRate->setCurrentText("230400");
                break;
            } else if (settings.at(i)->child(0)->text(0) == "256000")
            {
                ui->boxBaudRate->setCurrentText("256000");
                break;
            } else if (settings.at(i)->child(0)->text(0) == "460800")
            {
                ui->boxBaudRate->setCurrentText("460800");
                break;
            } else if (settings.at(i)->child(0)->text(0) == "921600")
            {
                ui->boxBaudRate->setCurrentText("921600");
                break;
            } else if (settings.at(i)->child(0)->text(0) == "1250000")
            {
                ui->boxBaudRate->setCurrentText("1250000");
                break;
            } else {
                ui->boxBaudRate->setCurrentText("1200");
                break;
            }
        }
    }

    for (int i = 0; i < settings.count(); ++i)
    {
        QString settingName = "SerialPortParity";
        if (settings.at(i)->text(0) == settingName)
        {
            if (settings.at(i)->child(0)->text(0) == '0')
            {
                ui->boxParity->setCurrentText("Нет");
                break;
            } else if (settings.at(i)->child(0)->text(0) == '1')
            {
                ui->boxParity->setCurrentText("Чётный");
                break;
            } else if (settings.at(i)->child(0)->text(0) == '2')
            {
                ui->boxParity->setCurrentText("Нечётный");
                break;
            } else if (settings.at(i)->child(0)->text(0) == '3')
            {
                ui->boxParity->setCurrentText("Пробел");
                break;
            } else if (settings.at(i)->child(0)->text(0) == '4')
            {
                ui->boxParity->setCurrentText("Маркер");
                break;
            } else {
                ui->boxParity->setCurrentText("Нет");
                break;
            }
        }
    }


    for (int i = 0; i < settings.count(); ++i)
    {
        QString settingName = "SerialPortDataBits";
        if (settings.at(i)->text(0) == settingName)
        {
            if (settings.at(i)->child(0)->text(0) == '5')
            {
                ui->boxDataBits->setCurrentText("5");
                break;
            } else if (settings.at(i)->child(0)->text(0) == '6')
            {
                ui->boxDataBits->setCurrentText("6");
                break;
            } else if (settings.at(i)->child(0)->text(0) == '7')
            {
                ui->boxDataBits->setCurrentText("7");
                break;
            } else if (settings.at(i)->child(0)->text(0) == '8'){
                ui->boxDataBits->setCurrentText("8");
                break;
            } else {
                ui->boxDataBits->setCurrentText("5");
                break;
            }
        }
    }


    for (int i = 0; i < settings.count(); ++i)
    {
        QString settingName = "SerialPortStopBits";
        if (settings.at(i)->text(0) == settingName)
        {
            if (settings.at(i)->child(0)->text(0) == '1')
            {
                ui->boxStopBits->setCurrentText("1");
                break;
            } else if (settings.at(i)->child(0)->text(0) == '3'){
                ui->boxStopBits->setCurrentText("1,5");
                break;
            } else if (settings.at(i)->child(0)->text(0) == '2'){
                ui->boxStopBits->setCurrentText("2");
                break;
            } else {
                ui->boxStopBits->setCurrentText("1");
                break;
            }
        }
    }
}

void MainWindow::resetSettings()
{
    ui->TCPIPLineEdit->clear();
    ui->TCPPortLineEdit->clear();
    ui->UDPIPLineEdit->clear();
    ui->UDPPortLineEdit->clear();
    ui->ModBusIPLineEdit->clear();
    ui->ModBusPortComboBox->clear();
    ui->boxCOMPort->setCurrentIndex(0);
    ui->boxBaudRate->setCurrentIndex(0);
    ui->boxParity->setCurrentIndex(0);
    ui->boxDataBits->setCurrentIndex(0);
    ui->boxStopBits->setCurrentIndex(0);
    ui->milSTDModeLineEdit->clear();
    ui->milSTDDeviceNumberLineEdit->clear();
    ui->milSTDChannelNumberLineEdit->clear();
}

QList<QTreeWidgetItem*> MainWindow::getChildItems(QTreeWidgetItem* parentItem)
{
    QList<QTreeWidgetItem*> childItems;

    if (parentItem)
    {
        int childCount = parentItem->childCount();
        for (int i = 0; i < childCount; ++i)
        {
            QTreeWidgetItem* childItem = parentItem->child((i));
            childItems.append(childItem);
        }
    }

    return childItems;
}

QTreeWidgetItem* MainWindow::findItemByValue(QTreeWidget* tree, QString value)
{
    QList<QTreeWidgetItem*> items = tree->findItems(value, Qt::MatchExactly, 0);

    if (!items.isEmpty())
    {
        return items.first();
    }

    return nullptr;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_connectionTestButton_clicked()
{
    connectionProtocol connectionProtocol;

    // TCP settings
    QString TCPIP = ui->TCPIPLineEdit->text();
    int TCPPort = ui->TCPPortLineEdit->text().toInt();

    // UDP settings
    QString UDPIP = ui->UDPIPLineEdit->text();
    int UDPPort = ui->UDPPortLineEdit->text().toInt();

    // Serial Port settings
    QString serialPortCOMPort = ui->boxCOMPort->currentText();
    int serialPortBaudRate = ui->boxBaudRate->currentText().toInt();

    QSerialPort::Parity SerialPortParity;
    if (ui->boxParity->currentText() == "Нет")
    {
        SerialPortParity = QSerialPort::NoParity;
    } else if (ui->boxParity->currentText() == "Чётный")
    {
        SerialPortParity = QSerialPort::EvenParity;
    } else if (ui->boxParity->currentText() == "Нечётный")
    {
        SerialPortParity = QSerialPort::OddParity;
    } else if (ui->boxParity->currentText() == "Пробел")
    {
        SerialPortParity = QSerialPort::SpaceParity;
    } else if (ui->boxParity->currentText() == "Маркер")
    {
        SerialPortParity = QSerialPort::MarkParity;
    }

    QSerialPort::DataBits serialPortDataBits;
    if (ui->boxDataBits->currentText() == "5")
    {
        serialPortDataBits = QSerialPort::Data5;
    } else if (ui->boxDataBits->currentText() == "6")
    {
        serialPortDataBits = QSerialPort::Data6;
    } else if (ui->boxDataBits->currentText() == "7")
    {
        serialPortDataBits = QSerialPort::Data7;
    } else if (ui->boxDataBits->currentText() == "8")
    {
        serialPortDataBits = QSerialPort::Data8;
    }

    QSerialPort::StopBits serialPortStopBits;
    if (ui->boxStopBits->currentText() == "1")
    {
        serialPortStopBits = QSerialPort::OneStop;
    } else if (ui->boxStopBits->currentText() == "1,5")
    {
        serialPortStopBits = QSerialPort::OneAndHalfStop;
    } else if (ui->boxStopBits->currentText() == "2")
    {
        serialPortStopBits = QSerialPort::TwoStop;
    }

    // MilStd settings
    QString MilStdMode = ui->milSTDModeLineEdit->text();
    int MilStdDeviceNumber = ui->milSTDDeviceNumberLineEdit->text().toInt();
    int MilStdChannelNumber = ui->milSTDChannelNumberLineEdit->text().toInt();

    // ModBus settings
    QString ModBusIP = ui->ModBusIPLineEdit->text();
    int ModBusPort = ui->ModBusPortLineEdit->text().toInt();

    for (int row = 0; row < ui->listOfNetworkConnections->count(); row++) {

        QString item = ui->listOfNetworkConnections->item(row)->text();

        if (item == "TCP")
        {
            if(connectionProtocol.checkTCPConnection(TCPIP, TCPPort) == true)
            {
                ui->outputLineEdit->setText("Успешное подключение по протоколу TCP");
            } else
            {
                ui->outputLineEdit->setText("Ошибка подключения по протоколу TCP");
            }
        }
        else if (item == "UDP")
        {
            if(connectionProtocol.checkUDPConnection(UDPIP, UDPPort) == true)
            {
                ui->outputLineEdit->setText("Успешное подключение по протоколу UDP");
            } else
            {
                ui->outputLineEdit->setText("Ошибка подключения по протоколу UDP");
            }
        }
        else if (item == "Serial Port")
        {
            if(connectionProtocol.checkSerialPortConnection(serialPortCOMPort, serialPortBaudRate, SerialPortParity,
                                      serialPortDataBits, serialPortStopBits) == true)
            {
                ui->outputLineEdit->setText("Успешное подключение по Serial Port");
            } else
            {
                ui->outputLineEdit->setText("Ошибка подключения по Serial Port");
            }
        }
        else if (item == "MilStd")
        {
            if(connectionProtocol.checkMilStdConnection(MilStdMode, MilStdDeviceNumber, MilStdChannelNumber) == true)
            {
                ui->outputLineEdit->setText("Успешное подключение по протоколу MilStd");
            } else
            {
                ui->outputLineEdit->setText("Ошибка подключения по протоколу MilStd");
            }
        }
        else if (item == "ModBus")
        {
            if(connectionProtocol.checkModbusConnection(ModBusIP, ModBusPort) == true)
            {
                ui->outputLineEdit->setText("Успешное подключение по протоколу ModBus");
            } else
            {
                ui->outputLineEdit->setText("Ошибка подключения по протоколу ModBus");
            }
        }
    }
}

void MainWindow::saveSettings(Tree tree)
{
    tree.saveTreeToRegistry(tree.getSettingsTree());

    QString currentSerialNumber = ui->serialNumberLineEdit->text();

    int topLevelItemCount = tree.getSettingsTree()->topLevelItemCount();
    for (int i = 0; i < topLevelItemCount; ++i)
    {
        QString item = tree.getSettingsTree()->topLevelItem(i)->data(0, 0).toString();
        if (ui->serialNumberComboBox->findText(item) == -1)
        {
            ui->serialNumberComboBox->addItem(item);
        }

        loadSettingsFromRegistry(item);
    }

    loadSettingsFromRegistry(currentSerialNumber);
}


void MainWindow::on_saveSettingsButton_clicked(Tree tree)
{
    QMap<QString, QVariant> settingsMap;

    // Serial number
    QString serialNumber = ui->serialNumberLineEdit->text();
    settingsMap.insert("serialNumber", QVariant(serialNumber));

    // TCP settings
    QString TCPIP = ui->TCPIPLineEdit->text();
    int TCPPort = ui->TCPPortLineEdit->text().toInt();

    settingsMap.insert("TCPIP", QVariant(TCPIP));
    settingsMap.insert("TCPPort", QVariant(TCPPort));

    // UDP settings
    QString UDPIP = ui->UDPIPLineEdit->text();
    int UDPPort = ui->UDPPortLineEdit->text().toInt();

    settingsMap.insert("UDPIP", QVariant(UDPIP));
    settingsMap.insert("UDPPort", QVariant(UDPPort));

    // Serial Port settings
    QString serialPortCOMPort = ui->boxCOMPort->currentText();
    int serialPortBaudRate = ui->boxBaudRate->currentText().toInt();
    QSerialPort::Parity SerialPortParity;
    if (ui->boxParity->currentText() == "Чётный")
    {
        SerialPortParity = QSerialPort::EvenParity;
    } else if (ui->boxParity->currentText() == "Не чётный")
    {
        SerialPortParity = QSerialPort::OddParity;
    } else if (ui->boxParity->currentText() == "Пробел")
    {
        SerialPortParity = QSerialPort::SpaceParity;
    } else if (ui->boxParity->currentText() == "Маркер")
    {
        SerialPortParity = QSerialPort::MarkParity;
    } else
    {
        SerialPortParity = QSerialPort::NoParity;
    }

    QSerialPort::DataBits serialPortDataBits;
    if (ui->boxDataBits->currentText() == "6")
    {
        serialPortDataBits = QSerialPort::Data6;
    } else if (ui->boxDataBits->currentText() == "7")
    {
        serialPortDataBits = QSerialPort::Data7;
    } else if (ui->boxDataBits->currentText() == "8")
    {
        serialPortDataBits = QSerialPort::Data8;
    } else {
        serialPortDataBits = QSerialPort::Data5;
    }

    QSerialPort::StopBits serialPortStopBits;
    if (ui->boxStopBits->currentText() == "2")
    {
        serialPortStopBits = QSerialPort::TwoStop;
    } else if (ui->boxStopBits->currentText() == "1,5")
    {
        serialPortStopBits = QSerialPort::OneAndHalfStop;
    } else {
        serialPortStopBits = QSerialPort::OneStop;
    }

    settingsMap.insert("SerialPortCOMPort", QVariant(serialPortCOMPort));
    settingsMap.insert("SerialPortBaudRate", QVariant(serialPortBaudRate));
    settingsMap.insert("SerialPortParity", QVariant(SerialPortParity));
    settingsMap.insert("SerialPortDataBits", QVariant(serialPortDataBits));
    settingsMap.insert("SerialPortStopBits", QVariant(serialPortStopBits));

    // MilStd settings
    QString MilStdMode = ui->milSTDModeLineEdit->text();
    int MilStdDeviceNumber = ui->milSTDDeviceNumberLineEdit->text().toInt();
    int MilStdChannelNumber = ui->milSTDChannelNumberLineEdit->text().toInt();

    settingsMap.insert("MilStdMode", QVariant(MilStdMode));
    settingsMap.insert("MilStdDeviceNumber", QVariant(MilStdDeviceNumber));
    settingsMap.insert("MilStdChannelNumber", QVariant(MilStdChannelNumber));

    // ModBus settings
    QString ModBusIP = ui->ModBusIPLineEdit->text();
    int ModBusPort = ui->ModBusPortLineEdit->text().toInt();

    settingsMap.insert("ModBusIP", QVariant(ModBusIP));
    settingsMap.insert("ModBusPort", QVariant(ModBusPort));

    QString type = ui->boxProductType->currentText();

    QString channel = ui->boxChannelOfDevice->currentText();

    QListWidget* listOfNetworkConnections = ui->listOfNetworkConnections;

    if (type == "Изделие")
    {
        Product product(serialNumber, type, channel, listOfNetworkConnections, settingsMap);
        tree.addItemsToTree(tree.getSettingsTree(), product.getSerialNumber(), product.getProductType(), product.getChannel(), &product.getListOfNetworkConnections(), product.getSettingsMap());
    } else
    {
        Device device(serialNumber, type, channel, listOfNetworkConnections, settingsMap);
        tree.addItemsToTree(tree.getSettingsTree(), device.getSerialNumber(), device.getProductType(), device.getChannel(), &device.getListOfNetworkConnections(), device.getSettingsMap());
    }

    saveSettings(tree);
}

void MainWindow::on_TCPIPComboBox_currentIndexChanged(int index)
{
    QString selectedTCPIP = ui->TCPIPComboBox->itemText(index);

    ui->TCPIPLineEdit->clear();
    ui->TCPIPLineEdit->setText(selectedTCPIP);
}


void MainWindow::on_TCPPortComboBox_currentIndexChanged(int index)
{
    QString selectedTCPPort = ui->TCPPortComboBox->itemText(index);

    ui->TCPPortLineEdit->clear();
    ui->TCPPortLineEdit->setText(selectedTCPPort);
}


void MainWindow::on_UDPIPComboBox_currentIndexChanged(int index)
{
    QString selectedUDPIP = ui->UDPIPComboBox->itemText(index);

    ui->UDPIPLineEdit->clear();
    ui->UDPIPLineEdit->setText(selectedUDPIP);
}


void MainWindow::on_UDPPortComboBox_currentIndexChanged(int index)
{
    QString selectedUDPPort = ui->UDPPortComboBox->itemText(index);

    ui->UDPPortLineEdit->clear();
    ui->UDPPortLineEdit->setText(selectedUDPPort);
}


void MainWindow::on_ModBusIPComboBox_currentIndexChanged(int index)
{
    QString selectedModBusIP = ui->ModBusIPComboBox->itemText(index);

    ui->ModBusIPLineEdit->clear();
    ui->ModBusIPLineEdit->setText(selectedModBusIP);
}


void MainWindow::on_ModBusPortComboBox_currentIndexChanged(int index)
{
    QString selectedModBusPort = ui->ModBusPortComboBox->itemText(index);

    ui->ModBusPortLineEdit->clear();
    ui->ModBusPortLineEdit->setText(selectedModBusPort);
}


void MainWindow::on_milSTDModeComboBox_currentIndexChanged(int index)
{
    QString selectedmilSTDMode = ui->milSTDModeComboBox->itemText(index);

    ui->milSTDModeLineEdit->clear();
    ui->milSTDModeLineEdit->setText(selectedmilSTDMode);
}


void MainWindow::on_milSTDDeviceNumberComboBox_currentIndexChanged(int index)
{
    QString selectedmilSTDDeviceNumber = ui->milSTDDeviceNumberComboBox->itemText(index);

    ui->milSTDDeviceNumberLineEdit->clear();
    ui->milSTDDeviceNumberLineEdit->setText(selectedmilSTDDeviceNumber);
}


void MainWindow::on_milSTDChannelNumberComboBox_currentIndexChanged(int index)
{
    QString selectedmilSTDChannelNumber = ui->milSTDChannelNumberComboBox->itemText(index);

    ui->milSTDChannelNumberLineEdit->clear();
    ui->milSTDChannelNumberLineEdit->setText(selectedmilSTDChannelNumber);
}


void MainWindow::on_serialNumberLineEdit_textChanged(const QString &arg1)
{
    if (ui->serialNumberLineEdit->text().isEmpty())
    {
        ui->saveSettingsButton->setEnabled(false);
    }
    else
    {
        ui->saveSettingsButton->setEnabled(true);
    }
}



void MainWindow::on_serialNumberLineEdit_editingFinished()
{
    ui->serialNumberComboBox->setCurrentText(ui->serialNumberLineEdit->text());
}
