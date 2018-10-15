#ifndef NODECONNECTIONLINES_H
#define NODECONNECTIONLINES_H

#include "core/Nodes.h"

#include <QtQuick/QQuickItem>
#include <QVector2D>
#include <QPointer>


class NodeConnectionLines : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(float lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged)

public:
    explicit NodeConnectionLines(QQuickItem *parent = 0);
    ~NodeConnectionLines();

    QSGNode* updatePaintNode(QSGNode *, UpdatePaintNodeData *);

public slots:
    void setNodeObject(NodeBase* value);

    QColor color() const { return m_color; }
    float lineWidth() const { return m_lineWidth; }

    void setColor(const QColor& color);
    void setLineWidth(float width);

signals:
    void colorChanged(const QColor &color);
    void lineWidthChanged(float width);

private:

    static QPointF calculateBezierPoint(double t, const QPointF& p0, const QPointF& p1, const QPointF& p2, const QPointF& p3);

    static QPointF calculateBezierTangent(double t, const QPointF& p0, const QPointF& p1, const QPointF& p2, const QPointF& p3);

    static QPointF normalFromTangent(const QPointF& tangent);

    QPointer<NodeBase> m_nodeObject;

    QColor      m_color;
    float       m_lineWidth;
    const float m_device_pixel_ratio;
};

#endif // NODECONNECTIONLINES_H
