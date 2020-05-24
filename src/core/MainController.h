#ifndef MAINCONTROLLER_H
#define MAINCONTROLLER_H

#include "core/manager/GuiManager.h"
#include "core/manager/FileSystemManager.h"
#include "core/manager/Engine.h"
#include "core/manager/ProjectManager.h"
#include "core/manager/LogManager.h"
#include "core/manager/UpdateManager.h"
#include "core/manager/AnchorManager.h"
#include "core/manager/BlockManager.h"
#include "core/manager/HandoffManager.h"
#include "core/block_data/BlockList.h"
#include "light/OutputManager.h"
#include "midi/MidiManager.h"
#include "midi/MidiMappingManager.h"
#include "osc/OSCNetworkManager.h"
#include "osc/GlobalOscCommands.h"
#include "eos_specific/EosOSCManager.h"
#include "eos_specific/EosCueListManager.h"
#include "eos_specific/EosActiveChannelsManager.h"
#include "other/PowermateListener.h"
#include "other/X32Manager.h"
#include "version.h"
#include "utils.h"

#include <QObject>
#include <QQmlApplicationEngine>

// forward declaration:
class AudioEngine;
class KeyboardEmulator;
class HogOSCManager;


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
    Q_PROPERTY(bool developerMode READ getDeveloperMode WRITE setDeveloperMode NOTIFY developerModeChanged)
    Q_PROPERTY(bool clickSounds READ getClickSounds WRITE setClickSounds NOTIFY clickSoundsChanged)
    Q_PROPERTY(QString templateFileToImport READ getTemplateFileBaseName NOTIFY templateFileToImportChanged)
    Q_PROPERTY(bool forceImport READ getForceImport NOTIFY forceImportChanged)

public:

    /**
     * @brief MainController creates a MainController object and initializes all Manager classes
     * @param qmlEngine is the QML enigne to use to create the GUI
     * @param parent the QObject parent
     */
    explicit MainController(QQmlApplicationEngine& qmlEngine, QString templateFile,
                            bool forceImport = false, QObject *parent = nullptr);


signals:
    /**
     * @brief sendCustomOscToEosChanged emitted when "Send Custom OSC to Eos" setting changed
     */
    void sendCustomOscToEosChanged();

    void developerModeChanged();

    void clickSoundsChanged();

    void templateFileToImportChanged();

    void forceImportChanged();


public slots:

    // ----------------------- Getter Methods for Manager classes:
    /**
     * @brief guiManager is a Getter for the only GuiManager instance to use in this application
     * @return a pointer to a GuiManager instance
     */
    GuiManager* guiManager() { return &m_guiManager; }
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
    AudioEngine* audioEngine() { return m_audioEngine; }
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
     * @brief customOsc returns the OSCNetworkManager instance to use for custom OSC commands
     * @return a pointer to an OSCNetworkManager instance
     */
    OSCNetworkManager* customOsc() { return &m_customOsc; }
    /**
     * @brief lightingConsole returns the OSCNetworkManager instance to use for sending commands to
     * a lighting console
     * @return a pointer to an OSCNetworkManager instance
     */
    OSCNetworkManager* lightingConsole() { return &m_lightingConsoleConnection; }
    /**
     * @brief audioConsole returns the OSCNetworkManager instance to use for sending commands to
     * an audio console
     * @return a pointer to an OSCNetworkManager instance
     */
    OSCNetworkManager* audioConsole() { return &m_audioConsoleConnection; }
    /**
     * @brief eosManager is a Getter for the only EosOSCManager instance to use in this application
     * @return a pointer to an EosOSCManager instance
     */
    EosOSCManager* eosManager() { return &m_eosManager; }
    /**
     * @brief hogManager is a Getter for the only HogOSCManager instance to use in this application
     * @return a pointer to a HogOSCManager instance
     */
    HogOSCManager* hogManager() { return m_hogManager; }
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
    /**
     * @brief midiMapping is a Getter for the only MidiMappingManager instance to use in this application
     * @return  a pointer to a MidiMappingManager instance
     */
    MidiMappingManager* midiMapping() { return &m_midiMappingManager; }
    /**
     * @brief x32Manager is a Getter for the only X32Manager instance to use in this application
     * @return a pointer to a X32Manager instance
     */
    X32Manager* x32Manager() { return &m_x32Manager; }
    /**
     * @brief handoffManager is a Getter for the only HandoffManager instance to use in this application
     * @return a pointer to a HandoffManager instance
     */
    HandoffManager* handoffManager() { return &m_handoffManager; }
	/**
	 * @brief keyboardEmulator is a Getter for the only KeyboardEmulator instance to use in this application
	 * @return a pointer to a KeyboardEmulator instance
	 */
	KeyboardEmulator* keyboardEmulator() { return m_keyboardEmulator; }



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



    // ----------------------------- Start / Exit:
    /**
     * @brief onExit performs actions to be done before a normal application exit
     * (stops the Engine, saves everything and deletes all blocks so that they can perform
     * "on exit" actions)
     */
    void onExit();
    /**
     * @brief onFirstStart is called only on first appliction start and loads the tutorial project
     */
    void onFirstStart();
    /**
     * @brief askForPermissions shows dialogs to ask the user permission for various feautures
     * on Android 6.0+
     */
    void askForPermissions();



    // ---------------------------- Powermate / universal Controller:
    /**
     * @brief onControllerRotated forwards the event to the currently focused block
     * @param relativeAmount amount of rotation typicaly between 1/512 and 1/64 (accelerated)
     * @param degrees amount of rotation in degrees (not accelerated)
     * @param pressed true if powermate is pressed
     */
    void onControllerRotated(double relativeAmount, double degrees, bool pressed);
    /**
     * @brief onControllerPressed forwards the event to the currently focused block
     */
    void onControllerPressed();
    /**
     * @brief onControllerReleased forwards the event to the currently focused block
     * @param duration how long the powermate was pressed
     */
    void onControllerReleased(double duration);



    // --------------------------- Getter / Setter:
    /**
     * @brief getSendCustomOscToEos returns if custom OSC commands are send to the connected Eos
     * or to the custom OSC connection
     * @return true if custom OSC commands are send to the connected Eos
     */
    bool getSendCustomOscToEos() const { return false; /*m_sendCustomOscToEos not used anymore*/ }
    /**
     * @brief setSendCustomOscToEos sets if custom OSC commands are send to the connected Eos
     * or to the custom OSC connection
     * @param value true to send custom OSC commands to the connected Eos
     */
    void setSendCustomOscToEos(bool value) { m_sendCustomOscToEos = value; emit sendCustomOscToEosChanged(); }

    bool getClickSounds() const { return m_clickSounds; }
    void setClickSounds(bool value) { m_clickSounds = value; emit clickSoundsChanged(); }

    bool getForceImport() const { return m_forceImport; }

    QString getTemplateFileBaseName() const;
    void requestTemplateImport(QString filename);
    void onImportTemplateFileAccepted();



    // --------------------------- Used in QML:

    /**
     * @brief getDeveloperMode returns the state of the developer mode
     * @return true if developer mode is active
     */
    bool getDeveloperMode() const { return m_developerMode; }

    /**
     * @brief setDeveloperMode enables or disables the developer mode
     * @param value true to enable it
     */
    void setDeveloperMode(bool value) { m_developerMode = value; emit developerModeChanged(); }

    /**
     * @brief getVersionString returns the version string to be displayed in GUI
     * @return a version string
     */
    QString getVersionString() const;
    QString getBuildDate() const;

    void playClickSound() { m_blockManager.playClickSound(); }
    void playClickUpSound() { m_blockManager.playClickUpSound(); }

private:

protected:
    // Engines / Managers:
    GuiManager                      m_guiManager;  //!< GuiManager instance
    LogManager						m_logManager;  //!< LogManager instance
    Engine							m_engine;  //!< Engine instance
    AudioEngine*					m_audioEngine;  //!< AudioEngine instance
    OutputManager					m_output;  //!< OutputManager instance
    FileSystemManager				m_dao;  //!< FileSystemManager instance
    BlockManager					m_blockManager;  //!< BlockManager instance
    PowermateListener				m_powermate;  //!< PowermateListener instance
    MidiManager						m_midi;  //!< MidiManager instance
    OSCNetworkManager				m_customOsc;  //!< OSCNetworkManager instance for custom OSC commands
    OSCNetworkManager               m_lightingConsoleConnection;  //!< OSCNetworkManager instance for lighting console connection
    OSCNetworkManager               m_audioConsoleConnection;  //!< OSCNetworkManager instance for audio console connection
    EosOSCManager                   m_eosManager;  //!< EosOSCManager instance
    HogOSCManager*                  m_hogManager;  //!< HogOSCManager instance
    GlobalOscCommands				m_globalOscCommands;  //!< EosOSCManager instance
    ProjectManager					m_projectManager;  //!< GlobalOscCommands instance
    EosCueListManager               m_eosCueListManager;  //!< EosCueListManager instance
    EosActiveChannelsManager        m_activeChannelsManager;  //!< EosActiveChannelsManager instance
    UpdateManager                   m_updateManager;  //!< UpdateManager instance
    AnchorManager                   m_anchorManager;  //!< AnchorManager instance
    MidiMappingManager              m_midiMappingManager;  //!< MidiMappingManager instance
    X32Manager                      m_x32Manager;  //!< X32Manager instance
    HandoffManager                  m_handoffManager;  //!< HandoffManager instance
	KeyboardEmulator*				m_keyboardEmulator;  //!< KeyboardEmulator instance


	// Attributes:
    /**
     * @brief m_saveTimer timer to save app state on a regular basis
     */
    QTimer m_saveTimer;
    /**
     * @brief m_sendCustomOscToEos true if custom OSC commands are send to the connected Eos console,
     * false if they are send to the custom OSC connection
     */
    bool m_sendCustomOscToEos;
    /**
    * @brief m_developerMode true if developer mode is active
    */
    bool m_developerMode;
    /**
     * @brief m_clickSounds true if click sounds should be played
     */
    bool m_clickSounds;
    /**
     * @brief m_templateFileToImport file provided as commandline argument or via drag'n'drop that should be imported
     */
    QString m_templateFileToImport;

    bool m_forceImport;
};

#endif // MAINCONTROLLER_H
