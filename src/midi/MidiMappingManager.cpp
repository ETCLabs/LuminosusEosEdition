#include "MidiMappingManager.h"

#include "core/MainController.h"
#include "midi/MidiManager.h"


MidiMappingManager::MidiMappingManager(MainController* controller)
    : QObject(controller)
    , m_controller(controller)
    , m_midi(controller->midi())
    , m_controlThatWaitsForMapping("")
    , m_isWaitingForControl(false)
    , m_connectFeedback(false)
    , m_releaseNextControl(false)
    , m_feedbackEnabled(true)
{
    if (!m_midi) {
        qCritical() << "Could not get MidiManager instance.";
        return;
    }

    connect(m_midi, SIGNAL(messageReceived(MidiEvent)), this, SLOT(onExternalEvent(MidiEvent)));
}

QJsonObject MidiMappingManager::getState() const {
    QJsonObject state;
    state["midiToControl"] = serialize(m_midiToControlMapping);
    state["controlToFeedback"] = serialize(m_controlToFeedbackMapping);
    state["feedbackEnabled"] = getFeedbackEnabled();
    return state;
}

void MidiMappingManager::setState(const QJsonObject& state) {
    m_midiToControlMapping = deserialize<QMap<QString, QVector<QString>>>(state["midiToControl"].toString());
    m_controlToFeedbackMapping = deserialize<QMap<QString, QVector<QString>>>(state["controlToFeedback"].toString());
    if (!m_controlToFeedbackMapping.isEmpty()) {
        setFeedbackEnabled(state["feedbackEnabled"].toBool());
    }
}

// ------------- interface that is accessable from GUI: -------------

void MidiMappingManager::startMappingMidiToControl() {
    m_isWaitingForControl = true;
    m_connectFeedback = false;
}

void MidiMappingManager::startMappingMidiToControlWithFeedback(){
    m_isWaitingForControl = true;
    m_connectFeedback = true;
}

void MidiMappingManager::cancelMappingMidiToControl() {
    m_midi->clearNextEventCallbacks();
    m_controlThatWaitsForMapping = "";
    m_isWaitingForControl = false;
}

void MidiMappingManager::releaseNextControlMapping() {
    m_releaseNextControl = true;
}

void MidiMappingManager::cancelReleaseNextControlMapping() {
    m_releaseNextControl = false;
}

void MidiMappingManager::releaseNextMidiEventMapping() {
    m_midi->registerForNextEvent("releaseConnectionToControls",
                    [this](MidiEvent event) { this->releaseMapping(event); });
}

void MidiMappingManager::cancelReleaseNextMidiEventMapping() {
    m_midi->clearNextEventCallbacks();
}

// ----------------- interface to other classes: -----------------

void MidiMappingManager::registerGuiControl(QQuickItem* item, QString controlUid) {
    if (!item) return;
    if (controlUid.isEmpty()) return;
    m_registeredControls[controlUid] = item;
}

void MidiMappingManager::unregisterGuiControl(QString controlUid) {
    if (controlUid.isEmpty()) return;

    // if unregister happens during a loading process, the mapping should not be removed
    // because it could be that there is a block in the old project with the same UID as
    // a block in a (cloned) new project and the old block is deleted(later) when the
    // new mapping was already set
    // not releasing the mapping while loading a project is not problematic
    // as the new mapping is set in this process anyway and the old mapping is discarded
    // CHANGE: mapping is never released, because it is unknown if the block is deleted by the user
    // or if just the GUI item was destroyed because it was invisible
//    if (!m_controller->projectManager()->m_loadingIsInProgress) {
//        // only release mapping if no project is loading
//        releaseMapping(controlUid);
//    }
    m_registeredControls.remove(controlUid);
}

QQuickItem* MidiMappingManager::getControlFromUid(QString controlUid) const {
    if (!m_registeredControls.contains(controlUid)) return nullptr;
    return m_registeredControls[controlUid];
}

void MidiMappingManager::guiControlHasBeenTouched(QString controllerUid) {
    if (m_releaseNextControl) {
        releaseMapping(controllerUid);
        m_releaseNextControl = false;
    }
    if (!m_isWaitingForControl) return;  // <- most likely (in case of normal user input)

    m_isWaitingForControl = false;
    m_controlThatWaitsForMapping = controllerUid;

    m_midi->registerForNextEvent("inputConnection",
                    [this](MidiEvent event) { this->mapWaitingControlToMidi(event); });
}

void MidiMappingManager::sendFeedback(QString uid, double value) const {
    if (!m_feedbackEnabled) return;
    if (m_controlToFeedbackMapping.contains(uid)) {
        for (QString feedbackAddress: m_controlToFeedbackMapping[uid]) {
            m_midi->sendFeedback(feedbackAddress, value);
        }
    }
}

void MidiMappingManager::clearMapping() {
    m_midiToControlMapping.clear();
    m_controlToFeedbackMapping.clear();
}

// ---------------------- private -------------------------

void MidiMappingManager::mapWaitingControlToMidi(const MidiEvent& event) {
    if (m_controlThatWaitsForMapping.isEmpty()) return;
    mapControlToMidi(m_controlThatWaitsForMapping, event);
    m_controlThatWaitsForMapping = "";
}

void MidiMappingManager::mapControlToMidi(QString controlUid, const MidiEvent& event) {
    // append control uid to list if not already existing:
    if (!m_midiToControlMapping[event.inputId].contains(controlUid)) {
        m_midiToControlMapping[event.inputId].append(controlUid);
    }
    if (m_connectFeedback) {
        const QString feedbackAddress = m_midi->getFeedbackAddress(event.type, event.channel, event.target);
        if (!m_controlToFeedbackMapping[controlUid].contains(feedbackAddress)) {
            m_controlToFeedbackMapping[controlUid].append(feedbackAddress);
        }
    }
}

void MidiMappingManager::releaseMapping(QString controlUid) {
    for (auto& controlList: m_midiToControlMapping) {
        controlList.removeAll(controlUid);
    }
    m_controlToFeedbackMapping.remove(controlUid);
}

void MidiMappingManager::releaseMapping(const MidiEvent& event) {
    m_midiToControlMapping.remove(event.inputId);
    const QString feedbackAddress = m_midi->getFeedbackAddress(event.type, event.channel, event.target);
    for (auto& feedbackAddressList: m_controlToFeedbackMapping) {
        feedbackAddressList.removeAll(feedbackAddress);
    }
}

void MidiMappingManager::onExternalEvent(const MidiEvent& event) const {
    // set "externalInput" property on controls that are mapped to this input:
    if (m_midiToControlMapping.contains(event.inputId)) {
        for (QString controlUid: m_midiToControlMapping[event.inputId]) {
            QQuickItem* control = getControlFromUid(controlUid);
            // check if control still exists:
            if (!control) continue;
            control->setProperty("externalInput", event.value);
        }
    }
}
