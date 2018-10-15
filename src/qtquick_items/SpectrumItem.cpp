#include "SpectrumItem.h"

#include <QtQuick/qsgnode.h>
#include <QtQuick/qsgflatcolormaterial.h>
#include <QQuickWindow>
#include <QGuiApplication>
#include <QScreen>
#include <cmath>


SpectrumItem::SpectrumItem(QQuickItem *parent)
    : QQuickItem(parent)
    , m_color("blue")
    , m_lineWidth(3)
    , m_device_pixel_ratio(QGuiApplication::primaryScreen()->devicePixelRatio())
{
    setFlag(ItemHasContents, true);
}

SpectrumItem::~SpectrumItem() {
}

void SpectrumItem::setColor(const QColor &color) {
    if (m_color == color)
        return;

    m_color = color;
    emit colorChanged(color);
    update();
}

void SpectrumItem::setLineWidth(float width) {
    if (m_lineWidth == width)
        return;

    m_lineWidth = width;
    emit lineWidthChanged(width);
    update();
}

void SpectrumItem::setPoints(const QVector<double> value) {
    m_points = value;
    emit pointsChanged();
    update();
}

QPointF SpectrumItem::normalFromTangent(const QPointF& tangent) {
    // returns a normalized normal vector given a tangent vector
    if (tangent.manhattanLength() == 0) return QPointF(0, 0);
    QPointF n(tangent.y(), tangent.x() * (-1));
    n /= QVector2D(n).length();
    return n;

}

QSGNode* SpectrumItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) {

    const int pointCount = m_points.size();
    if (pointCount < 2) return oldNode;

    // -------------------- Prepare QSG Nodes:
    QSGNode* parentNode = nullptr;
    if (oldNode) {
        parentNode = static_cast<QSGNode*>(oldNode);
    } else {
        parentNode = new QSGNode;
    }
    // adapt child count:
    int childCount = parentNode->childCount();
    if (childCount != 2) {
        parentNode->removeAllChildNodes();
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

        node = new QSGGeometryNode;
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 3);
        geometry->setDrawingMode(GL_TRIANGLE_STRIP);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);
        material = new QSGFlatColorMaterial;
        material->setColor("#fff");
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
        parentNode->appendChildNode(node);
    }

    QSGGeometryNode* const qsgNode = static_cast<QSGGeometryNode*>(parentNode->childAtIndex(0));
    QSGGeometryNode* const qsgNodeOutline = static_cast<QSGGeometryNode*>(parentNode->childAtIndex(1));
    if (!qsgNode || !qsgNodeOutline) {
        qCritical() << "[SpectrumItem] Could not get QSG Node.";
        return nullptr;
    }
    QSGGeometry* const geometry = qsgNode->geometry();
    QSGGeometry* const geometryOutline = qsgNodeOutline->geometry();
    if (!geometry || !geometryOutline) {
        qCritical() << "[SpectrumItem] Could not get QSG Geometry.";
        return nullptr;
    }

    const int verticesCount = pointCount * 2;
    const int outlineVerticesCount = pointCount * 4;

    geometry->allocate(verticesCount);
    geometryOutline->allocate(outlineVerticesCount);
    QSGGeometry::Point2D* const vertices = geometry->vertexDataAsPoint2D();
    QSGGeometry::Point2D* const verticesOutline = geometryOutline->vertexDataAsPoint2D();

    if (! vertices || !verticesOutline) {
        qCritical() << "[SpectrumItem] Could not get QSG vertices.";
        return nullptr;
    }

    const double itemWidth = width();
    const double itemHeight = height();
    const double widthOffset = m_lineWidth / 2.0;

    // draw spectrum:
    for (int i = 0; i < pointCount; ++i) {
        const float x = itemWidth * (i / float(pointCount - 1));
        const float y = itemHeight * (1 - m_points[i]);

        vertices[i*2].set(x, itemHeight);
        vertices[i*2+1].set(x, y);

        // pos is the point on the curve at "t":
        const QPointF pos(x, y - widthOffset);

        // normal is the normal vector at that point
        QPointF prevNormal(0.0, -1.0);
        QPointF nextNormal(0.0, -1.0);
        if (i > 0 && i < pointCount - 1) {
            // this is not the first or last point
            // calculate normal as average between previous and next point:
            // m is slope (Steigung)
            const double prevM = (m_points[i] - m_points[i-1]) * itemHeight;
            const double nextM = (m_points[i+1] - m_points[i]) * itemHeight;
            const QPointF prevTangent(itemWidth / pointCount, -prevM);
            const QPointF nextTangent(itemWidth / pointCount, -nextM);
            prevNormal = normalFromTangent(prevTangent);
            nextNormal = normalFromTangent(nextTangent);
        }

        // first is a point offsetted in the normal direction by lineWidth / 2 from pos
        const QPointF first = pos + prevNormal * widthOffset;
        // second is a point offsetted in the negative normal direction by lineWidth / 2 from pos
        const QPointF second = pos - prevNormal * widthOffset;

        const QPointF third = pos + nextNormal * widthOffset;
        const QPointF forth = pos - nextNormal * widthOffset;

        // add first and second as vertices to this geometry:
        verticesOutline[i*4].set(first.x(), first.y());
        verticesOutline[i*4+1].set(second.x(), second.y());
        verticesOutline[i*4+2].set(third.x(), third.y());
        verticesOutline[i*4+3].set(forth.x(), forth.y());
    }

    // tell Scene Graph that this items needs to be drawn:
    qsgNode->markDirty(QSGNode::DirtyGeometry);
    qsgNodeOutline->markDirty(QSGNode::DirtyGeometry);

    return parentNode;
}
