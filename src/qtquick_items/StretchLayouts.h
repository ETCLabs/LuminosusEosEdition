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
//
// Original Code from Thomas BOUTROUE
// http://gitlab.unique-conception.org/qt-qml-tricks/qt-quick-ui-elements

#ifndef STRETCHLAYOUTBASE_H
#define STRETCHLAYOUTBASE_H

#include <QObject>
#include <QQuickItem>


/**
 * @brief The StretchLayoutBase class is the common base of StretchRow and StretchColumn.
 */
class StretchLayoutBase : public QQuickItem {

	Q_OBJECT

	Q_PROPERTY(int spacing READ getSpacing WRITE setSpacing NOTIFY spacingChanged)
	Q_PROPERTY(int defaultSize READ getDefaultSize WRITE setDefaultSize NOTIFY defaultSizeChanged)
	Q_PROPERTY(int leftMargin READ getLeftMargin WRITE setLeftMargin NOTIFY leftMarginChanged)
	Q_PROPERTY(int rightMargin READ getRightMargin WRITE setRightMargin NOTIFY rightMarginChanged)

public:
	/**
	 * @brief StretchLayoutBase creates an instance of this class (intended to be called by QmlEngine)
	 * @param parent QQuickItem parent
	 */
	explicit StretchLayoutBase(QQuickItem* parent = Q_NULLPTR);

signals:
	/**
	 * @brief spacingChanged emitted when the spacing changed
	 */
	void spacingChanged();
	/**
	 * @brief defaultSizeChanged emitted when the defaultSize changed
	 */
	void defaultSizeChanged();
	/**
	 * @brief leftMarginChanged emitted when the left margin changed
	 */
	void leftMarginChanged();
	/**
	 * @brief rightMarginChanged emitted when the right margin changed
	 */
	void rightMarginChanged();

public slots:
	/**
	 * @brief getSpacing returns the spacing between items in pixel
	 * @return spacing in pixel
	 */
	int getSpacing() const;
	/**
	 * @brief setSpacing sets the spacing between items in pixel
	 * @param spacing new value
	 */
	void setSpacing(int spacing);

	/**
	 * @brief getDefaultSize returns the defaultSize of items or 0 if not used
	 * @return defaultSize in pixel
	 */
	int getDefaultSize() const;
	/**
	 * @brief setDefaultSize sets the defaultSize of items
	 * @param defaultHeight new value or 0 to not use defaultSize function
	 */
	void setDefaultSize(int defaultHeight);

	/**
	 * @brief getLeftMargin returns the left margin in pixel
	 * @return left margin in pixel
	 */
	int getLeftMargin() const { return m_leftMargin; }
	/**
	 * @brief setLeftMargin sets the left margin
	 * @param value new value
	 */
	void setLeftMargin(int value) { m_leftMargin = value; emit leftMarginChanged(); }

	/**
	 * @brief getRightMargin returns the right margin in pixel
	 * @return right margin in pixel
	 */
	int getRightMargin() const { return m_rightMargin; }
	/**
	 * @brief setRightMargin sets the right margin
	 * @param value new value
	 */
	void setRightMargin(int value) { m_rightMargin = value; emit rightMarginChanged(); }

    /**
     * @brief polishIfNoDefaultSize requests polish if no default size is set
     */
    void polishIfNoDefaultSize();

protected:
	virtual void componentComplete() override;
	virtual void updatePolish() override;

private:
	/**
	 * @brief layoutChildrenWithProportions positions and resizes the children in limited space
	 * with negative implicitSizes as proportions
	 */
	virtual void layoutChildrenWithProportions() = 0;
	/**
     * @brief layoutChildrenWithDefaultSize positions and resizes the children in unlimited space
	 * and sets the sizes of all relative-sized items to a multiple of default size
	 * and sets the implicitSize of this layout
	 */
	virtual void layoutChildrenWithDefaultSize() = 0;

protected:
	/**
	 * @brief m_spacing the spacing between items in pixel (default = 0)
	 */
	int m_spacing;
	/**
	 * @brief m_defaultSize the defaultSize of items or 0 if not used (default = 0)
	 */
	int m_defaultSize;
	/**
	 * @brief m_leftMargin the left margin in pixel (default = 0)
	 */
	int m_leftMargin;
	/**
	 * @brief m_rightMargin the right margin in pixel (default = 0)
	 */
    int m_rightMargin;
};


// ---------------------------------- StretchColumn ----------------------------------

/**
 * @brief The StretchColumn class is a QML layout item that positions its children in a column.
 *
 * It can have two kinds of children: fixed-sized and relative-sized.
 *
 * The fixed-sized items must have a non-negative implicitHeight and a height > 0.
 * Their height will not be changed.
 *
 * The relative-sized items have a negative implicitHeight.
 *
 * If a defaultSize is set, the height of all relative-sized items will be set to the defaultSize
 * multiplied with the absolute value of implicitHeight.
 * The implicitHeight of the layout will then be set to the sum of all childrens height.
 * This method is perfect for unlimited space like in a scrollable list.
 *
 * If no defaultSize is set, the relative-sized items will fill the available space.
 * The available space is the height of the layout minus the height of all fixed-sized items.
 * The height of each relative-sized item will then be calculated by multiplying the available space
 * with the absolute value of the implicitHeight divided by the sum of all negative implicitHeights.
 * -> height = availableSpace * (abs(implicitHeight) / sum_of_implicitHeight_values)
 * The implicitHeight of the layout will not be changed in this case.
 * This method is perfect for distributing items in limited space.
 *
 * The width of all children will be set to the width of the layout in each case.
 *
 * The spacing between the items and the left and right margin can be specified.
 *
 * If an item is relative- or fixed-sized can't be changed after initialization because of an
 * implementation detail.
 *
 * Imagine the following situation:
 * - the layout is 1000px height
 * - 1. child: implicitHeight = 0, height 400
 * - 2. child: implicitHeight = -4
 * - 3. child: implicitHeight = -6
 * It will result in the following layout:
 * - sum of implicitHeights is 10
 * - available space to stretch items is 1000px - 400px = 600px
 * - -> all relative sizes will be multiplied by 600px / 10 = 60px
 * - 1. child is still 400px height
 * - 2. child is 240px height (4 * 60px = 240px)
 * - 2. child is 360px height (6 * 60px = 360px)
 */
class StretchColumn : public StretchLayoutBase {

	Q_OBJECT

public:
	/**
	 * @brief StretchRow creates an instance of this class (intended to be called by QmlEngine)
	 * @param parent QQuickItem parent
	 */
	explicit StretchColumn(QQuickItem* parent = Q_NULLPTR);

protected:
    virtual void classBegin() override;
	virtual void itemChange(ItemChange change, const ItemChangeData & value) override;

private:
	virtual void layoutChildrenWithProportions() override;
	virtual void layoutChildrenWithDefaultSize() override;
};


// ---------------------------------- StretchRow -------------------------------------

/**
 * @brief The StretchRow class is a QML layout item that positions its children in a row.
 *
 * It can have two kinds of children: fixed-sized and relative-sized.
 *
 * The fixed-sized items must have a non-negative implicitWidth and a width > 0.
 * Their width will not be changed.
 *
 * The relative-sized items have a negative implicitWidth.
 *
 * If a defaultSize is set, the width of all relative-sized items will be set to the defaultSize
 * multiplied with the absolute value of implicitWidth.
 * The implicitWidth of the layout will then be set to the sum of all childrens width.
 * This method is perfect for unlimited space like in a scrollable list.
 *
 * If no defaultSize is set, the relative-sized items will fill the available space.
 * The available space is the width of the layout minus the width of all fixed-sized items.
 * The width of each relative-sized item will then be calculated by multiplying the available space
 * with the absolute value of the implicitWidth divided by the sum of all negative implicitWidths.
 * -> width = availableSpace * (abs(implicitWidth) / sum_of_implicitWidth_values)
 * The implicitWidth of the layout will not be changed in this case.
 * This method is perfect for distributing items in limited space.
 *
 * The height of all children will be set to the height of the layout in each case.
 *
 * The spacing between the items and the left and right margin can be specified.
 *
 * If an item is relative- or fixed-sized can't be changed after initialization because of an
 * implementation detail.
 *
 */
class StretchRow : public StretchLayoutBase {

	Q_OBJECT

public:
	/**
	 * @brief StretchRow creates an instance of this class (intended to be called by QmlEngine)
	 * @param parent QQuickItem parent
	 */
	explicit StretchRow(QQuickItem* parent = Q_NULLPTR);

protected:
    virtual void classBegin() override;
	virtual void itemChange(ItemChange change, const ItemChangeData & value) override;

private:
	virtual void layoutChildrenWithProportions() override;
	virtual void layoutChildrenWithDefaultSize() override;
};

#endif // STRETCHLAYOUTBASE_H
