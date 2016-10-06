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

#include "StretchLayouts.h"

#include <QPointer>


StretchLayoutBase::StretchLayoutBase(QQuickItem* parent)
	: QQuickItem(parent)
	, m_spacing(0)
	, m_defaultSize(0)
	, m_leftMargin(0)
    , m_rightMargin(0)
{ }

int StretchLayoutBase::getSpacing() const {
	return m_spacing;
}

void StretchLayoutBase::setSpacing(int spacing) {
	if (m_spacing != spacing) {
		m_spacing = spacing;
		emit spacingChanged();
	}
}

int StretchLayoutBase::getDefaultSize() const {
	return m_defaultSize;
}

void StretchLayoutBase::setDefaultSize(int defaultSize) {
	if (m_defaultSize != defaultSize) {
		m_defaultSize = defaultSize;
		emit defaultSizeChanged();
	}
}

void StretchLayoutBase::componentComplete() {
	polish();
}

void StretchLayoutBase::updatePolish() {
	// items in this container can have a "stretch proportion" set
	// that will be used to stretch the items in the available space
	// or that indicates the the item should get the default size
	// - a "stretch proportion" is a negative implicitHeight/Width value
	// - items without will only be adjusted to fill the width/height

	if (m_defaultSize > 0) {
		layoutChildrenWithDefaultSize();
	} else {
		layoutChildrenWithProportions();
    }
}

void StretchLayoutBase::polishIfNoDefaultSize() {
    if (m_defaultSize <= 0) {
        polish();
    }
}


// ---------------------------------- StretchColumn ----------------------------------

StretchColumn::StretchColumn(QQuickItem* parent)
	: StretchLayoutBase(parent)
{

}

void StretchColumn::classBegin() {
    connect(this, &StretchRow::childrenChanged, this, &StretchRow::polish);
    // is polish really required when visibility changes?!
    //connect(this, &StretchRow::visibleChanged, this, &StretchRow::polish);
    connect(this, &StretchRow::widthChanged, this, &StretchRow::polish);
    connect(this, &StretchRow::spacingChanged, this, &StretchRow::polish);
    connect(this, &StretchRow::leftMarginChanged, this, &StretchRow::polish);
    connect(this, &StretchRow::rightMarginChanged, this, &StretchRow::polish);
    // only request polish because of height change if no defaultSize is set
    // othwerwise polish doesn't depend on height:
    connect(this, &StretchRow::heightChanged, this, &StretchLayoutBase::polishIfNoDefaultSize);
}

void StretchColumn::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData & value) {
	if (change == QQuickItem::ItemChildAddedChange) {
		QQuickItem* child = value.item;
		if (child != Q_NULLPTR) {
			connect(child, &QQuickItem::visibleChanged,
					this,  &StretchColumn::polish, Qt::UniqueConnection);
            if (child->implicitHeight() < 0) {
				// only listen to implicitHeight changes if this is a relative-sized item
				connect(child, &QQuickItem::implicitHeightChanged,
					this,  &StretchColumn::polish, Qt::UniqueConnection);
            } else {
				// only listen to height changes if this is a fixed-sized item
				connect(child, &QQuickItem::heightChanged,
					this,  &StretchColumn::polish, Qt::UniqueConnection);  // add | Qt::QueuedConnection?
            }
		}
    }
}

void StretchColumn::layoutChildrenWithProportions() {
	//static int count = 0;
	//qDebug() << "Column Proportions" << ++count;
	const QList<QQuickItem*> childrenList = childItems();
	if (childrenList.isEmpty()) return;
	if (height() <= 0) return;
	int visibleItems = 0;
    qreal availableForStretch = height();
	qreal sumStretchProportions = 0.0;

	// iterate over all children and calculate available size to stretch items:
	for (QList<QQuickItem *>::const_iterator it = childrenList.constBegin(); it != childrenList.constEnd(); it++) {
		QQuickItem* child = (*it);
		if (child == Q_NULLPTR || !child->isVisible()) {
			continue;
		}

		if (child->implicitHeight() >= 0) {
			// no stretch proportion set -> leave size and remove it from available space:
			availableForStretch -= child->height();
		} else {
			// a stretch proportion is set -> add it to sum:
			sumStretchProportions += (child->implicitHeight() * -1);
		}
		visibleItems++;
	}
	// remove spacing from available space:
	availableForStretch -= m_spacing * (visibleItems - 1);

    const int contentWidth = width() - m_leftMargin - m_rightMargin;
    QPointer<QQuickItem> lastStretchItem = nullptr;
    qreal currY = 0;
	for (QList<QQuickItem *>::const_iterator it = childrenList.constBegin(); it != childrenList.constEnd(); it++) {
		QQuickItem* child = (*it);
		if (child == Q_NULLPTR || !child->isVisible()) {
			continue;
		}

		// add spacing if it is not the first item:
		if (currY > 0) {
			currY += m_spacing;
		}
        if (currY != child->y()) child->setY(currY);
		// check if item has a stretch proportion set (if not do nothing):
		if (child->implicitHeight() < 0) {
			// get the stretch proportion:
			qreal stretchProportion = (child->implicitHeight() * -1);
			// calculate the relation of the total space available for stretched items
			// that this item will use:
			// (sumStretchProportion can not be 0 (in divison) because if this "if" is executed,
			// there has been at least one item with a stretch proportion set)
			qreal relationOfSpaceAvailableForStretch = stretchProportion / sumStretchProportions;
            qreal newHeight = qRound(availableForStretch * relationOfSpaceAvailableForStretch);
            if (newHeight != child->height()) child->setHeight(newHeight);
			lastStretchItem = child;
		}
		// set width of all items to fill the layout:
        if (m_leftMargin != child->x()) child->setX(m_leftMargin);
        if (contentWidth != child->width()) child->setWidth(contentWidth);
		currY += child->height();
	}

	// check if last item fills whole remaining space in this layout:
	// (can happen because of rounding mistakes)
	if (lastStretchItem) {
		double pxMissing = height() - currY;
        if (pxMissing > 0.0001) {
            // qInfo() << "StretchColumn layout mismatch: " << pxMissing;
			// it does not -> adjust it:
			lastStretchItem->setHeight(lastStretchItem->height() + pxMissing);
		}
	}
}

void StretchColumn::layoutChildrenWithDefaultSize() {
	//static int count = 0;
	//qDebug() << "Column Default Size" << ++count;
	Q_ASSERT(m_defaultSize != 0);

	const QList<QQuickItem *> childrenList = childItems();
	if (childrenList.isEmpty()) return;

    const int contentWidth = width() - m_leftMargin - m_rightMargin;
    qreal currY = 0;
	for (QList<QQuickItem *>::const_iterator it = childrenList.constBegin(); it != childrenList.constEnd(); it++) {
		QQuickItem* child = (*it);
		if (child == Q_NULLPTR || !child->isVisible()) {
			continue;
		}

		// add spacing if it is not the first item:
		if (currY > 0) {
			currY += m_spacing;
		}
		if (currY != child->y()) child->setY(currY);
		// check if item has a stretch proportion set:
		if (child->implicitHeight() < 0) {
			// yes -> set it to default size multiplied by relative size:
            qreal newHeight = m_defaultSize * child->implicitHeight() * -1;
            if (newHeight != child->height()) child->setHeight(newHeight);
		}
		// set width of all items to fill the layout:
		if (m_leftMargin != child->x()) child->setX(m_leftMargin);
		if (contentWidth != child->width()) child->setWidth(contentWidth);
		currY += child->height();
	}

	// set implicit size to fit all items:
	if (currY != implicitHeight()) setImplicitHeight(currY);
}


// ---------------------------------- StretchRow -------------------------------------

StretchRow::StretchRow(QQuickItem* parent)
	: StretchLayoutBase(parent)
{

}

void StretchRow::classBegin() {
    connect(this, &StretchRow::childrenChanged, this, &StretchRow::polish);
    // is polish really required when visibility changes?!
    //connect(this, &StretchRow::visibleChanged, this, &StretchRow::polish);
    connect(this, &StretchRow::heightChanged, this, &StretchRow::polish);
    connect(this, &StretchRow::spacingChanged, this, &StretchRow::polish);
    connect(this, &StretchRow::leftMarginChanged, this, &StretchRow::polish);
    connect(this, &StretchRow::rightMarginChanged, this, &StretchRow::polish);
    // only request polish because of width change if no defaultSize is set
    // othwerwise polish doesn't depend on width:
    connect(this, &StretchRow::widthChanged, this, &StretchLayoutBase::polishIfNoDefaultSize);
}

void StretchRow::itemChange(QQuickItem::ItemChange change, const QQuickItem::ItemChangeData & value) {
	if (change == QQuickItem::ItemChildAddedChange) {
        QQuickItem* child = value.item;
		if (child != Q_NULLPTR) {
			connect(child, &QQuickItem::visibleChanged,
					this,  &StretchRow::polish, Qt::UniqueConnection);
            if (child->implicitWidth() < 0) {
				// only listen to implicitWidth changes if this is a relative-sized item
                connect(child, &QQuickItem::implicitWidthChanged,
					this,  &StretchRow::polish, Qt::UniqueConnection);
            } else {
				// only listen to width changes if this is a relative-sized item
                connect(child, &QQuickItem::widthChanged,
                    this,  &StretchRow::polish, Qt::UniqueConnection);
            }
		}
    }
}

void StretchRow::layoutChildrenWithProportions() {
	//static int count = 0;
	//qDebug() << "Row Proportions" << ++count;
	const QList<QQuickItem*> childrenList = childItems();
	if (childrenList.isEmpty()) return;
	if (width() <= 0) return;
	int visibleItems = 0;
    qreal availableForStretch = width() - m_leftMargin - m_rightMargin;
	qreal sumStretchProportions = 0.0;

	// iterate over all children and calculate available size to stretch items:
	for (QList<QQuickItem *>::const_iterator it = childrenList.constBegin(); it != childrenList.constEnd(); it++) {
		QQuickItem* child = (*it);
		if (child == Q_NULLPTR || !child->isVisible()) {
			continue;
		}

		if (child->implicitWidth() >= 0) {
			// no stretch proportion set -> leave size and remove it from available space:
			availableForStretch -= child->width();
		} else {
			// a stretch proportion is set -> add it to sum:
			sumStretchProportions += (child->implicitWidth() * -1);
		}
		visibleItems++;
	}
	// remove spacing from available space:
	availableForStretch -= m_spacing * (visibleItems - 1);

    const int layoutHeight = height();
    QPointer<QQuickItem> lastStretchItem = nullptr;
    qreal currX = m_leftMargin;
	for (QList<QQuickItem *>::const_iterator it = childrenList.constBegin(); it != childrenList.constEnd(); it++) {
		QQuickItem* child = (*it);
		if (child == Q_NULLPTR || !child->isVisible()) {
			continue;
		}

		// add spacing if it is not the first item:
		if (currX > 0) {
			currX += m_spacing;
        }
        if (currX != child->x()) child->setX(currX);
		// check if item has a stretch proportion set (if not do nothing):
		if (child->implicitWidth() < 0) {
			// get the stretch proportion:
			qreal stretchProportion = (child->implicitWidth() * -1);
			// calculate the relation of the total space available for stretched items
			// that this item will use:
			// (sumStretchProportion can not be 0 (in divison) because if this "if" is executed,
			// there has been at least one item with a stretch proportion set)
			qreal relationOfSpaceAvailableForStretch = stretchProportion / sumStretchProportions;
            qreal newWidth = qRound(availableForStretch * relationOfSpaceAvailableForStretch);
            if (newWidth != child->width()) child->setWidth(newWidth);
			lastStretchItem = child;
		}
        // set height of all items to fill the layout:
        if (layoutHeight != child->height()) child->setHeight(layoutHeight);
		currX += child->width();
	}
	currX += m_rightMargin;

	// check if last item fills whole remaining space in this layout:
	// (can happen because of rounding mistakes)
	if (lastStretchItem) {
		double pxMissing = width() - currX;
        if (pxMissing > 0.0001) {
            // qInfo() << "StretchRow layout mismatch: " << pxMissing;
			// it does not -> adjust it:
			lastStretchItem->setWidth(lastStretchItem->width() + pxMissing);
		}
	}
}

void StretchRow::layoutChildrenWithDefaultSize() {
	//static int count = 0;
	//qDebug() << "Row Default Size" << ++count;
	Q_ASSERT(m_defaultSize != 0);

	const QList<QQuickItem *> childrenList = childItems();
	if (childrenList.isEmpty()) return;

    const int layoutHeight = height();
	qreal currX = m_leftMargin;
	for (QList<QQuickItem *>::const_iterator it = childrenList.constBegin(); it != childrenList.constEnd(); it++) {
		QQuickItem* child = (*it);
		if (child == Q_NULLPTR || !child->isVisible()) {
			continue;
		}

		// add spacing if it is not the first item:
		if (currX > 0) {
			currX += m_spacing;
		}
		if (currX != child->x()) child->setX(currX);
		// check if item has a stretch proportion set:
		if (child->implicitWidth() < 0) {
			// yes -> set it to default size:
			qreal newWidth = m_defaultSize * child->implicitWidth() * -1;
			if (newWidth != child->width()) child->setWidth(newWidth);
		}
		// set height of all items to fill the layout:
		if (layoutHeight != child->height()) child->setHeight(layoutHeight);
		currX += child->width();
	}
	currX += m_rightMargin;

	// set implicit size to fit all items:
	if (currX != implicitWidth()) setImplicitWidth(currX);
}
