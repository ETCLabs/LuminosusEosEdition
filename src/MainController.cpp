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

#include "MainController.h"

#include <QtGui>
#include <QtQuick>
#include <QSysInfo>
#include <QJsonObject>
#include <QtQml>

#include <string>

MainController::MainController(QQmlApplicationEngine& qmlEngine, QObject *parent)
    : QObject(parent)
    , m_qmlEngine(qmlEngine)
    , m_logManager(this)
    , m_engine(this)
    , m_audioEngine(this)
    , m_output(this)
    , m_blockManager(this)
	, m_midi(this)
    , m_eosManager(this)
	, m_globalOscCommands(this)
	, m_projectManager(this)
    , m_eosCueListManager(this)
    , m_activeChannelsManager(this)
    , m_updateManager(this)
    , m_anchorManager(this)
    , m_waitingForExternalInput("")
    , m_waitingForControl(false)
    , m_releaseNextControl(false)
    , m_sendCustomOscToEos(true)
    , m_mainWindow(nullptr)
    , m_backgroundName(LuminosusConstants::defaultBackgroundName)
{
    // print Qt Version to verify that the right library is loaded:
    qInfo() << "Compiled with Qt Version" << QT_VERSION_STR;
    qInfo() << "Runtime Qt Version:" << qVersion();

    // prepare second OSCNetworkManager for Eos connection:
    m_eosConnection.setEosConnectionDefaults();

    // Register classes which slots should be accessible from QML:
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
	qmlRegisterType<ProjectManager>();
    qmlRegisterType<EosCueListManager>();
    qmlRegisterType<EosActiveChannelsManager>();
    qmlRegisterType<UpdateManager>();
    qmlRegisterType<AnchorManager>();
    // Tell QML that these objects are owned by C++ and should not be deleted by the JS GC:
    // This is very important because otherwise SEGFAULTS will appear randomly!
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
	QQmlEngine::setObjectOwnership(&m_qmlEngine, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(&m_logManager, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(&m_engine, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(&m_audioEngine, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(&m_output, QQmlEngine::CppOwnership);
	QQmlEngine::setObjectOwnership(&m_dao, QQmlEngine::CppOwnership);
	QQmlEngine::setObjectOwnership(&m_blockManager, QQmlEngine::CppOwnership);
	QQmlEngine::setObjectOwnership(&m_powermate, QQmlEngine::CppOwnership);
	QQmlEngine::setObjectOwnership(&m_midi, QQmlEngine::CppOwnership);
	QQmlEngine::setObjectOwnership(&m_osc, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(&m_eosConnection, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(&m_eosManager, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(&m_globalOscCommands, QQmlEngine::CppOwnership);
	QQmlEngine::setObjectOwnership(&m_projectManager, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(&m_eosCueListManager, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(&m_activeChannelsManager, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(&m_updateManager, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(&m_anchorManager, QQmlEngine::CppOwnership);

    // make this MainController accessable from QML with a context variable
    // the access to all other manager instances is done using this controller
    qmlEngine.rootContext()->setContextProperty("controller", this);

    // load main.qml file:
    qmlEngine.load(QUrl("qrc:/qml/main.qml"));
    if (qmlEngine.rootObjects().length() <= 0) {
        qCritical() << "Error while loading main.qml file. (No root object created)";
        std::exit(999);
    }
    QWindow* window = qobject_cast<QWindow*>(qmlEngine.rootObjects()[0]);
    if (!window) {
        qCritical() << "Error while loading main.qml file. (Window could not be created)";
        std::exit(998);
    }

    // TODO: check and cache all relevant GUI items

#ifdef Q_OS_LINUX
    // ----------- Anti-Aliasing ---------------
    QSurfaceFormat format(window->format());
    if (qmlEngine.rootContext()->contextProperty("dp").toFloat() > 1.5
            || window->screen()->devicePixelRatio() > 1.5) {
        qInfo() << "AntiAliasing: Not forced (HighDPI screen)";
        format.setSamples(-1);
        // platform decides if MSAA should be used
        // if not: "vertex" anti aliasing will be automatically used, which could result in
        // lower performance because of non-opaque primitves and less batching (~30% more batches)
        // but lower GPU memory usage which is more important on HighDPI
        // see http://doc.qt.io/qt-5/qtquick-visualcanvas-scenegraph-renderer.html#antialiasing
        // and export QSG_ANTIALIASING_METHOD=vertex
    } /*else if (window->screen()->availableSize().width() > 1920) {
       qInfo() << "AntiAliasing: MSAA 4x (High Resolution Screen)";
       format.setSamples(4);
       // -> "vertex" anti aliasing will be automatically disabled
       // opaque primitves are truly opaque
    } */else {
        qInfo() << "AntiAliasing: MSAA 4x (Normal Resolution Screen)";
        format.setSamples(4);
        // -> "vertex" anti aliasing will be automatically disabled
        // opaque primitves are truly opaque
    }
    qInfo() << "OpenGL Version used: " << format.majorVersion() << "." << format.minorVersion();
    window->setFormat(format);
#endif

    window->setIcon(QIcon(":/images/icon/app_icon_512.png"));
    //window->showFullScreen();
    window->show();

    // restore app settings and last project:
    restoreApp();

    // Powermate:
    connect(&m_powermate, SIGNAL(rotated(double, bool)), this, SLOT(onControllerRotated(double, bool)));
    connect(&m_powermate, SIGNAL(pressed()), this, SLOT(onControllerPressed()));
    connect(&m_powermate, SIGNAL(released(double)), this, SLOT(onControllerReleased(double)));
    m_powermate.start();

    // start App engine (for luminosus business logic):
    m_engine.start();

    // start save timer (save each 5s):
    connect(&m_saveTimer, SIGNAL(timeout()), this, SLOT(saveAll()));
    m_saveTimer.start(5000);
}

void MainController::saveAll() {
    QJsonObject appState;
    appState["version"] = 0.3;
	appState["windowGeometry"] = serialize<QRect>(getMainWindow()->geometry());
	bool maximized = (getMainWindow()->width() == QGuiApplication::primaryScreen()->availableSize().width());
	appState["windowMaximized"] = maximized;
	appState["currentProject"] = m_projectManager.getCurrentProjectName();
	appState["osc"] = m_osc.getState();
    appState["eosConnection"] = m_eosConnection.getState();
    appState["sendCustomOscToEosValid"] = true;
    appState["sendCustomOscToEos"] = getSendCustomOscToEos();
	appState["midi"] = m_midi.getState();
    appState["updateManager"] = m_updateManager.getState();
    m_dao.saveFile("", "autosave.ats", appState);

	m_projectManager.saveCurrentProject();
}

void MainController::restoreApp() {
    QJsonObject appState = m_dao.loadJsonObject("", "autosave.ats");
	if (appState.empty()) {
        // app state doesn't exist
        onFirstStart();
		return;
	}
	restoreWindowGeometry(appState);
	m_osc.setState(appState["osc"].toObject());
    m_eosConnection.setState(appState["eosConnection"].toObject());
    if (appState["sendCustomOscToEosValid"].toBool()) {
        setSendCustomOscToEos(appState["sendCustomOscToEos"].toBool());
    }
	m_midi.setState(appState["midi"].toObject());
    m_updateManager.setState(appState["updateManager"].toObject());
    if (appState["version"].toDouble() < 0.2) {
        m_projectManager.setCurrentProject("empty", /*createIfNotExist*/ true);
    } else if (appState["version"].toDouble() < 0.3) {
        onFirstStart();
    } else {
        m_projectManager.setCurrentProject(appState["currentProject"].toString());
    }
}

void MainController::setKeyboardFocusToPlane() {
    // TODO: make more elegant
	QObject* rootObject = (QObject*)(qmlEngine()->rootObjects()[0]);
	QQuickItem* plane = (QQuickItem*)(rootObject->findChild<QObject*>("plane"));
	QMetaObject::invokeMethod(plane, "forceActiveFocus");
}

void MainController::onExit() {
    m_engine.stop();
    saveAll();
    m_blockManager.deleteAllBlocks();
}

void MainController::showLockScreen() {
    // TODO

}

void MainController::onFirstStart() {
    m_projectManager.importProjectFile(":/examples/Tutorial.lpr");
    if (m_projectManager.getCurrentProjectName().isEmpty()) {
        m_projectManager.setCurrentProject(ProjectManagerConstants::defaultProjectName);
    }
}

void MainController::onControllerRotated(double value, bool pressed) {
    if (m_blockManager.getFocusedBlock()) {
        m_blockManager.getFocusedBlock()->onControllerRotated(value, pressed);
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

void MainController::connectWaitingControlToExternalInput(QString inputUid) {
    if (m_waitingForExternalInput.isEmpty()) return;
    if (inputUid.isEmpty()) return;
    m_midi.connectControl(m_waitingForExternalInput, inputUid);
    m_waitingForExternalInput = "";
}

void MainController::cancelWaitForExternalEvent() {
    m_midi.clearNextEventCallbacks();
    m_waitingForExternalInput = "";
    m_waitingForControl = false;
}

void MainController::waitForExternalEvent() {
    m_waitingForControl = true;
}

void MainController::waitToReleaseNextControl() {
    m_releaseNextControl = true;
}

void MainController::registerGuiControl(QQuickItem* item) {
    QString controlUid = item->property("uid").toString();
    if (controlUid.isEmpty()) return;
    m_registeredControls[controlUid] = item;
}

void MainController::unregisterGuiControl(QQuickItem* item) {
    if (!item) return;
    QString controlUid = item->property("uid").toString();
    if (controlUid.isEmpty()) return;
    m_midi.disconnectControl(controlUid);
    if (mapContains(m_registeredControls, controlUid)) {
        m_registeredControls.erase(controlUid);
    }
}

QQuickItem* MainController::getControlFromUid(QString controlUid) {
    if (!mapContains(m_registeredControls, controlUid)) return nullptr;
    return m_registeredControls[controlUid];
}

void MainController::checkForExternalInputConnection(QString controllerUid) {
    if (m_releaseNextControl) {
        m_midi.disconnectControl(controllerUid);
        m_releaseNextControl = false;
    }
    if (!m_waitingForControl) return;  // <- most likely (in case of normal user input)
    m_waitingForControl = false;
    m_waitingForExternalInput = controllerUid;
    m_midi.registerForNextEvent("inputConnection", [this](MidiEvent event) { this->connectWaitingControlToExternalInput(event.inputId); });
}

void MainController::setBackgroundName(QString value) {
    if (value.isEmpty()) value = LuminosusConstants::defaultBackgroundName;
    m_backgroundName = value;
    emit backgroundNameChanged();
}

QRect MainController::getWindowGeometryOfItem(const QVariant& item) const {
    return item.value<QQuickItem*>()->window()->geometry();
}

void MainController::setPropertyWithoutChangingBindings(const QVariant& item, QString name, QVariant value) const {
	QQuickItem* qitem = item.value<QQuickItem*>();
    qitem->setProperty(name.toLatin1().data(), value);
}

double MainController::getGuiScaling() const {
    return m_qmlEngine.rootContext()->contextProperty("dp").toDouble();
}

void MainController::setGuiScaling(double value) const {
	m_qmlEngine.rootContext()->setContextProperty("dp", value);
}

void MainController::setQmlContextProperty(QString propertyName, QVariant value) const {
	m_qmlEngine.rootContext()->setContextProperty(propertyName, value);
}

QString MainController::getVersionString() const {
	return LuminosusVersionInfo::VERSION_STRING;
}

bool MainController::pointIsInTrashArea(qreal x, qreal y) const {
    // TODO: cache rootObject or even trash
	QObject* rootObject = (QObject*)(m_qmlEngine.rootObjects()[0]);
	QQuickItem* trash = (QQuickItem*)(rootObject->findChild<QObject*>("trash"));
	Q_ASSERT_X(trash != nullptr, "pointIsInTrash()", "Can not find trash gui item.");
	QPointF localCoords = trash->mapFromScene(QPointF(x, y));
	bool isInTrash = trash->contains(localCoords);
	return isInTrash;
}

void MainController::showToast(QString text, bool isWarning) const {
    if (m_qmlEngine.rootObjects().isEmpty()) return;
	QObject* rootObject = (QObject*)(m_qmlEngine.rootObjects()[0]);
	QQuickItem* toast = (QQuickItem*)(rootObject->findChild<QObject*>("toast"));
    Q_ASSERT_X(toast != nullptr, "showToast()", "Can not find Toast gui item.");
    QMetaObject::invokeMethod(toast, "displayToast", Q_ARG(QVariant, text), Q_ARG(QVariant, isWarning));
}

QQuickWindow* MainController::getMainWindow() const {
    if (!m_mainWindow) {
        if (m_qmlEngine.rootObjects().length() <= 0) {
            qCritical() << "Error while accessing main window. (No root object found)";
            return nullptr;
        }
        m_mainWindow = qobject_cast<QQuickWindow*>(m_qmlEngine.rootObjects()[0]);
        if (!m_mainWindow) {
            qCritical() << "Error while accessing main window. (Window could not be found)";
            return nullptr;
        }
    }
    return m_mainWindow;
}

void MainController::restoreWindowGeometry(const QJsonObject& appState) const {
	QRect windowGeometry = deserialize<QRect>(appState["windowGeometry"].toString());
	QQuickWindow* window = getMainWindow();
    if (!window) return;
	bool maximized = appState["windowMaximized"].toBool();
	if (maximized) {
		window->showMaximized();
    } else if (!windowGeometry.isNull()) {
        window->setGeometry(windowGeometry);
    }
}
