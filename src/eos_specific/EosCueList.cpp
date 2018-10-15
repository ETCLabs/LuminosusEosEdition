#include "EosCueList.h"

#include "core/MainController.h"

#include <QDebug>


EosCueList::EosCueList(MainController* controller)
    : QObject(0)
    , m_controller(controller)
    , m_isValid(false)
    , m_index(-1)
    , m_independent(false)
    , m_htp(false)
    , m_assert(false)
    , m_block(false)
    , m_background(false)
    , m_soloMode(false)
    , m_timecodeList(-1)
    , m_oosSync(false)
{

}

EosCueList::EosCueList(MainController* controller, const EosOSCMessage& msg)
    : QObject(0)
    , m_controller(controller)
    , m_isValid(false)
    , m_index(-1)
    , m_independent(false)
    , m_htp(false)
    , m_assert(false)
    , m_block(false)
    , m_background(false)
    , m_soloMode(false)
    , m_timecodeList(-1)
    , m_oosSync(false)
{
    m_cueList = msg.pathPart(2);

    m_cuesChangedSignalDelay.setSingleShot(true);
    m_cuesChangedSignalDelay.setInterval(500);
    connect(&m_cuesChangedSignalDelay, SIGNAL(timeout()), this, SIGNAL(cuesChanged()));
    connect(controller->eosManager(), SIGNAL(eosMessageReceived(EosOSCMessage)),
            this, SLOT(onIncomingEosMessage(EosOSCMessage)));

    update(msg);

    QString message = "/eos/get/cue/" + m_cueList + "/count";
    m_controller->lightingConsole()->sendMessage(message);
}

void EosCueList::update(const EosOSCMessage& msg) {
    // check if message is valid:
    if (msg.pathPart(0) != "get" || msg.pathPart(1) != "cuelist") {
        qWarning() << "Tried to construct an EosCueList object from a non-cuelist OSC message.";
        return;
    }
    if (msg.pathPart(2) != m_cueList) {
        qWarning() << "OSC CueList message list number doesn't match with this object.";
        return;
    }
    if (msg.arguments().isEmpty()) {
        // no arguments are sent when this cuelist doesn't exist,
        // or in this case if it existed before when it was deleted
        qDebug() << "This CueList has been deleted.";
        emit deleted(m_cueList.toInt());
        deleteLater();
        return;
    }
    if (msg.arguments().size() != 13) {
        qWarning() << "OSC Cuelist message has invalid number of arguments: " << msg.arguments().size();
        return;
    }

    // fill attributes:
    const QVector<QVariant>& args = msg.arguments();
    m_index = args[0].toInt();
    m_uid = args[1].toString();
    m_label = args[2].toString();
    m_playbackMode = args[3].toString();
    m_faderMode = args[4].toString();
    m_independent = args[5].toBool();
    m_htp = args[6].toBool();
    m_assert = args[7].toBool();
    m_block = args[8].toBool();
    m_background = args[9].toBool();
    m_soloMode = args[10].toBool();
    m_timecodeList = args[11].toInt();
    m_oosSync = args[12].toBool();

    m_isValid = true;
    emit dataChanged();
}

void EosCueList::onIncomingEosMessage(const EosOSCMessage& msg) {
    if (msg.pathPart(1) != "cue" || msg.pathPart(2) != m_cueList) {
        return;
    }

    if (msg.pathPart(0) == "get") {
        if (msg.pathPart(3) == "count") {
            // this message contains the number of existing cues in this cuelist
            int cueCount = msg.numericValue();
            // request details for each cue:
            for (int i=0; i<cueCount; ++i) {
                QString message = "/eos/get/cue/" + m_cueList + "/index/" + QString::number(i);
                m_controller->lightingConsole()->sendMessage(message);
            }
        } else if (msg.path().size() <= 5) {
            // this message contains detailed information about a cue
            EosCueNumber cueNumber = EosCueNumber(msg.pathPart(2), msg.pathPart(3), msg.pathPart(4));
            if (m_cues.contains(cueNumber)) {
                if (!m_cues[cueNumber]) return;
                m_cues[cueNumber]->update(msg);
            } else if (!msg.arguments().isEmpty()) {
                EosCue* newCue = new EosCue(m_controller, msg);
                connect(newCue, SIGNAL(deleted(EosCueNumber)), this, SLOT(deleteCue(EosCueNumber)));
                m_cues[cueNumber] = newCue;
            }
            m_cuesChangedSignalDelay.start();
        }
    } else if (msg.pathPart(0) == "notify") {
        // this message contains a list of changed cues as arguments
        // ignore first argument, get details for the other:
        for (int i=1; i<msg.arguments().size(); ++i) {
            if (msg.arguments().at(i).type() == QVariant::Int) {
                QString changedCue = QString::number(msg.arguments().at(i).toInt());
                onNotifyCueChanged(changedCue);
            } else {
                QStringList changedCues = msg.arguments().at(i).toString().split("-");
                if (changedCues.size() == 1) {
                    onNotifyCueChanged(changedCues[0]);
                } else if (changedCues.size() == 2) {
                    int begin = changedCues[0].toInt();
                    int end = changedCues[1].toInt();
                    for (int i=begin; i<=end; ++i) {
                        onNotifyCueChanged(QString::number(i));
                    }
                } else {
                    qWarning() << "Received unknown argument in cue changed notify message: "
                               << msg.arguments().at(i).toString();
                }
            }
        }
    }
}

void EosCueList::deleteCue(const EosCueNumber& cueNumber) {
    if (m_cues.contains(cueNumber)) {
        m_cues.remove(cueNumber);
    }
}

QStringList EosCueList::getCueNumbers() const {
    QStringList numbers;
    for (const EosCueNumber& cueNumber: m_cues.keys()) {
        numbers.append(cueNumber.toString());
    }
    return numbers;
}

QList<QObject*> EosCueList::getCues() const {
    QList<QObject*> cues;
    // as QMap is already sorted -> this result is sorted too
    for (const QPointer<EosCue>& cue: m_cues.values()) {
        cues.append(cue);
    }
    return cues;
}

EosCue* EosCueList::getCue(const EosCueNumber& cueNumber) const {
    return m_cues.value(cueNumber, nullptr);
}

double EosCueList::getActiveCueIndex() const {
    EosCueNumber activeCue = m_controller->eosManager()->getActiveCueNumber(m_cueList.toInt());
    int i = 0;
    for (const EosCueNumber& cueNumber: m_cues.keys()) {
        if (cueNumber == activeCue) {
            // found active cue:
            return i;
        }
        ++i;
    }
    // active cue could not be found, try pending cue:
    EosCueNumber pendingCue = m_controller->eosManager()->getPendingCueNumber(m_cueList.toInt());
    i = 0;
    for (const EosCueNumber& cueNumber: m_cues.keys()) {
        if (cueNumber == pendingCue) {
            // found pending cue:
            return i;
        }
        ++i;
    }
    return 0;
}

void EosCueList::onNotifyCueChanged(QString changedCue) {
    QString message = "/eos/get/cue/" + m_cueList + "/" + changedCue;
    m_controller->lightingConsole()->sendMessage(message);
    // this message could also mean a part of that cue
    // -> request details for all parts:
    for (const EosCueNumber& cueNumber: m_cues.keys()) {
        if (cueNumber.number == changedCue && cueNumber.part != 0) {
            QString part = QString::number(cueNumber.part);
            QString message = "/eos/get/cue/" + m_cueList + "/" + changedCue + "/" + part;
            m_controller->lightingConsole()->sendMessage(message);
        }
    }
}
