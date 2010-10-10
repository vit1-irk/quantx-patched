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
    return model->Ebound.size();
}


int 
LevelModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 1;
}

void
LevelModel::invalidateModel()
{
    reset();
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

    if (role != Qt::DisplayRole)
        return QVariant();

    switch (index.column())
    {
    case 0: // En
        {
        int j = index.row();
        if (j < model->Ebound.size())
            return QVariant(model->Ebound[model->Ebound.size() - j - 1]);
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
        case 0: return QVariant("E(n)");
        default: return QVariant();
        }
    case Qt::Vertical:
        if (section >= 0 && section < model->Ebound.size())
            return QVariant(model->Ebound.size() - section);
        else
            return QVariant();
    default:
        return QVariant();
    }
}
