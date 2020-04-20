#include <QThread>
#include <QFile>
#include <QDir>
#include <QDataStream>
#include <QDateTime>
#include <QCryptographicHash>
#include "backend.h"
#include "qaesencryption.h"

#include <QDebug>

Backend::Backend(QObject *parent)
    : QObject(parent)
    , m_statisticsModel(new StatisticsModel)
    , m_recordsModel(new RecordsModel)
{
    qsrand(uint(QDateTime::currentSecsSinceEpoch()));

    m_statisticsModel->setStatistics(&m_statistics);
    m_recordsModel->setRecords(&m_records);
}

Backend::~Backend()
{
    delete m_statisticsModel;
    delete m_recordsModel;
}

bool Backend::isRunning() { return m_isRunning; }
int Backend::count() {
    if (m_isRunning) {
        return nowRecord() - m_records.back();
    } else {
        return 0;
    }
}
int Backend::today() {
    if (m_isRunning) {
        QDateTime last = toDateTime(m_lastAnalyzeTime);
        QDateTime now = QDateTime::currentDateTime();
        if (last.daysTo(now)) { // a new day
            QDateTime beg(now.date());
            return int(beg.secsTo(now))/60;
        } else { // same day
            return m_today + int(last.secsTo(now))/60;
        }
    } else {
        return m_today;
    }
}
int Backend::total() {
    if (m_isRunning) {
        return m_total + (nowRecord() - m_lastAnalyzeTime);
    } else {
        return m_total;
    }
}
int Backend::average() {
    if (m_isRunning) {
        QDateTime last = toDateTime(m_lastAnalyzeTime);
        QDateTime now = QDateTime::currentDateTime();
        int n = m_ndays + int(last.daysTo(now));
        return total() / n;
    } else {
        return m_average;
    }
}

RecordsModel *Backend::recordsModel() const
{
    return m_recordsModel;
}

bool Backend::edit(int row, int from, int to)
{
    int i = 2*row;
    if ((i && m_records[i-1] > from) ||
            (i+2<m_records.length() && m_records[i+2] < to)) {
        emit error(tr("Overlap"));
        return false;
    }
    m_recordsModel->beginResetAll();
    // to
    if (i+2<m_records.length() && m_records[i+2] == to) {
        m_records.remove(i+1, 2);
    } else {
        m_records[i+1] = to;
    }
    // from
    if (i && m_records[i-1] == from) {
        m_records.remove(i-1, 2);
    } else {
        m_records[i] = from;
    }
    m_recordsModel->endResetAll();
    return saveAndAnalyze();
}

bool Backend::remove(int row)
{
    m_recordsModel->beginRemove(row);
    int i = 2*row;
    int c = (i+1 == m_records.length()) ? 1 : 2;
    m_records.remove(i, c);
    m_recordsModel->endRomove();
    return saveAndAnalyze();
}

StatisticsModel *Backend::statisticsModel() const
{
    return m_statisticsModel;
}

void Backend::analyze()
{
    m_lastAnalyzeTime = nowRecord();
    // m_isRunning
    m_isRunning = m_records.length() % 2;
    emit isRunningChanged();
    // m_total
    QVector<QPair<QDate, int> > history;
    m_total = 0;
    for(int i = 0; i < m_records.length(); i += 2) {
        int fromMin = m_records[i];
        int toMin = (i == m_records.length()-1) ? m_lastAnalyzeTime : m_records[i+1];
        m_total += toMin - fromMin;
        QDateTime from = toDateTime(fromMin);
        QDateTime to = toDateTime(toMin);
        do {
            QDate fromDate = from.date();
            if(history.isEmpty() || history.back().first != fromDate)
                history.append({fromDate, 0});
            // calc
            QDateTime next(fromDate.addDays(1));
            history.back().second += int(from.secsTo(next < to ? next : to) / 60);
            from = next;
        } while (from < to);
    }
    m_ndays = history.length();
    // m_today, m_average, etc
    m_today = (history.back().first == toDateTime(m_lastAnalyzeTime).date()) ?
                history.back().second : 0;
    m_average = history.isEmpty() ? 0 : m_total / history.length();
    emit countChanged();
    emit todayChanged();
    emit totalChanged();
    emit averageChanged();
    // m_statistics
    m_statisticsModel->beginResetAll();
    int p = 0;
    m_statistics.clear();
    for (int i = history.length()-1; i >= 0; --i) {
        auto &x = history.at(i);
        Field field;
        field.year = x.first.year();
        field.month = x.first.month();
        field.day = x.first.day();
        field.isDay = true;
        field.length = x.second;
        if (m_statistics.isEmpty() || m_statistics.back().year != field.year || m_statistics.back().month != field.month) {
            Field m;
            m.year = field.year;
            m.month = field.month;
            m.isDay = false;
            m.length = 0;
            p = m_statistics.length();
            m_statistics.append(m);
        }
        m_statistics.append(field);
        m_statistics[p].length += field.length;
    }
    m_statisticsModel->endResetAll();
}

void Backend::update()
{
    if (m_isRunning) {
        emit countChanged();
        emit todayChanged();
        emit totalChanged();
        emit averageChanged();
    }
}

bool Backend::toggle()
{
    // append record
    m_recordsModel->beginResetAll();
    int now = int(QDateTime::currentSecsSinceEpoch()/60);
    if(!m_records.isEmpty() && m_records.back() == now)
        m_records.pop_back();
    else
        m_records.append(now);
    m_recordsModel->endResetAll();
    return saveAndAnalyze();
}

int Backend::last()
{
    if(!m_records.isEmpty())
        return m_records.back();
    else
        return int(QDateTime::currentSecsSinceEpoch()/60);
}

bool Backend::add(int from, int to)
{
    if(to > int(QDateTime::currentSecsSinceEpoch()/60)) {
        emit error(tr("Exceed current time"));
        return false;
    } else if(from >= to) {
        emit error(tr("Invalid"));
        return false;
    } else {
        // p is the index of the last record
        int p = m_records.length() - 1;
        // p should not be a start-record
        if (p % 2 == 0)
            p--;
        // p should be the last record <= from
        while (p > 0 && m_records[p] > from)
            p -= 2;
        // verify
        if (p+1 < m_records.length() && m_records[p+1] < to) {
            emit error(tr("Overlap"));
            return false;
        }
        m_recordsModel->beginResetAll();
        // insert to
        if (p+1 < m_records.length() && m_records[p+1] == to) {
            m_records.remove(p+1);
        } else {
            m_records.insert(p+1, to);
        }
        // insert from
        if(p>0 && m_records[p] == from) {
            m_records.remove(p);
        } else {
            m_records.insert(p+1, from);
        }
        // done
        m_recordsModel->endResetAll();
        return saveAndAnalyze();
    }
}

bool Backend::addOne(int record)
{
    if(!m_records.isEmpty() && record < m_records.back()) {
        emit error(tr("Invalid"));
        return false;
    }
    m_recordsModel->beginResetAll();
    if (!m_records.isEmpty() && record == m_records.back()) {
        m_records.pop_back();
    } else {
        m_records.append(record);
    }
    m_recordsModel->endResetAll();
    return saveAndAnalyze();
}

int Backend::nowRecord()
{
    return int(QDateTime::currentSecsSinceEpoch()/60);
}

QDateTime Backend::toDateTime(const int &record)
{
    return QDateTime::fromSecsSinceEpoch(qint64(record)*60);
}

bool Backend::saveAndAnalyze()
{
    QFile file(m_file);
    if (!file.open(QIODevice::WriteOnly)) {
        emit error(tr("Fail to write to file"));
        return false;
    }
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_4_0);
    for (int &x: m_records) {
        out << x;
    }
    file.close();
    analyze();
    return true;
}

QString Backend::password() const
{
    return m_password;
}

void Backend::setPassword(const QString &password)
{
    if (m_password != password) {
        m_password = password;
        m_hashKey = QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256);
        emit passwordChanged();
    }
}

QString Backend::decrypt(const QString &fileName)
{
    QFile fin(fileName);
    if (!fin.open(QIODevice::ReadOnly)) {
        emit error(tr("Fail to read file"));
        return "";
    }
    QByteArray data = fin.readAll();
    fin.close();
    QByteArray decrypted = QAESEncryption::Decrypt(QAESEncryption::AES_256, QAESEncryption::CBC,
                                                   data, m_hashKey, m_hashIV);
    return QString::fromUtf8(QAESEncryption::RemovePadding(decrypted));
}

bool Backend::encrypt(const QString &fileName, const QString &text)
{
    QFile fout(fileName);
    if (!fout.open(QIODevice::WriteOnly)) {
        emit error(tr("Fail to write to file"));
        return false;
    }
    QByteArray data = QAESEncryption::Crypt(QAESEncryption::AES_256, QAESEncryption::CBC,
                                     text.toUtf8(), m_hashKey, m_hashIV);
    fout.write(data);
    fout.close();
    return true;
}

QString Backend::randomString()
{
    QString ret;
    for (int i = 0; i < 10; ++i) {
        int n = qrand() % 62;
        char ch;
        if (n < 10) {
            ch = char('0' + n);
        } else if (n < 36) {
            ch = char('a' + (n - 10));
        } else {
            ch = char('A' + (n - 36));
        }
        ret.append(ch);
    }
    return ret;
}

bool Backend::createFile()
{
    QFile fout(m_file);
    if (!fout.open(QIODevice::WriteOnly)) {
        emit error(tr("Fail to create file"));
        return false;
    }
    fout.close();
    return true;
}

bool Backend::removeFileIfExists(const QString &fileName)
{
    QDir dir;
    if (dir.exists(fileName)) {
        if (!dir.remove(fileName)) {
            emit error(tr("Fail to remove file"));
            return false;
        }
    }
    return true;
}

bool Backend::readFile()
{
    QFile file(m_file);
    if (!file.open(QIODevice::ReadOnly)) {
        emit error(tr("Fail to read file"));
        return false;
    }

    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_4_0);
    int tmp;

    m_recordsModel->beginResetAll();
    m_records.clear();
    while (!in.atEnd()) {
        in >> tmp;
        m_records.append(tmp);
    }
    m_recordsModel->endResetAll();

    file.close();
    return true;
}

void Backend::clear()
{
    m_file = "";
    m_recordsModel->beginResetAll();
    m_records.clear();
    m_recordsModel->endResetAll();
    m_statisticsModel->beginResetAll();
    m_statistics.clear();
    m_statisticsModel->endResetAll();
    m_isRunning = false;
    m_today = m_total = m_average = m_ndays = 0;
    emit fileChanged();
    emit isRunningChanged();
    emit countChanged();
    emit todayChanged();
    emit totalChanged();
    emit averageChanged();
}

QString Backend::username() const
{
    return m_username;
}

void Backend::setUsername(const QString &username)
{
    if (m_username != username) {
        m_username = username;
        m_hashIV = QCryptographicHash::hash(username.toUtf8(), QCryptographicHash::Md5);
        emit usernameChanged();
    }
}

QString Backend::file() const
{
    return m_file;
}

void Backend::setFile(const QString &file)
{
    if (m_file != file) {
        m_file = file;
        emit fileChanged();
    }
}
