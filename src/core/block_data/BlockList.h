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

#ifndef BLOCKLIST
#define BLOCKLIST

#include "core/block_data/BlockInterface.h"
#include "utils.h"

#include <QObject>
#include <QString>
#include <QVector>
#include <map>

/**
 * @brief The BlockList class manages the information about all available block types.
 */
class BlockList : public QObject {

	Q_OBJECT

public:
    /**
     * @brief BlockList creates a BlockList object and adds all available blocks
     */
    explicit BlockList(MainController* controller);

public slots:
	/**
	 * @brief blockExists checks if a block type is available
	 * @param name type of the block to check
	 * @return true if the block is available
	 */
	bool blockExists(QString name) const;

	/**
	 * @brief getBlockInfoByName returns the BlockInfo of a block by its type name
	 * @param name type of the block to check
	 * @return BlockInfo struct
	 */
	const BlockInfo& getBlockInfoByName(QString name) const;

    /**
     * @brief getNameInUi return the name of the blokc to show in the UI
     * @param name type of the block
     * @return name in UI as QString
     */
    QString getNameInUi(QString name) const;

	/**
	 * @brief getJsonBlockModel creates a JSON array of all available blocks
	 * @return the string of a JSON array
	 */
    QString getJsonBlockModel() const;

	/**
	 * @brief getSearchResult creates a JSON array of all blocks that match the query
	 * @param query to find matching blocks
	 * @return the string of a JSON array
	 */
    QString getSearchResult(QString query) const;

	/**
	 * @brief getAllBlockTypes returns a list of all available block type names (i.e. for debugging)
	 * @return a list of block type names
	 */
    QStringList getAllBlockTypes() const;

private:
	/**
	 * @brief checkAvailableDependencies adds all available dependencies to m_availableDependencies
	 */
	void checkAvailableDependencies();

	/**
	 * @brief addAllBlocks adds all available blocks to m_blockNames
	 */
	void addAllBlocks();

	/**
	 * @brief addBlock adds th info of a single block to m_blockNames
	 * @param info Block info struct to add
	 */
	void addBlock(const BlockInfo& info);

	/**
	 * @brief blockMatchesQuery checks if a block matches a query
	 * (i.e. the query is part of the name etc.)
	 * @param blockInfo the block
	 * @param query i.e. text from a search field (should be all lower case)
	 * @return true if the block matches that query
	 */
	bool blockMatchesQuery(const BlockInfo& info, const QString& query) const;

    bool blockIsVisible(const BlockInfo& info) const;


protected:
    /**
     * @brief m_controller a pointer to the MainController
     */
    MainController* const m_controller;
	/**
     * @brief m_availabilityRequirements is a set of all available hard- and software dependencies
	 */
    QSet<AvailabilityRequirement> m_availabilityRequirements;

    QSet<VisibilityRequirement> m_visibilityRequirements;

	/**
	 * @brief m_blockNames maps the block type name to its BlockInfo struct
	 */
	std::map<QString, BlockInfo> m_blockNames;

    /**
     * @brief m_orderedBlockList contains all BlockInfos in the order they should appear in the UI
     */
    QVector<BlockInfo> m_orderedBlockList;

	/**
	 * @brief m_jsonBlockModel is the cached result of getJsonBlockModel for all blocks
	 *
	 * Is mutable because it is a cached result that doesn't change the objects state.
	 */
	mutable QString m_jsonBlockModel;

    /**
     * @brief m_jsonBlockModelDeveloperMode true if developer mode was enabled when model was created
     */
    mutable bool m_jsonBlockModelDeveloperMode;
};



#endif // BLOCKLIST

