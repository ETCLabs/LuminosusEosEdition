#ifndef MOVINGPATTERNBLOCK_H
#define MOVINGPATTERNBLOCK_H

#include "core/block_data/InOutBlock.h"
#include "core/SmartAttribute.h"
#include "utils.h"

#include <QMap>


class MovingPatternBlock : public InOutBlock
{
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Moving Pattern";
        info.category << "General" << "Pattern";
        info.visibilityRequirements << VisibilityRequirement::DeveloperMode;
        info.helpText = "";
        info.qmlFile = "qrc:/qml/Blocks/Logic/MovingPatternBlock.qml";
        info.complete<MovingPatternBlock>();
        return info;
    }

    explicit MovingPatternBlock(MainController* controller, QString uid);

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

private slots:
    void update();

protected:
    DoubleAttribute m_amount;

};

#endif // MOVINGPATTERNBLOCK_H
