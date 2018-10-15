#ifndef EOSACTIVECHANNELSMANAGER_H
#define EOSACTIVECHANNELSMANAGER_H

#include "eos_specific/EosOSCMessage.h"
#include "utils.h"

#include <QObject>
#include <QPointer>
#include <QMap>
#include <QVector>

// forward declaration to prevent dependency loop
class MainController;


/**
 * @brief The EosActiveChannelsManager class stores information about which channel is active
 * and their parameters (wheels).
 */
class EosActiveChannelsManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString activeChannelsLabel READ getActiveChannelsLabel NOTIFY activeChannelsLabelChanged)
    Q_PROPERTY(QStringList wheelLabels READ getWheelLabels NOTIFY wheelsChanged)
    Q_PROPERTY(QList<int> wheelValues READ getWheelValues NOTIFY wheelsChanged)
    Q_PROPERTY(int wheelCount READ getWheelCount NOTIFY wheelsChanged)

public:
    /**
     * @brief EosActiveChannelsManager creates an EosActiveChannelsManager object
     * @param controller apointer to the MainController
     */
    explicit EosActiveChannelsManager(MainController* controller);

signals:
    /**
     * @brief activeChannelsLabelChanged is emitted when the active channels changed
     */
    void activeChannelsLabelChanged();
    /**
     * @brief wheelsChanged is emitted when a parameter (wheel) changed
     */
    void wheelsChanged();

public slots:
    /**
     * @brief onConnectionEstablished performs intialization actions when a connection to the Eos
     * was established
     */
    void onConnectionEstablished();
    /**
     * @brief onIncomingEosMessage checks if an incoming OSC message contains information for this class
     * @param msg the OSC message
     */
    void onIncomingEosMessage(const EosOSCMessage& msg);

    /**
     * @brief startChangeParameter start changing a parameter over time
     * @param index of the parameter (wheel)
     * @param tickValue value to increase the property periodically (i.e. 1 or -1)
     */
    void startChangeParameter(int index, double tickValue);
    /**
     * @brief stopChangeParameter stop changing a parameter
     * @param index of the parameter (wheel)
     */
    void stopChangeParameter(int index);

    /**
     * @brief getActiveChannelsLabel returns a string describing which channels are active / selected
     * @return a string description label
     */
    QString getActiveChannelsLabel() const { return m_activeChannelsLabel; }
    /**
     * @brief setActiveChannelsLabel sets the label describing which channels are active / selected
     * @param value a string description label
     */
    void setActiveChannelsLabel(QString value) { m_activeChannelsLabel = value; emit activeChannelsLabelChanged(); }

    /**
     * @brief getWheelLabels returns a list of parameter labels
     * @return a list of strings
     */
    QStringList getWheelLabels() const { return m_wheelLabels.toList(); }
    /**
     * @brief getWheelValues returns a list of parameter values
     * @return a list of values
     */
    QList<int> getWheelValues() const { return m_wheelValues.toList(); }
    /**
     * @brief getWheelCount returns the amount of parameters available
     * @return count of parameters (wheels)
     */
    int getWheelCount() const { return m_wheelLabels.size(); }

protected:
    MainController* const m_controller;  //!< pointer to the MainController

    QString m_activeChannelsLabel;  //!< label describing which channels are active / selected

    QVector<QString> m_wheelLabels;  //!< list of parameter labels
    QVector<int> m_wheelValues;  //!< list of parameter values
};

#endif // EOSACTIVECHANNELSMANAGER_H
