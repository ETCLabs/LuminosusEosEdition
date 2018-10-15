#include "ChangeProjectBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"


ChangeProjectBlock::ChangeProjectBlock(MainController* controller, QString uid)
    : OneInputBlock(controller, uid)
    , m_projectName("")
    , m_animated(true)
    , m_lastValue(0.0)
{
    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onInputChanged()));
}

void ChangeProjectBlock::getAdditionalState(QJsonObject& state) const {
    state["projectName"] = getProjectName();
    state["animated"] = getAnimated();
}

void ChangeProjectBlock::setAdditionalState(const QJsonObject &state) {
    setProjectName(state["projectName"].toString());
    setAnimated(state["animated"].toBool());
}

void ChangeProjectBlock::onInputChanged() {
    double value = m_inputNode->getValue();
    if (value >= LuminosusConstants::triggerThreshold && m_lastValue < LuminosusConstants::triggerThreshold) {
        fire();
    } else {
        m_lastValue = value;
    }
}

void ChangeProjectBlock::fire() {
    QTimer::singleShot(20, [this](){ m_controller->projectManager()->setCurrentProject(m_projectName, false, m_animated); } );
}
