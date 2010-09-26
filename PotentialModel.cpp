#include "PotentialModel.h"

PotentialModel::PotentialModel(QObject * parent)
: QAbstractTableModel(parent)
{
}
 
PotentialModel::~PotentialModel(void)
{
}

int
PotentialModel::rowCount(const QModelIndex& /*parent*/) const
{
    return model->N + 2;
}

int 
PotentialModel::columnCount(const QModelIndex& /*parent*/) const
{
    return 3;
}

void
PotentialModel::invalidateModel()
{
    reset();
}

void
PotentialModel::setPotential(Model *_model)
{
    this->model = _model;
    connect(model,SIGNAL(potentialChanged()),this,SLOT(invalidateModel()));
}

QVariant 
PotentialModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    switch(role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        break;
    default:
        return QVariant();
    }

    switch (index.column())
    {
    case 0: // width
        if (index.row()==0 || index.row() == model->N + 1)
            return QVariant();
        else 
            return QVariant(model->d(index.row()));
    case 1: // U
        return QVariant(model->Ui(index.row()));
    case 2: // mass
        return QVariant(model->m(index.row()));
    default:
        return QVariant();
    }
}

QVariant 
PotentialModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();
    switch (orientation)
    {
    case Qt::Horizontal:
        switch (section)
        {
        case 0: return QVariant("width");
        case 1: return QVariant("U");
        case 2: return QVariant("mass");
        default: return QVariant();
        }
    case Qt::Vertical:
        if (section >= 0)
            return QVariant(section);
        else
            return QVariant();
    default:
        return QVariant();
    }
}

bool
PotentialModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (role != Qt::EditRole)
        return false;

    switch(index.column())
    {
    case 0: // width
        if (index.row() < 1 || index.row() > model->N) 
            return false;
        model->d(index.row()) = value.toDouble();
        break;

    case 1: // U
        if (index.row() < 0 || index.row() > model->N+1) 
            return false;
        model->Ui(index.row()) = value.toDouble();
        break;

    case 2: // mass
        if (index.row() < 0 || index.row() > model->N+1) 
            return false;
        model->m(index.row()) = value.toDouble();
		break;

    default:
        return false;
    }
    emit(dataChanged(index, index));
    return true;
}

Qt::ItemFlags 
PotentialModel::flags(const QModelIndex& index) const
{
    switch(index.column())
    {
    case 0: // d_i
    case 1: // U_i
    case 2: // m_i
        if (index.row() < 0 || index.row() > model->N+1)
            return Qt::NoItemFlags;
        else
            return Qt::ItemIsEditable | Qt::ItemIsEnabled;
    default:
        return Qt::NoItemFlags;
    }
}

//bool insertRows(int row,int count,const QModelIndex& parent = QModelIndex());
        // An insertRows() implementation must call beginInsertRows() before inserting new rows into the data structure, and it must call endInsertRows() immediately afterwards.
//bool removeRows(int row,int count,const QModelIndex& parent = QModelIndex());
    // A removeRows() implementation must call beginRemoveRows() before the rows are removed from the data structure, and it must call endRemoveRows() immediately afterwards.
