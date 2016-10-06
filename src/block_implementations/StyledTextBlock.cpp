#include "StyledTextBlock.h"


StyledTextBlock::StyledTextBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid, info().qmlFile)
    , m_text("")
    , m_fontSize(18)
    , m_fontFamily("Quicksand")
    , m_shadow(false)
    , m_color("white")
    , m_shadowColor("#555")
    , m_bold(false)
{

}

QJsonObject StyledTextBlock::getState() const {
    QJsonObject state;
    state["text"] = getText();
    state["fontSize"] = getFontSize();
    state["fontFamily"] = getFontFamily();
    state["shadow"] = getShadow();
    state["color"] = getColor().name();
    state["shadowColor"] = getShadowColor().name();
    state["bold"] = getBold();
    return state;
}

void StyledTextBlock::setState(const QJsonObject &state) {
    setText(state["text"].toString());
    setFontSize(state["fontSize"].toDouble());
    setFontFamily(state["fontFamily"].toString());
    setShadow(state["shadow"].toBool());
    setColor(QColor(state["color"].toString()));
    setShadowColor(QColor(state["shadowColor"].toString()));
    setBold(state["bold"].toBool());
}
