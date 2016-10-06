#ifndef EOSCUE_H
#define EOSCUE_H

#include "eos_specific/EosOSCMessage.h"

#include <QObject>


// forward declaration to prevent dependency loop
class MainController;


/**
 * @brief The EosCueNumber struct describes a unique cue number including its list and part.
 */
struct EosCueNumber {

public:
    /**
     * @brief EosCueNumber creates a dummy cue number (0/0.00)
     */
    EosCueNumber();
    /**
     * @brief EosCueNumber copies another cue number
     * @param other cue number to copy
     */
    EosCueNumber(const EosCueNumber& other);
    /**
     * @brief EosCueNumber creates an EosCueNumber object from strings
     * @param list number of the cue
     * @param number of the cue
     * @param part number of the cue
     */
    EosCueNumber(QString list, QString number, QString part);
    /**
     * @brief EosCueNumber creates an EosCueNumber object from numbers
     * @param list number of the cue
     * @param number of the cue
     * @param part number of the cue
     */
    EosCueNumber(int list, double number, int part);

    bool operator<(const EosCueNumber& rhs) const;
    bool operator ==(const EosCueNumber& rhs) const;
    bool operator !=(const EosCueNumber& rhs) const { return !(*this == rhs);}

    /**
     * @brief toString formates cue number as a string
     * @return a string represeting the cue number
     */
    QString toString() const;

    int list;  //!< list number of the cue
    QString number;  //!< number of the cue as a string
    int numberAsInt;  //!< number of the cue * 100 as an integer
    int part;  //!< part number of the cue
};

/**
 * @brief qHash generate a hash from a cue to support QHashMap
 * @param key the cue number
 * @return a hash value
 */
uint qHash(const EosCueNumber &key);


// ---------------------------- EosCue ----------------------------------


/**
 * @brief The EosCue class represent a Cue of an Eos console and stores its information.
 *
 * The cue number can change at runtime.
 */
class EosCue : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString cueNumber READ getCueNumber NOTIFY dataChanged)
    Q_PROPERTY(int cuePart READ getCuePart NOTIFY dataChanged)
    Q_PROPERTY(QString label READ getLabel WRITE setLabel NOTIFY dataChanged)
    Q_PROPERTY(double upTimeDuration READ getUpTimeDuration WRITE setUpTimeDuration NOTIFY dataChanged)
    Q_PROPERTY(double downTimeDuration READ getDownTimeDuration WRITE setDownTimeDuration NOTIFY dataChanged)
    Q_PROPERTY(bool isActive READ getIsActive NOTIFY activeStatusChanged)
    Q_PROPERTY(bool isPending READ getIsPending NOTIFY activeStatusChanged)
    Q_PROPERTY(QString notes READ getNotes WRITE setNotes NOTIFY dataChanged)

public:
    /**
     * @brief EosCue creates a cue object from an cue OSC message
     * @param controller a pointer to the MainController
     * @param msg the OSC message with cue information
     */
    EosCue(MainController* controller, EosOSCMessage msg);

    /**
     * @brief update updates the information with a new OSC message
     * @param msg an OSC message with cue information
     */
    void update(const EosOSCMessage& msg);

signals:
    /**
     * @brief deleted is emitted when this cue was deleted
     * @param cueNumber cue number of this cue when it was deleted
     */
    void deleted(const EosCueNumber& cueNumber);

    /**
     * @brief dataChanged emitted when the data in this cue changed
     */
    void dataChanged();
    /**
     * @brief activeStatusChanged emitted when the active / pending state of this cue changed
     */
    void activeStatusChanged();

public slots:
    /**
     * @brief isValid returns if this cue is in a valid state, if not it should be ignored
     * @return true if it is valid
     */
    bool isValid() const { return m_isValid; }

    /**
     * @brief fire sends an OSC command to fire this cue
     */
    void fire();
    /**
     * @brief deleteCue sends an OSC command to delete this cue
     */
    void deleteCue();

    /**
     * @brief createCueBlock creates the corresponding Eos Cue Block on the workspace
     */
    void createCueBlock();

    /**
     * @brief getLabel returns a label for this cue
     * @return a string label
     */
    QString getLabel() const { return m_label; }
    /**
     * @brief setLabel sets the label of this cue by the user and notifies the console
     * @param value the new label
     */
    void setLabel(QString value);

    /**
     * @brief getCueNumber returns the cue number of this cue
     * @return a cue number
     */
    QString getCueNumber() const;
    /**
     * @brief setCueNumberByString sets the cue number by a string from the user
     * and notifies the console
     * @param value the new cue number as a string
     */
    void setCueNumberByString(QString value);

    /**
     * @brief getCuePart returns the part that this cue is
     * @return a part number
     */
    int getCuePart() const { return m_cueNumber.part; }

    /**
     * @brief getUpTimeDuration returns the up time duration
     * @return a duration in seconds
     */
    double getUpTimeDuration() const { return m_upTimeDuration / 1000.0; }
    /**
     * @brief setUpTimeDuration sets the up time duration
     * @param value up time duration in seconds
     */
    void setUpTimeDuration(double value);

    /**
     * @brief getDownTimeDuration returns the down time duration
     * @return a duration in seconds
     */
    double getDownTimeDuration() const { return m_downTimeDuration / 1000.0; }
    /**
     * @brief setDownTimeDuration sets the down time duration
     * @param value down time duration in seconds
     */
    void setDownTimeDuration(double value);

    /**
     * @brief getIsActive returns if this cue is currently active
     * @return true if active
     */
    bool getIsActive() const;
    /**
     * @brief getIsPending returns if this cue is currently pending
     * @return true if pending
     */
    bool getIsPending() const;

    /**
     * @brief getNotes returns the cue notes
     * @return string of cue notes
     */
    QString getNotes() const { return m_notes; }
    /**
     * @brief setNotes sets the cue notes by the user and notifies the console
     * @param value new notes string
     */
    void setNotes(QString value);

protected:
    /**
     * @brief secToEosDuration returns a duration in seconds as an Eos time string
     * @param totalSecs duration in seconds
     * @return an Eos time string
     */
    static QString secToEosDuration(double totalSecs);

    MainController* const m_controller;  //!< pointer to MainController

    bool m_isValid;  //!< true if this cue is valid

    EosCueNumber m_cueNumber;  //!< cue number of this cue

    int m_index;  //!< see Eos manual
    QString m_uid;  //!<  see Eos manual

    QString m_label;  //!< see Eos manual
    int m_upTimeDuration;  //!< see Eos manual
    int m_upTimeDelay;  //!< see Eos manual
    int m_downTimeDuration;  //!< see Eos manual
    int m_downTimeDelay;  //!< see Eos manual
    int m_focusTimeDuration;  //!< see Eos manual
    int m_focusTimeDelay;  //!< see Eos manual
    int m_colorTimeDuration;  //!< see Eos manual
    int m_colorTimeDelay;  //!< see Eos manual
    int m_beamTimeDuration;  //!< see Eos manual
    int m_beamTimeDelay;  //!< see Eos manual
    bool m_preheat;  //!< see Eos manual
    int m_curve;  //!< see Eos manual, type OSC number
    int m_rate;  //!< see Eos manual
    QString m_mark;  //!< see Eos manual
    QString m_block;  //!< see Eos manual
    QString m_assert;  //!< see Eos manual
    int m_link;  //!< see Eos manual, type OSC number
    QString m_linkToOtherList;  //!< see Eos manual
    int m_followTime;  //!< see Eos manual
    int m_hangTime;  //!< see Eos manual
    bool m_allFade;  //!< see Eos manual
    int m_loop;  //!< see Eos manual
    bool m_solo;  //!< see Eos manual
    QString m_timecode;  //!< see Eos manual
    int m_numberOfParts;  //!< see Eos manual
    QString m_scene;  //!< see Eos manual
    QString m_notes;  //!< see Eos manual

};

#endif // EOSCUE_H
