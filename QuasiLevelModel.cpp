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

#include "QuasiLevelModel.h"

QuasiLevelModel::QuasiLevelModel(QObject * parent)
: QAbstractTableModel(parent)
{
}

QuasiLevelModel::~QuasiLevelModel(void)
{
}

int
QuasiLevelModel::rowCount(const QModelIndex& /*parent*/) const
{
    return model->getEquasi().size();
}


int
QuasiLevelModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 2;
}

void
QuasiLevelModel::invalidateModel()
{
    reset();
}

void
QuasiLevelModel::setQuasiLevels(PhysicalModel *_model)
{
    this->model = _model;
    connect(model,SIGNAL(signalEboundChanged()),this,SLOT(invalidateModel()));
}

QVariant
QuasiLevelModel::data(const QModelIndex& index, int role) const
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
        if (j < model->getEquasi().size())
            return QVariant(real(model->getEquasi(model->getEquasi().size() - j - 1)));
        else
            return QVariant();
        }
    case 1: // En
        {
        int j = index.row();
        if (j < model->getEquasi().size())
            return QVariant(imag(model->getEquasi(model->getEquasi().size() - j - 1)));
        else
            return QVariant();
        }
    default:
        return QVariant();
    }
}


QVariant
QuasiLevelModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    switch (orientation)
    {
    case Qt::Horizontal:
        switch (section)
        {
        case 0: return QVariant(tr("real(Ec(n))"));
        case 1: return QVariant(tr("imag(Ec(n))"));
        default: return QVariant();
        }
    case Qt::Vertical:
        if (section >= 0 && section < model->getEquasi().size())
//            return QVariant(model->getEquasi().size() - section);
            return QVariant(model->getEquasi().size() - section-1);
        else
            return QVariant();
    default:
        return QVariant();
    }
}


Qt::ItemFlags QuasiLevelModel::flags (const QModelIndex & index ) const
{
    (void)index;
    return Qt::NoItemFlags
        //| Qt::ItemIsSelectable
        //| Qt::ItemIsEditable
        ;
}
