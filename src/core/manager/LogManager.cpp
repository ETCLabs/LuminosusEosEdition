#include "LogManager.h"

#include "core/MainController.h"

#include <QDebug>
#include <QMutexLocker>
#include <QtGlobal>

QPointer<LogManager> LogManager::s_instance = nullptr;

QMutex LogManager::s_mutex;

LogManager::LogManager(MainController* controller)
	: QObject(controller)
	, m_controller(controller)
{
	m_previousMessageHandler = nullptr;
    s_instance = this;

    // prepare log changed signal:
    //m_logChangedSignalDelay.setSingleShot(true);
    m_logChangedSignalDelay.setInterval(3000);
    m_logChangedSignalDelay.start();
    connect(&m_logChangedSignalDelay, SIGNAL(timeout()), this, SIGNAL(logChanged()));

	// register custom message handler for qDebug, qWarning etc messages:
    m_previousMessageHandler = qInstallMessageHandler(staticQDebugMessageHandler);
}

LogManager::~LogManager() {
	qInstallMessageHandler(0);
}

void LogManager::staticQDebugMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg) {
    Q_ASSERT(s_instance);
    LogManager::s_instance->qDebugMessageHandler(type, context, msg);
}

void LogManager::qDebugMessageHandler(QtMsgType type, const QMessageLogContext& ctx, const QString& msg) {
    QMutexLocker locker(&s_mutex);
    // TODO: check if this is fast enough because this could be a bottleneck due to the lock
	QString logEntry;
	switch (type) {
	case QtInfoMsg:
		logEntry = "Info: %1";
		break;
	case QtDebugMsg:
        logEntry = "%1";
#ifndef Q_OS_IOS
        if (m_controller && m_controller->getDeveloperMode()) {
            m_controller->guiManager()->showToast(msg);
        }
#endif
		break;
	case QtWarningMsg:
        logEntry = "Warning: %1";
#ifndef Q_OS_IOS
        if (m_controller && m_controller->getDeveloperMode()) {
            m_controller->guiManager()->showToast(msg);
        }
#endif
		break;
	case QtCriticalMsg:
		logEntry = "Critical: %1";
		break;
	case QtFatalMsg:
        logEntry = "Fatal: %1";
		break;
	}
	logEntry = logEntry.arg(msg);
	// logEntry = logEntry += " (%2:%3, %4)";
	// logEntry = logEntry.arg(msg, QString(ctx.file), QString::number(ctx.line), QString(ctx.function));

	QString time = "[" + QTime::currentTime().toString() + "] ";
    m_log.prepend(time + logEntry);  // TODO: reverse this?
	if (m_log.size() > LogManagerConstants::historyLength) m_log.removeLast();

	// hand over to normal Qt message handler:
	// (chain of responsibility)
	if (m_previousMessageHandler) {
        // TODO: release lock before this?
		m_previousMessageHandler(type, ctx, msg);
	}
}
