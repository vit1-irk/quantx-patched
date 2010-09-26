#pragma once
#include <QAbstractTableModel>
#include "model.h"

class PotentialModel :
    public QAbstractTableModel
{
    Q_OBJECT
public:
    PotentialModel(QObject * parent = 0);
    ~PotentialModel(void);

    void setPotential(Model *model);

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex& index) const; //return Qt::ItemIsEditable!

    //bool insertRows(int row,int count,const QModelIndex& parent = QModelIndex());
    // An insertRows() implementation must call beginInsertRows() before inserting new rows into the data structure, and it must call endInsertRows() immediately afterwards.
    //bool removeRows(int row,int count,const QModelIndex& parent = QModelIndex());
    // A removeRows() implementation must call beginRemoveRows() before the rows are removed from the data structure, and it must call endRemoveRows() immediately afterwards.
public slots:
    void invalidateModel();

private:
    Model *model;
};
