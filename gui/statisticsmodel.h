#ifndef STATISTICSMODEL_H
#define STATISTICSMODEL_H

#include <QAbstractListModel>
#include <QDate>

struct Field {
    int year, month, day;
    bool isDay;
    int length;
    QString when() const {
        if(isDay) {
            return QString::number(day);
        } else {
            return QString::number(year) + "-" + QString::number(month);
        }
    }
};

class StatisticsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit StatisticsModel(QObject *parent = nullptr);

    enum {
        WhenRole = Qt::UserRole,
        IsDayRole,
        LengthRole
    };

    void setStatistics(QVector<Field> *statistics);
    void beginResetAll();
    void endResetAll();

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    virtual QHash<int, QByteArray> roleNames() const override;

private:
     QVector<Field> *mStatistics;
};

#endif // STATISTICSMODEL_H
