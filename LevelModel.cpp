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

#include "LevelModel.h"

LevelModel::LevelModel(QObject * parent)
: QAbstractTableModel(parent)
{
}

LevelModel::~LevelModel(void)
{
}

int
LevelModel::rowCount(const QModelIndex& /*parent*/) const
{
    return model->getEn().size();
}


int
LevelModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 1;
}

void
LevelModel::invalidateModel()
{
    beginResetModel();
	// clear data here
	endResetModel();
}

void
LevelModel::setLevels(PhysicalModel *_model)
{
    this->model = _model;
    connect(model,SIGNAL(signalEboundChanged()),this,SLOT(invalidateModel()));
}

QVariant
LevelModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        break;
    default:
        return QVariant();
    }

    switch (index.column())
    {
    case 0: // En
        {
        int j = index.row();
        if (j < model->getEn().size())
            return QVariant(model->getEn(model->getEn().size() - j - 1));
        else
            return QVariant();
        }
    default:
        return QVariant();
    }
}


QVariant
LevelModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    switch (orientation)
    {
    case Qt::Horizontal:
        switch (section)
        {
        case 0: return QVariant(tr("E(n)"));
        default: return QVariant();
        }
    case Qt::Vertical:
        if (section >= 0 && section < model->getEn().size())
//            return QVariant(model->getEn().size() - section);
            return QVariant(model->getEn().size() - section-1);
        else
            return QVariant();
    default:
        return QVariant();
    }
}


Qt::ItemFlags LevelModel::flags (const QModelIndex & index ) const
{
    (void)index;
    return Qt::NoItemFlags
//    return Qt::ItemIsSelectable
        //| Qt::ItemIsEditable
        ;
}
