#include "EosMasterPlaybackBlock.h"

#include "MainController.h"


EosMasterPlaybackBlock::EosMasterPlaybackBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid, info().qmlFile)
{

}

void EosMasterPlaybackBlock::sendLoadEvent(qreal buttonEdge) {
    QString message = "/eos/key/fader_ab";
    m_controller->eosConnection()->sendMessage(message, buttonEdge);
}

void EosMasterPlaybackBlock::sendUnloadEvent() {
    QString message = "/eos/key/shift";
    m_controller->eosConnection()->sendMessage(message, 1.0);
    message = "/eos/key/fader_ab";
    m_controller->eosConnection()->sendMessage(message);
    message = "/eos/key/shift";
    m_controller->eosConnection()->sendMessage(message, 0.0);
}

void EosMasterPlaybackBlock::sendStopEvent(qreal buttonEdge) {
    QString message = "/eos/key/stop";
    m_controller->eosConnection()->sendMessage(message, buttonEdge);
}

void EosMasterPlaybackBlock::sendGoEvent(qreal buttonEdge) {
    QString message = "/eos/key/go";
    m_controller->eosConnection()->sendMessage(message, buttonEdge);
}
