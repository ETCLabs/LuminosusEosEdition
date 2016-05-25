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

#include <QObject>
#include <QJsonObject>
#include <QQuickItem>
#include <QString>
#include <functional>
#include <vector>

// forward declaration to prevent dependency loop
class MainController;
class BlockInterface;

// forward declaration to reduce dependencies
class NodeBase;

enum BlockDependency {
	Midi = 0,
	AudioInput,
	Experimental,
	NonETC
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
    QString name = "";
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
	 * @brief dependencies of this block on the environment of the software,
	 * i.e. features of the hard- or software platform
     */
	std::vector<BlockDependency> dependencies = std::vector<BlockDependency>();
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
			nameInUi = name;
		}
		createInstanceOnHeap = [](MainController* controller, QString uid) -> BlockInterface* { return new T(controller, uid); };

		// additional checks if all attributes are valid could be done here
	}
};

/**
 * @brief This class describes the public interface of all Block classes.
 */
class BlockInterface : public QObject {

    Q_OBJECT

public:
    BlockInterface(MainController* controller) : QObject((QObject*)controller) { }
    //virtual ~BlockInterface() = 0;  // virtual destructor makes shure the right destructor is called

    /**
     * @brief getState is used to get the current state of the block to persist it
     * @return a QJsonObject containing the blocks state
     */
	virtual QJsonObject getState() const = 0;
    /**
     * @brief setState sets the state of the block instance, i.e. to restore it from a saved state
     */
    virtual void setState(const QJsonObject&) = 0;
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
     * @brief getGuiItem
     * @return the QQuickItem instance from the corresponding QML file of this block
     */
	virtual QQuickItem* getGuiItem() = 0;
	/**
	 * @brief getGuiItemConst
	 * @return the QQuickItem instance from the corresponding QML file of this block
	 */
	virtual const QQuickItem* getGuiItemConst() const = 0;
    /**
     * @brief mayBeRemoved checks if this block may be removed. It should not be removed for example when it is still part of a group.
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

signals:
    /**
     * @brief positionChanged is triggered when the position of the block in the UI changes and for example the connecting lines have to be updated
     */
    void positionChanged();

	/**
	 * @brief positionChangedExternal is emitted when the position was changed but not by the DragArea.
	 * This is used by the DragArea to update the values in the kineticEffect.
	 */
	void positionChangedExternal();

public slots:
    /**
     * @brief getUid
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
     * @param rotation amount of rotation typicaly between 1/512 and 1/64
     * @param pressed true if the knob controller is pressed
     */
    virtual void onControllerRotated(double rotation, bool pressed) = 0;
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

};


#endif // BLOCKINTERFACE
