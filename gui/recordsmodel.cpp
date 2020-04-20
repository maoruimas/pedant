#include <QDebug>
#include "recordsmodel.h"

RecordsModel::RecordsModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

void RecordsModel::setRecords(QVector<int> *records)
{
    m_records = records;
}

void RecordsModel::beginResetAll()
{
    beginResetModel();
}

void RecordsModel::endResetAll()
{
    endResetModel();
}

void RecordsModel::beginRemove(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
}

void RecordsModel::endRomove()
{
    endRemoveRows();
}

int RecordsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || !m_records)
        return 0;

    return (m_records->length()+1)/2;
}

QVariant RecordsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !m_records)
        return QVariant();

    switch (role) {
    case FromRole:
        return QVariant(m_records->at(2*index.row()));
    case ToRole:
        return 2*index.row()+1 < m_records->length() ?
                    QVariant(m_records->at(2*index.row()+1)) : QVariant(0);
    }
    return QVariant();
}

QHash<int, QByteArray> RecordsModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[FromRole] = "from";
    names[ToRole] = "to";
    return names;
}
