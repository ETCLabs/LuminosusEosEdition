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

#include "MainController.h"
#include "block_data/BlockManager.h"
#include "Nodes.h"
#include "utils.h"


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

void ProjectManager::setCurrentProject(QString name, bool createIfNotExist, bool animated) {
    // try to adapt name to an existing project with same name except for the case:
	name = correctCaseIfPossible(name);
	if (name == m_currentProjectName) return;
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
	if (!m_currentProjectName.isEmpty()) saveStateAsProject(m_currentProjectName);
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
	m_controller->blockManager()->deleteAllBlocks();
	// reset plane position:
	setPlanePosition(0, 0);
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

void ProjectManager::importProjectFile(QString filename, bool load) {
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
        m_controller->dao()->importFile(filename, PMC::subdirectory);
    }
    emit projectListChanged();

    // load project:
    if (load) {
        QString projectName = QFileInfo(filename).baseName();
        setCurrentProject(projectName, /*createIfNotExist*/ false);
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
    QQuickWindow* window = m_controller->getMainWindow();
    if (!window) return;
    midpoint -= QPoint(window->width() / 2, window->height() / 2);
    setPlanePosition(midpoint.x() * (-1), midpoint.y() * (-1));
}

void ProjectManager::setPlanePosition(int x, int y) const {
    QObject* rootObject = (QObject*)(m_controller->qmlEngine()->rootObjects()[0]);
    QQuickItem* plane = (QQuickItem*)(rootObject->findChild<QObject*>("plane"));
    plane->setX(x);
    plane->setY(y);
    QQuickItem* planeController = (QQuickItem*)(rootObject->findChild<QObject*>("planeController"));
    QMetaObject::invokeMethod(planeController, "onExternalPositionChange");
}

// ------------------------------- Private ----------------------------

void ProjectManager::loadProjectState(QString name, bool animated) {
    // try to load project file:
	QJsonObject projectState = m_controller->dao()->loadJsonObject(PMC::subdirectory, name + PMC::fileEnding);
	if (projectState.empty()) {
		qWarning() << "Project file does not exist or is empty.";
		return;
	}

    // project file exists -> start loading:
    m_loadingIsInProgress = true;

	// reset workspace:
	BlockManager* blockManager = m_controller->blockManager();
	blockManager->deleteAllBlocks();  // TODO: reuse blocks with same UID

	// restore project related settings:
	setPlanePosition(projectState["planeX"].toDouble(), projectState["planeY"].toDouble());
    // TODO: restore plane scale
    m_controller->anchorManager()->setState(projectState["anchors"].toObject());
    m_controller->setBackgroundName(projectState["backgroundName"].toString());

    // restoring the blocks often takes longer than one frame
    // to revent frames being skipped, the blocks are created in multiple chuncks

    // copy block states to temporary member variable:
    m_blocksToBeCreated.clear();
	for (QJsonValueRef blockStateRef: projectState["blocks"].toArray()) {
        m_blocksToBeCreated.append(blockStateRef.toObject());
    }
    // copy connections to be made after blocks have been created to memeber variable:
    m_connectionsToBeMade = projectState["connections"].toArray();

    // create first chunk of blocks in the next frame (in 10ms)
    QTimer::singleShot(10, [this, animated]() { this->createChunckOfBlocks(animated); } );
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

    // prevent snapshots being saved before the project is completely loaded
    // including animations:
    releaseLoadingStateAfter(500);
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

	// init JSON object:
	QJsonObject projectState;
	projectState["version"] = ProjectManagerConstants::formatVersion;

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
	QObject* rootObject = (QObject*)(m_controller->qmlEngine()->rootObjects()[0]);
	QQuickItem* plane = (QQuickItem*)(rootObject->findChild<QObject*>("plane"));
	projectState["planeX"] = plane->x();
	projectState["planeY"] = plane->y();

    projectState["anchors"] = m_controller->anchorManager()->getState();
    projectState["backgroundName"] = m_controller->getBackgroundName();

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
