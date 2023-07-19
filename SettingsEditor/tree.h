#ifndef TREE_H
#define TREE_H
#include <QTreeWidget>
#include <QSettings>
#include <QListWidget>

class Tree
{
public:
    Tree(QTreeWidget* settingsTree);

    QTreeWidget* getSettingsTree();

    void removeItem(QTreeWidgetItem* item);
    void addItemsToTree(QTreeWidget* treeWidget, QString serialNumber, QString type, QString channel,
                        QListWidget* listOfNetworkConnections, QMap<QString, QVariant> settingsMap);
    void saveTreeToRegistry(QTreeWidget *treeWidget);
    void saveItemToRegistry(QSettings& settings, QTreeWidgetItem* item);
    void saveSerialNumberToRegistry(QSettings& settings, QTreeWidgetItem* item);
    void updateItemInRegistry(QSettings& settings, QTreeWidgetItem* item);
    void loadTreeFromRegistry(QTreeWidget* treeWidget);
    void loadItemFromRegistry(QSettings& settings, QTreeWidgetItem* parentItem, const QString& groupKey);


    void setSettingsTree(QTreeWidget* settingsTree);

private:
    QTreeWidget* settingsTree;

};

#endif // TREE_H
