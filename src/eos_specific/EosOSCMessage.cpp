#include "EosOSCMessage.h"

#include <QDebug>

EosOSCMessage::EosOSCMessage()
    : m_userId(-1)
    , m_listConventionArgumentCount(0)
{

}

EosOSCMessage::EosOSCMessage(const OSCMessage& msg)
    : m_userId(-1)
    , m_listConventionArgumentCount(0)
{
    if (msg.path().size() < 3) {
        qWarning() << "Tried to convert invalid Eos message.";
        return;
    }
    m_path = QStringList(msg.path());
    // first element is always "eos" -> remove it:
    m_path.pop_front();
    if (m_path.first() != "fader") {
        // second element is always "out" -> remove it:
        // (except for fader feedback messages)
        m_path.pop_front();
    }
    // copy arguments:
    m_arguments = QVector<QVariant>(msg.arguments());
    // if first part is "user" and this is not the only part:
    if (m_path.first() == "user" && m_path.size() > 1) {
        // set user ID and remove these user id info:
        m_userId = m_path.at(1).toInt();
        m_path.pop_front();
        m_path.pop_front();
    }
    // check if this is an "OSC List Convention" message:
    // they end with ".../list/<index>/<count>"
    if (pathPart(-3) == "list") {
        // get total argument count:
        m_listConventionArgumentCount = pathPart(-1).toInt();
        // remove the last three path parts:
        m_path.pop_back();
        m_path.pop_back();
        m_path.pop_back();
    }
}

bool EosOSCMessage::add(const OSCMessage& msg) {
    if (msg.pathPart(-1).toInt() != m_arguments.size()) {
        qWarning() << "OSC List Convention argument index doesn't fit.";
        return false;
    }
    m_arguments.append(msg.arguments());
    return true;
}

QString EosOSCMessage::pathPart(int index) const {
    if (index < 0) {
        index = m_path.size() + index;
    }
    if (index < 0 || m_path.size() <= index) {
        return "";
    }
    return m_path[index];
}

bool EosOSCMessage::isTrue() const {
    if (m_arguments.isEmpty()) return true;
    if (m_arguments.first().type() == QVariant::Double && m_arguments.first().toDouble() > 0.99) return true;
    if (m_arguments.first().type() == QVariant::Int && m_arguments.first().toInt() == 1) return true;
    if (m_arguments.first().type() == QVariant::Bool && m_arguments.first().toBool()) return true;
    return false;
}

qreal EosOSCMessage::numericValue() const {
    if (m_arguments.isEmpty()) return 0.0;
    if (m_arguments.first().type() == QVariant::Double) return m_arguments.first().toDouble();
    if (m_arguments.first().type() == QVariant::Int) return qreal(m_arguments.first().toInt());
    return 0.0;
}

QString EosOSCMessage::stringValue() const {
    if (m_arguments.isEmpty()) return "";
    if (m_arguments.first().type() == QVariant::String) return m_arguments.first().toString();
    return "";
}
