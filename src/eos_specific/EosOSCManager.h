#ifndef EOSCONSOLE_H
#define EOSCONSOLE_H

#include "eos_specific/EosOSCMessage.h"
#include "osc/OSCMessage.h"
#include "EosCue.h"
#include "OSCDiscovery.h"
#include "utils.h"

#include <QObject>
#include <QTimer>
#include <QDebug>
#include <QJsonArray>

// forward declaration to prevent dependency loop
class MainController;


/**
 * @brief The EosOSCManagerConstants namespace contains all constants used in EosOSCManager.
 */
namespace EosOSCManagerConstants {
    /**
     * @brief latencyCheckInterval the inerval to check the latency in ms
     */
    static const int latencyCheckInterval = 10 * 1000;  // in ms
    /**
     * @brief latencyTimeout the time until a timout in the connection occures in ms
     */
    static const int latencyTimeout = 1500;  // in ms
}


/**
 * @brief The EosOSCManager class manages the connection to the Eos console with OSC messages.
 */
class EosOSCManager : public QObject, OSCDiscoveryClient::Client
{

    Q_OBJECT

    Q_PROPERTY(int oscUserId READ getOscUserId WRITE setOscUserId NOTIFY oscUserIdChanged)
    Q_PROPERTY(QString showTitle READ getShowTitle WRITE setShowTitle NOTIFY showTitleChanged)
    Q_PROPERTY(bool live READ getLive WRITE setLive NOTIFY liveChanged)
    Q_PROPERTY(int latency READ getLatency NOTIFY latencyChanged)
    Q_PROPERTY(QString consoleVersion READ getConsoleVersion NOTIFY consoleVersionChanged)
    Q_PROPERTY(QString cueInfo READ getCueInfo NOTIFY cueInfoChanged)
    Q_PROPERTY(double activeCuePercentComplete MEMBER m_activeCuePercentComplete NOTIFY cueInfoChanged)
    Q_PROPERTY(bool pingIsSuccessful READ getPingIsSuccessful NOTIFY latencyChanged)
    Q_PROPERTY(QJsonArray discoveredConsoles READ getDiscoveredConsoles NOTIFY discoveredConsolesChanged)

public:
    /**
     * @brief EosOSCManager creates an EosOSCManager object
     * @param controller a pointer to the MainController
     */
    explicit EosOSCManager(MainController* controller);

    virtual void OSCDiscoveryClientClient_Log(const QString &) override {}
    virtual void OSCDiscoveryClientClient_Found(const OSCDiscoveryClient::sDiscoveryServer& server) override;

signals:
    /**
     * @brief eosMessageReceived is emitted when a valid Eos OSC message was received
     * @param msg the received message
     */
    void eosMessageReceived(EosOSCMessage msg);
    /**
     * @brief connectionEstablished is emitted when a connection was established
     */
    void connectionEstablished();
    /**
     * @brief connectionReset
     */
    void connectionReset();

    /**
     * @brief oscUserIdChanged is emitted when the Eos OSC user id changed
     */
    void oscUserIdChanged();
    /**
     * @brief showTitleChanged is emitted when the show title changed
     */
    void showTitleChanged();
    /**
     * @brief cmdTextChanged is emitted when a commandline text changed
     */
    void cmdTextChanged();
    /**
     * @brief liveChanged is emitted when live / blind state changed
     */
    void liveChanged();
    /**
     * @brief cueInfoChanged is emitted when the active / pending state of a cue changed
     */
    void cueInfoChanged();

    /**
     * @brief latencyChanged is emitted when the measured latency changed
     */
    void latencyChanged();
    /**
     * @brief consoleVersionChanged is emitted when the information about the console version changed
     */
    void consoleVersionChanged();

    void discoveredConsolesChanged();


public slots:
    /**
     * @brief onIncomingMessage check if an incoming OSC message is an Eos OSC message
     * @param msg the OSC message
     */
    void onIncomingMessage(const OSCMessage& msg);

    /**
     * @brief onIncomingEosMessage checks if an incoming Eos OSC message contains general information
     * @param msg the Eos OSC message
     */
    void onIncomingEosMessage(const EosOSCMessage& msg);

    /**
     * @brief onConnectionChanged requests initial information when a connection was established
     */
    void onConnectionChanged();

    // ------------------------- latency:
    /**
     * @brief updateLatency tries to measure the connection latency
     */
    void updateLatency();
    /**
     * @brief onPingReceived handles a connection latency ping from the console
     * @param timestamp the timestamp in the message
     */
    void onPingReceived(int timestamp);
    /**
     * @brief onLatencyTimeout handles a latency check timeout
     */
    void onLatencyTimeout();

    // ---------------- Getter + Setter ------------------

    /**
     * @brief getOscUserId returns the current Eos OSC user id
     * @return a user id
     */
    int getOscUserId() const { return m_oscUserId; }
    /**
     * @brief setOscUserId internal method to set the user id
     * @param value new user id from OSC message
     */
    void setOscUserId(int value) { m_oscUserId = value; emit oscUserIdChanged(); }

    /**
     * @brief getShowTitle returns the current show title
     * @return show title string
     */
    QString getShowTitle() const { return m_showTitle; }
    /**
     * @brief setShowTitle internal method to set the show title
     * @param valueshow title string from an OSC message
     */
    void setShowTitle(QString value) { m_showTitle = value; emit showTitleChanged(); }

    /**
     * @brief getCmdText returns the current commandline text of a specific user
     * @param userId to get the commandline of
     * @return commandline text
     */
    QString getCmdText(int userId) const { return m_cmdText.value(userId, ""); }
    /**
     * @brief setCmdText internal method to set the commandline text of a user
     * @param userId to set the commandline of
     * @param value commandline text form an OSC message
     */
    void setCmdText(int userId, QString value);

    /**
     * @brief getLive returns if in live or blind mode
     * @return true if in live mode
     */
    bool getLive() const { return m_live; }
    /**
     * @brief setLive internal method to set live or blind state
     * @param value true if in live state
     */
    void setLive(bool value) { m_live = value; emit liveChanged(); }

    /**
     * @brief getCueInfo returns the current active and pending cue
     * @return string with active cue infos
     */
    QString getCueInfo() const;

    /**
     * @brief getLatency returns the measured latency
     * @return latency in ms
     */
    int getLatency() const { return m_latency; }
    /**
     * @brief setLatency internal method to set the connection latency
     * @param value latency in ms
     */
    void setLatency(int value) { m_latency = value; emit latencyChanged(); }

    /**
     * @brief getConsoleVersion returns the console version
     * @return console version as a string
     */
    QString getConsoleVersion() const { return m_consoleVersion; }
    /**
     * @brief setConsoleVersion internal method to set the console version
     * @param value console version from an OSC message
     */
    void setConsoleVersion(QString value) { m_consoleVersion = value; emit consoleVersionChanged(); }

    /**
     * @brief getActiveCueNumber returns the active cue of a specific cue list
     * @param list number of cue list
     * @return cue number of active cue
     */
    EosCueNumber getActiveCueNumber(int list) const;
    /**
     * @brief getPendingCueNumber returns the pending cue of a specific cue list
     * @param list number of cue list
     * @return cue number of pending cue
     */
    EosCueNumber getPendingCueNumber(int list) const;

    /**
     * @brief cueIsActive returns if a specific cue is active
     * @param cueNumber cue number to check
     * @return true if active
     */
    bool cueIsActive(const EosCueNumber& cueNumber) const;
    /**
     * @brief cueIsPending returns if a specific cue is pending
     * @param cueNumber cue number to check
     * @return true if pending
     */
    bool cueIsPending(const EosCueNumber& cueNumber) const;

    /**
     * @brief getPingIsSuccessful returns latency was successful measured
     * @return true if successful
     */
    bool getPingIsSuccessful() const { return m_latency > 0; }

    /**
     * @brief getNewFaderBankNumber returns a new fader bank number
     * @return a new unused fader bank number
     */
    int getNewFaderBankNumber();

    void startDiscovery();
    void stopDiscovery();
    QJsonArray getDiscoveredConsoles() const { return m_discoveredConsoles; }

protected:
    MainController* const m_controller;  //!< a pointer to the MainController

    QVariant m_incompleteMessage;  //!< incomplete message (of type EosOSCMessage)

    int m_oscUserId;  //!< Eos OSC user id
    QString m_showTitle;  //!< current show title
    QMap<int, QString> m_cmdText;  //!< map of user IDs and commandline texts
    bool m_live;  //!< true if live, false if in blind state

    QMap<int, EosCueNumber> m_activeCueNumbers;  //!< map of user IDs and active cues
    QMap<int, EosCueNumber> m_pendingCueNumbers;  //!< map of user IDs and pending cues
    QString m_activeCueDescription;  //!< active cue label
    QString m_pendingCueDescription;  //!< pending cue label
    double m_activeCuePercentComplete;  //!< percentage active cue is complete [0...1]

    QTimer m_latencyCheckTimer;  //!< timer to check latency periodically
    QTimer m_latencyTimeout;  //!< timer to detect connection timeout
    HighResTime::time_point_t m_startTime;  //!< start time used for timestamps
    int m_latency;  //!< connection latency in ms
    const QString m_instanceId;  //!< random instance id to not get confused by messages from other luminosus instances

    QString m_consoleVersion;  //!< detected console version

    int m_faderBankCount;  //!< count of created fader banks

    int m_timeouts;  //!< number of timeouts after the connection was lost

    OSCDiscoveryClient m_discoveryClient;
    QJsonArray m_discoveredConsoles;
};

#endif // EOSCONSOLE_H
