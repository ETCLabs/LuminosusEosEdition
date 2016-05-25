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

#include "NodeBase.h"
#include "BezierCurve.h"
#include "OutputManager.h"
#include "PowermateListener.h"

#include <QtGui>
#include <QtQuick>
#include <QSysInfo>
#include <QJsonObject>
#include <QtQml>

#include <string>

MainController::MainController(QQmlApplicationEngine& qmlEngine, QObject *parent)
    : QObject(parent)
    , m_qmlEngine(qmlEngine)
	, m_audioEngine(this)
    , m_blockManager(this)
	, m_midi(this)
	, m_globalOscCommands(this)
	, m_projectManager(this)
{
    m_output = new OutputManager(this);
    qmlEngine.rootContext()->setContextProperty("output", m_output);
    qmlEngine.rootContext()->setContextProperty("controller", this);

    // Register classes which slots should be accessible from QML:
    qmlRegisterType<Engine>();
    qmlRegisterType<AudioEngine>();
	qmlRegisterType<OutputManager>();
	qmlRegisterType<FileSystemManager>();
    qmlRegisterType<BlockManager>();
	qmlRegisterType<PowermateListener>();
	qmlRegisterType<MidiManager>();
	qmlRegisterType<OSCNetworkManager>();
	qmlRegisterType<ProjectManager>();
    // Tell QML that these objects are owned by C++ and should not be deleted by the JS GC:
    // This is very important because otherwise SEGFAULTS will appear randomly!
    QQmlEngine::setObjectOwnership(&m_engine, QQmlEngine::CppOwnership);
    QQmlEngine::setObjectOwnership(&m_audioEngine, QQmlEngine::CppOwnership);
	QQmlEngine::setObjectOwnership(m_output, QQmlEngine::CppOwnership);
	QQmlEngine::setObjectOwnership(&m_dao, QQmlEngine::CppOwnership);
	QQmlEngine::setObjectOwnership(&m_blockManager, QQmlEngine::CppOwnership);
	QQmlEngine::setObjectOwnership(&m_powermate, QQmlEngine::CppOwnership);
	QQmlEngine::setObjectOwnership(&m_midi, QQmlEngine::CppOwnership);
	QQmlEngine::setObjectOwnership(&m_osc, QQmlEngine::CppOwnership);
	QQmlEngine::setObjectOwnership(&m_projectManager, QQmlEngine::CppOwnership);

    qmlEngine.load(QUrl("qrc:/main.qml"));
    QWindow* window = (QWindow*)qmlEngine.rootObjects()[0];
    qmlEngine.rootContext()->setContextProperty("mainWindow", window);
    QSurfaceFormat format;
    if (qmlEngine.rootContext()->contextProperty("dp").toFloat() > 1.5
            || window->screen()->devicePixelRatio() > 1.5) {
        qDebug() << "No multisampling anti-aliasing because of retina screen.";
        format.setSamples(0);
    } else {
		qDebug() << "Multisampling anti-aliasing: 16x";
		format.setSamples(16);
    }
    window->setFormat(format);
    window->setIcon(QIcon(":/images/icon/app_icon_512.png"));
    //window->showFullScreen();
    window->show();
    restoreApp();

    // Powermate:
    connect(&m_powermate, SIGNAL(rotated(double, bool)), this, SLOT(onControllerRotated(double, bool)));
    connect(&m_powermate, SIGNAL(pressed()), this, SLOT(onControllerPressed()));
    connect(&m_powermate, SIGNAL(released(double)), this, SLOT(onControllerReleased(double)));
    m_powermate.start();

    m_engine.setParent(this);
    m_engine.start();

    QTimer* saveTimer = new QTimer(this);
    connect(saveTimer, SIGNAL(timeout()), this, SLOT(saveAll()));
    saveTimer->start(5000);
}

void MainController::saveAll() {
    QJsonObject appState;
    appState["version"] = 0.1;
	appState["windowGeometry"] = serialize<QRect>(getMainWindow()->geometry());
	bool maximized = (getMainWindow()->width() == QGuiApplication::primaryScreen()->availableSize().width());
	appState["windowMaximized"] = maximized;
	appState["currentProject"] = m_projectManager.getCurrentProjectName();
	appState["osc"] = m_osc.getState();
	appState["midi"] = m_midi.getState();
    m_dao.saveFile("", "autosave.ats", appState);

	m_projectManager.saveCurrentProject();
}

void MainController::restoreApp() {
    QJsonObject appState = m_dao.loadJsonObject("", "autosave.ats");
	if (appState.empty()) {
		// app state doesn't exist
		m_projectManager.setCurrentProject(ProjectManagerConstants::defaultProjectName);
		return;
	}
	restoreWindowGeometry(appState);
	m_osc.setState(appState["osc"].toObject());
	m_midi.setState(appState["midi"].toObject());
	m_projectManager.setCurrentProject(appState["currentProject"].toString());
}

void MainController::setKeyboardFocusToPlane() {
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

void MainController::connectWaitingControlToExternalInput(QString inputUid) {
    if (m_waitingForExternalInput.isEmpty()) return;
    if (inputUid.isEmpty()) return;
    m_midi.connectControl(m_waitingForExternalInput, inputUid);
    m_waitingForExternalInput = "";
}

void MainController::cancelWaitForExternalEvent() {
    m_midi.clearNextEventCallback();
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

QQuickItem *MainController::getControlFromUid(QString controlUid) {
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
	m_midi.registerForNextEvent([this](QString uid) { this->connectWaitingControlToExternalInput(uid); });
}

void MainController::setPropertyWithoutChangingBindings(const QVariant& item, QString name, QVariant value) {
	QQuickItem* qitem = item.value<QQuickItem*>();
	qitem->setProperty(name.toLatin1().data(), value);
}

QString MainController::getVersionString() const {
	return LumoinosusVersionInfo::VERSION_STRING;
}

bool MainController::pointIsInTrashArea(qreal x, qreal y) const {
	QObject* rootObject = (QObject*)(m_qmlEngine.rootObjects()[0]);
	QQuickItem* trash = (QQuickItem*)(rootObject->findChild<QObject*>("trash"));
	Q_ASSERT_X(trash != nullptr, "pointIsInTrash()", "Can not find trash gui item.");
	QPointF localCoords = trash->mapFromScene(QPointF(x, y));
	bool isInTrash = trash->contains(localCoords);
	return isInTrash;
}

QQuickWindow* MainController::getMainWindow() const {
	QQuickWindow* window = qobject_cast<QQuickWindow*>(m_qmlEngine.rootObjects()[0]);
	return window;
}

void MainController::restoreWindowGeometry(const QJsonObject& appState) {
	QRect windowGeometry = deserialize<QRect>(appState["windowGeometry"].toString());
	QQuickWindow* window = getMainWindow();
	if (!window) return;
	if (!windowGeometry.isNull()) {
		window->setGeometry(windowGeometry);
	}
	bool maximized = appState["windowMaximized"].toBool();
	if (maximized) {
		window->showMaximized();
	}
}
