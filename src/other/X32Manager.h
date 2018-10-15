#ifndef X32MANAGER_H
#define X32MANAGER_H

#include "osc/OSCMessage.h"

#include <QObject>
#include <QTimer>


// forward declaration to prevent dependency loop
class MainController;


/**
 * @brief The X32ManagerConstants namespace contains all constants used in X32Manager.
 */
namespace X32ManagerConstants {

    static const int infoTimeout = 15 * 1000;  // in ms

    static const int requestConsoleInfoInterval = 10 * 1000;  // in ms
}


/**
 * @brief The X32Manager class manages the connection to the X32 console with OSC messages.
 */
class X32Manager : public QObject
{

    Q_OBJECT

    Q_PROPERTY(QString consoleInfoName READ getConsoleInfoName NOTIFY consoleInfoChanged)
    Q_PROPERTY(QString consoleInfoModel READ getConsoleInfoModel NOTIFY consoleInfoChanged)
    Q_PROPERTY(QString consoleInfoVersion READ getConsoleInfoVersion NOTIFY consoleInfoChanged)
    Q_PROPERTY(bool isConnected READ getIsConnected NOTIFY isConnectedChanged)

public:
    /**
     * @brief X32Manager creates an X32Manager object
     * @param controller a pointer to the MainController
     */
    explicit X32Manager(MainController* controller);

    ~X32Manager();


signals:
    void consoleInfoChanged();
    void isConnectedChanged();

public slots:

    void onIncomingMessage(const OSCMessage& msg);

    void requestConsoleInfo();

    void onInfoTimeout();

    // -------------------- Getter + Setter ----------------------

    QString getConsoleInfoName() const { return m_consoleInfoName; }
    QString getConsoleInfoModel() const { return m_consoleInfoModel; }
    QString getConsoleInfoVersion() const { return m_consoleInfoVersion; }

    bool getIsConnected() const { return m_isConnected; }


protected:
    MainController* const m_controller;  //!< a pointer to the MainController

    QString m_consoleInfoName;
    QString m_consoleInfoModel;
    QString m_consoleInfoVersion;

    bool m_isConnected;

    QTimer m_infoTimeout;

    QTimer m_requestConsoleInfoTimer;
};

#endif // X32MANAGER_H
