#include "tree.h"
#include "ui_mainwindow.h"
#include <QSettings>
#include <QMap>
#include <QDebug>

Tree::Tree(QTreeWidget* settingsTree)
{
    setSettingsTree(settingsTree);
}

void Tree::setSettingsTree(QTreeWidget* settingsTree)
{
    this->settingsTree = settingsTree;
}

QTreeWidget* Tree::getSettingsTree()
{
    return settingsTree;
}

void Tree::removeItem(QTreeWidgetItem* item)
{
    while(item->childCount() > 0)
    {
        QTreeWidgetItem* child = item->takeChild(0);
        removeItem(child);
    }

    QTreeWidgetItem* parent = item->parent();
    if (parent)
        parent->removeChild(item);
    else if (item->treeWidget())
        item->treeWidget()->takeTopLevelItem(item->treeWidget()->indexOfTopLevelItem(item));
    delete item;
}
void Tree::addItemsToTree(QTreeWidget* treeWidget, QString serialNumber, QString type, QString channel,
                          QListWidget* listOfNetworkConnections, QMap<QString, QVariant> settingsMap)
{
    QTreeWidgetItem* serialNumberItem = nullptr;
    int topLevelItemCount = treeWidget->topLevelItemCount();
    for (int i = 0; i < topLevelItemCount; ++i)
    {
        QTreeWidgetItem* item = treeWidget->topLevelItem(i);
        if (item->text(0) == serialNumber)
        {
            serialNumberItem = item;
            break;
        }
    }

    if (!serialNumberItem)
    {
        serialNumberItem = new QTreeWidgetItem(treeWidget);
        serialNumberItem->setText(0, serialNumber);
    }

    QTreeWidgetItem* typeItem = nullptr;
    int childSerialNumberItemCount = serialNumberItem->childCount();
    for (int i = 0; i < childSerialNumberItemCount; ++i)
    {
        QTreeWidgetItem* item = serialNumberItem->child(i);
        if (item->text(0) == type)
        {
            typeItem = item;
            break;
        }
        else
        {
            removeItem(item);
        }
    }

    if (!typeItem)
    {
        typeItem = new QTreeWidgetItem(serialNumberItem);
        typeItem->setText(0, type);
    }


    QTreeWidgetItem* channelItem = nullptr;
    int childTypeItemCount = typeItem->childCount();
    for (int i = 0; i < childTypeItemCount; ++i)
    {
        QTreeWidgetItem* item = typeItem->child(i);
        if (item->text(0) == channel)
        {
            channelItem = item;
            break;
        }
    }

    if (!channelItem)
    {
        channelItem = new QTreeWidgetItem(typeItem);
        channelItem->setText(0, channel);
    }

    for (auto it = settingsMap.begin(); it != settingsMap.end(); ++it)
    {
        const QVariant& settingKey = it.key();
        const QVariant& settingValue = it.value();

        if (settingValue != "")
        {
            QTreeWidgetItem* settingKeyItem = nullptr;
            int childChannelItemCount = channelItem->childCount();
            for (int i = 0; i < childChannelItemCount; ++i)
            {
                QTreeWidgetItem* item = channelItem->child(i);
                if (item->text(0) == settingKey)
                {
                    settingKeyItem = item;
                    settingKeyItem->child(0)->setText(0, settingValue.toString());
                    break;
                }
            }

            if (!settingKeyItem)
            {
                QTreeWidgetItem* settingKeyItem = new QTreeWidgetItem(channelItem);
                settingKeyItem->setText(0, settingKey.toString());

                QTreeWidgetItem* settingValueItem = new QTreeWidgetItem(settingKeyItem);
                settingValueItem->setText(0, settingValue.toString());
            }
        }
    }
}

void Tree::saveTreeToRegistry(QTreeWidget* treeWidget)
{
    QSettings settings("OPMO", "SettingsEditor");
    settings.beginGroup("SerialNumbers");

    for (int i = 0; i < treeWidget->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem* item = treeWidget->topLevelItem(i);
        QString itemText = item->text(0);

        if (!settings.childGroups().contains(itemText))
        {
            qDebug() << settings.childGroups();
            saveItemToRegistry(settings, item);
        }
        else
        {
            qDebug() << settings.childGroups();
            settings.beginGroup(itemText);
            updateItemInRegistry(settings, item);
            settings.endGroup();
        }
    }

    settings.endGroup();
}

void Tree::saveItemToRegistry(QSettings& settings, QTreeWidgetItem* item)
{
    QString itemText = item->text(0);

    if (itemText == "Изделие" )
    {
        settings.beginGroup("Поворотный стол");
        settings.remove("");
        settings.endGroup();
        settings.beginGroup("Термокамера");
        settings.remove("");
        settings.endGroup();
        settings.beginGroup("Источник питания");
        settings.remove("");
        settings.endGroup();
    } else if (itemText == "Поворотный стол")
    {
        settings.beginGroup("Изделие");
        settings.remove("");
        settings.endGroup();
        settings.beginGroup("Термокамера");
        settings.remove("");
        settings.endGroup();
        settings.beginGroup("Источник питания");
        settings.remove("");
        settings.endGroup();
    } else if (itemText == "Термокамера")
    {
        settings.beginGroup("Изделие");
        settings.remove("");
        settings.endGroup();
        settings.beginGroup("Поворотный стол");
        settings.remove("");
        settings.endGroup();
        settings.beginGroup("Источник питания");
        settings.remove("");
        settings.endGroup();
    } else if (itemText == "Источник питания")
    {
        settings.beginGroup("Изделие");
        settings.remove("");
        settings.endGroup();
        settings.beginGroup("Поворотный стол");
        settings.remove("");
        settings.endGroup();
        settings.beginGroup("Термокамера");
        settings.remove("");
        settings.endGroup();
    }

    settings.remove(itemText);

    settings.beginGroup(itemText);

    settings.setValue("Text", itemText);

    for (int i = 0; i < item->childCount(); ++i)
    {
        QTreeWidgetItem* childItem = item->child(i);
        saveItemToRegistry(settings, childItem);
    }
    settings.endGroup();
}

void Tree::updateItemInRegistry(QSettings& settings, QTreeWidgetItem* item)
{
    QString itemText = item->text(0);

    for (int i = 0; i < item->childCount(); ++i)
    {
        QTreeWidgetItem* childItem = item->child(i);
        saveItemToRegistry(settings, childItem);
    }
}

void Tree::loadTreeFromRegistry(QTreeWidget* treeWidget)
{
    QSettings settings("OPMO", "SettingsEditor");
    settings.beginGroup("SerialNumbers");

    QStringList groupKeys = settings.childGroups();
    foreach (const QString& groupKey, groupKeys)
    {
        QTreeWidgetItem* item = new QTreeWidgetItem(treeWidget);
        loadItemFromRegistry(settings, item, groupKey);
    }

    settings.endGroup();
}

void Tree::loadItemFromRegistry(QSettings& settings, QTreeWidgetItem* parentItem, const QString& groupKey)
{
    settings.beginGroup(groupKey);

    QString itemText = settings.value("Text").toString();
    parentItem->setText(0, itemText);

    QStringList groupKeys = settings.childGroups();
    foreach (const QString& childGroupKey, groupKeys)
    {
        QTreeWidgetItem* childItem = new QTreeWidgetItem(parentItem);
        loadItemFromRegistry(settings, childItem, childGroupKey);
    }

    settings.endGroup();
}
