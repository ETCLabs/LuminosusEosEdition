#ifndef STYLEDTEXTBLOCK_H
#define STYLEDTEXTBLOCK_H

#include "core/block_data/BlockBase.h"

#include <QColor>


class StyledTextBlock : public BlockBase
{
    Q_OBJECT

    Q_PROPERTY(QString text READ getText WRITE setText NOTIFY textChanged)
    Q_PROPERTY(double fontSize READ getFontSize WRITE setFontSize NOTIFY fontSizeChanged)
    Q_PROPERTY(QString fontFamily READ getFontFamily WRITE setFontFamily NOTIFY fontFamilyChanged)
    Q_PROPERTY(bool shadow READ getShadow WRITE setShadow NOTIFY shadowChanged)
    Q_PROPERTY(QColor color READ getColor WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(QColor shadowColor READ getShadowColor WRITE setShadowColor NOTIFY shadowColorChanged)
    Q_PROPERTY(bool bold READ getBold WRITE setBold NOTIFY boldChanged)
    Q_PROPERTY(bool centered READ getCentered WRITE setCentered NOTIFY centeredChanged)
    Q_PROPERTY(bool underline READ getUnderline WRITE setUnderline NOTIFY underlineChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Styled Text";
        info.nameInUi = "Styled Text";
        info.category << "General" << "Presentation";
		info.visibilityRequirements << VisibilityRequirement::StandaloneVersion;
        info.helpText = "A styled text for presentations.";
        info.qmlFile = "qrc:/qml/Blocks/Info/StyledTextBlock.qml";
        info.complete<StyledTextBlock>();
        return info;
    }

    explicit StyledTextBlock(MainController* controller, QString uid);

    virtual void getAdditionalState(QJsonObject& state) const override;
    virtual void setAdditionalState(const QJsonObject& state) override;

signals:
    void textChanged();
    void fontSizeChanged();
    void fontFamilyChanged();
    void shadowChanged();
    void colorChanged();
    void shadowColorChanged();
    void boldChanged();
    void centeredChanged();
    void underlineChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    QString getText() const { return m_text; }
    void setText(const QString& value) { m_text = value; emit textChanged(); }

    double getFontSize() const { return m_fontSize; }
    void setFontSize(double value) { m_fontSize = value; emit fontSizeChanged(); }

    QString getFontFamily() const { return m_fontFamily; }
    void setFontFamily(const QString& value) { m_fontFamily = value; emit fontFamilyChanged(); }

    bool getShadow() const { return m_shadow; }
    void setShadow(bool value) { m_shadow = value; emit shadowChanged(); }

    QColor getColor() const { return m_color; }
    void setColor(QColor value) { m_color = value; emit colorChanged(); }

    QColor getShadowColor() const { return m_shadowColor; }
    void setShadowColor(QColor value) { m_shadowColor = value; emit shadowColorChanged(); }

    bool getBold() const { return m_bold; }
    void setBold(bool value) { m_bold = value; emit boldChanged(); }

    bool getCentered() const { return m_centered; }
    void setCentered(bool value) { m_centered = value; emit centeredChanged(); }

    bool getUnderline() const { return m_underline; }
    void setUnderline(bool value) { m_underline = value; emit underlineChanged(); }

protected:
    QString m_text;
    double m_fontSize;
    QString m_fontFamily;
    bool m_shadow;
    QColor m_color;
    QColor m_shadowColor;
    bool m_bold;
    bool m_centered;
    bool m_underline;
};

#endif // STYLEDTEXTBLOCK_H
