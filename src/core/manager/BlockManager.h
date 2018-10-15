#ifndef BLOCKMANAGER_H
#define BLOCKMANAGER_H

#include "core/block_data/BlockList.h"
#include "core/QCircularBuffer.h"
#include "utils.h"

#include <QObject>
#include <QPointer>
#include <vector>
#include <QTimer>
#include <QSoundEffect>


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

    Q_PROPERTY(QString displayedGroup READ getDisplayedGroup NOTIFY displayedGroupChanged)
    Q_PROPERTY(QString displayedGroupLabel READ getDisplayedGroupLabel NOTIFY displayedGroupChanged)

    friend class BlockBase;

public:
	/**
	 * @brief BlockManager creates an instance of this manager
	 * @param controller pointer to the MainController
	 */
    explicit BlockManager(MainController* controller);

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
     * @brief isInViewport returns wether the block is inside the visible viewport
     * @param workspace a pointer to the GUI items that represents the viewport
     * @param block pointer to a block
     * @return true if inside viewport
     */
    bool isInViewport(QQuickItem* workspace, BlockInterface* block) const;

    /**
     * @brief setHideBlocksOutsideViewports defines if blocks outside the viewport should be rendered
     * or not (can be used to force rendering all blocks before zooming out)
     * @param value true to hide blocks outside the viewport (prevent rendering them)
     */
    void setHideBlocksOutsideViewports(bool value);

    QString getDisplayedGroup() const { return m_displayedGroup; }
    QString getDisplayedGroupLabel();
    void setDisplayedGroup(QString group);
    void goToParentGroup();
    void setGroupOfBlock(BlockInterface* block, QString group);

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
     * @return a pointer to the created block
	 */
    BlockInterface* addBlockByNameQml(QString blockType);
	/**
     * @brief deleteAllBlocks deletes all blocks (forced)
     * @param immediate if true, blocks are deleted immediatley (never use when a signal from the same blocks is involved, i.e. in BlockBase::deletedByUser()!)
	 */
    void deleteAllBlocks(bool immediate=false);
	/**
	 * @brief deleteBlock deletes a specific block by a pointer to it
	 * @param block pointer to the block
	 * @param forced true if it should be removed even if it is still in use (i.e. in a group)
     * @param noRestore if true, the block can't be restored with the restore-button
     * @param immediate if true, block is deleted immediatley (never use when a signal from the same block is involved, i.e. in BlockBase::deletedByUser()!)
	 */
    void deleteBlock(BlockInterface* block, bool forced=false, bool noRestore=false, bool immediate=false);
	/**
	 * @brief deleteBlock deletes a specific block by its UID
	 * @param uid unique id of the block
	 * @param forced true if it should be removed even if it is still in use (i.e. in a group)
     * @param noRestore if true, the block can't be restored with the restore-button
     * @param immediate if true, block is deleted immediatley (never use when a signal from the same block is involved, i.e. in BlockBase::deletedByUser()!)
	 */
    void deleteBlock(QString uid, bool forced=false, bool noRestore=false, bool immediate=false);
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

    void playClickSound();
    void playClickUpSound();

    void destroyAllGuiItemsExcept(BlockInterface* exception=nullptr);


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

    // ----------------- Smoke Tests:

    void startRandomConnectionTest();

    void stopRandomConnectionTest();

    void toggleRandomConnectionTest();

    bool randomConnectionTestIsRunning() const { return m_randomConnectionTimer.isActive(); }

    /**
     * @brief makeRandomConnection creates a connection between two random nodes,
     * used for random smoke tests
     */
    void makeRandomConnection();

signals:
	/**
	 * @brief focusChanged emitted when the focused block changed (or the focus was released)
	 */
	void focusChanged();

    /**
     * @brief blockInstanceCountChanged emitted when the number of block instances changed
     */
    void blockInstanceCountChanged();

    void displayedGroupChanged();

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
     * @brief m_displayedGroup is the UID of the currently displayed group
     */
    QString m_displayedGroup;
    /**
     * @brief m_blocksInDisplayedGroup contains all blocks of the currently displayed group
     */
    QVector<QPointer<BlockInterface>> m_blocksInDisplayedGroup;
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

    /**
     * @brief m_randomConnectionTimer when activated it triggers the creation of random
     * node connections each few frames
     */
    QTimer m_randomConnectionTimer;

    QSoundEffect m_clickSound;
    QSoundEffect m_clickUpSound;
};

#endif // BLOCKMANAGER_H
