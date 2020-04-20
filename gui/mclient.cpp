#include <QThread>
#include "mclient.h"
#include "mclientworker.h"

MClient::MClient(QObject *parent)
    : QObject(parent)
    , thread(new QThread)
    , worker(new MClientWorker)
{
    qRegisterMetaType<Result>("Result");
    connect(this, SIGNAL(upload(const QString &, const QString &)),
            worker, SLOT(upload(const QString &, const QString &)));
    connect(this, SIGNAL(download(const QString &, const QString &)),
            worker, SLOT(download(const QString &, const QString &)));
    connect(this, SIGNAL(existsFile(const QString &)),
            worker, SLOT(existsFile(const QString &)));
    connect(this, SIGNAL(existsDir(const QString &)),
            worker, SLOT(existsDir(const QString &)));
    connect(this, SIGNAL(removeFile(const QString &)),
            worker, SLOT(removeFile(const QString &)));
    connect(this, SIGNAL(removeDir(const QString &)),
            worker, SLOT(removeDir(const QString &)));
    connect(this, SIGNAL(renameFile(const QString &)),
            worker, SLOT(renameFile(const QString &)));
    connect(this, SIGNAL(renameDir(const QString &)),
            worker, SLOT(renameDir(const QString &)));
    connect(worker, SIGNAL(finished(const Result &)),
            this, SIGNAL(finished(const Result &)));
    worker->moveToThread(thread);
    thread->start();
}

MClient::~MClient()
{
    thread->quit();
    thread->wait();
}

QString MClient::hostName() const
{
    return worker->hostName;
}

void MClient::setHostName(const QString &hostName)
{
    if (worker->hostName != hostName) {
        worker->hostName = hostName;
        emit hostNameChanged();
    }
}

int MClient::port() const
{
    return int(worker->port);
}

void MClient::setPort(const int &port)
{
    if (worker->port != quint16(port)) {
        worker->port = quint16(port);
        emit portChanged();
    }
}
