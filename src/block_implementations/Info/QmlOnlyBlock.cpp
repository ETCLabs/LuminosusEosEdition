#include "QmlOnlyBlock.h"

#include <QFileInfo>


QmlOnlyBlock::QmlOnlyBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid)
    , m_qmlFilePath("")
{

}

void QmlOnlyBlock::getAdditionalState(QJsonObject& state) const {
    state["qmlFilePath"] = getQmlFilePath();
}

void QmlOnlyBlock::setAdditionalState(const QJsonObject &state) {
    setQmlFilePath(state["qmlFilePath"].toString());
}

QString QmlOnlyBlock::getFileBaseName() const {
    return QFileInfo(m_qmlFilePath).baseName();
}
