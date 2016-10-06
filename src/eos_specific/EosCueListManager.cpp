#include "EosCueListManager.h"

#include "MainController.h"

#include <QTimer>


EosCueListManager::EosCueListManager(MainController* controller)
    : QObject(controller)
    , m_controller(controller)
    , m_dummyCueList(controller)
{
    qmlRegisterType<EosCueList>();
    qmlRegisterType<EosCue>();

    connect(controller->eosManager(), SIGNAL(connectionEstablished()),
            this, SLOT(onConnectionEstablished()));
    connect(controller->eosManager(), SIGNAL(eosMessageReceived(EosOSCMessage)),
            this, SLOT(onIncomingEosMessage(EosOSCMessage)));
    connect(controller->eosManager(), SIGNAL(connectionReset()),
            this, SLOT(onConnectionReset()));
}

void EosCueListManager::onConnectionEstablished() {
    QTimer::singleShot(500, this, SLOT(requestCueListCount()));
}

void EosCueListManager::onIncomingEosMessage(const EosOSCMessage& msg) {
    if (msg.pathPart(1) != "cuelist") return;

    if (msg.pathPart(0) == "get") {
        if (msg.pathPart(2) == "count") {
            // this message contains the number of existing cuelists
            // reset existing data:
            clear();
            int cueListCount = msg.numericValue();
            // request details for each cuelists:
            for (int i=0; i<cueListCount; ++i) {
                QString message = "/eos/get/cuelist/index/" + QString::number(i);
                m_controller->eosConnection()->sendMessage(message);
            }
        } else if (msg.pathPart(3) == "links") {
            // this message contains information about linked cue lists
            // TODO
        } else {
            // this message contains detailed information about a cuelist
            int cueListNumber = msg.pathPart(2).toInt();
            if (m_cueLists.contains(cueListNumber)) {
                m_cueLists[cueListNumber]->update(msg);
            } else {
                EosCueList* newCueList = new EosCueList(m_controller, msg);
                connect(newCueList, SIGNAL(deleted(int)), this, SLOT(deleteCueList(int)));
                m_cueLists[cueListNumber] = newCueList;
            }
            emit cueListsChanged();
        }
    } else if (msg.pathPart(0) == "notify") {
        // this message contains a list of changed cue lists as arguments
        // ignore first argument, get details for the other:
        for (int i=1; i<msg.arguments().size(); ++i) {
            int changedCueList = msg.arguments().at(i).toInt();
            QString message = "/eos/get/cuelist/" + QString::number(changedCueList);
            m_controller->eosConnection()->sendMessage(message);
        }
    }
}

void EosCueListManager::deleteCueList(int cueList) {
    if (m_cueLists.contains(cueList)) {
        m_cueLists.remove(cueList);
    }
}

QList<int> EosCueListManager::getCueListNumbers() const {
    return m_cueLists.keys();
}

EosCueList* EosCueListManager::getCueList(int cueListNumber) const {
    EosCueList* cueList = m_cueLists.value(cueListNumber, &m_dummyCueList);
    QQmlEngine::setObjectOwnership(cueList, QQmlEngine::CppOwnership);
    return cueList;
}

void EosCueListManager::requestCueListCount() {
    QString message = "/eos/get/cuelist/count";
    m_controller->eosConnection()->sendMessage(message);
}

void EosCueListManager::clear() {
    for (EosCueList* cueList: QList<QPointer<EosCueList>>(m_cueLists.values())) {
        delete cueList;
    }
    m_cueLists.clear();
    emit cueListsChanged();
}

void EosCueListManager::onConnectionReset() {
    QTimer::singleShot(500, this, SLOT(requestCueListCount()));
}
