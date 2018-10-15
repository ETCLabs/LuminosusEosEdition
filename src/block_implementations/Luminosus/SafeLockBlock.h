#ifndef SAFELOCKBLOCK_H
#define SAFELOCKBLOCK_H

#include "core/block_data/InOutBlock.h"
#include "core/SmartAttribute.h"


class SafeLockBlock: public InOutBlock
{
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Safe Lock";
        info.nameInUi = "Safe Lock";
        info.category << "General" << "Other";
        info.visibilityRequirements << VisibilityRequirement::DeveloperMode;
        info.helpText = "";
        info.qmlFile = "qrc:/qml/Blocks/Luminosus/SafeLockBlock.qml";
        info.complete<SafeLockBlock>();
        return info;
    }

    explicit SafeLockBlock(MainController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void show();
    void hide();

    void clear();
    void evaluate();

    virtual void onControllerRotated(double, double degrees, bool) override;

protected:
    DoubleAttribute m_angle;
    IntegerAttribute m_code1;
    IntegerAttribute m_code2;
    IntegerAttribute m_code3;
    IntegerAttribute m_try1;
    IntegerAttribute m_try2;
    IntegerAttribute m_try3;
    BoolAttribute m_valid;
    double m_direction;
    double m_otherDirection;

    QPointer<QQuickItem> m_displayedItem;
};

#endif // SAFELOCKBLOCK_H
