#ifndef UPDATEMANAGER_H
#define UPDATEMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>


// Forward declaration to reduce dependencies
class MainController;

/**
 * @brief The UpdateManagerConstants namespace contains all constants used in UpdateManager
 */
namespace UpdateManagerConstants {

    /**
     * @brief restApiAddress is the URL to the REST service
     */
    const QString restApiAddress = "http://www.luminosus.org/api";

    /**
     * @brief stableVersionNumberPath is the REST path to the stable version number
     */
    const QString stableVersionNumberPath = "/stable_version_number/";
    /**
     * @brief unstableVersionNumberPath is the REST path to the stable version number
     */
    const QString unstableVersionNumberPath = "/unstable_version_number/";
    /**
     * @brief startupReportPath is the REST path to report the startup to
     */
    const QString startupReportPath = "/startup_report/startup_report.php";

    /**
     * @brief versionNumberPrefix is the prefix before a version number in the REST interface
     */
    const QString versionNumberPrefix = "version ";
}


/**
 * @brief The UpdateManager class checks if an update is available by connecting to a server.
 */
class UpdateManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool updateIsAvailable READ getUpdateIsAvailable NOTIFY newestStableVersionNumberChanged)
    Q_PROPERTY(QString newestStableVersionNumber READ getNewestStableVersionNumber NOTIFY newestStableVersionNumberChanged)

public:

    /**
     * @brief UpdateManager creates an UpdateManager object
     * @param controller a pointer to the MainController
     */
    explicit UpdateManager(MainController* controller);

    /**
     * @brief getState returns the current state to persist it
     * @return state as Json object
     */
    QJsonObject getState() const;
    /**
     * @brief setState restores a saved state
     * @param state Json object
     */
    void setState(QJsonObject state);

signals:
    /**
     * @brief newestStableVersionNumberChanged is emitted when the stable version number changed
     */
    void newestStableVersionNumberChanged();

public slots:
    /**
     * @brief reportStartup reports the startup to a server for debugging purposes
     */
    void reportStartup();

    /**
     * @brief checkForNewVersion checks if a new version is available
     */
    void checkForNewVersion();

    /**
     * @brief onVersionCheckReply handles the reply from the version check
     */
    void onVersionCheckReply();

    /**
     * @brief requestError handles a request error
     * @param error that occured
     */
    void requestError(QNetworkReply::NetworkError error);

    /**
     * @brief sslError handles an SSL error
     * @param errorList list of SSL errors that occured
     */
    void sslError(QList<QSslError> errorList);

    /**
     * @brief getUpdateIsAvailable returns if an update is available
     * @return true if an update is available
     */
    bool getUpdateIsAvailable() const;

    /**
     * @brief getNewestStableVersionNumber returns the version number of the newest stable version
     * @return a version number string
     */
    QString getNewestStableVersionNumber() const { return m_newestStableVersion; }

private:
    /**
     * @brief setNewestStableVersionNumber sets the newest stable version number based on a
     * reply from the server
     * @param value the newest stable version number string
     */
    void setNewestStableVersionNumber(QString value);

    /**
     * @brief getTimeZoneOffset returns the time zone offset from UTC
     * @return time zone offset in hours
     */
    static int getTimeZoneOffset();

protected:
    /**
     * @brief m_controller pointer to MainController instance
     */
    MainController* const m_controller;

    /**
     * @brief m_uid is the UID of this installation
     */
    QString m_uid;

    /**
     * @brief m_networkManager is the QNetworkAccessManager object used for HTTP requests
     */
    QNetworkAccessManager m_networkManager;

    /**
     * @brief m_versionCheckReply a pointer to the reply from the server
     */
    QNetworkReply* m_versionCheckReply;

    /**
     * @brief m_updateIsAvailable true if an update is available
     */
    bool m_updateIsAvailable;

    /**
     * @brief m_newestStableVersion empty string or newest stable version number string
     */
    QString m_newestStableVersion;

    bool m_internetConnectionDisabled;
};

#endif // UPDATEMANAGER_H
