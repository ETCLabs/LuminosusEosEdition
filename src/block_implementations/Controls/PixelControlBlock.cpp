#include "PixelControlBlock.h"

#include "core/Nodes.h"
#include "qtquick_items/CustomImagePainter.h"


PixelControlBlock::PixelControlBlock(MainController* controller, QString uid)
    : OneOutputBlock(controller, uid)
    , m_matrix(1, 1)
    , m_modificationMode(this, "modificationMode", 2, 0, 2)
{
    updateRequestedSize();
    connect(m_outputNode, SIGNAL(requestedSizeChanged()), this, SLOT(updateRequestedSize()));
}

void PixelControlBlock::getAdditionalState(QJsonObject& state) const {
    Size s = m_outputNode->getRequestedSize();
    int mWidth = m_matrix.width();
    int mHeight = m_matrix.height();
    if (mWidth < int(s.height) || mHeight < int(s.height)) {
        qWarning() << "Pixel getState: matrix too small";
        return;
    }
    state["mWidth"] = int(s.width);
    state["mHeight"] = int(s.height);
    QJsonArray arr;
    for (int x=0; x<int(s.width); ++x) {
        for (int y=0; y<int(s.height); ++y) {
            arr.append(m_matrix.at(x, y).h);
            arr.append(m_matrix.at(x, y).s);
            arr.append(m_matrix.at(x, y).v);
        }
    }
    state["matrix"] = arr;
}

void PixelControlBlock::setAdditionalState(const QJsonObject &state) {
    readAttributesFrom(state);

    int mWidth = state["mWidth"].toInt();
    int mHeight = state["mHeight"].toInt();
    if (mWidth < 1 || mHeight < 1) return;

    expandTo(mWidth, mHeight);

    QJsonArray arr = state["matrix"].toArray();
    if (arr.size() < ((mWidth-1)*mHeight*3 + (mHeight-1)*3 + 2)) {
        qWarning() << "Pixel setState: JSON array too small.";
        return;
    }
    for (int x=0; x<mWidth; ++x) {
        for (int y=0; y<mHeight; ++y) {
            m_matrix.at(x, y).h = arr.at(x*mHeight*3 + y*3).toDouble();
            m_matrix.at(x, y).s = arr.at(x*mHeight*3 + y*3 + 1).toDouble();
            m_matrix.at(x, y).v = arr.at(x*mHeight*3 + y*3 + 2).toDouble();
        }
    }
    updateOutput();

}

int PixelControlBlock::getMatrixWidth() const {
    return int(m_outputNode->getRequestedSize().width);
}

int PixelControlBlock::getMatrixHeight() const {
    return int(m_outputNode->getRequestedSize().height);
}

double PixelControlBlock::getHue(int x, int y) const {
    return m_matrix.at(x, y).h;
}

double PixelControlBlock::getSat(int x, int y) const {
    return m_matrix.at(x, y).s;
}

double PixelControlBlock::getVal(int x, int y) const {
    return m_matrix.at(x, y).v;
}

void PixelControlBlock::setHue(int x, int y, double value) {
    m_matrix.at(x, y).h = value;
    updateOutput();
    emit matrixChanged();
}

void PixelControlBlock::setSat(int x, int y, double value) {
    m_matrix.at(x, y).s = value;
    updateOutput();
    emit matrixChanged();
}

void PixelControlBlock::setVal(int x, int y, double value) {
    m_matrix.at(x, y).v = value;
    updateOutput();
    emit matrixChanged();
}

void PixelControlBlock::setHueAll(double value) {
    for (int x=0; x<m_matrix.width(); ++x) {
        for (int y=0; y<m_matrix.height(); ++y) {
            m_matrix.at(x, y).h = value;
        }
    }
    updateOutput();
    emit matrixChanged();
}

void PixelControlBlock::setSatAll(double value) {
    for (int x=0; x<m_matrix.width(); ++x) {
        for (int y=0; y<m_matrix.height(); ++y) {
            m_matrix.at(x, y).s = value;
        }
    }
    updateOutput();
    emit matrixChanged();
}

void PixelControlBlock::setValAll(double value) {
    for (int x=0; x<m_matrix.width(); ++x) {
        for (int y=0; y<m_matrix.height(); ++y) {
            m_matrix.at(x, y).v = value;
        }
    }
    updateOutput();
    emit matrixChanged();
}

QImage PixelControlBlock::getImage() const {
    return CustomImagePainter::toQImage(m_matrix);
}

void PixelControlBlock::updateOutput() {
    auto hsv = HsvDataModifier(m_outputNode);
    int mWidth = m_matrix.width();
    int mHeight = m_matrix.height();
    if (mWidth < int(hsv.height) || mHeight < int(hsv.height)) {
        qWarning() << "PixelWise: Matrix too small.";
        return;
    }

    for (int x = 0; x < hsv.width; ++x) {
        for (int y = 0; y < hsv.height; ++y) {
            hsv.set(x, y, m_matrix.at(x, y));
        }
    }
}

void PixelControlBlock::updateRequestedSize() {
    Size s = m_outputNode->getRequestedSize();
    expandTo(s.width, s.height);
    emit matrixSizeChanged();
    updateOutput();
}

void PixelControlBlock::expandTo(int width, int height) {
    m_matrix.expandTo(width, height);
}
