#ifndef LINEITEM_H
#define LINEITEM_H

#include <QtQuick/QQuickItem>
#include <QVector2D>
#include <QPointer>


class LineItem : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(float lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged)
    Q_PROPERTY(double x1 MEMBER m_x1 NOTIFY coordsChanged)
    Q_PROPERTY(double y1 MEMBER m_y1 NOTIFY coordsChanged)
    Q_PROPERTY(double x2 MEMBER m_x2 NOTIFY coordsChanged)
    Q_PROPERTY(double y2 MEMBER m_y2 NOTIFY coordsChanged)

public:
    explicit LineItem(QQuickItem* parent = 0);
    ~LineItem();

    QSGNode* updatePaintNode(QSGNode *, UpdatePaintNodeData *);

public slots:

    QColor color() const { return m_color; }
    float lineWidth() const { return m_lineWidth; }

    void setColor(const QColor& color);
    void setLineWidth(float width);

signals:
    void colorChanged(const QColor &color);
    void lineWidthChanged(float width);
    void coordsChanged();

private:

    static QPointF calculateBezierPoint(double t, const QPointF& p0, const QPointF& p1, const QPointF& p2, const QPointF& p3);

    static QPointF calculateBezierTangent(double t, const QPointF& p0, const QPointF& p1, const QPointF& p2, const QPointF& p3);

    static QPointF normalFromTangent(const QPointF& tangent);

    QColor      m_color;
    float       m_lineWidth;
    double m_x1;
    double m_y1;
    double m_x2;
    double m_y2;
    const float m_device_pixel_ratio;
};

#endif // LINEITEM_H
