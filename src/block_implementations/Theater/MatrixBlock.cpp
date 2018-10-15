#include "MatrixBlock.h"

#include "core/MainController.h"
#include "core/Nodes.h"
#include "qtquick_items/CustomImagePainter.h"


MatrixBlock::MatrixBlock(MainController *controller, QString uid)
    : FixtureBlock(controller, uid, /*footprint*/ 6)
    , m_matrixWidth(this, "matrixWidth", 1, 1, 2000)
    , m_matrixHeight(this, "matrixHeight", 1, 1, 2000)
{
    m_address = m_controller->output()->getUnusedAddress(m_footprint);

    //connect signals and slots:
    connect(m_inputNode, SIGNAL(dataChanged()), this, SLOT(onInputChanged()));
    connect(&m_matrixWidth, SIGNAL(valueChanged()), this, SLOT(updateMatrixSize()));
    connect(&m_matrixHeight, SIGNAL(valueChanged()), this, SLOT(updateMatrixSize()));
}

void MatrixBlock::onInputChanged() {
    emit matrixImageChanged();
}

void MatrixBlock::update() {

}

QImage MatrixBlock::getMatrixImage() {
    return CustomImagePainter::toQImage(m_inputNode->constData().getRgb());
}

void MatrixBlock::updateMatrixSize() {
    m_inputNode->setRequestedSize(Size(m_matrixWidth, m_matrixHeight));
}
