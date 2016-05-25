// Copyright (c) 2016 Electronic Theatre Controls, Inc., http://www.etcconnect.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

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

#ifndef BEZIERCURVE_H
#define BEZIERCURVE_H

#include <QtQuick/QQuickItem>

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
    Q_PROPERTY(int segmentCount READ segmentCount WRITE setSegmentCount NOTIFY segmentCountChanged)

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
    int segmentCount() const { return m_segmentCount; }

    void setLeftItem(QQuickItem* item);
    void setRightItem(QQuickItem* item);
    void setStartPoint(const QPointF &p);
    void setStartHandle(const QPointF &p);
    void setEndHandle(const QPointF &p);
    void setEndPoint(const QPointF &p);

    void setColor(const QColor &color);
    void setLineWidth(float width);
    void setSegmentCount(int count);

signals:
    void leftItemChanged(QQuickItem* item);
    void rightItemChanged(QQuickItem* item);
    void startPointChanged(const QPointF &p);
    void startHandleChanged(const QPointF &p);
    void endHandleChanged(const QPointF &p);
    void endPointChanged(const QPointF &p);

    void colorChanged(const QColor &color);
    void lineWidthChanged(float width);
    void segmentCountChanged(int count);

private:
    QQuickItem* m_leftItem;
    QQuickItem* m_rightItem;
    QPointF     m_startPoint;
    QPointF     m_startHandle;
    QPointF     m_endHandle;
    QPointF     m_endPoint;

    QColor      m_color;
    float       m_lineWidth;
    int         m_segmentCount;
    const float m_device_pixel_ratio;
};

#endif
