/*
 *  Copyright (C) 2011 Olga Tkachenko
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 *  02110-1301 USA
 */

#pragma once
#include <QAbstractTableModel>
#include "PhysicalModel.h"

class PotentialModel :
    public QAbstractTableModel
{
    Q_OBJECT
public:
    PotentialModel(QObject * parent = 0);
    ~PotentialModel(void);

    void setPotential(PhysicalModel *model);

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
    PhysicalModel *model;
};
