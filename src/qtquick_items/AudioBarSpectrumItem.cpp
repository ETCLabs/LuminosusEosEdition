#include "AudioBarSpectrumItem.h"

#include <QtQuick/qsgnode.h>
#include <QtQuick/qsgflatcolormaterial.h>
#include <QQuickWindow>
#include <QGuiApplication>
#include <QScreen>
#include <cmath>


AudioBarSpectrumItem::AudioBarSpectrumItem(QQuickItem *parent)
    : QQuickItem(parent)
    , m_color("blue")
    , m_lineWidth(10)
    , m_agcEnabled(true)
    , m_manualGain(1.0)
    , m_device_pixel_ratio(QGuiApplication::primaryScreen()->devicePixelRatio())
{
    setFlag(ItemHasContents, true);
}

AudioBarSpectrumItem::~AudioBarSpectrumItem()
{
}

void AudioBarSpectrumItem::setColor(const QColor &color) {
    if (m_color == color)
        return;

    m_color = color;
    emit colorChanged(color);
    update();
}

void AudioBarSpectrumItem::setLineWidth(float width) {
    if (m_lineWidth == width)
        return;

    m_lineWidth = width;
    emit lineWidthChanged(width);
    update();
}

void AudioBarSpectrumItem::setAnalyzer(AudioInputAnalyzer* value) {
    if (m_analyzer) {
        disconnect(m_analyzer, SIGNAL(spectrumChanged()), this, SLOT(update()));
    }
    m_analyzer = value;
    if (m_analyzer) {
        connect(m_analyzer, SIGNAL(spectrumChanged()), this, SLOT(update()));
    }
    emit analyzerChanged();
    update();
}

QPointF AudioBarSpectrumItem::normalFromTangent(const QPointF& tangent) {
    // returns a normalized normal vector given a tangent vector
    if (tangent.manhattanLength() == 0) return QPointF(0, 0);
    QPointF n(tangent.y(), tangent.x() * (-1));
    n /= QVector2D(n).length();
    return n;
}

QSGNode* AudioBarSpectrumItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) {
    if (!m_analyzer) return oldNode;
    if (!isVisible()) return oldNode;

    const std::vector<double>& points = m_analyzer->getSimplifiedSpectrum();
    const double gain = m_agcEnabled ? m_analyzer->getAgcValue() : m_manualGain;
    const int pointCount = points.size();
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
        material->setColor("#fff");
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
        parentNode->appendChildNode(node);

        // Attention: TODO: colors are swapped!

        node = new QSGGeometryNode;
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 3);
        geometry->setDrawingMode(GL_TRIANGLE_STRIP);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);
        material = new QSGFlatColorMaterial;
        material->setColor(m_color);
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

    const int verticesCount = pointCount * 7;
    const int outlineVerticesCount = pointCount * 7;

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
    const double barWidth = (itemWidth / pointCount) * 0.4;
    const double spaceWidth = (itemWidth / pointCount) * 0.6;
    const double endLineHeight = itemHeight / 100.0;

    // draw spectrum:
    for (int i = 0; i < pointCount; ++i) {
        const float x = itemWidth * (i / float(pointCount));
        const float y = itemHeight * (1 - points[i] * gain);

        vertices[i*7].set(x, itemHeight);
        vertices[i*7+1].set(x, y);
        vertices[i*7+2].set(x + barWidth, itemHeight);
        vertices[i*7+3].set(x + barWidth, y);
        vertices[i*7+4].set(x + barWidth, itemHeight);
        vertices[i*7+5].set(x + barWidth, itemHeight);
        vertices[i*7+6].set(x + barWidth + spaceWidth, itemHeight);

        const float y2 = qMin(itemHeight, itemHeight * (1 - points[i] * gain) + endLineHeight);

        verticesOutline[i*7].set(x, itemHeight);
        verticesOutline[i*7+1].set(x, y2);
        verticesOutline[i*7+2].set(x + barWidth, itemHeight);
        verticesOutline[i*7+3].set(x + barWidth, y2);
        verticesOutline[i*7+4].set(x + barWidth, itemHeight);
        verticesOutline[i*7+5].set(x + barWidth, itemHeight);
        verticesOutline[i*7+6].set(x + barWidth + spaceWidth, itemHeight);
    }

    // tell Scene Graph that this items needs to be drawn:
    qsgNode->markDirty(QSGNode::DirtyGeometry);
    qsgNodeOutline->markDirty(QSGNode::DirtyGeometry);

    return parentNode;
}
