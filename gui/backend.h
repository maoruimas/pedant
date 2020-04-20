#ifndef BACKEND_H
#define BACKEND_H

#include <QObject>
#include <QVector>
#include "statisticsmodel.h"
#include "recordsmodel.h"
class Worker;

class Backend : public QObject
{
    Q_OBJECT
public:
    explicit Backend(QObject *parent = nullptr);
    ~Backend();
    Q_PROPERTY(StatisticsModel *statisticsModel READ statisticsModel)
    Q_PROPERTY(RecordsModel *recordsModel READ recordsModel)
    Q_PROPERTY(bool isRunning READ isRunning NOTIFY isRunningChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int today READ today NOTIFY todayChanged)
    Q_PROPERTY(int total READ total NOTIFY totalChanged)
    Q_PROPERTY(int average READ average NOTIFY averageChanged)
    Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged)
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)

    bool isRunning();
    int count();
    int today();
    int total();
    int average();

    StatisticsModel *statisticsModel() const;
    RecordsModel *recordsModel() const;

    QString file() const;
    void setFile(const QString &file);

    QString username() const;
    void setUsername(const QString &username);

    QString password() const;
    void setPassword(const QString &password);

signals:
    void error(const QString &error);
    void isRunningChanged();
    void countChanged();
    void todayChanged();
    void totalChanged();
    void averageChanged();
    void fileChanged();
    void usernameChanged();
    void passwordChanged();

public slots:
    QString decrypt(const QString &fileName);
    bool encrypt(const QString &fileName, const QString &text);

    QString randomString();

    bool createFile();
    bool removeFileIfExists(const QString &fileName);
    bool readFile();

    void clear();

    // statistics
    bool edit(int row, int from, int to);
    bool remove(int row);
    bool add(int from, int to);
    // main
    void analyze();
    void update();
    bool toggle();
    int last();
    bool addOne(int record);
private:
    int nowRecord();
    QDateTime toDateTime(const int &record);
    bool saveAndAnalyze();

    QString m_file;

    QString m_username, m_password;
    QByteArray m_hashKey, m_hashIV;

    StatisticsModel *m_statisticsModel;
    RecordsModel *m_recordsModel;

    QVector<int> m_records;
    QVector<Field> m_statistics;

    bool m_isRunning;
    int m_today, m_total, m_average;
};

#endif // BACKEND_H
