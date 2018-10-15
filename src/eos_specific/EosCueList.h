#ifndef EOSCUELIST_H
#define EOSCUELIST_H

#include "eos_specific/EosOSCMessage.h"
#include "eos_specific/EosCue.h"

#include <QObject>
#include <QPointer>
#include <QMap>
#include <QTimer>


// forward declaration to prevent dependency loop
class MainController;


/**
 * @brief The EosCueList class represents a Cue List of an Eos console.
 */
class EosCueList : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString label MEMBER m_label NOTIFY dataChanged)

public:
    /**
     * @brief EosCueList used to construt an empty dummy cue list
     * @param controller pointer to MainController
     */
    explicit EosCueList(MainController* controller);
    /**
     * @brief EosCueList creates an EosCueList object from an OSC message
     * @param controller a pointer to the main controller
     * @param msg OSC message with information about this Cue List
     */
    EosCueList(MainController* controller, const EosOSCMessage& msg);

    /**
     * @brief update updates this object with new information from an OSC message
     * @param msg OSC message with information about this Cue List
     */
    void update(const EosOSCMessage& msg);

signals:
    /**
     * @brief deleted is emitted when this cue list was deleted
     * @param cueList number of this cue list
     */
    void deleted(int cueList);

    /**
     * @brief dataChanged is emitted when the data in this cue list changed
     */
    void dataChanged();
    /**
     * @brief cuesChanged is emitted when a cue in this list changed
     */
    void cuesChanged();

public slots:
    /**
     * @brief onIncomingEosMessage checks if an incoming OSC message contains information for this class
     * @param msg an OSC message from an Eos console
     */
    void onIncomingEosMessage(const EosOSCMessage& msg);

    /**
     * @brief deleteCue deletes a Cue in this list
     * @param cueNumber current number of the cue
     */
    void deleteCue(const EosCueNumber& cueNumber);

    /**
     * @brief isValid true if this Cue List is valid, if not it should be ignored
     * @return true if valid
     */
    bool isValid() const { return m_isValid; }

    /**
     * @brief getCueNumbers returns a list of all cue numbers in this cue list
     * @return a list of cue numbers
     */
    QStringList getCueNumbers() const;

    /**
     * @brief getCues returns a list cues in this cue list
     * @return a list of pointer to cue objects
     */
    QList<QObject*> getCues() const;

    /**
     * @brief getCue returns the cue object for the given cue number if it is in this list
     * @param cueNumber cue number to look for
     * @return a pointer to the cue object or nullptr if it is not in this list
     */
    EosCue* getCue(const EosCueNumber& cueNumber) const;

    /**
     * @brief getActiveCueIndex returns the index of the active cue in this list
     * @return the index or 0 if there is not active or pending cue
     */
    double getActiveCueIndex() const;

protected slots:
    /**
     * @brief onNotifyCueChanged request detailed information about a changed cue or its parts
     * @param changedCue the cue number of the cue that changed
     */
    void onNotifyCueChanged(QString changedCue);

protected:
    MainController* const m_controller;  //!< a pointer to the MainController

    bool m_isValid;  //!< true if this cue list is valid

    QString m_cueList;  //!< number of the list as a string

    QMap<EosCueNumber, QPointer<EosCue>> m_cues;  //!< map of cue numbers and cue objects in this list

    int m_index;  //!< see Eos manual
    QString m_uid;  //!< see Eos manual

    QString m_label;  //!< see Eos manual
    QString m_playbackMode;  //!< see Eos manual
    QString m_faderMode;  //!< see Eos manual
    bool m_independent;  //!< see Eos manual
    bool m_htp;  //!< see Eos manual
    bool m_assert;  //!< see Eos manual
    bool m_block;  //!< see Eos manual
    bool m_background;  //!< see Eos manual
    bool m_soloMode;  //!< see Eos manual
    int m_timecodeList;  //!< see Eos manual
    bool m_oosSync;  //!< see Eos manual

    QTimer m_cuesChangedSignalDelay;  //!< timer to delay the cues changed signal

};

#endif // EOSCUELIST_H
