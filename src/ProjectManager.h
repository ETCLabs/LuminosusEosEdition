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

#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QObject>

// forward declaration to prevent dependency on it
class MainController;


namespace ProjectManagerConstants {
/**
 * @brief formatVersion is the version of the format used to save projects
 */
static const double formatVersion = 0.1;
/**
 * @brief fileEnding is the file suffix of project files as a string
 */
static const QString fileEnding = ".lpr";
/**
 * @brief subdirectory is the name of the dir to save the projects in inside the app data dir
 */
static const QString subdirectory = "projects";
/**
 * @brief defaultProjectName is the name of the default project (i.e. that is created on first start)
 */
static const QString defaultProjectName = "default";

}

/**
 * @brief The ProjectManager class is responsible for loading and saving projects.
 */
class ProjectManager : public QObject
{
	Q_OBJECT

	Q_PROPERTY(QStringList projectList READ getProjectList NOTIFY projectListChanged)

public:
	/**
	 * @brief ProjectManager creates an instance and initializes the attributes
	 * @param controller a pointer to the MainController
	 */
	ProjectManager(MainController* controller);

signals:
	/**
	 * @brief projectChanged emitted when the currently loaded project changed
	 */
	void projectChanged();
	/**
	 * @brief projectListChanged emitted when the list of projects changed
	 * (because of adding or deleting projects)
	 */
	void projectListChanged();

public slots:
	// functions accessible from Projects Drawer in GUI:

	/**
	 * @brief setCurrentProject sets the current project by its name
	 * @param name of the project file without fileending
	 * @param createIfNotExist true to create a project with this name if it doesn't already exists
	 * @param animated true to animate the loading of the blocks
	 */
	void setCurrentProject(QString name, bool createIfNotExist = true, bool animated = true);
	/**
	 * @brief createAndLoad creates a new projects and loads it
	 * @param name the name of the project to create
	 */
	void createAndLoad(QString name);
	/**
	 * @brief copyCurrentAndLoad copies the currently loaded project,
	 * saves it with a different name and loads it
	 * @param name of the copy of the project
	 */
	void copyCurrentAndLoad(QString name);
	/**
	 * @brief deleteProject deletes a project
	 * @param name of the project to delete (filename without fileending)
	 */
	void deleteProject(QString name);

	// functions called by MainController:
	/**
	 * @brief saveCurrentProject saves the current state in the file of the currently loaded project
	 */
	void saveCurrentProject();

	// getter (also used in QML):
	/**
	 * @brief getCurrentProjectName returns the name of the loaded project (filename without fileending)
	 * @return a project name
	 */
	QString getCurrentProjectName() const { return m_currentProjectName; }

	/**
	 * @brief getProjectList returns a list of all available projects
	 * @return a list of project names
	 */
	QStringList getProjectList() const;


private:
	/**
	 * @brief loadProjectState loads a project from a file (internal, use setCurrentProject() instead)
	 * @param name of the project (filename without fileending)
	 * @param animated true to animate the loading of the blocks
	 */
	void loadProjectState(QString name, bool animated = true);
	/**
	 * @brief setLoadingStateFor activates the "loading state" for a given number of milliseconds
	 *  - the "loading state" prevents other projects from being saved or loaded
	 * @param ms number of milliseconds after wich to release the loading state
	 */
	void setLoadingStateFor(int ms);

	/**
	 * @brief saveStateAsProject saves the current state in a project file
	 * (internal, use saveCurrentProject() instead)
	 * @param name of the project (filename without fileending)
	 */
	void saveStateAsProject(QString name);

	/**
	 * @brief correctCaseIfPossible tries to find an existing project with the same letters as
	 * the provided string (but maybe in different case) and returns the name of it
	 * @param name to look for (case insensitive)
	 * @return the name of an existing project or the name parameter if there is no
	 * matching existing project
	 */
	QString correctCaseIfPossible(QString name);

	/**
	 * @brief setPlanePosition sets the position of the InfinitePlane
	 * @param x position in px
	 * @param y position in px
	 */
	void setPlanePosition(int x, int y);


protected:
	/**
	 * @brief m_controller a pointer to the MainController
	 */
	MainController* m_controller;

	/**
	 * @brief m_currentProjectName the name of the currently loaded project
	 */
	QString m_currentProjectName;
	/**
	 * @brief m_loadingIsInProgress true if in loading state
	 *  - the "loading state" prevents other projects from being saved or loaded
	 */
	bool m_loadingIsInProgress;

};

#endif // PROJECTMANAGER_H
