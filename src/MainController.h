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

#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "OutputManager.h"
#include "FileSystemManager.h"
#include "Engine.h"
#include "AudioEngine.h"
#include "OutputManager.h"
#include "PowermateListener.h"
#include "MidiManager.h"
#include "block_data/BlockList.h"
#include "block_data/BlockManager.h"
#include "OSCNetworkManager.h"
#include "GlobalOscCommands.h"
#include "ProjectManager.h"
#include "LogManager.h"
#include "eos_specific/EosOSCManager.h"
#include "eos_specific/EosCueListManager.h"
#include "eos_specific/EosActiveChannelsManager.h"
#include "UpdateManager.h"
#include "AnchorManager.h"
#include "version.h"
#include "utils.h"

#include <QObject>
#include <QtQuick>
#include <QWindow>
#include <QPointer>

#include <vector>

/**
 * @brief The LuminosusConstants namespace contains all constants used in more than one component.
 */
namespace LuminosusConstants {
    /**
     * @brief triggerThreshold is the minium value to trigger an action in a node [0...1]
     */
	static const double triggerThreshold = 1.0 / 256;

    /**
     * @brief blockUpdateFps is the rate the block logic gets updated by the engine in Hz
     */
    static const int blockUpdateFps = 50;

    static const QString defaultBackgroundName = "blueprint_grey_tiled.jpg";
}


/**
 * @brief The MainController class initializes all components and makes them available by getter functions.
 */
class MainController : public QObject
{
	Q_OBJECT

    Q_PROPERTY(bool sendCustomOscToEos READ getSendCustomOscToEos WRITE setSendCustomOscToEos NOTIFY sendCustomOscToEosChanged)
    Q_PROPERTY(QString backgroundName READ getBackgroundName WRITE setBackgroundName NOTIFY backgroundNameChanged)

public:

    /**
     * @brief MainController creates a MainController object and initializes all Manager classes
     * @param qmlEngine is the QML enigne to use to create the GUI
     * @param parent the QObject parent
     */
    explicit MainController(QQmlApplicationEngine& qmlEngine, QObject *parent = 0);


signals:
    /**
     * @brief openBlockSettings can be emitted to tell the GUI to open the Block Settings area
     */
    void openBlockSettings();
    /**
     * @brief sendCustomOscToEosChanged emitted when "Send Custom OSC to Eos" setting changed
     */
    void sendCustomOscToEosChanged();

    void backgroundNameChanged();



public slots:

    // ----------------------- Getter Methods for Manager classes:
    /**
     * @brief qmlEngine is a Getter for the only QQmlApplicationEngine instance to use in this application
     * @return  the QQmlApplicationEngine object used to generate the GUI
     */
	QQmlApplicationEngine* qmlEngine() { return &m_qmlEngine; }
    /**
     * @brief logManager is a Getter for the only LogManager instance to use in this application
     * @return a pointer to a LogManager instance
     */
    LogManager* logManager() { return &m_logManager; }
    /**
     * @brief engine is a Getter for the only Engine instance to use in this application
     * @return a pointer to a LogManager instance
     */
    Engine* engine() { return &m_engine; }
    /**
     * @brief audioEngine is a Getter for the only AudioEngine instance to use in this application
     * @return a pointer to a AudioEngine instance
     */
    AudioEngine* audioEngine() { return &m_audioEngine; }
    /**
     * @brief output is a Getter for the only OutputManager instance to use in this application
     * @return a pointer to a OutputManager instance
     */
    OutputManager* output() { return &m_output; }
    /**
     * @brief dao is a Getter for the only FileSystemManager instance to use in this application
     * @return a pointer to a FileSystemManager instance
     */
	FileSystemManager* dao() { return &m_dao; }
    /**
     * @brief blockManager is a Getter for the only BlockManager instance to use in this application
     * @return a pointer to a BlockManager instance
     */
	BlockManager* blockManager() { return &m_blockManager; }
    /**
     * @brief powermate is a Getter for the only PowermateListener instance to use in this application
     * @return a pointer to a PowermateListener instance
     */
	PowermateListener* powermate() { return &m_powermate; }
    /**
     * @brief midi is a Getter for the only MidiManager instance to use in this application
     * @return a pointer to a MidiManager instance
     */
	MidiManager* midi() { return &m_midi; }
    /**
     * @brief osc returns the OSCNetworkManager instance to use for custom OSC commands
     * @return a pointer to an OSCNetworkManager instance
     */
	OSCNetworkManager* osc() { return &m_osc; }
    /**
     * @brief eosConnection returns the OSCNetworkManager instance to use for Eos connection OSC commands
     * @return a pointer to an OSCNetworkManager instance
     */
    OSCNetworkManager* eosConnection() { return &m_eosConnection; }
    /**
     * @brief eosManager is a Getter for the only EosOSCManager instance to use in this application
     * @return a pointer to an EosOSCManager instance
     */
    EosOSCManager* eosManager() { return &m_eosManager; }
    /**
     * @brief projectManager is a Getter for the only ProjectManager instance to use in this application
     * @return a pointer to a ProjectManager instance
     */
	ProjectManager* projectManager() { return &m_projectManager; }
    /**
     * @brief cueListManager is a Getter for the only EosCueListManager instance to use in this application
     * @return a pointer to an EosCueListManager instance
     */
    EosCueListManager* cueListManager() { return &m_eosCueListManager; }
    /**
     * @brief activeChannelsManager is a Getter for the only EosActiveChannelsManager instance to use in this application
     * @return a pointer to an EosActiveChannelsManager instance
     */
    EosActiveChannelsManager* activeChannelsManager() { return &m_activeChannelsManager; }
    /**
     * @brief updateManager is a Getter for the only UpdateManager instance to use in this application
     * @return a pointer to an UpdateManager instance
     */
    UpdateManager* updateManager() { return &m_updateManager; }
    /**
     * @brief anchorManager is a Getter for the only AnchorManager instance to use in this application
     * @return  a pointer to an AnchorManager instance
     */
    AnchorManager* anchorManager() { return &m_anchorManager; }



    // -----------------------------  State / Projects:
    /**
     * @brief saveAll saves the application state and current project to the filesystem
     */
    void saveAll();
    /**
     * @brief restoreApp restores the application state and last loaded project from the filesystem,
     * to be used at application startup only
     */
	void restoreApp();
    /**
     * @brief setKeyboardFocusToPlane sets the Qt keyboard focus to the workspace plane,
     * can be used to unfocus a GUI item but preserve keyboard shortcuts
     */
	void setKeyboardFocusToPlane();



    // ----------------------------- Start / Exit:
    /**
     * @brief onExit performs actions to be done before a normal application exit
     * (stops the Engine, saves everything and deletes all blocks so that they can perform
     * "on exit" actions)
     */
    void onExit();
    /**
     * @brief showLockScreen shows the lockscreen (not yet implemented)
     */
    void showLockScreen();
    /**
     * @brief onFirstStart is called only on first appliction start and loads the tutorial project
     */
    void onFirstStart();



    // ---------------------------- Powermate / universal Controller:
    /**
     * @brief onControllerRotated forwards the event to the currently focused block
     * @param value of the event
     * @param pressed true if powermate is pressed
     */
    void onControllerRotated(double value, bool pressed);
    /**
     * @brief onControllerPressed forwards the event to the currently focused block
     */
    void onControllerPressed();
    /**
     * @brief onControllerReleased forwards the event to the currently focused block
     * @param duration how long the powermate was pressed
     */
    void onControllerReleased(double duration);



    // ------------------------ External Input
    /**
     * @brief connectWaitingControlToExternalInput connects the waiting GUI item to the
     * specified external input event
     * @param inputUid the external input event id to connect to
     */
    void connectWaitingControlToExternalInput(QString inputUid);
    /**
     * @brief cancelWaitForExternalEvent cancels the current GUI item to input event
     * connection attempt (clears the waiting object and midi callback)
     */
    void cancelWaitForExternalEvent();
    /**
     * @brief waitForExternalEvent starts waiting for a GUI control to be touched that will be
     * connected to the next input event
     */
    void waitForExternalEvent();
    /**
     * @brief waitToReleaseNextControl starts waiting for a GUI control to be touched that will be
     * released from all external input events
     */
    void waitToReleaseNextControl();
    /**
     * @brief registerGuiControl register a GUI control so that it can be
     * connected to an external event later
     * @param item the GUI item to register
     */
    void registerGuiControl(QQuickItem* item);
    /**
     * @brief unregisterGuiControl unregister a GUI control previously
     * registered with registerGuiControl() because it doesn't any longer exist
     * @param item the GUI item to unregister
     */
    void unregisterGuiControl(QQuickItem* item);
    /**
     * @brief getControlFromUid returns a pointer to the control with the given control UID,
     * or a nullptr if the control UID doesn't exist
     * @param controlUid of a GUI item previously registered with registerGuiControl()
     * @return a pointer to a GUI item or nullptr
     */
    QQuickItem* getControlFromUid(QString controlUid);
    /**
     * @brief checkForExternalInputConnection checks if application is waiting for a GUI control
     * to be touched to connect it to an external event
     * @param controllerUid the UID of the control
     */
    void checkForExternalInputConnection(QString controllerUid);



    // --------------------------- Getter / Setter:
    /**
     * @brief getSendCustomOscToEos returns if custom OSC commands are send to the connected Eos
     * or to the custom OSC connection
     * @return true if custom OSC commands are send to the connected Eos
     */
    bool getSendCustomOscToEos() const { return m_sendCustomOscToEos; }
    /**
     * @brief setSendCustomOscToEos sets if custom OSC commands are send to the connected Eos
     * or to the custom OSC connection
     * @param value true to send custom OSC commands to the connected Eos
     */
    void setSendCustomOscToEos(bool value) { m_sendCustomOscToEos = value; emit sendCustomOscToEosChanged(); }

    QString getBackgroundName() const { return m_backgroundName; }
    void setBackgroundName(QString value);



    // --------------------------- Used in QML:
    /**
     * @brief getWindowGeometryOfItem returns the geometry of the window that contains the item
     * @param item a GUI item
     * @return position and size of a window
     */
    QRect getWindowGeometryOfItem(const QVariant& item) const;
    /**
     * @brief setPropertyWithoutChangingBindings sets a property of a QQuickItem to a value
     * without changing its bindings (in opposite to setting it from QML directly)
     * @param item that contains the property
     * @param name of the property
     * @param value to set the property to
     */
    void setPropertyWithoutChangingBindings(const QVariant& item, QString name, QVariant value) const;
    /**
     * @brief getGuiScaling returns the GUI scaling factor (-> dp unit) in addition to the Qt
     * internal scaling
     * @return the factor the GUI is scaled with
     */
    double getGuiScaling() const;
    /**
     * @brief setGuiScaling sets the GUI scaling factor (-> dp unit) in addition to the Qt
     * internal scaling
     * @param value factor the GUI should be scaled with
     */
	void setGuiScaling(double value) const;
    /**
     * @brief setQmlContextProperty convenience method to set a property in the current QML context
     *  to a specific value
     * @param propertyName name of the property to set
     * @param value to set the property to
     */
	void setQmlContextProperty(QString propertyName, QVariant value) const;
    /**
     * @brief getVersionString returns the version string to be displayed in GUI
     * @return a version string
     */
	QString getVersionString() const;
    /**
     * @brief pointIsInTrashArea checks if a point in the GUI is in the area of the trash icon
     * @param x position in screen coordinates
     * @param y position in screen coordinates
     * @return true if the point is in the trash area
     */
	bool pointIsInTrashArea(qreal x, qreal y) const;
    /**
     * @brief showToast shows a toast message at the bottom of the screen
     * @param text to show
     * @param isWarning true if this is a warning message and should have a red background
     */
	void showToast(QString text, bool isWarning = false) const;

    /**
     * @brief getMainWindow returns the main window
     * (result is cached)
     * @return a pointer to the main window or null if it hasn't been created yet
     */
	QQuickWindow* getMainWindow() const;

private:
    /**
     * @brief restoreWindowGeometry sets the window position and geometry based on an appState
     * @param appState containing the window geometry
     */
	void restoreWindowGeometry(const QJsonObject& appState) const;

protected:
	// Engines / Managers:
    QQmlApplicationEngine&          m_qmlEngine;  //!< QQmlApplicationEngine object, created in main.cpp
    LogManager						m_logManager;  //!< LogManager instance
    Engine							m_engine;  //!< Engine instance
    AudioEngine						m_audioEngine;  //!< AudioEngine instance
    OutputManager					m_output;  //!< OutputManager instance
    FileSystemManager				m_dao;  //!< FileSystemManager instance
    BlockManager					m_blockManager;  //!< BlockManager instance
    PowermateListener				m_powermate;  //!< PowermateListener instance
    MidiManager						m_midi;  //!< MidiManager instance
    OSCNetworkManager				m_osc;  //!< OSCNetworkManager instance for custom OSC commands
    OSCNetworkManager               m_eosConnection;  //!< OSCNetworkManager instance for the Eos connection
    EosOSCManager                   m_eosManager;  //!< EosOSCManager instance
    GlobalOscCommands				m_globalOscCommands;  //!< EosOSCManager instance
    ProjectManager					m_projectManager;  //!< GlobalOscCommands instance
    EosCueListManager               m_eosCueListManager;  //!< EosCueListManager instance
    EosActiveChannelsManager        m_activeChannelsManager;  //!< EosActiveChannelsManager instance
    UpdateManager                   m_updateManager;  //!< UpdateManager instance
    AnchorManager                   m_anchorManager;  //!< AnchorManager instance

	// Attributes:
    /**
     * @brief m_waitingForExternalInput contains a GUI control UID that waits to be connected
     * to an external event or nothing
     */
    QString                         m_waitingForExternalInput;
    /**
     * @brief m_waitingForControl true if application is waiting for a GUI control
     * to connect to a external event
     */
    bool                            m_waitingForControl;
    /**
     * @brief m_releaseNextControl true if application is waiting for a GUI control
     * to be released from all external events
     */
    bool                            m_releaseNextControl;
    /**
     * @brief m_registeredControls map of control UIDs and pointer to the control items
     */
    std::map<QString, QPointer<QQuickItem>>  m_registeredControls;
    /**
     * @brief m_saveTimer timer to save app state on a regular basis
     */
    QTimer                          m_saveTimer;
    /**
     * @brief m_sendCustomOscToEos true if custom OSC commands are send to the connected Eos console,
     * false if they are send to the custom OSC connection
     */
    bool                            m_sendCustomOscToEos;
    /**
     * @brief m_mainWindow cached result of getMainWindow(), is null before the method has been
     * called at least once
     */
    mutable QPointer<QQuickWindow>  m_mainWindow;

    QString m_backgroundName;

};

#endif // MAINCONTROLLER_H
