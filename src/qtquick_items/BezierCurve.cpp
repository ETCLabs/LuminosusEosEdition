#include "BezierCurve.h"

#include <QtQuick/qsgnode.h>
#include <QtQuick/qsgflatcolormaterial.h>
#include <QQuickWindow>
#include <QGuiApplication>
#include <QScreen>
#include <cmath>

BezierCurve::BezierCurve(QQuickItem *parent)
    : QQuickItem(parent)
    , m_leftItem(nullptr)
    , m_rightItem(nullptr)
    , m_startPoint(0, 0)
    , m_startHandle(300, 0)
    , m_endHandle(-300, 0)
    , m_endPoint(500, 500)
    , m_color("blue")
    , m_lineWidth(1)
    , m_device_pixel_ratio(QGuiApplication::primaryScreen()->devicePixelRatio())
{
    setFlag(ItemHasContents, true);
}

BezierCurve::~BezierCurve()
{
}

void BezierCurve::setLeftItem(QQuickItem *item)
{
    if (item == m_leftItem)
        return;

    m_leftItem = item;
    emit leftItemChanged(item);
    update();
}

void BezierCurve::setRightItem(QQuickItem *item)
{
    if (item == m_rightItem)
        return;

    m_rightItem = item;
    emit rightItemChanged(item);
    update();
}

void BezierCurve::setStartPoint(const QPointF &p)
{
    if (p == m_startPoint)
        return;

    m_startPoint = p;
    emit startPointChanged(p);
    update();
}

void BezierCurve::setStartHandle(const QPointF &p)
{
    if (p == m_startHandle)
        return;

    m_startHandle = p;
    emit startHandleChanged(p);
    update();
}

void BezierCurve::setEndHandle(const QPointF &p)
{
    if (p == m_endHandle)
        return;

    m_endHandle = p;
    emit endHandleChanged(p);
    update();
}

void BezierCurve::setEndPoint(const QPointF &p)
{
    if (p == m_endPoint)
        return;

    m_endPoint = p;
    emit endPointChanged(p);
    update();
}

void BezierCurve::setColor(const QColor &color)
{
    if (m_color == color)
        return;

    m_color = color;
    emit colorChanged(color);
    update();
}

void BezierCurve::setLineWidth(float width)
{
    if (m_lineWidth == width)
        return;

    m_lineWidth = width;
    emit lineWidthChanged(width);
    update();
}

QPointF BezierCurve::calculateBezierPoint(double t, const QPointF& p0, const QPointF& p1, const QPointF& p2, const QPointF& p3) {
    // from http://devmag.org.za/2011/04/05/bzier-curves-a-tutorial
    double u = 1 - t;
    double tt = t*t;
    double uu = u*u;
    double uuu = uu * u;
    double ttt = tt * t;

    QPointF p = uuu * p0; //first term
    p += 3 * uu * t * p1; //second term
    p += 3 * u * tt * p2; //third term
    p += ttt * p3; //fourth term

    return p;
}

QPointF BezierCurve::calculateBezierTangent(double t, const QPointF& p0, const QPointF& p1, const QPointF& p2, const QPointF& p3) {
    // from http://stackoverflow.com/questions/19605179/drawing-tangent-lines-for-each-point-in-bezier-curve
    double u = 1 - t;
    double tt = t*t;
    double uu = u*u;

    QPointF p = (-3) * p0 * uu;
    p += 3 * p1 * (uu - 2 * t * u);
    p += 3 * p2 * (-tt + u * 2 * t);
    p += 3 * p3 * tt;

    return p;
}

QPointF BezierCurve::normalFromTangent(const QPointF& tangent) {
    // returns a normalized normal vector given a tangent vector
    if (tangent.manhattanLength() == 0) return QPointF(0, 0);
    QPointF n(tangent.y(), tangent.x() * (-1));
    n /= QVector2D(n).length();
    return n;
}

QSGNode* BezierCurve::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*)
{
    // check if both left and right item exist:
    if (m_leftItem == nullptr || m_rightItem == nullptr) {
        return nullptr;
    }

    // -------------------- Prepare QSG Node:
    QSGGeometryNode* node = 0;
    QSGGeometry* geometry = 0;
    // calculate reasonable segment count:
    int segmentCount = qMax(16.0, qMin(qAbs(m_endPoint.y() - m_startPoint.y()) / 25, 50.0));
    int verticesCount = segmentCount * 2;
    if (!oldNode) {
        node = new QSGGeometryNode;
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), verticesCount);
        geometry->setDrawingMode(GL_TRIANGLE_STRIP);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);
        QSGFlatColorMaterial* material = new QSGFlatColorMaterial;
        material->setColor(m_color);
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
    } else {
        node = static_cast<QSGGeometryNode *>(oldNode);
        geometry = node->geometry();
        geometry->allocate(verticesCount);
    }
    QSGGeometry::Point2D* vertices = geometry->vertexDataAsPoint2D();

    // calculate control points:
    int handleLength = std::max(50, std::min(int(m_endPoint.x() - m_startPoint.x()), 80));
    const QPointF& p0 = m_startPoint;
    const QPointF p1(m_startPoint.x() + handleLength, m_startPoint.y());
    const QPointF p2(m_endPoint.x() - handleLength, m_endPoint.y());
    const QPointF& p3 = m_endPoint;

    // triangulate cubic bezier curve:
    const QPointF posInScene = mapToScene(QPointF(0, 0));
    double widthOffset = m_lineWidth / 2;
    for (int i = 0; i < segmentCount; ++i) {
        // t is the position on the line:
        const qreal t = i / qreal(segmentCount - 1);

        // pos is the point on the curve at "t":
        const QPointF pos = calculateBezierPoint(t, p0, p1, p2, p3) - posInScene;

        // normal is the normal vector at that point
        const QPointF normal = normalFromTangent(calculateBezierTangent(t, p0, p1, p2, p3));

        // first is a point offsetted in the normal direction by lineWidth / 2 from pos
        const QPointF first = pos - normal * widthOffset;

        // ssecond is a point offsetted in the negative normal direction by lineWidth / 2 from pos
        const QPointF second = pos + normal * widthOffset;

        // add first and second as vertices to this geometry:
        vertices[i*2].set(first.x(), first.y());
        vertices[i*2+1].set(second.x(), second.y());
    }
    // tell Scene Graph that this items needs to be drawn:
    node->markDirty(QSGNode::DirtyGeometry);

    return node;
}
