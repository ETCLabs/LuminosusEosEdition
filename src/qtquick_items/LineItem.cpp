#include "LineItem.h"

#include <QtQuick/qsgnode.h>
#include <QtQuick/qsgflatcolormaterial.h>
#include <QGuiApplication>
#include <QScreen>
#include <cmath>


LineItem::LineItem(QQuickItem *parent)
    : QQuickItem(parent)
    , m_color("blue")
    , m_lineWidth(1)
    , m_x1(0)
    , m_y1(0)
    , m_x2(0)
    , m_y2(0)
    , m_device_pixel_ratio(QGuiApplication::primaryScreen()->devicePixelRatio())
{
    setFlag(ItemHasContents, true);
    connect(this, SIGNAL(coordsChanged()), this, SLOT(update()));
}

LineItem::~LineItem()
{
}

void LineItem::setColor(const QColor &color)
{
    if (m_color == color) return;

    m_color = color;
    emit colorChanged(color);
    update();
}

void LineItem::setLineWidth(float width)
{
    if (m_lineWidth == width) return;

    m_lineWidth = width;
    emit lineWidthChanged(width);
    update();
}

QPointF LineItem::normalFromTangent(const QPointF& tangent) {
    // returns a normalized normal vector given a tangent vector
    if (tangent.manhattanLength() == 0) return QPointF(0, 0);
    QPointF n(tangent.y(), tangent.x() * (-1));
    n /= QVector2D(n).length();
    return n;
}

QSGNode* LineItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*)
{

    // -------------------- Prepare QSG Node:
    QSGGeometryNode* node = nullptr;
    QSGGeometry* geometry = nullptr;

    int verticesCount = 4;
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

    const QPointF begin(m_x1, m_y1);
    const QPointF end(m_x2, m_y2);
    const QPointF normal = normalFromTangent(end - begin);

    const double widthOffset = m_lineWidth / 2.0;

    const QPointF first = begin + normal * widthOffset;
    const QPointF second = begin - normal * widthOffset;
    const QPointF third = end + normal * widthOffset;
    const QPointF fourth = end - normal * widthOffset;

    vertices[0].set(first.x(), first.y());
    vertices[1].set(second.x(), second.y());
    vertices[2].set(third.x(), third.y());
    vertices[3].set(fourth.x(), fourth.y());

    // tell Scene Graph that this items needs to be drawn:
    node->markDirty(QSGNode::DirtyGeometry);

    return node;
}
