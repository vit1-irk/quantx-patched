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
