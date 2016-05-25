/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL21$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 or version 3 as published by the Free
** Software Foundation and appearing in the file LICENSE.LGPLv21 and
** LICENSE.LGPLv3 included in the packaging of this file. Please review the
** following information to ensure the GNU Lesser General Public License
** requirements will be met: https://www.gnu.org/licenses/lgpl.html and
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** As a special exception, The Qt Company gives you certain additional
** rights. These rights are described in The Qt Company LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
    , m_segmentCount(32)
    , m_device_pixel_ratio(QGuiApplication::primaryScreen()->devicePixelRatio())
{
    setFlag(ItemHasContents, true);
	setAntialiasing(true);
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


void BezierCurve::setSegmentCount(int count)
{
    if (m_segmentCount == count)
        return;

    m_segmentCount = count;
    emit segmentCountChanged(count);
    update();
}

QSGNode* BezierCurve::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *)
{
    QSGGeometryNode *node = 0;
    QSGGeometry *geometry = 0;

    if (m_leftItem == nullptr || m_rightItem == nullptr) {
        return nullptr;
    }

    if (!oldNode) {
        node = new QSGGeometryNode;
        geometry = new QSGGeometry(QSGGeometry::defaultAttributes_Point2D(), m_segmentCount);
        geometry->setLineWidth(m_lineWidth * m_device_pixel_ratio);
        geometry->setDrawingMode(GL_LINE_STRIP);
        node->setGeometry(geometry);
        node->setFlag(QSGNode::OwnsGeometry);
        QSGFlatColorMaterial *material = new QSGFlatColorMaterial;
        material->setColor(m_color);
        node->setMaterial(material);
        node->setFlag(QSGNode::OwnsMaterial);
    } else {
        node = static_cast<QSGGeometryNode *>(oldNode);
        geometry = node->geometry();
        geometry->allocate(m_segmentCount);
    }

    QSGGeometry::Point2D *vertices = geometry->vertexDataAsPoint2D();
    int deltaX = std::max(30, std::min(int(m_endPoint.x() - m_startPoint.x()), 200));
    QPointF endHandle(-deltaX, 0);
    QPointF startHandle(deltaX, 0);
    for (int i = 0; i < m_segmentCount; ++i) {
        qreal t = i / qreal(m_segmentCount - 1);
        qreal invt = 1 - t;

        QPointF pos = invt * invt * invt * m_startPoint
                    + 3 * invt * invt * t * (m_startPoint + startHandle)
                    + 3 * invt * t * t * (m_endPoint + endHandle)
                    + t * t * t * m_endPoint;
        QPointF posInScene = mapToScene(QPointF(this->x(), this->y()));
        float x = pos.x() - posInScene.x() + this->x();
        float y = pos.y() - posInScene.y() + this->y();

        vertices[i].set(x, y);
    }
    node->markDirty(QSGNode::DirtyGeometry);

    return node;
}
