#ifndef EOSCUELISTMANAGER_H
#define EOSCUELISTMANAGER_H

#include "eos_specific/EosOSCMessage.h"
#include "eos_specific/EosCueList.h"

#include <QObject>
#include <QPointer>
#include <QMap>

// forward declaration to prevent dependency loop
class MainController;


/**
 * @brief The EosCueListManager class manages all Eos Cue Lists.
 */
class EosCueListManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief EosCueListManager creates an EosCueListManager object
     * @param controller a pointer to the MainController
     */
    explicit EosCueListManager(MainController* controller);

signals:
    /**
     * @brief cueListsChanged is emitted when a Cue List changed
     */
    void cueListsChanged();

public slots:
    /**
     * @brief onConnectionEstablished requests cue list information when an OSC connection was
     * established
     */
    void onConnectionEstablished();
    /**
     * @brief onIncomingEosMessage checks if an incoming OSC message contains information for this class
     * @param msg an OSC message from an Eos console
     */
    void onIncomingEosMessage(const EosOSCMessage& msg);

    /**
     * @brief deleteCueList deletes a cue list in this manager
     * @param cueList number of cue list to delete
     */
    void deleteCueList(int cueList);

    /**
     * @brief getCueListNumbers returns a list of all cue list numbers
     * @return a list of cue list numbers
     */
    QList<int> getCueListNumbers() const;

    /**
     * @brief getCueList returns a specific cue list object
     * @param cueListNumber number of the cue list
     * @return a pointer to the corresponding cue list object or nullptr if it doesn't exist
     */
    EosCueList* getCueList(int cueListNumber) const;

private slots:
    /**
     * @brief requestCueListCount request the count of cue lists from the console
     */
    void requestCueListCount();
    /**
     * @brief clear deletes all cue lists in this manager
     */
    void clear();
    /**
     * @brief onConnectionReset clears cue lists and requests new information on connection reset
     */
    void onConnectionReset();

protected:
    QPointer<MainController> const m_controller;  //!< a pointer to the MainController

    QMap<int, QPointer<EosCueList>> m_cueLists;  //!< map of cue list number and object

    mutable EosCueList m_dummyCueList;  //!< a dummy cue list to show if no cue lists available
};

#endif // EOSCUELISTMANAGER_H
