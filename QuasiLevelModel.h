#pragma once
#include <QAbstractTableModel>
#include "PhysicalModel.h"

class QuasiLevelModel :
    public QAbstractTableModel
{
    Q_OBJECT
public:
    QuasiLevelModel(QObject * parent = 0);
    ~QuasiLevelModel(void);

    void setQuasiLevels(PhysicalModel *model);
        
    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
    Qt::ItemFlags flags (const QModelIndex & index ) const;

public slots:
    void invalidateModel();

private:
    PhysicalModel *model;
};
