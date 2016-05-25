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
#include "NodeBase.h"

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
	// try to adapt name to an exisiting project with same name except for the case:
	name = correctCaseIfPossible(name);
	if (name == m_currentProjectName) return;
	// loading new projects is only allowed if previous loading is completed:
	if (m_loadingIsInProgress) return;
	// check if file for project exists:
	if (!m_controller->dao()->fileExists(PMC::subdirectory, name + PMC::fileEnding)) {
		qWarning() << "Project does not exist: " << name;
		if (createIfNotExist) {
			return createAndLoad(name);
		} else {
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

// ------------------------------- Private ----------------------------

void ProjectManager::loadProjectState(QString name, bool animated) {
	QJsonObject projectState = m_controller->dao()->loadJsonObject(PMC::subdirectory, name + PMC::fileEnding);
	if (projectState.empty()) {
		qWarning() << "Project file does not exist or is empty.";
		return;
	}
	// prevent snapshots being saved before the project is completely loaded
	// including animations:
	setLoadingStateFor(500);

	// reset workspace:
	BlockManager* blockManager = m_controller->blockManager();
	blockManager->deleteAllBlocks();  // TODO: reuse blocks with same UID

	// restore project related settings:
	setPlanePosition(projectState["planeX"].toDouble(), projectState["planeY"].toDouble());
	// TODO: restore plane scale

	// restore blocks:
	for (QJsonValueRef blockStateRef: projectState["blocks"].toArray()) {
		QJsonObject blockState = blockStateRef.toObject();
		blockManager->restoreBlock(blockState, animated);
	}

	// restore block connections:
	for (QJsonValueRef connectionRef: projectState["connections"].toArray()) {
		QString connection = connectionRef.toString();
		QString outputUid = connection.split("->").at(0);
		QString inputUid = connection.split("->").at(1);
		NodeBase* outputNode = blockManager->getNodeByUid(outputUid);
		NodeBase* inputNode = blockManager->getNodeByUid(inputUid);
		if (outputNode && inputNode) {
			outputNode->connectTo(inputNode);
		}
	}
}

void ProjectManager::setLoadingStateFor(int ms) {
	m_loadingIsInProgress = true;
	// change value back to false after ms milliseconds:
	QTimer::singleShot(ms, [this]() { this->m_loadingIsInProgress = false; } );
}

void ProjectManager::saveStateAsProject(QString name) {
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

	// write file to file system:
	m_controller->dao()->saveFile(PMC::subdirectory, name + PMC::fileEnding, projectState);
}

QString ProjectManager::correctCaseIfPossible(QString name) {
	QStringList projectNames = getProjectList();
	for (int i=0; i<projectNames.count(); ++i) {
		if (isEqualInsensitive(name, projectNames.at(i))) {
			return projectNames.at(i);
		}
	}
	return name;

}

void ProjectManager::setPlanePosition(int x, int y) {
	QObject* rootObject = (QObject*)(m_controller->qmlEngine()->rootObjects()[0]);
	QQuickItem* plane = (QQuickItem*)(rootObject->findChild<QObject*>("plane"));
	plane->setX(x);
	plane->setY(y);
	QQuickItem* planeController = (QQuickItem*)(rootObject->findChild<QObject*>("planeController"));
	QMetaObject::invokeMethod(planeController, "onExternalPositionChange");
}
