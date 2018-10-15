#include "CustomImagePainter.h"

#include <QPainter>

CustomImagePainter::CustomImagePainter(QQuickItem* parent)
    : QQuickPaintedItem(parent)
{
    // rendering in a FramebufferObject is faster than to an Image:
    setRenderTarget(QQuickPaintedItem::FramebufferObject);
}

void CustomImagePainter::paint(QPainter* painter) {
    painter->drawImage(QRect(0, 0, width(), height()), m_image);
}

QImage CustomImagePainter::toQImage(const HsvMatrix& matrix) {
    // Format_RGB32 is the best for QML without alpha,
    // Format_ARGB32_Premultiplied is the best with alpha channel
    QImage image(matrix.width(), matrix.height(), QImage::Format_RGB32);

    for (int x=0; x < matrix.width(); ++x) {
        for (int y=0; y < matrix.height(); ++y) {
            const HSV col = matrix.at(x, y);
            const QColor qc = QColor::fromHsvF(col.h, col.s, col.v);
            image.setPixel(x, y, qc.rgba());
        }
    }

    return image;
}

QImage CustomImagePainter::toQImage(const RgbMatrix& matrix) {
    // Format_RGB32 is the best for QML without alpha,
    // Format_ARGB32_Premultiplied is the best with alpha channel
    QImage image(matrix.width(), matrix.height(), QImage::Format_RGB32);

    for (int x=0; x < matrix.width(); ++x) {
        for (int y=0; y < matrix.height(); ++y) {
            const RGB col = matrix.at(x, y);
            const QRgb val = qRgb(col.r * 255, col.g * 255, col.b * 255);
            image.setPixel(x, y, val);
        }
    }

    return image;
}

void CustomImagePainter::setImage(const QImage& value) {
    m_image = value;
    emit imageChanged();

    // set the texture size to the size of the image to increase performance:
    if (textureSize() != m_image.size()) {
        setTextureSize(m_image.size());
    }
    update();
}
