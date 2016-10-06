#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#include <QtQuick/QQuickItem>
#include <QVector2D>
#include <QPointer>

class BezierCurve : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QQuickItem* leftItem READ leftItem WRITE setLeftItem NOTIFY leftItemChanged)
    Q_PROPERTY(QQuickItem* rightItem READ rightItem WRITE setRightItem NOTIFY rightItemChanged)
    Q_PROPERTY(QPointF startPoint READ startPoint WRITE setStartPoint NOTIFY startPointChanged)
    Q_PROPERTY(QPointF startHandle READ startHandle WRITE setStartHandle NOTIFY startHandleChanged)
    Q_PROPERTY(QPointF endHandle READ endHandle WRITE setEndHandle NOTIFY endHandleChanged)
    Q_PROPERTY(QPointF endPoint READ endPoint WRITE setEndPoint NOTIFY endPointChanged)

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(float lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged)

public:
    BezierCurve(QQuickItem *parent = 0);
    ~BezierCurve();

    QSGNode *updatePaintNode(QSGNode *, UpdatePaintNodeData *);

    QQuickItem* leftItem() const { return m_leftItem; }
    QQuickItem* rightItem() const { return m_rightItem; }
    QPointF startPoint() const { return m_startPoint; }
    QPointF startHandle() const { return m_startHandle; }
    QPointF endHandle() const { return m_endHandle; }
    QPointF endPoint() const { return m_endPoint; }

    QColor color() const { return m_color; }
    float lineWidth() const { return m_lineWidth; }

    void setLeftItem(QQuickItem* item);
    void setRightItem(QQuickItem* item);
    void setStartPoint(const QPointF &p);
    void setStartHandle(const QPointF &p);
    void setEndHandle(const QPointF &p);
    void setEndPoint(const QPointF &p);

    void setColor(const QColor& color);
    void setLineWidth(float width);

signals:
    void leftItemChanged(QQuickItem* item);
    void rightItemChanged(QQuickItem* item);
    void startPointChanged(const QPointF &p);
    void startHandleChanged(const QPointF &p);
    void endHandleChanged(const QPointF &p);
    void endPointChanged(const QPointF &p);

    void colorChanged(const QColor &color);
    void lineWidthChanged(float width);

private:

    static QPointF calculateBezierPoint(double t, const QPointF& p0, const QPointF& p1, const QPointF& p2, const QPointF& p3);

    static QPointF calculateBezierTangent(double t, const QPointF& p0, const QPointF& p1, const QPointF& p2, const QPointF& p3);

    static QPointF normalFromTangent(const QPointF& tangent);

    QPointer<QQuickItem> m_leftItem;
    QPointer<QQuickItem> m_rightItem;
    QPointF     m_startPoint;
    QPointF     m_startHandle;
    QPointF     m_endHandle;
    QPointF     m_endPoint;

    QColor      m_color;
    float       m_lineWidth;
    const float m_device_pixel_ratio;
};

#endif
