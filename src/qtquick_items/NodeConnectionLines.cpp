#include "NodeConnectionLines.h"

#include <QtQuick/qsgnode.h>
#include <QtQuick/qsgflatcolormaterial.h>
#include <QQuickWindow>
#include <QGuiApplication>
#include <QScreen>
#include <cmath>


NodeConnectionLines::NodeConnectionLines(QQuickItem *parent)
    : QQuickItem(parent)
    , m_nodeObject(nullptr)
    , m_color("blue")
    , m_lineWidth(1)
    , m_device_pixel_ratio(QGuiApplication::primaryScreen()->devicePixelRatio())
{
    setFlag(ItemHasContents, true);
}

NodeConnectionLines::~NodeConnectionLines() {

}

void NodeConnectionLines::setColor(const QColor &color) {
    if (m_color == color)
        return;

    m_color = color;
    emit colorChanged(color);
    update();
}

void NodeConnectionLines::setLineWidth(float width) {
    if (m_lineWidth == width)
        return;

    m_lineWidth = width;
    emit lineWidthChanged(width);
    update();
}

QPointF NodeConnectionLines::calculateBezierPoint(double t, const QPointF& p0, const QPointF& p1, const QPointF& p2, const QPointF& p3) {
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

QPointF NodeConnectionLines::calculateBezierTangent(double t, const QPointF& p0, const QPointF& p1, const QPointF& p2, const QPointF& p3) {
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

QPointF NodeConnectionLines::normalFromTangent(const QPointF& tangent) {
    // returns a normalized normal vector given a tangent vector
    if (tangent.manhattanLength() == 0) return QPointF(0, 0);
    QPointF n(tangent.y(), tangent.x() * (-1));
    n /= QVector2D(n).length();
    return n;
}

QSGNode* NodeConnectionLines::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) {
    if (!m_nodeObject) return nullptr;
    const QVector<QPointer<NodeBase>>& connectedNodes = m_nodeObject->getConnectedNodes();
    int connectionCount = connectedNodes.size();

    // -------------------- Prepare QSG Nodes:
    QSGNode* parentNode = oldNode;
    if (!oldNode) {
        parentNode = new QSGNode();
    } else {
        // update color in case it changed:
        // FIXME: is there a more efficient way to do this?
        for (int i=0; i<parentNode->childCount(); ++i) {
            QSGGeometryNode* childNode = static_cast<QSGGeometryNode*>(parentNode->childAtIndex(i));
            if (!childNode) continue;
            QSGFlatColorMaterial* material = static_cast<QSGFlatColorMaterial*>(childNode->material());
            if (!material) continue;
            material->setColor(m_color);
        }
    }
    // adapt child count:
    int childCount = parentNode->childCount();
    if (childCount < connectionCount) {
        for (int i=0; i<(connectionCount - childCount); ++i) {
            QSGGeometryNode* node = new QSGGeometryNode;
            QSGGeometry* geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 3);
            geometry->setDrawingMode(GL_TRIANGLE_STRIP);
            node->setGeometry(geometry);
            node->setFlag(QSGNode::OwnsGeometry);
            QSGFlatColorMaterial* material = new QSGFlatColorMaterial;
            material->setColor(m_color);
            node->setMaterial(material);
            node->setFlag(QSGNode::OwnsMaterial);
            parentNode->appendChildNode(node);
        }
    } else if (childCount > connectionCount) {
        for (int i=0; i<(childCount - connectionCount); ++i) {
            parentNode->removeChildNode(parentNode->childAtIndex(0));
        }
    }
    Q_ASSERT(parentNode->childCount() == connectionCount);

    // calculate common start point:
    const QPointF p0(width(), height() / 2);
    double widthOffset = m_lineWidth / 2;
    //const QPointF posInScene = mapToScene(QPointF(0, 0));


    for (int i=0; i<connectionCount; ++i) {
        NodeBase* otherNode = connectedNodes[i];
        if (!otherNode) continue;
        QQuickItem* otherGuiItem = otherNode->getGuiItem();
        if (!otherGuiItem) continue;
        const QPointF p3 = mapFromItem(otherGuiItem, QPointF(-otherGuiItem->width() / 2, otherGuiItem->height() / 2));
        int handleLength = std::max(50, std::min(int(p3.x() - p0.x()), 80));
        const QPointF p1(p0.x() + handleLength, p0.y());
        const QPointF p2(p3.x() - handleLength, p3.y());

        // calculate reasonable segment count:
        int segmentCount = qMax(16.0, qMin(qAbs(p3.y() - p0.y()) / 25, 50.0));
        int verticesCount = segmentCount * 2;

        QSGGeometryNode* qsgNode = static_cast<QSGGeometryNode*>(parentNode->childAtIndex(i));
        if (!qsgNode) continue;

        QSGGeometry* geometry = qsgNode->geometry();
        if (!geometry) continue;
        geometry->allocate(verticesCount);
        QSGGeometry::Point2D* vertices = geometry->vertexDataAsPoint2D();

        // triangulate cubic bezier curve:
        for (int i = 0; i < segmentCount; ++i) {
            // t is the position on the line:
            const qreal t = i / qreal(segmentCount - 1);

            // pos is the point on the curve at "t":
            const QPointF pos = calculateBezierPoint(t, p0, p1, p2, p3);

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
        qsgNode->markDirty(QSGNode::DirtyGeometry);
    }

    return parentNode;
}

void NodeConnectionLines::setNodeObject(NodeBase* value) {
    if (!value) {
        qWarning() << "NodeConnectionLines: setNodeObject: received nullptr";
        return;
    }
    m_nodeObject = value;
    connect(m_nodeObject, SIGNAL(connectionLinesChanged()), this, SLOT(update()));
}
