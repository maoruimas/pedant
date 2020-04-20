#ifndef RECORDSMODEL_H
#define RECORDSMODEL_H

#include <QAbstractListModel>

class RecordsModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit RecordsModel(QObject *parent = nullptr);

    enum {
        FromRole = Qt::UserRole,
        ToRole
    };

    void setRecords(QVector<int> *records);
    void beginResetAll();
    void endResetAll();
    void beginRemove(int row);
    void endRomove();

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    virtual QHash<int, QByteArray> roleNames() const override;

private:
    QVector<int> *m_records;
};

#endif // RECORDSMODEL_H
