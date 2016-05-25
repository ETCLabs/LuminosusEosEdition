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
#include "PowermateListener.h"
#include "MidiManager.h"
#include "block_data/BlockList.h"
#include "block_data/BlockManager.h"
#include "OSCNetworkManager.h"
#include "GlobalOscCommands.h"
#include "ProjectManager.h"
#include "version.h"
#include "utils.h"

#include <QObject>
#include <QtQuick>
#include <QWindow>

#include <vector>

// The main controller of the application.
// Initializes all components and makes them available by getter functions.

class MainController : public QObject
{
	Q_OBJECT

public:

	explicit MainController(QQmlApplicationEngine& m_qmlEngine, QObject *parent = 0);

signals:
    void projectChanged();

public slots:

    // Getter Methods for Manager classes:
	QQmlApplicationEngine* qmlEngine() { return &m_qmlEngine; }
    Engine* engine() { return &m_engine; }
    AudioEngine* audioEngine() { return &m_audioEngine; }
	OutputManager* output() { return m_output; }
	FileSystemManager* dao() { return &m_dao; }
	BlockManager* blockManager() { return &m_blockManager; }
	PowermateListener* powermate() { return &m_powermate; }
	MidiManager* midi() { return &m_midi; }
	OSCNetworkManager* osc() { return &m_osc; }
	ProjectManager* projectManager() { return &m_projectManager; }

    // State / Projects:
    void saveAll();
	void restoreApp();
	void setKeyboardFocusToPlane();

    // Start / Exit:
    void onExit();
    void showLockScreen();

    // Powermate / universal Controller:
	void onControllerRotated(double value, bool pressed) { if (m_blockManager.getFocusedBlock()) m_blockManager.getFocusedBlock()->onControllerRotated(value, pressed); }
	void onControllerPressed() { if (m_blockManager.getFocusedBlock()) m_blockManager.getFocusedBlock()->onControllerPressed(); }
	void onControllerReleased(double duration) { if (m_blockManager.getFocusedBlock()) m_blockManager.getFocusedBlock()->onControllerReleased(duration); }

    // External Input
    void connectWaitingControlToExternalInput(QString inputUid);
    void cancelWaitForExternalEvent();
    void waitForExternalEvent();
    void waitToReleaseNextControl();

    // Used in QML:
    void registerGuiControl(QQuickItem* item);
    void unregisterGuiControl(QQuickItem* item);
    QQuickItem* getControlFromUid(QString controlUid);
	void checkForExternalInputConnection(QString controllerUid);
	// returns the geometry of the window that contains the item
	QRect getWindowGeometryOfItem(const QVariant& item) const { return item.value<QQuickItem*>()->window()->geometry(); }
	// sets a property of a QQuickItem to a value without changing its bindings (in opposite to setting it from QML directly)
	void setPropertyWithoutChangingBindings(const QVariant& item, QString name, QVariant value);
	// sets the dp context property
	void setGuiScaling(double value) const {
		m_qmlEngine.rootContext()->setContextProperty("dp", value);
	}
	void setQmlContextProperty(QString propertyName, QVariant value) const {
		m_qmlEngine.rootContext()->setContextProperty(propertyName, value);
	}
	QString getVersionString() const;
	bool pointIsInTrashArea(qreal x, qreal y) const;

private:
	// returns the main window
	QQuickWindow* getMainWindow() const;

	void restoreWindowGeometry(const QJsonObject& appState);

protected:
	// Engines / Managers:
	QQmlApplicationEngine&          m_qmlEngine;
	Engine							m_engine;
	AudioEngine						m_audioEngine;
	OutputManager*					m_output;
	FileSystemManager				m_dao;
	BlockManager					m_blockManager;
	PowermateListener				m_powermate;
	MidiManager						m_midi;
	OSCNetworkManager				m_osc;
	GlobalOscCommands				m_globalOscCommands;
	ProjectManager					m_projectManager;

	// Attributes:
	QString                         m_waitingForExternalInput = "";
	bool                            m_waitingForControl = false;
	bool                            m_releaseNextControl = false;
	std::map<QString, QQuickItem*>  m_registeredControls;

};

#endif // MAINCONTROLLER_H
