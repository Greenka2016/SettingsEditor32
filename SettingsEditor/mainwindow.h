#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QTreeWidget>
#include <QSettings>

#include "connectionprotocol.h"
#include "tree.h"
#include "product.h"
#include "powersource.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void saveSettings(Tree tree);
    void uploadTreeData(QTreeWidget* treeWidget);
    void loadSettingsFromRegistry(QString selectedSerialNumber);
    void loadSettingsFromRegistry(QString selectedSerialNumber, QString selectedChannelOfDevice);
    void resetSettings();
    void insertSettings(QList<QTreeWidgetItem*> settings);
    QTreeWidgetItem* findItemByValue(QTreeWidget* settingsTree, QString selectedSerialNumber);
    QList<QTreeWidgetItem*> getChildItems(QTreeWidgetItem* parentItem);

private slots:
    void on_connectionTestButton_clicked();

    void on_saveSettingsButton_clicked(Tree tree);

    void on_serialNumberComboBox_currentIndexChanged(int index);

    void on_boxProductType_currentIndexChanged(int index);

    void on_boxChannelOfDevice_activated(int index);

    void on_TCPIPComboBox_currentIndexChanged(int index);

    void on_TCPPortComboBox_currentIndexChanged(int index);

    void on_UDPIPComboBox_currentIndexChanged(int index);

    void on_UDPPortComboBox_currentIndexChanged(int index);

    void on_ModBusIPComboBox_currentIndexChanged(int index);

    void on_ModBusPortComboBox_currentIndexChanged(int index);

    void on_milSTDModeComboBox_currentIndexChanged(int index);

    void on_milSTDDeviceNumberComboBox_currentIndexChanged(int index);

    void on_milSTDChannelNumberComboBox_currentIndexChanged(int index);

    void on_serialNumberLineEdit_textChanged(const QString &arg1);

    void on_serialNumberLineEdit_editingFinished();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
