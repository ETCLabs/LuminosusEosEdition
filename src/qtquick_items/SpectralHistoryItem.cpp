#include "SpectralHistoryItem.h"

#include <QtQuick/qsgnode.h>
#include <QtQuick/qsgflatcolormaterial.h>
#include <QSGVertexColorMaterial>
#include <QQuickWindow>
#include <QGuiApplication>
#include <QScreen>
#include <cmath>


SpectralHistoryItem::SpectralHistoryItem(QQuickItem *parent)
    : QQuickItem(parent)
    , m_color("white")
    , m_lineWidth(1)
    , m_analyzer(nullptr)
    , m_device_pixel_ratio(QGuiApplication::primaryScreen()->devicePixelRatio())
{
    setFlag(ItemHasContents, true);
}

SpectralHistoryItem::~SpectralHistoryItem()
{
}

void SpectralHistoryItem::setColor(const QColor &color) {
    if (m_color == color)
        return;

    m_color = color;
    emit colorChanged(color);
    update();
}

void SpectralHistoryItem::setLineWidth(float width) {
    if (m_lineWidth == width)
        return;

    m_lineWidth = width;
    emit lineWidthChanged(width);
    update();
}

void SpectralHistoryItem::setAnalyzer(AudioInputAnalyzer* value) {
    if (m_analyzer) {
        disconnect(m_analyzer, SIGNAL(spectralFluxHistoryChanged()), this, SLOT(update()));
    }
    m_analyzer = value;
    if (m_analyzer) {
        connect(m_analyzer, SIGNAL(spectralFluxHistoryChanged()), this, SLOT(update()));
    }
    emit analyzerChanged();
    update();
}

void SpectralHistoryItem::updatePoints() {
    if (!m_analyzer) return;
    const auto fluxHistory = m_analyzer->getSpectralFluxHistory();
    m_points.resize(fluxHistory.size());
    const double gain = m_analyzer->getSpectralFluxAgcValue();
    for (int i=0; i<fluxHistory.size(); ++i) {
        m_points[i] = limit(0, fluxHistory[i] * gain / 8000.0, 1);
    }
}

QSGNode* SpectralHistoryItem::updatePaintNode(QSGNode* oldNode, UpdatePaintNodeData*) {
    if (!m_analyzer) return nullptr;

    // prepare spectral flux data:
    updatePoints();

    // prepare local variables:
    const int pointCount = m_points.size();
    if (pointCount < 2) return nullptr;
    const auto colors = m_analyzer->getSpectralColorHistory();
    const QVector<double>& onsets = m_analyzer->getDetectedOnsets();
    const double itemWidth = width();
    const double itemHeight = height();

    // ------- Prepare QSG Nodes:
    QSGNode* parentNode = nullptr;
    if (oldNode) {
        parentNode = static_cast<QSGNode*>(oldNode);
    } else {
        parentNode = new QSGNode;
    }
    // adapt child count:
    int childCount = parentNode->childCount();
    if (childCount != (1 + onsets.size())) {
        parentNode->removeAllChildNodes();
        QSGGeometryNode* node = new QSGGeometryNode;
        QSGGeometry* geometry = new QSGGeometry(QSGGeometry::defaultAttributes_ColoredPoint2D(), 3);
        geometry->setDrawingMode(GL_TRIANGLE_STRIP);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);
        QSGVertexColorMaterial* material = new QSGVertexColorMaterial();
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
        parentNode->appendChildNode(node);

        for (int i=0; i<onsets.size(); ++i) {
            node = new QSGGeometryNode;
            geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), 4);
            geometry->setDrawingMode(GL_TRIANGLE_STRIP);
            node->setGeometry(geometry);
            node->setFlag(QSGNode::OwnsGeometry);
            QSGFlatColorMaterial* flatMaterial = new QSGFlatColorMaterial;
            flatMaterial->setColor(m_color);
            node->setMaterial(flatMaterial);
            node->setFlag(QSGNode::OwnsMaterial);
            parentNode->appendChildNode(node);
        }
    }

    // ------------------- Draw Spectral Flux History -------------------

    const int verticesCount = pointCount * 2;

    QSGGeometryNode* qsgNode = static_cast<QSGGeometryNode*>(parentNode->childAtIndex(0));
    if (!qsgNode) {
        qCritical() << "[SpectrumItem] Could not get QSG Node.";
        return nullptr;
    }
    QSGGeometry* geometry = qsgNode->geometry();
    if (!geometry) {
        qCritical() << "[SpectrumItem] Could not get QSG Geometry.";
        return nullptr;
    }
    if (geometry->vertexCount() != verticesCount) {
        geometry->allocate(verticesCount);
    }
    QSGGeometry::ColoredPoint2D* vertices = geometry->vertexDataAsColoredPoint2D();
    if (!vertices) {
        qCritical() << "[SpectrumItem] Could not get QSG vertices.";
        return nullptr;
    }
    const double vCenter = itemHeight / 2.0;

    // draw each spectral flux value in the history (points):
    for (int i = 0; i < pointCount; ++i) {
        const float x = itemWidth * (i / float(pointCount - 1));
        const float halfBar = itemHeight * m_points[i] / 2.0;

        vertices[i*2].set(x, vCenter + halfBar, colors[i].red(), colors[i].green(), colors[i].blue(), 255);
        vertices[i*2+1].set(x, vCenter - halfBar, colors[i].red(), colors[i].green(), colors[i].blue(), 255);
    }

    // tell Scene Graph that this items needs to be redrawn:
    qsgNode->markDirty(QSGNode::DirtyGeometry);

    // ------------------- Draw Onset Lines -------------------

    const int lineWidth = qMax(1, int(m_lineWidth));

    // for each onset draw a vertical line:
    for (int i=0; i<onsets.size(); ++i) {
        QSGGeometryNode* qsgNodeOnset = static_cast<QSGGeometryNode*>(parentNode->childAtIndex(i+1));
        if (!qsgNodeOnset) {
            qCritical() << "[SpectrumItem] Could not get QSG Node.";
            return nullptr;
        }
        QSGGeometry* geometryOnset = qsgNodeOnset->geometry();
        if (!geometryOnset) {
            qCritical() << "[SpectrumItem] Could not get QSG Geometry.";
            return nullptr;
        }
        QSGGeometry::Point2D* verticesOnset = geometryOnset->vertexDataAsPoint2D();
        if (!verticesOnset) {
            qCritical() << "[SpectrumItem] Could not get QSG vertices.";
            return nullptr;
        }

        // horizontal position of the onset line:
        const float x = int(itemWidth * onsets[i] - (lineWidth / 2.0));

        // set 4 vertices per line (bottom left, top left, bottom right, top right):
        verticesOnset[0].set(x, itemHeight);
        verticesOnset[1].set(x, 0);

        verticesOnset[2].set(x + lineWidth, itemHeight);
        verticesOnset[3].set(x + lineWidth, 0);

        // tell Scene Graph that this items needs to be redrawn:
        qsgNodeOnset->markDirty(QSGNode::DirtyGeometry);
    }

    return parentNode;
}
