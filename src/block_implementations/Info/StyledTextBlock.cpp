#include "StyledTextBlock.h"


StyledTextBlock::StyledTextBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid)
    , m_text("")
    , m_fontSize(18)
    , m_fontFamily("Quicksand")
    , m_shadow(false)
    , m_color("white")
    , m_shadowColor("#555")
    , m_bold(false)
    , m_centered(false)
    , m_underline(false)
{

}

void StyledTextBlock::getAdditionalState(QJsonObject& state) const {
    state["text"] = getText();
    state["fontSize"] = getFontSize();
    state["fontFamily"] = getFontFamily();
    state["shadow"] = getShadow();
    state["color"] = getColor().name();
    state["shadowColor"] = getShadowColor().name();
    state["bold"] = getBold();
    state["centered"] = getCentered();
    state["underline"] = getUnderline();
}

void StyledTextBlock::setAdditionalState(const QJsonObject &state) {
    setText(state["text"].toString());
    setFontSize(state["fontSize"].toDouble());
    setFontFamily(state["fontFamily"].toString());
    setShadow(state["shadow"].toBool());
    setColor(QColor(state["color"].toString()));
    setShadowColor(QColor(state["shadowColor"].toString()));
    setBold(state["bold"].toBool());
    setCentered(state["centered"].toBool());
    setUnderline(state["underline"].toBool());
}
