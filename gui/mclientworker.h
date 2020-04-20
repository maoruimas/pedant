#ifndef MCLIENTWORKER_H
#define MCLIENTWORKER_H

#include <QObject>
#include "mclient.h"

class MClientWorker : public QObject
{
    Q_OBJECT
public:
    explicit MClientWorker(QObject *parent = nullptr);

    QString hostName;
    quint16 port;

signals:
    void finished(const MClient::Result &result);

public slots:
    void upload(const QString &localPath, const QString &remotePath);
    void download(const QString &remotePath, const QString &localPath);
    void existsFile(const QString &remotePath);
    void existsDir(const QString &remotePath);
    void removeFile(const QString &remotePath);
    void removeDir(const QString &remotePath);
    void renameFile(const QString &remotePath);
    void renameDir(const QString &remotePath);
};

#endif // MCLIENTWORKER_H
