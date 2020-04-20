#ifndef MCLIENT_H
#define MCLIENT_H

#include <QObject>
class MClientWorker;

class MClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString hostName READ hostName WRITE setHostName NOTIFY hostNameChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)

public:
    explicit MClient(QObject *parent = nullptr);
    ~MClient();

    enum Result {
        Positive, Negative, UnknownError, ConnectionFailure, TimeOut, LocalError
    };
    Q_ENUM(Result)

    QString hostName() const;
    void setHostName(const QString &hostName);

    int port() const;
    void setPort(const int &port);

signals:
    void hostNameChanged();
    void portChanged();

    void finished(const Result &result);

    void upload(const QString &localPath, const QString &remotePath);
    void download(const QString &remotePath, const QString &localPath);
    void existsFile(const QString &remotePath);
    void existsDir(const QString &remotePath);
    void removeFile(const QString &remotePath);
    void removeDir(const QString &remotePath);
    void renameFile(const QString &remotePath);
    void renameDir(const QString &remotePath);

private:
    QThread *thread;
    MClientWorker *worker;
};

#endif // MCLIENT_H
