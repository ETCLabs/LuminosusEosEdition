#include "EosOSCManager.h"

#include "core/MainController.h"
#include <QUuid>


EosOSCManager::EosOSCManager(MainController* controller)
    : QObject(controller)
    , m_controller(controller)
    , m_oscUserId(-1)
    , m_showTitle("")
    , m_cmdText()
    , m_live(true)
    , m_activeCuePercentComplete(0.0)
    , m_latency(-1)
    , m_instanceId(QUuid::createUuid().toString())
    , m_consoleVersion("")
    , m_faderBankCount(0)
    , m_timeouts(0)
    , m_discoveryClient(this)
{
    connect(m_controller->lightingConsole(), SIGNAL(isConnectedChanged()),
            this, SLOT(onConnectionChanged()));
    connect(m_controller->lightingConsole(), SIGNAL(messageReceived(OSCMessage)),
            this, SLOT(onIncomingMessage(OSCMessage)));

    m_startTime = HighResTime::now();
    m_latencyTimeout.setSingleShot(true);
    m_latencyTimeout.setInterval(EosOSCManagerConstants::latencyTimeout);
    connect(&m_latencyTimeout, SIGNAL(timeout()), this, SLOT(onLatencyTimeout()));
    connect(&m_latencyCheckTimer, SIGNAL(timeout()), this, SLOT(updateLatency()));
    m_latencyCheckTimer.start(EosOSCManagerConstants::latencyCheckInterval);
}

void EosOSCManager::OSCDiscoveryClientClient_Found(const OSCDiscoveryClient::sDiscoveryServer& server) {
    for (QJsonValueRef ref: m_discoveredConsoles) {
        QJsonObject console = ref.toObject();
        if (console["ip"] == server.hostAddress.toString()) return;
    }
    QJsonObject obj;
    obj["name"] = server.name;
    obj["ip"] = server.hostAddress.toString();
    obj["port"] = server.port;
    m_discoveredConsoles.append(obj);
    emit discoveredConsolesChanged();
}

void EosOSCManager::onIncomingMessage(const OSCMessage& msg) {
    if (m_controller->lightingConsole()->getCurrentType() != OscConnectionType::Eos) return;
    // all Eos message start with /eos/out/ except fader feedback messages:
    if (!msg.pathStartsWith("/eos/out/") && !msg.pathStartsWith("/eos/fader/")) {
        // this is not an Eos message, ignore it:
        return;
    }
    //msg.printToQDebug();

    // check if there is an incomplete message (i.e. from list convention):
    if (msg.pathPart(-3) == "list" && m_incompleteMessage.isValid()
            && m_incompleteMessage.canConvert<EosOSCMessage>()) {
        // yes, there is an incomplete message and this message could be a continuation of it
        EosOSCMessage incompleteMsg = m_incompleteMessage.value<EosOSCMessage>();
        bool success = incompleteMsg.add(msg);
        if (success) {
            // messages were joined -> check if message is complete or exit:
            if (incompleteMsg.isComplete()) {
                m_incompleteMessage.clear();
                onIncomingEosMessage(incompleteMsg);
            }
            return;
        } else {
            // the two messages could not be joined, discard incomplete message:
            m_incompleteMessage.clear();
        }
    }

    // create a new message:
    EosOSCMessage eosMsg(msg);
    if (eosMsg.isComplete()) {
        onIncomingEosMessage(eosMsg);
    } else {
        m_incompleteMessage = QVariant::fromValue<EosOSCMessage>(eosMsg);
    }
}

void EosOSCManager::onIncomingEosMessage(const EosOSCMessage& msg) {
    if (m_latencyTimeout.isActive()) {
        // if latencyTimout is running
        // reset it with each incoming Eos message
        // because the connection can't timeout while messages are received:
        m_latencyTimeout.start();
    }
    if (msg.path().isEmpty()) return;
    if (msg.pathPart(0) == "user") {
        setOscUserId(int(msg.numericValue()));
    } else if (msg.pathPart(0) == "cmd") {
        if (msg.userIdProvided()) {
            setCmdText(msg.userId(), msg.stringValue());
        }
    } else if (msg.pathPart(0) == "show" && msg.pathPart(1) == "name") {
        setShowTitle(msg.stringValue());
    } else if (msg.pathPart(2) == "loaded" && msg.pathPart(1) == "show" && msg.pathPart(0) == "event") {
        m_controller->lightingConsole()->sendMessage("/eos/reset");
        m_controller->lightingConsole()->sendMessage("/eos/subscribe=1");
        m_controller->lightingConsole()->sendMessage("/eos/get/version");
        emit connectionReset();
    } else if (msg.pathPart(0) == "event" && msg.pathPart(1) == "state") {
        setLive(msg.numericValue() != 0.0);
    } else if (msg.pathPart(0) == "ping") {
        if (msg.arguments().size() == 2) {
            if (msg.arguments()[0] != m_instanceId) {
                // this is a ping message meant for a different instance
                return;
            }
            onPingReceived(msg.arguments()[1].toInt());
        }
    } else if (msg.pathPart(0) == "get" && msg.pathPart(1) == "version") {
        setConsoleVersion(msg.stringValue());
    } else if (msg.pathPart(0) == "active" && msg.pathPart(1) == "cue") {
        if (msg.pathPart(2) == "text") {
            // this message contains a descriptive text for the active cue:
            m_activeCueDescription = msg.stringValue();
            emit cueInfoChanged();
        } else if (msg.path().size() == 2) {
            // this message contains only "percent complete" information during a fade:
            m_activeCuePercentComplete = msg.numericValue();
            emit cueInfoChanged();
        } else if (msg.path().size() == 3) {
            // this message is sent when there is no active cue:
            //m_activeCueNumber = EosCueNumber();
            m_activeCueDescription = "";
            m_activeCuePercentComplete = 0.0;
            emit cueInfoChanged();
        } else if (msg.path().size() >= 4) {
            // normal active cue message:
            // /eos/out/active/cue/<list>/<number>[/<part]=<percent complete [0...1]>
            EosCueNumber activeCue(msg.pathPart(2), msg.pathPart(3), msg.pathPart(4));
            m_activeCueNumbers[activeCue.list] = activeCue;
            m_activeCuePercentComplete = msg.numericValue();
            emit cueInfoChanged();
        }
    } else if (msg.pathPart(0) == "pending" && msg.pathPart(1) == "cue") {
        if (msg.pathPart(2) == "text") {
            // this message contains a descriptive text for the active cue:
            m_pendingCueDescription = msg.stringValue();
            emit cueInfoChanged();
        } else if (msg.path().size() == 3) {
            //this message is sent when there is no pending cue:
            //m_pendingCueNumber = EosCueNumber();
            m_pendingCueDescription = "";
            emit cueInfoChanged();
        } else if (msg.path().size() >= 4) {
            // normal pending cue message:
            // /eos/out/pending/cue/<list>/<number>[/<part]
            EosCueNumber pendingCue(msg.pathPart(2), msg.pathPart(3), msg.pathPart(4));
            m_pendingCueNumbers[pendingCue.list] = pendingCue;
            emit cueInfoChanged();
        }
    } else {
        // emit signal, let specific Blocks handle this message:
        emit eosMessageReceived(msg);
    }
}

void EosOSCManager::onConnectionChanged() {
    if (!m_controller->lightingConsole()->isConnected()
            || m_controller->lightingConsole()->getCurrentType() != OscConnectionType::Eos) {
        return;
    }
    // new connection was established, send initial messsages:
    updateLatency();
    // sending reset results in information sent twice?
    //m_controller->eosConnection()->sendMessage("/eos/reset");
    m_controller->lightingConsole()->sendMessage("/eos/subscribe=1");
    m_controller->lightingConsole()->sendMessage("/eos/get/version");
    emit connectionEstablished();
}

void EosOSCManager::updateLatency() {
    // check if TCP socket is connected:
    if (m_controller->lightingConsole()->isConnected()
            && m_controller->lightingConsole()->getCurrentType() == OscConnectionType::Eos) {
        // it is, check latency:
        // calculate current time stamp (milliseconds since program start):
        // (possible overflow with int32 at around 600h runtime)
        long timestamp = long(HighResTime::elapsedSecSince(m_startTime) * 1000);
        // send ping with current time stamp:
        m_controller->lightingConsole()->sendMessage("/eos/ping", m_instanceId, QString::number(timestamp));
        // start timeout timer:
        m_latencyTimeout.start();
    } else {
        // it is not, reset connection infos:
        setLatency(-1);
        setConsoleVersion("");
    }
}

void EosOSCManager::onPingReceived(int timestamp) {
    // cancel latency timeout:
    m_latencyTimeout.stop();
    m_timeouts = 0;
    // calculate current time stamp (milliseconds since program start):
    long now = long(HighResTime::elapsedSecSince(m_startTime) * 1000);
    // update latency value:
    setLatency(int(now - timestamp));
}

void EosOSCManager::onLatencyTimeout() {
    setLatency(-1);
    setConsoleVersion("");
    ++m_timeouts;
    if (m_timeouts > 1) {
        qWarning() << "Eos Connection timed out.";
    }
    if (m_timeouts < 3 && m_controller->lightingConsole()->getCurrentType() == OscConnectionType::Eos) {
        m_controller->lightingConsole()->reconnect();
    }
}

void EosOSCManager::setCmdText(int userId, QString value) {
    if (value.endsWith("#")) {
        value.remove(value.size() - 1, 1);
        value.append("◆");
    }
    m_cmdText[userId] = value; emit cmdTextChanged();
}

QString EosOSCManager::getCueInfo() const {
    QString cueInfo;
    QStringList activeInfo = m_activeCueDescription.split(" ");
    QStringList pendingInfo = m_pendingCueDescription.split(" ");
    if (activeInfo.size() >= 1) {
        cueInfo.append(activeInfo[0]);
    }
    if (pendingInfo.size() >= 1 && !pendingInfo.first().isEmpty()) {
        cueInfo.append(" -> ");
        cueInfo.append(pendingInfo[0]);
    }
    return cueInfo;
}

EosCueNumber EosOSCManager::getActiveCueNumber(int list) const {
    return m_activeCueNumbers.value(list);
}

EosCueNumber EosOSCManager::getPendingCueNumber(int list) const {
    return m_pendingCueNumbers.value(list);
}

bool EosOSCManager::cueIsActive(const EosCueNumber& cueNumber) const {
    if (m_activeCueNumbers.contains(cueNumber.list)) {
        return m_activeCueNumbers[cueNumber.list] == cueNumber;
    } else {
        return false;
    }
}

bool EosOSCManager::cueIsPending(const EosCueNumber& cueNumber) const {
    if (m_pendingCueNumbers.contains(cueNumber.list)) {
        return m_pendingCueNumbers[cueNumber.list] == cueNumber;
    } else {
        return false;
    }
}

int EosOSCManager::getNewFaderBankNumber() {
    ++m_faderBankCount;
    return m_faderBankCount;
}

void EosOSCManager::startDiscovery() {
    m_discoveryClient.Initialize(*this, "Luminosus");
}

void EosOSCManager::stopDiscovery() {
    m_discoveryClient.Shutdown();
}
