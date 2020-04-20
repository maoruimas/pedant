#include <QFile>
#include <QDir>
#include <QTcpSocket>
#include "mclientworker.h"
#include "handshake.h"

MClientWorker::MClientWorker(QObject *parent) : QObject(parent)
{

}

void MClientWorker::upload(const QString &localPath, const QString &remotePath)
{
    //////MLog::log(QStringLiteral("Upload %1 to %2 on %3:%4").arg(localPath).arg(remotePath).arg(hostName).arg(port));
    // connect to server
    QTcpSocket socket;
    socket.connectToHost(hostName, port);
    if(!socket.waitForConnected(TIME_OUT)){
        //////MLog::log(QStringLiteral("Fail to connect to server"));
        emit finished(MClient::ConnectionFailure);
        return;
    }
    // read local file
    QFile file(localPath);
    if(!file.open(QIODevice::ReadOnly)){
        //////MLog::log(QStringLiteral("Fail to read local file"));
        emit finished(MClient::LocalError);
        return;
    }
    /******************** STEP 1 ********************/
    // send request
    socket.write(HandShake::buildRequest(remotePath, file.size()));
    if(!socket.waitForBytesWritten(TIME_OUT)){
        //////MLog::log(QStringLiteral("Fail to send UPLOAD request to server"));
        file.close();
        emit finished(MClient::TimeOut);
        return;
    }
    /******************** STEP 2 ********************/
    // recieve response
    if(!socket.waitForReadyRead(TIME_OUT)){
        //////MLog::log(QStringLiteral("Fail to recieve response from server"));
        file.close();
        emit finished(MClient::TimeOut);
        return;
    }
    Response response = HandShake::parseResponse(socket.readAll());
    if(!response.parseSucceed || (response.id != RES_OK && response.id != RES_NO)){
        //////MLog::log(QStringLiteral("Fail to parse response from server"));
        file.close();
        emit finished(MClient::UnknownError);
        return;
    }else if(response.id == RES_NO){
        //////MLog::log(QStringLiteral("Upload refused by server"));
        file.close();
        emit finished(MClient::Negative);
        return;
    }
    /******************** STEP 3 ********************/
    // upload file
    //////MLog::log(QStringLiteral("%1 bytes of data to upload").arg(file.size()));
    char buf[PAY_LOAD];
    qint64 uploadTotal = 0;
    qint64 uploaded;
    while((uploaded = file.read(buf, PAY_LOAD)) > 0){
        uploadTotal += uploaded;
        socket.write(buf, uploaded);
        if(!socket.waitForBytesWritten(TIME_OUT)){
            //////MLog::log(QStringLiteral("Network interruption during upload"));
            file.close();
            emit finished(MClient::TimeOut);
            return;
        }
    }
    file.close();
    /******************** STEP 4 ********************/
    // confirm with server
    if(!socket.waitForReadyRead(TIME_OUT)){
        //////MLog::log(QStringLiteral("Fail to recieve confirm form server"));
        emit finished(MClient::TimeOut);
        return;
    }
    response = HandShake::parseResponse(socket.readAll());
    if(!response.parseSucceed || (response.id != RES_OK && response.id != RES_NO)){
        //////MLog::log(QStringLiteral("Fail to parse confirm form server"));
        emit finished(MClient::UnknownError);
        return;
    }else if(response.id == RES_NO){
        //////MLog::log(QStringLiteral("Server fail to recieve uploaded file"));
        emit finished(MClient::Negative);
        return;
    }
    //////MLog::log(QStringLiteral("Upload succeeded"));
    emit finished(MClient::Positive);
}

void MClientWorker::download(const QString &remotePath, const QString &localPath)
{
    ////MLog::log(QStringLiteral("Download %1 on %2:%3 to %4").arg(remotePath).arg(hostName).arg(port).arg(localPath));
    // connect to server
    QTcpSocket socket;
    socket.connectToHost(hostName, port);
    if(!socket.waitForConnected(TIME_OUT)){
        ////MLog::log(QStringLiteral("Fail to connect to server"));
        emit finished(MClient::ConnectionFailure);
        return;
    }
    /******************** STEP 1 ********************/
    // send request
    socket.write(HandShake::buildRequest(remotePath, REQ_DOWNLOAD));
    if(!socket.waitForBytesWritten(TIME_OUT)){
        ////MLog::log(QStringLiteral("Fail to send DOWNLOAD request to server"));
        emit finished(MClient::TimeOut);
        return;
    }
    /******************** STEP 2 ********************/
    // recieve response
    if(!socket.waitForReadyRead(TIME_OUT)){
        ////MLog::log(QStringLiteral("Fail to recieve response from server"));
        emit finished(MClient::TimeOut);
        return;
    }
    Response response = HandShake::parseResponse(socket.readAll());
    if(!response.parseSucceed || (response.id != RES_FILE && response.id != RES_NO)){
        ////MLog::log(QStringLiteral("Fail to parse response from server"));
        emit finished(MClient::UnknownError);
        return;
    }else if(response.id == RES_NO){
        ////MLog::log(QStringLiteral("Download refused by server"));
        emit finished(MClient::Negative);
        return;
    }
    /******************** STEP 3 ********************/
    socket.write(HandShake::buildRequest());
    if(!socket.waitForBytesWritten(TIME_OUT)){
        ////MLog::log(QStringLiteral("Fail to send OK request to server"));
        emit finished(MClient::TimeOut);
        return;
    }
    /******************** STEP 4 ********************/
    // prepare file
    QString dirPath = localPath.left(localPath.lastIndexOf('/') + 1);
    if(!dirPath.isEmpty()){
        QDir dir;
        if(!dir.mkpath(dirPath)){
            ////MLog::log(QStringLiteral("Fail to create necessary directories"));
            emit finished(MClient::LocalError);
            return;
        }
    }
    QFile dfile(localPath + ".download");
    if(!dfile.open(QIODevice::WriteOnly)){
        ////MLog::log(QStringLiteral("Fail to create .download file"));
        emit finished(MClient::LocalError);
        return;
    }
    // begin downloading
    ////MLog::log(QStringLiteral("%1 bytes of data to download").arg(response.fileSize));
    char buf[PAY_LOAD];
    qint64 downloadTotal = 0;
    qint64 downloaded;
    while(downloadTotal < response.fileSize){
        if(!socket.bytesAvailable() && !socket.waitForReadyRead(TIME_OUT)){
            ////MLog::log(QStringLiteral("Network interruption during download"));
            emit finished(MClient::TimeOut);
            return;
        }
        downloaded = socket.read(buf, PAY_LOAD);
        downloadTotal += downloaded;
        dfile.write(buf, downloaded);
    }
    dfile.close();
    // save data
    QFile file(localPath);
    if(file.exists() && !file.remove()){
        ////MLog::log(QStringLiteral("Fail to save downloaded data"));
        dfile.remove();
        emit finished(MClient::LocalError);
        return;
    }
    dfile.rename(localPath);
    ////MLog::log(QStringLiteral("Download succeeded"));
    /******************** STEP 5 ********************/
    socket.write(HandShake::buildRequest());
    if(!socket.waitForBytesWritten(TIME_OUT)){
        ////MLog::log(QStringLiteral("Fail to send confirm to server"));
        emit finished(MClient::TimeOut);
        return;
    }
    emit finished(MClient::Positive);
}

void MClientWorker::existsFile(const QString &remotePath)
{
    //MLog::log(QStringLiteral("Check existence of file %1 on %2:%3").arg(remotePath).arg(hostName).arg(port));
    // connect to server
    QTcpSocket socket;
    socket.connectToHost(hostName, port);
    if(!socket.waitForConnected(TIME_OUT)){
        //MLog::log(QStringLiteral("Fail to connect to server"));
        emit finished(MClient::ConnectionFailure);
        return;
    }
    /******************** STEP 1 ********************/
    // send request
    socket.write(HandShake::buildRequest(remotePath, REQ_EXFILE));
    if(!socket.waitForBytesWritten(TIME_OUT)){
        //MLog::log(QStringLiteral("Fail to send EXFILE request to server"));
        emit finished(MClient::TimeOut);
        return;
    }
    /******************** STEP 2 ********************/
    // recieve response
    if(!socket.waitForReadyRead(TIME_OUT)){
        //MLog::log(QStringLiteral("Fail to recieve response from server"));
        emit finished(MClient::TimeOut);
        return;
    }
    Response response = HandShake::parseResponse(socket.readAll());
    if(!response.parseSucceed || (response.id != RES_NO && response.id != RES_OK)){
        //MLog::log(QStringLiteral("Fail to parse response from server"));
        emit finished(MClient::UnknownError);
        return;
    }
    //if(response.id == RES_OK)
        //MLog::log(QStringLiteral("File exists"));
    //else
        //MLog::log(QStringLiteral("File does not exist"));
    emit finished(response.id == RES_OK ? MClient::Positive : MClient::Negative);
}

void MClientWorker::existsDir(const QString &remotePath)
{
    emit finished(MClient::UnknownError);
}

void MClientWorker::removeFile(const QString &remotePath)
{
    //MLog::log(QStringLiteral("Remove file %1 on %2:%3").arg(remotePath).arg(hostName).arg(port));
    // connect to server
    QTcpSocket socket;
    socket.connectToHost(hostName, port);
    if(!socket.waitForConnected(TIME_OUT)){
        //MLog::log(QStringLiteral("Fail to connect to server"));
        emit finished(MClient::ConnectionFailure);
        return;
    }
    /******************** STEP 1 ********************/
    // send request
    socket.write(HandShake::buildRequest(remotePath, REQ_RMFILE));
    if(!socket.waitForBytesWritten(TIME_OUT)){
        //MLog::log(QStringLiteral("Fail to send RMFILE request to server"));
        emit finished(MClient::TimeOut);
        return;
    }
    /******************** STEP 2 ********************/
    // recieve response
    if(!socket.waitForReadyRead(TIME_OUT)){
        //MLog::log(QStringLiteral("[0x%1] fail to recieve response from server"));
        emit finished(MClient::TimeOut);
        return;
    }
    Response response = HandShake::parseResponse(socket.readAll());
    if(!response.parseSucceed || (response.id != RES_OK && response.id != RES_NO)){
        //MLog::log(QStringLiteral("[0x%1] fail to parse response from server"));
        emit finished(MClient::UnknownError);
        return;
    }
    //if(response.id == RES_OK)
        //MLog::log(QStringLiteral("[0x%1] remove file succeeded"));
    //else
        //MLog::log(QStringLiteral("[0x%1] remove file failed"));
    emit finished(response.id == RES_OK ? MClient::Positive : MClient::Negative);
}

void MClientWorker::removeDir(const QString &remotePath)
{
    emit finished(MClient::UnknownError);
}

void MClientWorker::renameFile(const QString &remotePath)
{
    emit finished(MClient::UnknownError);
}

void MClientWorker::renameDir(const QString &remotePath)
{
    emit finished(MClient::UnknownError);
}
