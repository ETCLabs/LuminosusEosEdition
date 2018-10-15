#ifndef PIXELCONTROLBLOCK_H
#define PIXELCONTROLBLOCK_H

#include "core/block_data/OneOutputBlock.h"
#include "core/SmartAttribute.h"
#include "core/Matrix.h"
#include "utils.h"

#include <QImage>


class PixelControlBlock : public OneOutputBlock
{
    Q_OBJECT

    Q_PROPERTY(int matrixWidth READ getMatrixWidth NOTIFY matrixSizeChanged)
    Q_PROPERTY(int matrixHeight READ getMatrixHeight NOTIFY matrixSizeChanged)
    Q_PROPERTY(QImage image READ getImage NOTIFY matrixChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Pixel Control";
        info.category << "General" << "Controls";
        info.helpText = "Controls the brightness and color of each segment or pixel individually.\n"
                        "A 'segment' can also be a single fixture in a row of fixtures.\n\n"
                        "Click on one of the buttons ’H, S, V’ in the bottom left corner and then "
                        "click or drag on a pixel to change either Hue, Saturation or Value.\n"
                        "Click or drag on the 'A' field to adjust all pixels together.\n\n"
                        "This is a very versatile block, it can be used for example with the "
                        "sequencer to create chasers and effects or can be used used with "
                        "the Gradient Block to set the steps of a gradient.";
        info.qmlFile = "qrc:/qml/Blocks/Controls/PixelControlBlock.qml";
        info.complete<PixelControlBlock>();
        return info;
    }

    explicit PixelControlBlock(MainController* controller, QString uid);

    virtual void getAdditionalState(QJsonObject& state) const override;
    virtual void setAdditionalState(const QJsonObject& state) override;

signals:
    void matrixChanged();
    void matrixSizeChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    int getMatrixWidth() const;
    int getMatrixHeight() const;

    double getHue(int x, int y) const;
    double getSat(int x, int y) const;
    double getVal(int x, int y) const;

    void setHue(int x, int y, double value);
    void setSat(int x, int y, double value);
    void setVal(int x, int y, double value);

    void setHueAll(double value);
    void setSatAll(double value);
    void setValAll(double value);

    QImage getImage() const;

private slots:
    void updateOutput();
    void updateRequestedSize();
    void expandTo(int width, int height);

protected:
    HsvMatrix m_matrix;

    IntegerAttribute m_modificationMode;



};

#endif // PIXELCONTROLBLOCK_H
