#ifndef SPECTRUMITEM_H
#define SPECTRUMITEM_H

#include <QtQuick/QQuickItem>
#include <QVector2D>
#include <QPointer>


class SpectrumItem : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(float lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged)
    Q_PROPERTY(QVector<double> points READ points WRITE setPoints NOTIFY pointsChanged)

public:
    SpectrumItem(QQuickItem* parent = 0);
    ~SpectrumItem();

    QSGNode *updatePaintNode(QSGNode*, UpdatePaintNodeData*);

signals:
    void colorChanged(const QColor& color);
    void lineWidthChanged(float width);
    void pointsChanged();

public slots:
    QColor color() const { return m_color; }
    float lineWidth() const { return m_lineWidth; }
    QVector<double> points() const { return m_points; }

    void setColor(const QColor& color);
    void setLineWidth(float width);
    void setPoints(const QVector<double> value);

private:
    QColor      m_color;
    float       m_lineWidth;
    QVector<double> m_points;

    const float m_device_pixel_ratio;
};

#endif // SPECTRUMITEM_H
