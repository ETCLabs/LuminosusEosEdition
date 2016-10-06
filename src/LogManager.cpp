#include "LogManager.h"

#include "MainController.h"

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
    m_logChangedSignalDelay.setSingleShot(true);
    m_logChangedSignalDelay.setInterval(100);
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
	QString logEntry;
	switch (type) {
	case QtInfoMsg:
		logEntry = "Info: %1";
		break;
	case QtDebugMsg:
		logEntry = "%1";
        Q_ASSERT(m_controller);
		m_controller->showToast(msg);
		break;
	case QtWarningMsg:
		logEntry = "Warning: %1";
        Q_ASSERT(m_controller);
        m_controller->showToast(msg, true);
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
	m_log.prepend(time + logEntry);
	if (m_log.size() > LogManagerConstants::historyLength) m_log.removeLast();
    m_logChangedSignalDelay.start();

	// hand over to normal Qt message handler:
	// (chain of responsibility)
	if (m_previousMessageHandler) {
		m_previousMessageHandler(type, ctx, msg);
	}
}
