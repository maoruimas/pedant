#include "statisticsmodel.h"

StatisticsModel::StatisticsModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

void StatisticsModel::setStatistics(QVector<Field> *statistics)
{
    mStatistics = statistics;
}

void StatisticsModel::beginResetAll()
{
    beginResetModel();
}

void StatisticsModel::endResetAll()
{
    endResetModel();
}

int StatisticsModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || !mStatistics)
        return 0;

    return mStatistics->length();
}

QVariant StatisticsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !mStatistics)
        return QVariant();

    const Field field = mStatistics->at(index.row());
    switch (role) {
    case WhenRole:
        return QVariant(field.when());
    case IsDayRole:
        return QVariant(field.isDay);
    case LengthRole:
        return QVariant(field.length);
    }

    return QVariant();
}

QHash<int, QByteArray> StatisticsModel::roleNames() const
{
    QHash<int, QByteArray> names;
    names[WhenRole] = "when";
    names[IsDayRole] = "isDay";
    names[LengthRole] = "length";
    return names;
}
