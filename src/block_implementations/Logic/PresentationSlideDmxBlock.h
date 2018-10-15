#ifndef PRESENTATIONSLIDEDMXBLOCK_H
#define PRESENTATIONSLIDEDMXBLOCK_H


#include "core/block_data/OneInputBlock.h"
#include "core/SmartAttribute.h"


class PresentationSlideDmxBlock : public OneInputBlock {

    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Presentation Slide DMX Control";
        info.nameInUi = "DMX -> Slide Number";
        info.category << "General" << "Media + Presentation";
        info.visibilityRequirements << VisibilityRequirement::KeyboardEmulation;
        info.helpText = "Interprets the incoming DMX value (0-255) as a slide number and goes to "
                        "that slide.<br>"
                        "Example: if the input is the DMX value 35 (internally represented as "
                        "35/255), slide 35 will be shown.<br>"
                        "Useful in combination with the sACN In Block to trigger slides with a "
                        "virtual fixture from the lighting console.<br><br>"
                        "Works by emulating the press of the page number and the enter key on the "
                        "keyboard. This means that it only works while <b>PowerPoint is in the "
                        "foreground!</b><br><br>"
                        "Also works with LibreOffice Impress and other programs that support "
                        "this keyboard combination.<br><br>"
                        "The 'PowerPoint Remote Block' can be used to go to the next or "
                        "previous slide and to show black and white slides.<br><br>"
                        "Only works on Windows.";
        info.qmlFile = "qrc:/qml/Blocks/Logic/PresentationSlideDmxBlock.qml";
        info.complete<PresentationSlideDmxBlock>();
        return info;
    }

    explicit PresentationSlideDmxBlock(MainController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void onInputChanged();

    void goToSlide(int number);

private:
    void pressNextKey(QVector<quint32> scanCodes, int index);

    int m_lastSlide;
    bool m_sequenceRunning;
};

#endif // PRESENTATIONSLIDEDMXBLOCK_H
