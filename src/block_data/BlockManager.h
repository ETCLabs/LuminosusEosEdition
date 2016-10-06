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

#ifndef BLOCKMANAGER_H
#define BLOCKMANAGER_H

#include "BlockList.h"
#include "utils.h"

#include "QCircularBuffer.h"
#include <QObject>
#include <QPointer>
#include <vector>


// forward declaration to reduce dependencies
class MainController;
class BlockInterface;
class NodeBase;

/**
 * @brief The BlockManagerConstants namespace contains all constants used in BlockManager.
 */
namespace BlockManagerConstants {
	/**
	 * @brief undoHistoryLength is the number of deleted blocks to be available for undo
	 */
	static const int undoHistoryLength = 5;

    /**
     * @brief defaultBlockPositionOffset is the amount of pixel to randomly offset new blocks
     * if not other value is specified
     */
    static const int defaultBlockPositionOffset = 400;
}


/**
 * @brief The BlockManager class is responsible for adding, deleting, focusing and selecting blocks.
 */
class BlockManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int blockInstanceCount READ getBlockInstanceCount NOTIFY blockInstanceCountChanged)

public:
	/**
	 * @brief BlockManager creates an instance of this manager
	 * @param controller pointer to the MainController
	 */
    BlockManager(MainController* controller);

public slots:

	/**
	 * @brief blockList returns a pointer to the BlockList object
	 * @return a pointer to the BlockList
	 */
	BlockList* blockList() { return &m_blockList; }

	/**
	 * @brief getNodeByUid returns a pointer to a Node by its unique id
	 * @param uid the id of the node
	 * @return a pointer to the Node or nullptr if Node doesn't exist
	 */
	NodeBase* getNodeByUid(QString uid);

    /**
     * @brief getBlockInstanceCount
     * @return number of block instances in this project
     */
    int getBlockInstanceCount() const { return m_currentBlocks.size(); }

    /**
     * @brief updateBlockVisibility sets "visible" property of blocks that are not in the
     * current viewport to false
     * @param workspace a pointer to the GUI items that represents the viewport
     */
    void updateBlockVisibility(QQuickItem* workspace);

    /**
     * @brief setHideBlocksOutsideViewports defines if blocks outside the viewport should be rendered
     * or not (can be used to force rendering all blocks before zooming out)
     * @param value true to hide blocks outside the viewport (prevent rendering them)
     */
    void setHideBlocksOutsideViewports(bool value);

	// ---------------- Blocks:

	/**
	 * @brief restoreBlock restores a block from a saved state
	 * @param blockState state of the block
	 * @param animated true if the block should "fly" to the right position
	 * @return a pointer to the created Block
	 */
    BlockInterface* restoreBlock(const QJsonObject& blockState, bool animated = true, bool connectOnAdd = false);
	/**
	 * @brief addNewBlock creates a new block of the given type
	 * @param blockType type of the block
     * @param randomOffset is the amount of pixel (dpi independent) to randomly offset
     * the position of this block
	 * @return a pointer to the created block
	 */
    BlockInterface* addNewBlock(QString blockType, int randomOffset=-1);
	/**
	 * @brief addBlockByNameQml calls addNewBlock() but doesn't return a pointer
	 * @param blockType type of the block
	 */
	void addBlockByNameQml(QString blockType);
	/**
	 * @brief deleteAllBlocks deletes all blocks (forced)
	 */
    void deleteAllBlocks();
	/**
	 * @brief deleteBlock deletes a specific block by a pointer to it
	 * @param block pointer to the block
	 * @param forced true if it should be removed even if it is still in use (i.e. in a group)
	 */
    void deleteBlock(BlockInterface* block, bool forced=false);
	/**
	 * @brief deleteBlock deletes a specific block by its UID
	 * @param uid unique id of the block
	 * @param forced true if it should be removed even if it is still in use (i.e. in a group)
	 */
    void deleteBlock(QString uid, bool forced=false);
	/**
	 * @brief deleteFocusedBlock deletes the currently focused block (not forced)
	 */
	void deleteFocusedBlock();

	/**
	 * @brief duplicateFocusedBlock adds a new block of the type of the focused block
	 * and sets its state to the same as the focused block
	 */
	void duplicateFocusedBlock();

	/**
	 * @brief copyFocusedBlock stores the state of a block to paste it later
	 * (i.e. with Ctrl + C and Ctrl + V)
	 */
	void copyFocusedBlock();

	/**
	 * @brief pasteBlock "pastes" the last copied block (i.e. with Ctrl + C and Ctrl + V)
	 */
	void pasteBlock();

	/**
	 * @brief restoreDeletedBlock tries to restore the last deleted block
	 */
	void restoreDeletedBlock();

	/**
	 * @brief getCurrentBlocks returns a list of all existing block instances
	 * @return a list of pointers to blocks
	 */
	const std::vector<QPointer<BlockInterface>>& getCurrentBlocks() const { return m_currentBlocks; }

	/**
	 * @brief getBlockByUid returns a pointer to the block with the given uid
	 * or nullptr if it does not exist
	 * @param uid of the block
	 * @return pointer to a block or nullptr
	 */
	BlockInterface* getBlockByUid(QString uid);

	/**
	 * @brief getBlockState returns the state of a block (including position etc. and internal state)
	 * @param block pointer to the block
	 * @return QJsonObject containing the state information
	 */
	QJsonObject getBlockState(BlockInterface* block) const;

    /**
     * @brief getBlocksMidpoint return the geometric middle point of all blocks
     * (used to bring blocks back to viewport)
     * @return a point on the workspace
     */
    QPoint getBlocksMidpoint() const;

	// ----------------- Focused Block:

	/**
	 * @brief focusBlock focuses a given block
	 * @param block pointer to the block
	 */
    void focusBlock(BlockInterface* block);
	/**
	 * @brief defocusBlock removes focus from a given block
	 * @param block pointer to the block
	 */
    void defocusBlock(BlockInterface* block);
	/**
	 * @brief getFocusedBlock returns a pointer to the focused Block
	 * @return a pointer to a Block or nullptr if no block is focused
	 */
	BlockInterface* getFocusedBlock() { return m_focusedBlock; }
	/**
	 * @brief getFocusedBlock returns a pointer to the focused Block
	 * @return a pointer to a Block or nullptr if no block is focused
	 */
    BlockInterface* getFocusedBlock() const { return m_focusedBlock; }

signals:
	/**
	 * @brief focusChanged emitted when the focused block changed (or the focus was released)
	 */
	void focusChanged();

    /**
     * @brief blockInstanceCountChanged emitted when the number of block instances changed
     */
    void blockInstanceCountChanged();

private:
	/**
	 * @brief createBlockInstance creates a block instance and adds it to the correct lists
	 * @param blockType type of the block to add
	 * @param uid of th block if this is used to restore a saved block state
	 * or nothing to generate a new uid
	 * @return a pointer to the created Block
	 */
	BlockInterface* createBlockInstance(QString blockType, QString uid = "");
	/**
	 * @brief createGuiItem creates the GUI item for a block instance
     * and adds it to the "WorkspacePlane"
	 * @param block pointer to the block instance
	 * @return a pointer to the GUI item
	 */
	QQuickItem* createGuiItem(BlockInterface* block);
	/**
	 * @brief getSpawnPosition returns the preferred creation / spawn position in the GUI
     * @return a position on the "WorkspacePlane"
	 */
    QPoint getSpawnPosition(int randomOffset) const;
	/**
	 * @brief getBlockListPosition returns the position of the BlockList drawer in the GUI
     * relative to the "WorkspacePlane"
     * @return a position on the "WorkspacePlane"
	 */
	QPoint getBlockListPosition() const;


protected:
	/**
	 * @brief m_blockList manages the information about all available block types
	 */
	BlockList m_blockList;
	/**
	 * @brief m_currentBlocks is the list of all currently existing block instances
	 */
	std::vector<QPointer<BlockInterface>> m_currentBlocks;
	/**
	 * @brief m_currentBlocksByUid maps the uid to the pointer of an existing block instance
	 */
	std::map<QString, QPointer<BlockInterface>> m_currentBlocksByUid;
	/**
	 * @brief m_focusedBlock is a pointer to the currently focused block
	 * (or nullptr if no block is focused)
	 */
	QPointer<BlockInterface> m_focusedBlock;
	/**
	 * @brief m_controller a pointer to the MainController
	 */
	MainController* const m_controller;
	/**
	 * @brief m_startChannel is the start channel for the next added device block
	 */
	int m_startChannel;

	/**
	 * @brief m_lastDeletedBlockState stores the state of the last deleted blocks to be able to restore it
	 */
	Qt3DCore::QCircularBuffer<QJsonObject> m_lastDeletedBlockStates;

	/**
	 * @brief m_copiedBlockState stores the state of the last copied block (i.e. with Ctrl + C)
	 */
	QJsonObject m_copiedBlockState;

    /**
     * @brief m_hideBlocksOutsideViewports when this is true, blocks outside the viewport
     * won't be rendered (default)
     *
     * Can be disabled to force rendering all blocks (i.e. zoomed out and when scale is < 1)
     */
    bool m_hideBlocksOutsideViewports;
};

#endif // BLOCKMANAGER_H
