#ifndef BASICTRANSFORMBLOCK_H
#define BASICTRANSFORMBLOCK_H

#include "core/block_data/InOutBlock.h"
#include "core/SmartAttribute.h"
#include "utils.h"


class BasicTransformBlock : public InOutBlock
{
    Q_OBJECT

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Basic Transform";
        info.nameInUi = "Rotate / Mirror";
        info.category << "General" << "Line / Matrix";
        info.helpText = "Applies rotation and mirror transformations to the input matrix.";
        info.qmlFile = "qrc:/qml/Blocks/Logic/BasicTransformBlock.qml";
        info.complete<BasicTransformBlock>();
        return info;
    }

    explicit BasicTransformBlock(MainController* controller, QString uid);

signals:

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

private slots:
    void update();
    void updateRequestedSize();

protected:
    IntegerAttribute m_rotate;
    BoolAttribute m_mirrorV;
    BoolAttribute m_mirrorH;

};

#endif // BASICTRANSFORMBLOCK_H
