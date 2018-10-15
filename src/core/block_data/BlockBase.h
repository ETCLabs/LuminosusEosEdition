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

#include "core/block_data/BlockInterface.h"
#include "core/SmartAttribute.h"

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QQuickItem>
#include <QPointer>

// forward declaration to prevent dependency loop
class MainController;

// forward declaration to reduce dependencies
class NodeBase;
class OutputNodeHsv;
class InputNodeHsv;
class SmartAttribute;

/**
 * @brief The BlockBase class is the basis of all blocks.
 * It implements most of all functions from the BlockInterface
 * and provides convenience functions often used in Block implementations.
 */
class BlockBase : public BlockInterface
{
	Q_OBJECT

    Q_PROPERTY(bool focused MEMBER m_focused NOTIFY focusedChanged)

    Q_PROPERTY(int sceneGroup READ getSceneGroup WRITE setSceneGroup NOTIFY sceneGroupChanged)

    Q_PROPERTY(bool guiIsHidden READ guiIsHidden NOTIFY guiIsHiddenChanged)

public:
    // constructor etc:
    /**
     * @brief BlockBase creates a BlockBase object
     * @param controller a pointer to the main controller
     * @param uid set this to restore a block or leave empty to create new uid
     * @param qmlUrl path to QML file to load
     */
    explicit BlockBase(MainController* controller, QString uid);
    /**
     * @brief ~BlockBase deletes Nodes that were created on the heap
     */
    virtual ~BlockBase() override;

	// interface methods (documentation is in interface):
    virtual QJsonObject getState() const override;
    virtual void setState(const QJsonObject& state) override;
    virtual void getAdditionalState(QJsonObject& /*state*/) const override {}
    virtual void setAdditionalState(const QJsonObject& /*state*/) override {}
    virtual QJsonArray getConnections() override;
    virtual NodeBase* getNodeById(int id) override;
    virtual bool mayBeRemoved() override { return true; }
    virtual void disconnectAllNodes() override;
	virtual NodeBase* getDefaultInputNode() override;
	virtual NodeBase* getDefaultOutputNode() override;
    virtual QList<QPointer<NodeBase>> getNodes() const override;
    virtual QJsonObject getNodeMergeModes() const override;
    virtual void setNodeMergeModes(const QJsonObject& state) override;
    virtual bool renderIfNotVisible() const override { return false; }
    virtual void registerAttribute(SmartAttribute* attr) override;
    virtual void setGuiItemCode(QString code) override;

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

    void removeNode(NodeBase* node);

    QQuickItem* createQmlItem(QString qmlPath, QQuickItem* parent);

    void writeAttributesTo(QJsonObject& state) const;
    void readAttributesFrom(const QJsonObject& state);

signals:
    /**
     * @brief focusedChanged emitted when the internal focus changed
     */
    void focusedChanged();

    void sceneGroupChanged();

    void guiIsHiddenChanged();

public slots:
    virtual QString getUid() const override { return m_uid; }
    virtual void setUid(QString id) override { m_uid = id; }
	virtual QString getBlockName() const override;
    virtual void focus() override;
    virtual void defocus() override;
    virtual void onFocus() override;

    // Powermate / universal Controller:
    virtual void onControllerPressed() override { m_controllerFunctionSelected = (m_controllerFunctionSelected + 1) % m_controllerFunctionCount; }
    virtual void onControllerReleased(double) override {}
    virtual void onControllerRotated(double, double, bool) override {}

    virtual void onRemove() override {}
    virtual QQmlComponent* getSettingsComponent() const override;
    virtual QString getHelpText() const override { return getBlockInfo().helpText; }
    virtual void deletedByUser() override;
    virtual void onDeleteAnimationEnd() override;
    virtual void makeBlocksConnectedToInputsVisible() override;
    virtual QObject* attr(QString name) override;

    // GUI Item
    virtual void createGuiItem() override;
    virtual void destroyGuiItem(bool immediate=false) override;
    virtual QQuickItem* getGuiItem() override;
    virtual const QQuickItem* getGuiItemConst() const override;
    virtual double getGuiX() const override;
    virtual void setGuiX(double value) override;
    virtual double getGuiY() const override;
    virtual void setGuiY(double value) override;
    virtual void moveTo(double x, double y) override;
    virtual double getGuiWidth() const override;
    virtual void setGuiWidth(double value) override;
    virtual double getGuiHeight() const override;
    virtual void setGuiHeight(double value) override;
    virtual bool widthIsResizable() const override { return m_widthIsResizable; }
    virtual bool heightIsResizable() const override { return m_heightIsResizable; }
    virtual void hideGui() override;
    virtual void unhideGui() override;
    virtual bool guiIsHidden() const override;
    virtual bool guiShouldBeHidden() const override { return m_guiShouldBeHidden; }
    virtual void setGuiParentItem(QQuickItem* parent) override;
    virtual void onGuiItemCreated() override {}
    virtual QString getGroup() const override { return m_group; }
    virtual void setGroup(QString group) override { m_group = group; }

    // ---------------------------- Scenes ----------------------------

    virtual bool isSceneBlock() const override { return m_isSceneBlock; }

    virtual int getSceneGroup() const override { return m_sceneGroup; }

    virtual void setSceneGroup(int value) override { m_sceneGroup = value; }

    virtual void addToBench(const SceneData& /*data*/) override {}

    virtual void clearBench() override {}

    virtual HsvMatrix getMixData() const override { return HsvMatrix(); }

    virtual HsvMatrix getBenchData() const override { return HsvMatrix(); }

    virtual void setSceneData(const void* origin, double factor, const HsvMatrix& data) override;

    virtual void removeSceneData(const void* origin) override;

    virtual void updateFromSceneData() {}

    // ------------------------

    QObject* node(QString name);

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
     * @brief m_guiX is the x position of the GUI item relative to its parent
     */
    double m_guiX;
    /**
     * @brief m_guiY is the y position of the GUI item relative to its parent
     */
    double m_guiY;
    /**
     * @brief m_guiWidth is the width of the GUI item
     */
    double m_guiWidth;
    /**
     * @brief m_guiHeight is the height of the GUI item
     */
    double m_guiHeight;
    /**
     * @brief m_widthIsResizable stores if width is changable
     */
    bool m_widthIsResizable;
    /**
     * @brief m_heightIsResizable stores if height is changable
     */
    bool m_heightIsResizable;
    /**
     * @brief m_guiItemParent stores the GUI item parent while the GUI item is hidden
     */
    QQuickItem* m_guiItemParent;

    /**
     * @brief m_guiShouldBeHidden is true if the GUI of this block should be hidden
     */
    bool m_guiShouldBeHidden;
	/**
	 * @brief m_guiItem is a pointer to the create GUI item
	 */
	QPointer<QQuickItem> m_guiItem;
    /**
     * @brief m_group is the UID of the group of this block
     */
    QString m_group;
	/**
	 * @brief m_nodes stores pointer to all created nodes
	 */
    QMap<int, QPointer<NodeBase>> m_nodes;
    /**
     * @brief m_nodesByName stores pointers to all nodes by its names
     */
    QMap<QString, QPointer<NodeBase>> m_nodesByName;
	/**
	 * @brief m_externalInputMapping stores the mapping of external events to controls
	 */
    QMap<QString, QString> m_externalInputMapping;

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
     * @brief m_controllerFunctionCount is the count of function reachable by pressing the controller
     */
    int m_controllerFunctionCount;

    /**
     * @brief m_controllerFunctionSelected is the currently "selected" function by pressing the controller
     */
    int m_controllerFunctionSelected;

    /**
     * @brief m_blockAttributes contains all BlockAttributes of this block
     */
    QMap<QString, QPointer<SmartAttribute>> m_blockAttributes;

    /**
     * @brief m_persistentAttributes contains pointers to all attributes that should be persistet
     */
    QVector<QPointer<SmartAttribute>> m_persistentAttributes;


    // -------------- Scene Data ------------------

    bool m_isSceneBlock;

    /**
     * @brief m_sceneGroup 0 for not in a group, 1-? for the scene groups
     */
    int m_sceneGroup;

    QMap<const void*, SceneData> m_sceneValues;

    const void* m_lastChangedSceneOrigin;

    // -------------- Attributes ------------------

    /**
     * @brief m_label the user-chosen label text
     */
    StringAttribute m_label;

};

#endif // BLOCKBASE_H
