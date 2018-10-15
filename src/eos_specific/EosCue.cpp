#include "EosCue.h"

#include "core/MainController.h"
#include "block_implementations/Eos/EosCueBlock.h"

#include <QDebug>


EosCueNumber::EosCueNumber()
    : list(0)
    , number("0")
    , numberAsInt(0)
    , part(0)
{

}

EosCueNumber::EosCueNumber(const EosCueNumber& other)
    : list(other.list)
    , number(other.number)
    , numberAsInt(other.numberAsInt)
    , part(other.part)
{

}

EosCueNumber::EosCueNumber(QString list, QString number, QString part) {
    this->list = list.toInt();
    this->number = number;
    this->numberAsInt = qRound(number.toDouble() * 100);
    this->part = part.toInt();
}

EosCueNumber::EosCueNumber(int list, double number, int part) {
    this->list = list;
    this->number = QString::number(number, 'f', 2);
    this->numberAsInt = qRound(number * 100);
    this->part = part;
}

bool EosCueNumber::operator<(const EosCueNumber& rhs) const {
    return (numberAsInt < rhs.numberAsInt)
            || ((numberAsInt == rhs.numberAsInt) && part < rhs.part);
}

bool EosCueNumber::operator ==(const EosCueNumber& rhs) const {
    return (list == rhs.list)
            && (numberAsInt == rhs.numberAsInt)
            && (part == rhs.part);

}

QString EosCueNumber::toString() const {
    return number + "/" + QString::number(part);
}

uint qHash(const EosCueNumber& key) {
    return key.list * 1000000 * 100 + key.numberAsInt * 100 + key.part;
}


// ---------------------------- EosCue ----------------------------------


EosCue::EosCue(MainController* controller, EosOSCMessage msg)
    : QObject(0)
    , m_controller(controller)
    , m_isValid(false)
{
    connect(m_controller->eosManager(), SIGNAL(cueInfoChanged()),
            this, SIGNAL(activeStatusChanged()));

    m_cueNumber = EosCueNumber(msg.pathPart(2), msg.pathPart(3), msg.pathPart(4));

    update(msg);
}

void EosCue::update(const EosOSCMessage& msg)
{
    // check if message is valid:
    if (msg.pathPart(0) != "get" || msg.pathPart(1) != "cue") {
        qWarning() << "Tried to construct an EosCue object from a non-cue OSC message.";
        return;
    }
    if (m_cueNumber != EosCueNumber(msg.pathPart(2), msg.pathPart(3), msg.pathPart(4))) {
        qWarning() << "OSC Cue message list, number or part doesn't match with this object.";
        return;
    }
    if (msg.arguments().isEmpty()) {
        // no arguments are sent when this cue doesn't exist,
        // or in this case if it existed before when it was deleted
        qDebug() << "This Cue has been deleted.";
        emit deleted(m_cueNumber);
        deleteLater();
        return;
    }
    if (msg.arguments().size() < 27) {
        qWarning() << "OSC Cue message has invalid number of arguments: " << msg.arguments().size();
        return;
    }

    // fill attributes:
    const QVector<QVariant>& args = msg.arguments();
    m_index = args[0].toInt();
    m_uid = args[1].toString();
    m_label = args[2].toString().trimmed();
    m_upTimeDuration = args[3].toInt();
    m_upTimeDelay = args[4].toInt();
    m_downTimeDuration = args[5].toInt();
    m_downTimeDelay = args[6].toInt();
    m_focusTimeDuration = args[7].toInt();
    m_focusTimeDelay = args[8].toInt();
    m_colorTimeDuration = args[9].toInt();
    m_colorTimeDelay = args[10].toInt();
    m_beamTimeDuration = args[11].toInt();
    m_beamTimeDelay = args[12].toInt();
    m_preheat = args[13].toBool();
    m_curve = args[14].toInt();  // type OSC number? could be string if range?
    m_rate = args[15].toInt();
    m_mark = args[16].toString();
    m_block = args[17].toString();
    m_assert = args[18].toString();
    if (args[19].type() == QVariant::String) {
        m_linkToOtherList = args[19].toString();
    } else {
        m_link = args[19].toInt();  // type OSC number? could be string if range?
    }
    m_followTime = args[20].toInt();
    m_hangTime = args[21].toInt();
    m_allFade = args[22].toBool();
    m_loop = args[23].toInt();
    m_solo = args[24].toBool();
    m_timecode = args[25].toString();

    if (msg.arguments().size() >= 30) {
        // this message is from Eos 2.4 and contains additional infos:
        m_numberOfParts = args[26].toInt();
        m_notes = args[27].toString();
        m_scene = args[28].toString();
    } else {
        // this message is from Eos 2.3, without scenes or notes:
        m_numberOfParts = -1;
        m_notes = "";
        m_scene = "";
    }

    m_isValid = true;
    emit dataChanged();
}

void EosCue::fire() {
    QString message = "/eos/cue/%1/%2/%3/fire";
    message = message.arg(QString::number(m_cueNumber.list), m_cueNumber.number, QString::number(m_cueNumber.part));
    m_controller->lightingConsole()->sendMessage(message);
}

void EosCue::deleteCue() {
    QString list = QString::number(m_cueNumber.list);
    QString number;
    if (m_cueNumber.part == 0) {
        number = list + "/" + m_cueNumber.number;
    } else {
        number = list + "/" + m_cueNumber.number + " Part " + QString::number(m_cueNumber.part);
    }

    QString message = "/eos/newcmd=Delete Cue %1##";
    message = message.arg(number);
    m_controller->lightingConsole()->sendMessage(message);
}

void EosCue::createCueBlock() {
    EosCueBlock* block = qobject_cast<EosCueBlock*>(m_controller->blockManager()->addNewBlock("Eos Cue"));
    if (!block) {
        qWarning() << "Could not create Cue Block.";
        return;
    }
    block->setCueNumber(m_cueNumber);
    block->focus();
}

void EosCue::setLabel(QString value) {
    // change local:
    m_label = value;
    emit dataChanged();

    // change in console:
    QString message = "/eos/set/cue/%1/%2/%3/label";
    message = message.arg(QString::number(m_cueNumber.list), m_cueNumber.number, QString::number(m_cueNumber.part));
    m_controller->lightingConsole()->sendMessage(message, value);
}

QString EosCue::getCueNumber() const {
    if (m_cueNumber.part == 0) {
        return m_cueNumber.number;
    } else {
        return m_cueNumber.number + " P" + QString::number(m_cueNumber.part);
    }
}

void EosCue::setCueNumberByString(QString value) {
    QString list = QString::number(m_cueNumber.list);
    QString oldNumber;
    QString newNumber;

    if (m_cueNumber.part == 0) {
        oldNumber = list + "/" + m_cueNumber.number;
    } else {
        oldNumber = list + "/" + m_cueNumber.number + " Part " + QString::number(m_cueNumber.part);
    }

    QStringList valueSplitted = value.toLower().split(" p");
    if (valueSplitted.size() == 1) {
        newNumber = list + "/" + valueSplitted[0];
    } else {
        newNumber = list + "/" + valueSplitted[0] + " Part " + valueSplitted[1];
    }
    QString msg = "/eos/newcmd=Cue %1 Move_To Cue %2##";
    msg = msg.arg(oldNumber, newNumber);
    m_controller->lightingConsole()->sendMessage(msg);
}

void EosCue::setUpTimeDuration(double value) {
    if (value < 0 && m_upTimeDuration < 0) return;
    if (qRound(value * 1000) == m_upTimeDuration) return;
    // change local:
    m_upTimeDuration = int(value * 1000);
    emit dataChanged();

    // change in console:
    QString list = QString::number(m_cueNumber.list);
    QString cueString;

    if (m_cueNumber.part == 0) {
        cueString = list + "/" + m_cueNumber.number;
    } else {
        cueString = list + "/" + m_cueNumber.number + " Part " + QString::number(m_cueNumber.part);
    }

    if (value < 0) {
        // reset time to default value:
        QString msg = "/eos/newcmd=Cue %1 Time#";
        msg = msg.arg(cueString);
        m_controller->lightingConsole()->sendMessage(msg);
    } else {
        // set cue time:
        QString msg = "/eos/newcmd=Cue %1 Time %2#";
        msg = msg.arg(cueString, secToEosDuration(value));
        m_controller->lightingConsole()->sendMessage(msg);
    }
}

void EosCue::setDownTimeDuration(double value) {
    if (value < 0 && m_downTimeDuration < 0) return;
    if (qRound(value * 1000) == m_downTimeDuration) return;
    // change local:
    m_downTimeDuration = int(value * 1000);
    emit dataChanged();

    // change in console:
    QString list = QString::number(m_cueNumber.list);
    QString cueString;

    if (m_cueNumber.part == 0) {
        cueString = list + "/" + m_cueNumber.number;
    } else {
        cueString = list + "/" + m_cueNumber.number + " Part " + QString::number(m_cueNumber.part);
    }

    if (value < 0) {
        // reset time to default value:
        QString msg = "/eos/newcmd=Cue %1 Down Time#";
        msg = msg.arg(cueString);
        m_controller->lightingConsole()->sendMessage(msg);
    } else {
        // set cue time:
        QString msg = "/eos/newcmd=Cue %1 Down Time %2#";
        msg = msg.arg(cueString, QString::number(value, 'f', 3));
        m_controller->lightingConsole()->sendMessage(msg);
    }
}

QString EosCue::secToEosDuration(double totalSecs) {
    int secs = fmod(totalSecs, 60);
    int mins = fmod((totalSecs / 60), 60);
    mins = qMin(mins, 59);
    QString duration = QString("%1:%2").arg(mins).arg(secs, 2, 'f', 2, '0');
    return duration;
}

bool EosCue::getIsActive() const {
    return m_controller->eosManager()->cueIsActive(m_cueNumber);
}

bool EosCue::getIsPending() const {
    return m_controller->eosManager()->cueIsPending(m_cueNumber);
}

void EosCue::setNotes(QString value) {
    // change local:
    m_notes = value;
    emit dataChanged();

    // change in console:
    QString message = "/eos/set/cue/%1/%2/%3/notes";
    message = message.arg(QString::number(m_cueNumber.list), m_cueNumber.number, QString::number(m_cueNumber.part));
    m_controller->lightingConsole()->sendMessage(message, value);
}
