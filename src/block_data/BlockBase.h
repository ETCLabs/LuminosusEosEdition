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

#ifndef BLOCKBASE_H
#define BLOCKBASE_H

#include "BlockInterface.h"

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QQuickItem>
#include <QPointer>

#include <map>

// forward declaration to prevent dependency loop
class MainController;

// forward declaration to reduce dependencies
class NodeBase;
class OutputNodeHsv;
class InputNodeHsv;

/**
 * @brief The BlockBase class is the basis of all blocks.
 * It implements most of all functions from the BlockInterface
 * and provides convenience functions often used in Block implementations.
 */
class BlockBase : public BlockInterface
{
	Q_OBJECT

    Q_PROPERTY(bool focused MEMBER m_focused NOTIFY focusedChanged)
    Q_PROPERTY(QString label READ getLabel WRITE setLabel NOTIFY labelChanged)

public:
    // constructor etc:
    /**
     * @brief BlockBase creates a BlockBase object
     * @param controller a pointer to the main controller
     * @param uid set this to restore a block or leave empty to create new uid
     * @param qmlUrl path to QML file to load
     */
	explicit BlockBase(MainController* controller, QString uid, QString qmlUrl);
    /**
     * @brief ~BlockBase deletes Nodes that were created on the heap
     */
	virtual ~BlockBase() override;
    /**
     * @brief completeGuiItemCreation completes GUI item creation process, to be called after all
     * properties were intialized
     */
    void completeGuiItemCreation() override;

	// interface methods (documentation is in interface):
	virtual QJsonObject getState() const override { return QJsonObject(); }
    virtual void setState(const QJsonObject&) override { }
    virtual QJsonArray getConnections() override;
    virtual NodeBase* getNodeById(int id) override;
	QQuickItem* getGuiItem() override;
	const QQuickItem* getGuiItemConst() const override;
    virtual bool mayBeRemoved() override { return true; }
    virtual void disconnectAllNodes() override;
	virtual NodeBase* getDefaultInputNode() override;
	virtual NodeBase* getDefaultOutputNode() override;
    virtual QJsonObject getNodeMergeModes() const override;
    virtual void setNodeMergeModes(const QJsonObject& state) override;
    virtual bool renderIfNotVisible() const override { return false; }

    // convenience methods:
	/**
	 * @brief getGuiItemChild returns a GUI element by its "objectName"
	 * @param name the "objectName" of the QML item (must be a child of the GUI item of this block)
	 * @return a pointer to the item
	 */
    QQuickItem* getGuiItemChild(QString name);
	/**
     * @brief createOutputNode creates a NodeBase object on the heap,
	 * registers it to this block and connects it to an GUI Node item
	 * @param guiItemName the "objectName" of the associated QML element
	 * @return a pointer to the Node object
	 */
    NodeBase* createOutputNode(QString guiItemName);
	/**
     * @brief createInputNode creates a NodeBase object on the heap,
	 * registers it to this block and connects it to an GUI Node item
	 * @param guiItemName the "objectName" of the associated QML element
	 * @return a pointer to the Node object
	 */
    NodeBase* createInputNode(QString guiItemName);

signals:
    /**
     * @brief focusedChanged emitted when the internal focus changed
     */
    void focusedChanged();

    /**
     * @brief labelChanged is emitted when the user changed the label text
     */
    void labelChanged();

public slots:
	QString getUid() const override { return m_uid; }
	void setUid(QString id) override { m_uid = id; }
	virtual QString getBlockName() const override;
    virtual void focus() override;
    virtual void defocus() override;
    virtual void onFocus() override;

    // Powermate / universal Controller:
    virtual void onControllerPressed() override {}
    virtual void onControllerReleased(double) override {}
    virtual void onControllerRotated(double, bool) override {}

    virtual void onRemove() override {}
	QQmlComponent* getSettingsComponent() const override;
	QString getHelpText() const override { return getBlockInfo().helpText; }
    void deletedByUser() override;
    void onDeleteAnimationEnd() override;
    void makeBlocksConnectedToInputsVisible() override;
    virtual QString getLabel() const override { return m_label; }
    virtual void setLabel(QString value) override { m_label = value; emit labelChanged(); }

protected:
	/**
	 * @brief m_uid stores the unique ID of this block
	 */
	QString m_uid;
	/**
	 * @brief m_controller is a pointer to the main controller
	 */
	MainController* const m_controller;
	/**
	 * @brief m_component is the QML component of the GUI item
	 */
	QQmlComponent m_component;
	/**
	 * @brief m_guiItem is a pointer to the create GUI item
	 */
	QPointer<QQuickItem> m_guiItem;
	/**
	 * @brief m_nodes stores pointer to all created nodes
	 */
	std::map<int, QPointer<NodeBase>> m_nodes;
	/**
	 * @brief m_externalInputMapping stores the mapping of external events to controls
	 */
	std::map<QString, QString> m_externalInputMapping;

    /**
     * @brief m_focused true if this block is focused (independent from Qt keyboard focus)
     */
    bool m_focused;

    /**
     * @brief m_guiItemCompleted false if the GUI item has not been completed yet and
     * completeGuiItemCreation() should be called before accessing it
     */
    bool m_guiItemCompleted;

    /**
     * @brief m_label the user-chosen label text
     */
    QString m_label;

};

#endif // BLOCKBASE_H
