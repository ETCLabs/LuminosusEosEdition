#include "QmlOnlyBlock.h"

#include <QFileInfo>


QmlOnlyBlock::QmlOnlyBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid, info().qmlFile)
    , m_qmlFilePath("")
{

}

QJsonObject QmlOnlyBlock::getState() const {
    QJsonObject state;
    state["qmlFilePath"] = getQmlFilePath();
    return state;
}

void QmlOnlyBlock::setState(const QJsonObject &state) {
    setQmlFilePath(state["qmlFilePath"].toString());
}

QString QmlOnlyBlock::getFileBaseName() const {
    return QFileInfo(m_qmlFilePath).baseName();
}
