#ifndef HOGOSCMANAGER_H
#define HOGOSCMANAGER_H

#include "osc/OSCMessage.h"
#include "utils.h"

#include <QObject>
#include <QTimer>

// forward declaration to prevent dependency loop
class MainController;


/**
 * @brief The HogOSCManagerConstants namespace contains all constants used in HogOSCManager.
 */
namespace HogOSCManagerConstants {
    /**
     * @brief connectionTimeout the time until a timout in the connection occures in ms
     */
    static const int connectionTimeout = 1500;  // in ms
}


/**
 * @brief The HogOSCManager class manages the connection to the Hog console with OSC messages.
 */
class HogOSCManager : public QObject {

    Q_OBJECT

    Q_PROPERTY(bool connected READ isConnected NOTIFY isConnectedChanged)

public:
    /**
     * @brief HogOSCManager creates an HogOSCManager object
     * @param controller a pointer to the MainController
     */
    explicit HogOSCManager(MainController* controller);

signals:
    void isConnectedChanged();

public slots:
    /**
     * @brief onIncomingMessage check if an incoming OSC message is an Hog OSC message
     * @param msg the OSC message
     */
    void onIncomingMessage(const OSCMessage& msg);

    /**
     * @brief onConnectionChanged requests initial information when a connection was established
     */
    void onConnectionChanged();

    bool isConnected() const { return m_isConnected; }

private slots:

    void onConnectionTimeout();

protected:
    MainController* const m_controller;  //!< a pointer to the MainController

    bool m_isConnected;
    QTimer m_connectionTimeout;
};

#endif // HOGOSCMANAGER_H
