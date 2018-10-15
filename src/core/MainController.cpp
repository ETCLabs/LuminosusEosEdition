#include "core/MainController.h"

#include "audio/AudioEngine.h"
#include "core/manager/KeyboardEmulator.h"
#include "eos_specific/HogOSCManager.h"

#include <QSysInfo>
#include <QJsonObject>
#include <QFileInfo>

#ifdef Q_OS_ANDROID
#include <QAndroidJniEnvironment>
#include <QtCore/private/qjnihelpers_p.h>
#endif

#include <string>

MainController::MainController(QQmlApplicationEngine& qmlEngine, QString templateFile, QObject* parent)
    : QObject(parent)
    , m_guiManager(this, qmlEngine)
    , m_logManager(this)
    , m_engine(this)
    , m_audioEngine(new AudioEngine(this))
    , m_output(this)
    , m_blockManager(this)
    , m_powermate()
	, m_midi(this)
    , m_customOsc(this, { OscConnectionType::Custom, OscConnectionType::Eos, OscConnectionType::Hog4, OscConnectionType::X32, OscConnectionType::X_Air })
    , m_lightingConsoleConnection(this, { OscConnectionType::Eos, OscConnectionType::Hog4 })
    , m_audioConsoleConnection(this, { OscConnectionType::X32, OscConnectionType::X_Air })
    , m_eosManager(this)
    , m_hogManager(new HogOSCManager(this))
	, m_globalOscCommands(this)
	, m_projectManager(this)
    , m_eosCueListManager(this)
    , m_activeChannelsManager(this)
    , m_updateManager(this)
    , m_anchorManager(this)
    , m_midiMappingManager(this)
    , m_x32Manager(this)
    , m_handoffManager(this)
	, m_keyboardEmulator(new KeyboardEmulator(this))
    , m_sendCustomOscToEos(true)
    , m_developerMode(false)
    , m_clickSounds(false)
    , m_templateFileToImport(templateFile)
{
    // print Qt Version to verify that the right library is loaded:
    qInfo() << "Compiled with Qt Version" << QT_VERSION_STR;
    qInfo() << "Runtime Qt Version:" << qVersion();

    // Register classes which slots should be accessible from QML:
    qmlRegisterType<GuiManager>();
    qmlRegisterType<LogManager>();
    qmlRegisterType<Engine>();
    qmlRegisterType<AudioEngine>();
	qmlRegisterType<OutputManager>();
	qmlRegisterType<FileSystemManager>();
    qmlRegisterType<BlockManager>();
	qmlRegisterType<PowermateListener>();
	qmlRegisterType<MidiManager>();
	qmlRegisterType<OSCNetworkManager>();
    qmlRegisterType<EosOSCManager>();
    qmlRegisterType<HogOSCManager>();
	qmlRegisterType<ProjectManager>();
    qmlRegisterType<EosCueListManager>();
    qmlRegisterType<EosActiveChannelsManager>();
    qmlRegisterType<UpdateManager>();
    qmlRegisterType<AnchorManager>();
    qmlRegisterType<MidiMappingManager>();
    qmlRegisterType<X32Manager>();
    qmlRegisterType<HandoffManager>();
	qmlRegisterType<KeyboardEmulator>();
    // Tell QML that these objects are owned by C++ and should not be deleted by the JS GC:
    // This is very important because otherwise SEGFAULTS will appear randomly!
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(&m_guiManager, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(&m_logManager, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(&m_engine, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(m_audioEngine, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(&m_output, QQmlEngine::CppOwnership);
	QQmlEngine::setObjectOwnership(&m_dao, QQmlEngine::CppOwnership);
	QQmlEngine::setObjectOwnership(&m_blockManager, QQmlEngine::CppOwnership);
	QQmlEngine::setObjectOwnership(&m_powermate, QQmlEngine::CppOwnership);
	QQmlEngine::setObjectOwnership(&m_midi, QQmlEngine::CppOwnership);
	QQmlEngine::setObjectOwnership(&m_customOsc, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(&m_lightingConsoleConnection, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(&m_audioConsoleConnection, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(&m_eosManager, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(m_hogManager, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(&m_globalOscCommands, QQmlEngine::CppOwnership);
	QQmlEngine::setObjectOwnership(&m_projectManager, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(&m_eosCueListManager, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(&m_activeChannelsManager, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(&m_updateManager, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(&m_anchorManager, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(&m_midiMappingManager, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(&m_x32Manager, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(&m_handoffManager, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(m_keyboardEmulator, QQmlEngine::CppOwnership);

    m_guiManager.createAndShowWindow();

    // restore app settings and last project:
    restoreApp();

    // Powermate:
    connect(&m_powermate, SIGNAL(rotated(double, double, bool)), this, SLOT(onControllerRotated(double, double, bool)));
    connect(&m_powermate, SIGNAL(pressed()), this, SLOT(onControllerPressed()));
    connect(&m_powermate, SIGNAL(released(double)), this, SLOT(onControllerReleased(double)));
    m_powermate.start();

    // start App engine (for luminosus business logic):
    m_engine.start();

    // start save timer (save each 5s):
    connect(&m_saveTimer, SIGNAL(timeout()), this, SLOT(saveAll()));
    if (qgetenv("ALARM_CLOCK_MODE") == "1") {
        qDebug() << "Alarm clock mode: saving only each 2 minutes.";
        m_saveTimer.start(2*60*1000);
    } else {
        m_saveTimer.start(5000);
    }

#ifdef Q_OS_ANDROID
    QTimer::singleShot(4000, this, SLOT(askForPermissions()));
#endif

    qInfo() << "-------------------------------------------";
}

void MainController::saveAll() {
    QJsonObject appState;
    appState["version"] = 0.3;
    m_guiManager.writeTo(appState);
	appState["currentProject"] = m_projectManager.getCurrentProjectName();
	appState["osc"] = m_customOsc.getState();
    appState["eosConnection"] = m_lightingConsoleConnection.getState();
    appState["x32Connection"] = m_audioConsoleConnection.getState();
    appState["sendCustomOscToEosValid"] = true;
    appState["sendCustomOscToEos"] = getSendCustomOscToEos();
    appState["midi"] = m_midi.getState();
    appState["updateManager"] = m_updateManager.getState();
    appState["developerMode"] = getDeveloperMode();
    appState["clickSounds"] = getClickSounds();
    appState["outputManager"] = m_output.getState();
    m_dao.saveFile("", "autosave.ats", appState);

	m_projectManager.saveCurrentProject();
}

void MainController::restoreApp() {
    bool lockExisted = m_dao.fileExists("", "luminosus.lock");
    m_dao.saveFile("", "luminosus.lock", QByteArray());

    QStringList combinations{"PowerPoint Fixture.lbc"};
    for (QString filename: combinations) {
        if (!m_dao.fileExists("combinations", filename)) {
            m_dao.importFile(":/examples/" + filename, "combinations", true);
        }
    }

    QJsonObject appState = m_dao.loadJsonObject("", "autosave.ats");
	if (appState.empty()) {
        // app state doesn't exist
        onFirstStart();
		return;
    }
    m_guiManager.readFrom(appState);
	m_customOsc.setState(appState["osc"].toObject());
    m_lightingConsoleConnection.setState(appState["eosConnection"].toObject());
    if (appState["sendCustomOscToEosValid"].toBool()) {
        setSendCustomOscToEos(appState["sendCustomOscToEos"].toBool());
    }
    m_audioConsoleConnection.setState(appState["x32Connection"].toObject());
    m_midi.setState(appState["midi"].toObject());
    m_updateManager.setState(appState["updateManager"].toObject());
    setDeveloperMode(appState["developerMode"].toBool());
    setClickSounds(appState["clickSounds"].toBool());
    m_output.setState(appState["outputManager"].toObject());
#ifndef Q_OS_ANDROID
    if (lockExisted) {
#else
    if (false) {
#endif
        // -> another instance is running or previously running instance
        // didn't exit successfully (crashed)
        // create 'No Project' and load it (because crashes there are unlikely):
        qWarning() << "Another instance is running or the program crashed on last run!";
        m_projectManager.importProjectFile(":/examples/No Project (Crashed).lpr", /*load=*/ true);
        return;
    } else if (appState["version"].toDouble() < 0.2) {
        m_projectManager.setCurrentProject("empty", /*createIfNotExist*/ true);
    } else if (appState["version"].toDouble() < 0.3) {
        onFirstStart();
    } else {
        m_projectManager.setCurrentProject(appState["currentProject"].toString());
    }
}

void MainController::onExit() {
    qInfo() << "-------------------------------------------";
    m_engine.stop();
    // kill powermate thread (probably in blocking read() function):
    if (m_powermate.isRunning()) m_powermate.terminate();
    saveAll();
    m_blockManager.deleteAllBlocks();
    m_dao.deleteFile("", "luminosus.lock");
    qInfo() << "Luminosus exited normally.";
}

void MainController::onFirstStart() {
    m_projectManager.importProjectFile(":/examples/No Project.lpr");
    if (m_projectManager.getCurrentProjectName().isEmpty()) {
        m_projectManager.setCurrentProject(ProjectManagerConstants::defaultProjectName);
    }
}

void MainController::askForPermissions() {
#ifdef Q_OS_ANDROID
    QAndroidJniEnvironment qjniEnv;

    QStringList permissions;
    permissions << "android.permission.ACCESS_NETWORK_STATE";
    permissions << "android.permission.CAMERA";
    permissions << "android.permission.INTERNET";
    permissions << "android.permission.READ_EXTERNAL_STORAGE";
    permissions << "android.permission.WRITE_EXTERNAL_STORAGE";
    permissions << "android.permission.RECORD_AUDIO";
#ifdef BLUETOOTH_LE_SUPPORTED
    // for Bluetooth Low Energy:
    permissions << "android.permission.ACCESS_COARSE_LOCATION";
#endif

    QtAndroidPrivate::requestPermissions(qjniEnv, permissions, [](const QtAndroidPrivate::PermissionsHash & result){
        qInfo() << "--- Android permissions granted:";
        for (QString key: result.keys()) {
            qInfo() << key << ": " << (result[key] == QtAndroidPrivate::PermissionsResult::Granted);
        }
    });
#endif
}

void MainController::onControllerRotated(double relativeAmount, double degrees, bool pressed) {
    if (m_blockManager.getFocusedBlock()) {
        m_blockManager.getFocusedBlock()->onControllerRotated(relativeAmount, degrees, pressed);
    }
}

void MainController::onControllerPressed() {
    if (m_blockManager.getFocusedBlock()) {
        m_blockManager.getFocusedBlock()->onControllerPressed();
    }
}

void MainController::onControllerReleased(double duration) {
    if (m_blockManager.getFocusedBlock()) {
        m_blockManager.getFocusedBlock()->onControllerReleased(duration);
    }
}

QString MainController::getTemplateFileBaseName() const {
    return QFileInfo(m_templateFileToImport).fileName();
}

void MainController::requestTemplateImport(QString filename) {
    m_templateFileToImport = filename;
    emit templateFileToImportChanged();
}

void MainController::onImportTemplateFileAccepted() {
    if (m_templateFileToImport.isEmpty()) return;
    m_projectManager.importProjectFile(m_templateFileToImport, true);
}

QString MainController::getVersionString() const {
    return LuminosusVersionInfo::VERSION_STRING;
}

QString MainController::getBuildDate() const { return __DATE__; }
