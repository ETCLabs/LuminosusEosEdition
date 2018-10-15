#ifndef MATRIXBLOCK_H
#define MATRIXBLOCK_H

#include "core/block_data/FixtureBlock.h"
#include "core/SmartAttribute.h"
#include "core/Matrix.h"

#include <QImage>


class MatrixBlock : public FixtureBlock
{
    Q_OBJECT

    Q_PROPERTY(QImage matrixImage READ getMatrixImage NOTIFY matrixImageChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "RGB Matrix";
        info.nameInUi = "RGB Matrix";
        info.category << "Theater";
        info.helpText = "Outputs RGB values of a matrix via rudimentary ArtNet.";
        info.visibilityRequirements << VisibilityRequirement::StandaloneVersion;
        info.qmlFile = "qrc:/qml/Blocks/Theater/MatrixBlock.qml";
        info.complete<MatrixBlock>();
        return info;
    }

    explicit MatrixBlock(MainController* controller, QString uid);

signals:
    void matrixImageChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void onInputChanged();

    void update();

    QImage getMatrixImage();

    void updateMatrixSize();

protected:
    IntegerAttribute m_matrixWidth;
    IntegerAttribute m_matrixHeight;

};

#endif // MATRIXBLOCK_H
