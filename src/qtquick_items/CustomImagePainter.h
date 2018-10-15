#ifndef CUSTOMIMAGEPAINTER_H
#define CUSTOMIMAGEPAINTER_H

#include <QQuickPaintedItem>
#include <QImage>

#include "core/Matrix.h"


class CustomImagePainter : public QQuickPaintedItem
{

    Q_OBJECT

    Q_PROPERTY(QImage image READ getImage WRITE setImage NOTIFY imageChanged)

public:

    explicit CustomImagePainter(QQuickItem* parent = 0);

    void paint(QPainter* painter);

    static QImage toQImage(const HsvMatrix& matrix);
    static QImage toQImage(const RgbMatrix& matrix);

signals:
    void imageChanged();

public slots:
    const QImage& getImage() const { return m_image; }
    void setImage(const QImage& value);

protected:
    QImage m_image;
};

#endif // CUSTOMIMAGEPAINTER_H
