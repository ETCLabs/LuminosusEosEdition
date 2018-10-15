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

#ifndef BLOCKINTERFACE
#define BLOCKINTERFACE

#include "core/Matrix.h"

#include <QObject>
#include <QJsonObject>
#include <QQuickItem>
#include <QString>

#include <functional>

// forward declaration to prevent dependency loop
class MainController;
class BlockInterface;
class SmartAttribute;

// forward declaration to reduce dependencies
class NodeBase;

enum AvailabilityRequirement {
	Midi = 0,
	AudioInput,
    Scripting,
    Experimental,
    LocalFileSelection
};

enum VisibilityRequirement {
    CompleteLicense = 0,
    DeveloperMode,
    StandaloneVersion,
    KeyboardEmulation
};

struct SceneData {
    double factor;
    HsvMatrix values;
};

/**
 * @brief The BlockInfo struct describes a block and is used to instanciate it.
 */
struct BlockInfo {

    /**
	 * @brief name of the block (unique identifier to create it)
	 *
	 * This should not change on updates to be backward compatible.
     */
    QString typeName = "";
	/**
	 * @brief nameInUi is the name of the block to be displayed in the UI
	 *
	 * This can change as it is not used to create the block.
	 */
	QString nameInUi = "";
    /**
     * @brief description of the block in the UI
     */
    QString description = "";
    /**
	 * @brief category and subcategories of the block in the UI
     */
	QStringList category = QStringList();
    /**
	 * @brief the next added device block will have a start channel this amount after the one before
     */
	int typicalNumberOfChannelsUsed = 0;
    /**
     * @brief availabilityRequirements of this block on the environment of the software,
	 * i.e. features of the hard- or software platform
     */
    QSet<AvailabilityRequirement> availabilityRequirements = QSet<AvailabilityRequirement>();
    /**
     * @brief visibilityRequirements define what requirements have to be fullfilled for this block to be visible
     * in the block list
     */
    QSet<VisibilityRequirement> visibilityRequirements = QSet<VisibilityRequirement>();
	/**
	 * @brief qmlFile is the path to the file containing the QML component for this block
	 */
	QString qmlFile = "";
	/**
	 * @brief helpText is a text to be displayed in the help section of the UI
	 */
	QString helpText = "";

	/**
	 * @brief createInstanceOnHeap is a function used to instantiate this block type
	 *
	 * The first parameter is a pointer to the main controller, the second is a UID or empty.
	 */
	std::function<BlockInterface*(MainController*, QString)> createInstanceOnHeap;

	/**
	 * @brief This function should be called to complete the creation of this info.
	 * It may check if all required parameters are provided and
	 * sets the createInstanceOnHeap function.
	 */
	template <typename T>
	void complete() {
		if (nameInUi.isEmpty()) {
            nameInUi = typeName;
		}
		createInstanceOnHeap = [](MainController* controller, QString uid) -> BlockInterface* { return new T(controller, uid); };

        // additional checks if all attributes are valid could be done here

//        if (helpText.isEmpty()) {
//            qInfo() << "Missing help text for " << typeName;
//        }

	}
};

/**
 * @brief This class describes the public interface of all Block classes.
 */
class BlockInterface : public QObject {

    Q_OBJECT

public:
    BlockInterface(MainController* controller) : QObject((QObject*)controller) { }
    virtual ~BlockInterface() {}  // virtual destructor makes shure the right destructor is called

    /**
     * @brief getState is used to get the current state of the block to persist it
     * (including the attributes)
     * @return a QJsonObject containing the blocks state
     */
    virtual QJsonObject getState() const = 0;
    /**
     * @brief setState sets the state of the block instance, i.e. to restore it from a saved state
     * (including the attributes)
     */
    virtual void setState(const QJsonObject&) = 0;
    /**
     * @brief getAdditionalState is used to get the current additional state of the block beside
     * the attributes to persist it
     * @param state a JSON object to write the state into
     */
    virtual void getAdditionalState(QJsonObject& state) const = 0;
    /**
     * @brief setAdditionalState sets the additional state of the block instance beside the attributes
     * @param state a JSON object containing the state
     */
    virtual void setAdditionalState(const QJsonObject& state) = 0;
    /**
     * @brief getConnections is used to get the connections of this block, i.e. to save and restore them
     * @return a QJsonArray with strings representing the connection "outputUid->inputUid"
     */
    virtual QJsonArray getConnections() = 0;
    /**
     * @brief getNodeById
     * @return the node with the given id
     */
    virtual NodeBase* getNodeById(int) = 0;

    /**
     * @brief mayBeRemoved checks if this block may be removed. It should not be removed for 
     * example when it is still part of a group.
     * @return true if this block may be safely removed
     */
    virtual bool mayBeRemoved() = 0;
    /**
     * @brief disconnectAllNodes is used when this block will be deleted
     */
    virtual void disconnectAllNodes() = 0;

	/**
	 * @brief getDefaultInputNode returns the default input node (i.e. for "connect on add")
	 */
	virtual NodeBase* getDefaultInputNode() = 0;

	/**
	 * @brief getDefaultOutputNode returns the default output node (i.e. for "connect on add")
	 */
	virtual NodeBase* getDefaultOutputNode() = 0;

    /**
     * @brief getNodes used to get all nodes of this block
     * @return a list of all nodes associated with this block
     */
    virtual QList<QPointer<NodeBase>> getNodes() const = 0;

    /**
     * @brief getNodeMergeModes returns an JSON object that contains information about the
     * merge mode of each node of this block
     *
     * the keys are the node IDs and the value is true if the merge mode is HTP
     * @return a JSON Object
     */
    virtual QJsonObject getNodeMergeModes() const = 0;

    /**
     * @brief setNodeMergeModes sets the merge mode of all nodes of this block on the base of a
     * JSON object returned from getNodeMergeModes()
     * @param state a JSON object returned from getNodeMergeModes()
     */
    virtual void setNodeMergeModes(const QJsonObject& state) = 0;

    /**
     * @brief renderIfNotVisible returns if the block should be rendered even if it is not visible
     * @return true if it should be rendered always
     */
    virtual bool renderIfNotVisible() const = 0;

    /**
     * @brief registerAttribute registers an attribute to be available by attr()
     * and to be persisted if requested
     * @param attr a pointer to the attribute to register
     */
    virtual void registerAttribute(SmartAttribute* attr) = 0;

    /**
     * @brief setGuiItemCode replaces the GUI item with one generated from the provided QML code
     * @param code QML code
     */
    virtual void setGuiItemCode(QString code) = 0;

signals:
    /**
     * @brief positionChanged is triggered when the position of the block in the UI changes and
     * for example the connecting lines have to be updated
     */
    void positionChanged();

	/**
	 * @brief positionChangedExternal is emitted when the position was changed but not by the DragArea.
	 * This is used by the DragArea to update the values in the kineticEffect.
	 */
    void positionChangedExternal();

public slots:
    /**
     * @brief getUid returns the UID of this block
     * @return the unique and persistent ID of the block instance
     */
	virtual QString getUid() const = 0;
    /**
     * @brief setUid sets the unique and persistent ID of the block instance
     */
    virtual void setUid(QString) = 0;
    /**
     * @brief getBlockInfo returns a BlockInfo instance containing infos about this block type
     * @return a BlockInfo instance for the corresponding block
     */
	virtual BlockInfo getBlockInfo() const = 0;
    /**
     * @brief getBlockName
     * @return the name of this blocks type
     */
	virtual QString getBlockName() const = 0;
    /**
     * @brief focus will focus this block and defocus the previously focused block
     */
    virtual void focus() = 0;
    /**
     * @brief defocus will defocus this block if it is focused
     */
    virtual void defocus() = 0;
    /**
     * @brief onFocus will be called before this block is focused
     */
    virtual void onFocus() = 0;
    /**
     * @brief onControllerPressed will be called when this block is focused and the knob controller is pressed
     */
    virtual void onControllerPressed() = 0;
    /**
     * @brief onControllerReleased will be called when this block is focused and the knob controller is released
     * @param duration time between press and release in seconds
     */
    virtual void onControllerReleased(double duration) = 0;
    /**
     * @brief onControllerRotated will be called when this block is focused and the knob controller is turned
     * @param relativeAmount amount of rotation typicaly between 1/512 and 1/64 (accelerated)
     * @param degrees amount of rotation in degrees (not accelerated)
     * @param pressed true if the knob controller is pressed
     */
    virtual void onControllerRotated(double relativeAmount, double degrees, bool pressed) = 0;
    /**
     * @brief onRemove will be called before this block will be removed
     */
    virtual void onRemove() = 0;
    /**
     * @brief getSettingsComponent returns the gui item to be displayed in the settings area of the drawer
     * @return a QQUickItem instance
     */
	virtual QQmlComponent* getSettingsComponent() const = 0;
	/**
	 * @brief getHelpText returns a text to be displayed in GUI as help text for this block
	 * @return help text string
	 */
	virtual QString getHelpText() const = 0;

    /**
     * @brief deletedByUser removes the block with animations
     */
    virtual void deletedByUser() = 0;
    /**
     * @brief onDeleteAnimationEnd finally deletes the block after the delete animation
     */
    virtual void onDeleteAnimationEnd() = 0;

    /**
     * @brief makeBlocksConnectedToInputsVisible iterates over blocks that are connected to input
     * nodes of this block and makes them visible because they provide the connection lines
     * (they could be invisible because they are not in the window viewport)
     */
    virtual void makeBlocksConnectedToInputsVisible() = 0;

    /**
     * @brief attr returns a pointer to the BlockAttribute with the given name
     * @param name of the BlockAttribute
     * @return pointer to it or nullptr if it doesn't exist
     */
    virtual QObject* attr(QString name) = 0;

    // -------------------------------- GUI Item ------------------------------
    /**
     * @brief createGuiItem creates the GUI item instance (if not already present)
     */
    virtual void createGuiItem() = 0;
    /**
     * @brief destroyGuiItem destroys the GUI item instance (if present)
     * @param immediate if true, GUI item is deleted immediatley (never use when a signal from the same block is involved, i.e. in BlockBase::deletedByUser()!)
     */
    virtual void destroyGuiItem(bool immediate=false) = 0;
    /**
     * @brief getGuiItem returns the GUI item instance
     * @return the QQuickItem instance from the corresponding QML file of this block or nullptr
     */
    virtual QQuickItem* getGuiItem() = 0;
    /**
     * @brief getGuiItemConst returns the GUI item instance
     * @return the QQuickItem instance from the corresponding QML file of this block or nullptr
     */
    virtual const QQuickItem* getGuiItemConst() const = 0;
    /**
     * @brief getGuiX returns the x position of the GUI item relative to its parent
     * @return x position in px (depends on dp unit!)
     */
    virtual double getGuiX() const = 0;
    /**
     * @brief setGuiX sets the x position of the GUI item relative to its parent
     * @param value x position in px (depends on dp unit!)
     */
    virtual void setGuiX(double value) = 0;
    /**
     * @brief getGuiY returns the y position of the GUI item relative to its parent
     * @return y position in px (depends on dp unit!)
     */
    virtual double getGuiY() const = 0;
    /**
     * @brief setGuiY sets the y position of the GUI item relative to its parent
     * @param value y position in px (depends on dp unit!)
     */
    virtual void setGuiY(double value) = 0;
    /**
     * @brief moveTo moves the GUI item animated
     * @param x destination position relative to parent
     * @param y destination position relative to parent
     */
    virtual void moveTo(double x, double y) = 0;
    /**
     * @brief getGuiWidth returns the width of the GUI item
     * @return width in px (depends on dp unit!)
     */
    virtual double getGuiWidth() const = 0;
    /**
     * @brief setGuiWidth sets the width of the GUI item
     * @param value width in px (depends on dp unit!)
     */
    virtual void setGuiWidth(double value) = 0;
    /**
     * @brief getGuiHeight returns the height of the GUI item
     * @return height in px (depends on dp unit!)
     */
    virtual double getGuiHeight() const = 0;
    /**
     * @brief setGuiHeight sets the height of the GUI item
     * @param value height in px (depends on dp unit!)
     */
    virtual void setGuiHeight(double value) = 0;
    /**
     * @brief widthIsResizable returns wether the width is user changable
     * @return true if changable
     */
    virtual bool widthIsResizable() const = 0;
    /**
     * @brief heightIsResizable returns wether the height is user changable
     * @return true if changable
     */
    virtual bool heightIsResizable() const = 0;
    /**
     * @brief hideGui hides the GUI of this block
     */
    virtual void hideGui() = 0;
    /**
     * @brief unhideGui shows the GUI of this block if it was hidden before
     */
    virtual void unhideGui() = 0;
    /**
     * @brief guiIsHidden returns if the block is hidden by hideBlock() or not
     * @return true if it is hidden
     */
    virtual bool guiIsHidden() const = 0;
    /**
     * @brief guiShouldBeHidden returns if the GUI should be hidden, regardless if it is
     * already created or not
     * @return true if it should be hidden
     */
    virtual bool guiShouldBeHidden() const = 0;
    /**
     * @brief setGuiParentItem sets the parent item of the GUI item of this block (takes care of
     * hidden state)
     * @param parent new GUI parent item
     */
    virtual void setGuiParentItem(QQuickItem* parent) = 0;
    /**
     * @brief onGuiItemCreated is called after the GUI item was created
     */
    virtual void onGuiItemCreated() = 0;

    virtual QString getGroup() const = 0;
    virtual void setGroup(QString group) = 0;

    // ---------------------------- Scenes ----------------------------

    virtual bool isSceneBlock() const = 0;

    virtual int getSceneGroup() const = 0;

    virtual void setSceneGroup(int value) = 0;

    virtual void addToBench(const SceneData& /*data*/) = 0;

    virtual void clearBench() = 0;

    virtual HsvMatrix getMixData() const = 0;

    virtual HsvMatrix getBenchData() const = 0;

    virtual void setSceneData(const void* origin, double factor, const HsvMatrix& data) = 0;

    virtual void removeSceneData(const void* origin) = 0;

};


#endif // BLOCKINTERFACE
