#include "UpdateManager.h"

#include "MainController.h"
#include "version.h"

#include <QSysInfo>
#include <QLocale>


UpdateManager::UpdateManager(MainController* controller)
    : QObject(controller)
    , m_controller(controller)
    , m_networkManager(this)
    , m_versionCheckReply(nullptr)
{
    // disable SSL warnings:
    QLoggingCategory::setFilterRules("qt.network.ssl.warning=false");

    // report application startup after 10s to server:
    QTimer::singleShot(10*1000, this, SLOT(reportStartup()));

    // check for new version after 6s:
    QTimer::singleShot(6*1000, this, SLOT(checkForNewVersion()));
}

QJsonObject UpdateManager::getState() const {
    QJsonObject state;
    state["uid"] = m_uid;
    state["newestStableVersionNumber"] = getNewestStableVersionNumber();
    return state;
}

void UpdateManager::setState(QJsonObject state) {
    m_uid = state["uid"].toString();
    setNewestStableVersionNumber(state["newestStableVersionNumber"].toString());
}

void UpdateManager::reportStartup() {
    QNetworkRequest request;
    QString url = UpdateManagerConstants::restApiAddress + UpdateManagerConstants::startupReportPath;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", "LuminosusUpdateManager 1.0");
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");

    if (m_uid.isEmpty()) {
        // this is the first start and there is no unique ID yet
        // -> generate it:
        m_uid = QString::number(qrand());
    }

    QStringList data;
    data << m_uid;
    data << LuminosusVersionInfo::VERSION_STRING;
    data << QSysInfo::prettyProductName() + " " + QSysInfo::currentCpuArchitecture();
    data << QLocale::system().name();
    data << QString::number(getTimeZoneOffset());
    data << m_controller->projectManager()->getCurrentProjectName();
    data << QString::number(m_controller->blockManager()->getBlockInstanceCount());
    data << QString::number(m_controller->eosManager()->getLatency());
    QString dataString = data.join(";");

    m_networkManager.post(request, dataString.toLatin1());
}

void UpdateManager::checkForNewVersion() {
    QNetworkRequest request;
    QString url = UpdateManagerConstants::restApiAddress + UpdateManagerConstants::stableVersionNumberPath;
    qInfo() << "Requesting version number from:" << url;
    request.setUrl(QUrl(url));
    request.setRawHeader("User-Agent", "LuminosusUpdateManager 1.0");

    m_versionCheckReply = m_networkManager.get(request);
    connect(m_versionCheckReply, SIGNAL(readyRead()), this, SLOT(onVersionCheckReply()));
    connect(m_versionCheckReply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(requestError(QNetworkReply::NetworkError)));
    connect(m_versionCheckReply, SIGNAL(sslErrors(QList<QSslError>)),
            this, SLOT(sslError(QList<QSslError>)));
}

void UpdateManager::onVersionCheckReply() {
    if (!m_versionCheckReply) {
        qCritical() << "m_versionCheckReply is empty";
        return;
    }
    QByteArray rawMessage = m_versionCheckReply->readAll();
    QString versionString = QString::fromLatin1(rawMessage);
    if (versionString.isEmpty()) {
        qInfo() << "Received empty version string from server.";
        return;
    }
    if (!versionString.startsWith(UpdateManagerConstants::versionNumberPrefix)) {
        qInfo() << "Unknown Reply received:" << versionString;
        return;
    }
    versionString = versionString.remove(UpdateManagerConstants::versionNumberPrefix);
    versionString = versionString.remove("\n");
    setNewestStableVersionNumber(versionString);
    qInfo() << "Current Stable Version Number:" << versionString;

    if (getUpdateIsAvailable()) {
        m_controller->showToast("A newer version is available on luminosus.org:\n" + versionString);
    }
}

void UpdateManager::requestError(QNetworkReply::NetworkError error) {
    qInfo() << "Network Request Error:" << error;
}

void UpdateManager::sslError(QList<QSslError> errorList) {
    for (QSslError error: errorList) {
        qInfo() << "Network Request Error (SSL):" << error.errorString();
    }
}

bool UpdateManager::getUpdateIsAvailable() const {
    QVersionNumber newestStable = QVersionNumber::fromString(m_newestStableVersion);
    QVersionNumber thisVersion = QVersionNumber::fromString(LuminosusVersionInfo::VERSION_STRING);
    return newestStable > thisVersion;
}

void UpdateManager::setNewestStableVersionNumber(QString value) {
    m_newestStableVersion = value;
    emit newestStableVersionNumberChanged();
}

int UpdateManager::getTimeZoneOffset() {
    const QDateTime dateTime1 = QDateTime::currentDateTime();
    const QDateTime dateTime2 = QDateTime(dateTime1.date(), dateTime1.time(), Qt::UTC);
    return dateTime1.secsTo(dateTime2) / 3600;
}
