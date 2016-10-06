#include "PreviewBlock.h"

#include "Nodes.h"


PreviewBlock::PreviewBlock(MainController* controller, QString uid)
    : OneInputBlock(controller, uid, info().qmlFile)
    , m_value(0.0)
{
	connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onValueChanged()));
}

void PreviewBlock::onValueChanged() {
    if (m_inputNode->getDataToRead().absoluteMaximumIsProvided()) {
        double value = m_inputNode->getAbsoluteValue();
        setValue(value);
    } else {
        double value = m_inputNode->getValue();
        setValue(value);
    }
}
