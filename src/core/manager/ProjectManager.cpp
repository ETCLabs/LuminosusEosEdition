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

#include "ProjectManager.h"

#include "core/MainController.h"
#include "core/manager/BlockManager.h"
#include "core/Nodes.h"
#include "utils.h"

#include <QFileInfo>
#include <QQuickWindow>


// create a shorter alias for the constants namespace:
namespace PMC = ProjectManagerConstants;

// ------------------------------- Public ----------------------------

ProjectManager::ProjectManager(MainController* controller)
	: QObject(controller)
	, m_controller(controller)
	, m_currentProjectName("")
	, m_loadingIsInProgress(false)
{

}

void ProjectManager::setCurrentProject(QString name, bool createIfNotExist, bool animated, bool reload) {
    if (m_controller->blockManager()->randomConnectionTestIsRunning()) {
        qWarning() << "Random connection test is running, can't change project.";
        return;
    }
    // try to adapt name to an existing project with same name except for the case:
	name = correctCaseIfPossible(name);
    if (name == m_currentProjectName && !reload) return;
	// loading new projects is only allowed if previous loading is completed:
	if (m_loadingIsInProgress) return;
	// check if file for project exists:
    if (!m_controller->dao()->fileExists(PMC::subdirectory, name + PMC::fileEnding)) {
		if (createIfNotExist) {
            qDebug() << "New project created: " << name;
			return createAndLoad(name);
		} else {
            qWarning() << "Project does not exist: " << name;
			return;
		}
	}
	// save current before load new one:
    if (!m_currentProjectName.isEmpty() && !reload) saveStateAsProject(m_currentProjectName);
	// set and load new project:
	m_currentProjectName = name;
	loadProjectState(name, animated);
	emit projectChanged();
}

void ProjectManager::createAndLoad(QString name) {
	// don't create a new project if it already exists:
	if (m_controller->dao()->fileExists(PMC::subdirectory, name + PMC::fileEnding)) {
		return setCurrentProject(name);
	}
	// save current before create new one:
	if (!m_currentProjectName.isEmpty()) saveStateAsProject(m_currentProjectName);
	// reset workspace:
	m_currentProjectName = "";
    m_controller->blockManager()->deleteAllBlocks(/*immediate*/ true);
    m_controller->midiMapping()->clearMapping();
	// reset plane position:
    m_controller->guiManager()->setWorkspacePosition(0, 0);
	// save with new name:
	saveStateAsProject(name);
	setCurrentProject(name);
	emit projectListChanged();
}

void ProjectManager::copyCurrentAndLoad(QString name) {
	// save current before create new one:
	if (m_currentProjectName.length()) saveStateAsProject(m_currentProjectName);
	// save same state with new name:
	saveStateAsProject(name);
	// switch to new copied project:
	setCurrentProject(name);
	emit projectListChanged();
}

void ProjectManager::deleteProject(QString name) {
	// check if the project to delete is the current project:
	if (name == m_currentProjectName) {
		QStringList projectFiles = getProjectList();
		projectFiles.removeOne(name);
		if (!projectFiles.isEmpty()) {
			// it is not the last project, change to another:
			QString otherProject = projectFiles.first();
			setCurrentProject(otherProject);
			m_controller->dao()->deleteFile(PMC::subdirectory, name + PMC::fileEnding);
		} else {
			// it is the only project, delete it and create new default project:
			m_controller->dao()->deleteFile(PMC::subdirectory, name + PMC::fileEnding);
			createAndLoad(PMC::defaultProjectName);
		}
	} else {
		// it is not the current project -> just delete the file:
		m_controller->dao()->deleteFile(PMC::subdirectory, name + PMC::fileEnding);
	}
	emit projectListChanged();
}

void ProjectManager::saveCurrentProject() {
    saveStateAsProject(m_currentProjectName);
}

QJsonObject ProjectManager::getCurrentProjectState() const {
    // saving the state is only allowed if previous loading is completed:
    if (m_loadingIsInProgress) {
        qWarning() << "Can't save project while it is loaded.";
        return QJsonObject();
    }

    // init JSON object:
    QJsonObject projectState;
    projectState["version"] = ProjectManagerConstants::formatVersion;
    projectState["fileName"] = m_currentProjectName;

    // save block states:
    QJsonArray blocks;
    BlockManager* blockManager = m_controller->blockManager();
    for (BlockInterface* block: blockManager->getCurrentBlocks()) {
        blocks.append(blockManager->getBlockState(block));
    }
    projectState["blocks"] = blocks;

    // save connections between blocks:
    QJsonArray connections;
    for (BlockInterface* block: blockManager->getCurrentBlocks()) {
        for (QJsonValueRef connectionRef: block->getConnections()) {
            connections.append(connectionRef.toString());
        }
    }
    projectState["connections"] = connections;

    // save anything else project related:
    QQuickItem* workspace = m_controller->guiManager()->getWorkspaceItem();
    const double dp = m_controller->guiManager()->getGuiScaling();
    projectState["planeX"] = workspace->x() / dp;
    projectState["planeY"] = workspace->y() / dp;

    projectState["displayedGroup"] = m_controller->blockManager()->getDisplayedGroup();
    projectState["anchors"] = m_controller->anchorManager()->getState();
    projectState["backgroundName"] = m_controller->guiManager()->getBackgroundName();
    projectState["midiMapping"] = m_controller->midiMapping()->getState();

    return projectState;
}

void ProjectManager::reloadCurrentProject() {
    loadProjectState(m_currentProjectName, /*animated*/ false);
}

QStringList ProjectManager::getProjectList() const {
	QStringList projectFiles = m_controller->dao()->getFilenames(PMC::subdirectory, "*" + PMC::fileEnding);
	QStringList projectNames;
	for (int i=0; i<projectFiles.count(); ++i) {
		QString filename = projectFiles.at(i);
		filename.remove(PMC::fileEnding);
		projectNames.append(filename);
	}
    return projectNames;
}

void ProjectManager::importProjectFile(QString filename, bool load, bool overwrite) {
    // import file to config directory:
#ifdef Q_OS_WIN
    // under Windows, filename often starts with three slashes:
    if (filename.startsWith("file:///")) {
        filename.remove("file:///");
    }
#endif
    if (filename.startsWith("file://")) {
        filename.remove("file://");
    }
    qDebug() << "Import project " << filename;
    if (!filename.isEmpty()) {
        m_controller->dao()->importFile(filename, PMC::subdirectory, overwrite);
    }
    emit projectListChanged();

    // load project:
    if (load) {
        QString projectName = QFileInfo(filename).baseName();
        setCurrentProject(projectName, /*createIfNotExist*/ false, /*animated*/ true, /*reload*/ true);
    }
}

void ProjectManager::exportCurrentProjectTo(QString filename) const {
#ifdef Q_OS_WIN
    // under Windows, filename often starts with three slashes:
    if (filename.startsWith("file:///")) {
        filename.remove("file:///");
    }
#endif
    if (filename.startsWith("file://")) {
        filename.remove("file://");
    }
    if (!filename.endsWith(PMC::fileEnding)) {
        filename.append(PMC::fileEnding);
    }
    qDebug() << "Export project to " << filename;
    if (!filename.isEmpty()) {
        m_controller->dao()->exportFile(PMC::subdirectory, m_currentProjectName + PMC::fileEnding, filename);
    }
}

QStringList ProjectManager::getFilenameFilters() const {
    QStringList filters;
    filters.append("Luminosus Projects (*" + PMC::fileEnding + ")");
    return filters;
}

void ProjectManager::centerViewOnBlocks() {
    QPoint midpoint = m_controller->blockManager()->getBlocksMidpoint();
    QQuickWindow* window = m_controller->guiManager()->getMainWindow();
    if (!window) return;
    midpoint -= QPoint(window->width() / 2, window->height() / 2);
    m_controller->guiManager()->setWorkspacePosition(midpoint.x() * (-1), midpoint.y() * (-1));
}

void ProjectManager::saveCombination(QString title) {
    if (title.isEmpty()) return;
    if (m_loadingIsInProgress) {
        qWarning() << "Can't save combination while project is loading.";
        return;
    }

    QJsonObject blockCombination;

    QString currentGroup = m_controller->blockManager()->getDisplayedGroup();
    QQuickItem* workspace = m_controller->guiManager()->getWorkspaceItem();
    const double dp = m_controller->guiManager()->getGuiScaling();
    double centerX = (-workspace->x() + workspace->width() / 2) / dp;
    double centerY = (-workspace->y() + workspace->height() / 2) / dp;

    // save block states:
    QJsonArray blocks;
    BlockManager* blockManager = m_controller->blockManager();
    for (BlockInterface* block: blockManager->getCurrentBlocks()) {
        if (block->getGroup() != currentGroup) continue;
        QJsonObject blockState = blockManager->getBlockState(block);
        blockState["posX"] = blockState["posX"].toDouble() - centerX;
        blockState["posY"] = blockState["posY"].toDouble() - centerY;
        blocks.append(blockState);
    }
    blockCombination["blocks"] = blocks;

    // save connections between blocks:
    QJsonArray connections;
    for (BlockInterface* block: blockManager->getCurrentBlocks()) {
        for (QJsonValueRef connectionRef: block->getConnections()) {
            if (block->getGroup() != currentGroup) continue;
            connections.append(connectionRef.toString());
        }
    }
    blockCombination["connections"] = connections;

    m_controller->dao()->saveFile(PMC::combinationsSubdirectory,
                                  title + PMC::combinationFileEnding,
                                  blockCombination);
    emit combinationsChanged();
}

void ProjectManager::removeCombination(QString title) {
    if (title.isEmpty()) return;
    m_controller->dao()->deleteFile(PMC::combinationsSubdirectory,
                                    title + PMC::combinationFileEnding);
    emit combinationsChanged();
}

void ProjectManager::addCombination(QString title) {
    if (title.isEmpty()) return;
    QJsonObject blockCombination = m_controller->dao()->loadJsonObject(PMC::combinationsSubdirectory,
                                                                   title + PMC::combinationFileEnding);
    if (blockCombination.empty()) {
        qWarning() << "Combination file does not exist or is empty.";
        return;
    }

    QString currentGroup = m_controller->blockManager()->getDisplayedGroup();
    QQuickItem* workspace = m_controller->guiManager()->getWorkspaceItem();
    const double dp = m_controller->guiManager()->getGuiScaling();
    double centerX = (-workspace->x() + workspace->width() / 2) / dp;
    double centerY = (-workspace->y() + workspace->height() / 2) / dp;

    QMap<QString, QString> newUids;

    BlockManager* blockManager = m_controller->blockManager();
    for (QJsonValueRef ref: blockCombination["blocks"].toArray()) {
        QJsonObject blockState = ref.toObject();
        blockState["posX"] = blockState["posX"].toDouble() + centerX;
        blockState["posY"] = blockState["posY"].toDouble() + centerY;
        QString oldUid = blockState["uid"].toString();
        blockState["uid"] = "";
        BlockInterface* block = blockManager->restoreBlock(blockState, /*animated*/ true, /*connectOnAdd*/ false);
        blockManager->setGroupOfBlock(block, currentGroup);
        newUids.insert(oldUid, block->getUid());
    }

    for (QJsonValueRef connectionRef: blockCombination["connections"].toArray()) {
        QString connection = connectionRef.toString();
        QStringList outputUid = connection.split("->").at(0).split("|");
        QStringList inputUid = connection.split("->").at(1).split("|");
        NodeBase* outputNode = blockManager->getNodeByUid(newUids[outputUid.at(0)] + "|" + outputUid.at(1));
        NodeBase* inputNode = blockManager->getNodeByUid(newUids[inputUid.at(0)] + "|" + inputUid.at(1));
        if (outputNode && inputNode) {
            outputNode->connectTo(inputNode);
        }
    }

    m_controller->blockManager()->updateBlockVisibility(workspace);
}

QStringList ProjectManager::getCombinations() const {
    QStringList combinationFiles = m_controller->dao()->getFilenames(PMC::combinationsSubdirectory,
                                                                 "*" + PMC::combinationFileEnding);
    QStringList combinationNames;
    for (QString filename: combinationFiles) {
        filename.remove(PMC::combinationFileEnding);
        combinationNames.append(filename);
    }
    return combinationNames;
}

// ------------------------------- Private ----------------------------

void ProjectManager::loadProjectState(QString name, bool animated) {
    if (name.isEmpty()) return;
    // try to load project file:
	QJsonObject projectState = m_controller->dao()->loadJsonObject(PMC::subdirectory, name + PMC::fileEnding);
	if (projectState.empty()) {
		qWarning() << "Project file does not exist or is empty.";
		return;
	}

    // project file exists -> start loading:
    m_loadingIsInProgress = true;

    // reset workspace:
    m_controller->blockManager()->deleteAllBlocks(/*immediate*/ true);  // TODO: reuse blocks with same UID

	// restore project related settings:
    const double dp = m_controller->guiManager()->getGuiScaling();
    m_controller->guiManager()->setWorkspacePosition(projectState["planeX"].toDouble() * dp, projectState["planeY"].toDouble() * dp);
    // TODO: restore plane scale
    m_controller->blockManager()->setDisplayedGroup(projectState["displayedGroup"].toString());
    m_controller->anchorManager()->setState(projectState["anchors"].toObject());
    m_controller->guiManager()->setBackgroundName(projectState["backgroundName"].toString());
    m_controller->midiMapping()->setState(projectState["midiMapping"].toObject());

    // restoring the blocks often takes longer than one frame
    // to revent frames being skipped, the blocks are created in multiple chuncks

    // copy block states to temporary member variable:
    m_blocksToBeCreated.clear();
	for (QJsonValueRef blockStateRef: projectState["blocks"].toArray()) {
        m_blocksToBeCreated.append(blockStateRef.toObject());
    }
    // copy connections to be made after blocks have been created to memeber variable:
    m_connectionsToBeMade = projectState["connections"].toArray();

    // create first chunk of blocks in the next frame (in 40ms)
    QTimer::singleShot(40, [this, animated]() { this->createChunckOfBlocks(animated); } );
}

void ProjectManager::createChunckOfBlocks(bool animated) {
    // this is called with QTimer by loadProjectState() or previous createChunckOfBlocks() call
    // try to create as many blocks as possible in the next 12 ms:

    HighResTime::time_point_t start = HighResTime::now();
    BlockManager* blockManager = m_controller->blockManager();
    while (!m_blocksToBeCreated.isEmpty()) {
        QJsonObject blockState = m_blocksToBeCreated.takeLast();
        blockManager->restoreBlock(blockState, animated);

        if (HighResTime::elapsedSecSince(start) * 1000 > 12) {
            // 12ms are over, continue work in next frame:
            break;
        }
    }

    if (m_blocksToBeCreated.isEmpty()) {
        // all blocks have been created -> continue with connections:
        QTimer::singleShot(8, this, SLOT(completeProjectLoading()));
    } else {
        // there are still blocks to be created:
        QTimer::singleShot(8, [this, animated]() { this->createChunckOfBlocks(animated); } );
    }

}

void ProjectManager::completeProjectLoading() {
    // this is called after all blocks have been created by createChunckOfBlocks()
    // restore block connections:
    BlockManager* blockManager = m_controller->blockManager();
    for (QJsonValueRef connectionRef: m_connectionsToBeMade) {
        QString connection = connectionRef.toString();
        QString outputUid = connection.split("->").at(0);
        QString inputUid = connection.split("->").at(1);
        NodeBase* outputNode = blockManager->getNodeByUid(outputUid);
        NodeBase* inputNode = blockManager->getNodeByUid(inputUid);
        if (outputNode && inputNode) {
            outputNode->connectTo(inputNode);
        }
    }

    emit projectLoadingFinished();

    // prevent snapshots being saved before the project is completely loaded
    // including animations:
    releaseLoadingStateAfter(500);
    QQuickItem* workspace = m_controller->guiManager()->getWorkspaceItem();
    if (workspace) {
        m_controller->blockManager()->updateBlockVisibility(workspace);
    }
    // update group label at the top:
    emit m_controller->blockManager()->displayedGroupChanged();
}

void ProjectManager::releaseLoadingStateAfter(int ms) {
	m_loadingIsInProgress = true;
	// change value back to false after ms milliseconds:
    QTimer::singleShot(ms, [this]() { this->m_loadingIsInProgress = false; } );
}

void ProjectManager::saveStateAsProject(QString name) const {
	if (name.isEmpty()) return;
	// saving the state is only allowed if previous loading is completed:
	if (m_loadingIsInProgress) return;

    QJsonObject projectState = getCurrentProjectState();

	// write file to file system:
	m_controller->dao()->saveFile(PMC::subdirectory, name + PMC::fileEnding, projectState);
}

QString ProjectManager::correctCaseIfPossible(QString name) const {
	QStringList projectNames = getProjectList();
	for (int i=0; i<projectNames.count(); ++i) {
		if (isEqualInsensitive(name, projectNames.at(i))) {
			return projectNames.at(i);
		}
	}
	return name;

}
