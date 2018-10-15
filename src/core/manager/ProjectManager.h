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
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include <QTimer>

// forward declaration to prevent dependency loop
class MainController;

/**
 * @brief The ProjectManagerConstants namespace contains all constants used in ProjectManager.
 */
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
     * @brief fileEnding is the file suffix of block combination files as a string
     */
    static const QString combinationFileEnding = ".lbc";
	/**
	 * @brief subdirectory is the name of the dir to save the projects in inside the app data dir
	 */
	static const QString subdirectory = "projects";
    /**
     * @brief combinationsSubdirectory is the name of the dir to save the block combinations in inside the app data dir
     */
    static const QString combinationsSubdirectory = "combinations";
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
    Q_PROPERTY(QString currentProjectName READ getCurrentProjectName NOTIFY projectChanged)
    Q_PROPERTY(QStringList combinations READ getCombinations NOTIFY combinationsChanged)

public:
	/**
	 * @brief ProjectManager creates an instance and initializes the attributes
	 * @param controller a pointer to the MainController
	 */
    explicit ProjectManager(MainController* controller);

    friend class MidiMappingManager;  // TODO: why is this required?

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

    /**
     * @brief projectLoadingFinished emitted when the creation of blocks and connections is finished
     */
    void projectLoadingFinished();

    void combinationsChanged();

public slots:
	// functions accessible from Projects Drawer in GUI:

	/**
	 * @brief setCurrentProject sets the current project by its name
     * Never call this with a signal from a block involved! (It deletes blocks immediately and
     * pending signals from blocks will lead to a crash.)
	 * @param name of the project file without fileending
     * @param createIfNotExist true to create a project with this name if it doesn't already exists (default = true)
     * @param animated true to animate the loading of the blocks (default = true)
     * @param reload true to reload the project if it is already active (default = false)
	 */
    void setCurrentProject(QString name, bool createIfNotExist = true, bool animated = true, bool reload = false);
	/**
	 * @brief createAndLoad creates a new projects and loads it
     * Never call this with a signal from a block involved! (It deletes blocks immediately and
     * pending signals from blocks will lead to a crash.)
	 * @param name the name of the project to create
	 */
	void createAndLoad(QString name);
	/**
	 * @brief copyCurrentAndLoad copies the currently loaded project,
	 * saves it with a different name and loads it
     * Never call this with a signal from a block involved! (It deletes blocks immediately and
     * pending signals from blocks will lead to a crash.)
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

    /**
     * @brief getCurrentProjectState returns the current project state
     * @return project state as JSON
     */
    QJsonObject getCurrentProjectState() const;

    /**
     * @brief reloadCurrentProject reloads the current project from file without saving it before that
     */
    void reloadCurrentProject();

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

    /**
     * @brief importProjectFile imports a JSON project file from the filesystem to the app data dir
     * @param filename path to the file
     * @param load true to instantly load the imported file (default = true)
     * @param overwrite true to overwrite projects with the same name (default = true)
     */
    void importProjectFile(QString filename, bool load=true, bool overwrite=true);

    /**
     * @brief exportCurrentProjectTo exports the currently loaded project as a JSON file to the
     * filesystem
     * @param filename path to the new file
     */
    void exportCurrentProjectTo(QString filename) const;

    /**
     * @brief getFilenameFilters returns the list of filename filters for the im- and export dialogs
     * @return a list of filename filters matching project files
     */
    QStringList getFilenameFilters() const;

    /**
     * @brief centerViewOnBlocks centers the view on the geometric middle point of all blocks
     */
    void centerViewOnBlocks();  // TODO: move this to GuiManager?

    /**
     * @brief isLoading returns if loading a project is in progress
     * @return true if loading is in progress
     */
    bool isLoading() const { return m_loadingIsInProgress; }

    // -----------------------------------------------------------------

    void saveCombination(QString title);
    void removeCombination(QString title);
    void addCombination(QString title);

    QStringList getCombinations() const;


private slots:
	/**
	 * @brief loadProjectState loads a project from a file (internal, use setCurrentProject() instead)
     * Never call this with a signal from a block involved! (It deletes blocks immediately and
     * pending signals from blocks will lead to a crash.)
	 * @param name of the project (filename without fileending)
	 * @param animated true to animate the loading of the blocks
	 */
	void loadProjectState(QString name, bool animated = true);

    /**
     * @brief createChunckOfBlocks creates as much blocks from m_blocksToBeCreated as possible
     * in 12ms, the remaining blocks are created in the next chunk
     * @param animated true if the creation should be animated
     */
    void createChunckOfBlocks(bool animated);

    /**
     * @brief completeProjectLoading called wehn all blocks have been created, restores connections
     */
    void completeProjectLoading();

	/**
	 * @brief setLoadingStateFor activates the "loading state" for a given number of milliseconds
	 *  - the "loading state" prevents other projects from being saved or loaded
	 * @param ms number of milliseconds after wich to release the loading state
	 */
    void releaseLoadingStateAfter(int ms);

private:
	/**
	 * @brief saveStateAsProject saves the current state in a project file
	 * (internal, use saveCurrentProject() instead)
	 * @param name of the project (filename without fileending)
	 */
	void saveStateAsProject(QString name) const;

	/**
	 * @brief correctCaseIfPossible tries to find an existing project with the same letters as
	 * the provided string (but maybe in different case) and returns the name of it
	 * @param name to look for (case insensitive)
	 * @return the name of an existing project or the name parameter if there is no
	 * matching existing project
	 */
    QString correctCaseIfPossible(QString name) const;


protected:
	/**
	 * @brief m_controller a pointer to the MainController
	 */
	MainController* const m_controller;

	/**
	 * @brief m_currentProjectName the name of the currently loaded project
	 */
	QString m_currentProjectName;
	/**
	 * @brief m_loadingIsInProgress true if in loading state
	 *  - the "loading state" prevents other projects from being saved or loaded
	 */
	bool m_loadingIsInProgress;

    /**
     * @brief m_blocksToBeCreated a list of blocks to be created to restore a project,
     * only used while loading a project to create the blocks in multiple chunks
     */
    QVector<QJsonObject> m_blocksToBeCreated;

    /**
     * @brief m_connectionsToBeMade list of connections to be made as soon as all blocks in
     * m_blocksToBeCreated have been created
     */
    QJsonArray m_connectionsToBeMade;

};

#endif // PROJECTMANAGER_H
