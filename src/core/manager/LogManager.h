#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <QObject>
#include <QStringList>
#include <QPointer>
#include <QMutex>
#include <QTimer>

// forward declaration to prevent dependency loop
class MainController;

/**
 * @brief The LogManagerConstants namespace contains all constants used in LogManager.
 */
namespace LogManagerConstants {
    /**
     * @brief historyLength is the amount of messages to be logged before the oldest will be deleted
     */
	static const int historyLength = 100;
}


/**
 * @brief The LogManager class logs all debug messages and warning from the Qt and QML debug
 * message system (QDebug).
 * It redirects all messages through this class and then calls the normal message handler
 * (chain of responsibility). This is a singleton and thread-safe.
 */
class LogManager : public QObject
{
	Q_OBJECT

public:
    /**
     * @brief LogManager creates a LogManager object and starts redirecting QDebug messages through
     * this class.
     * @param controller
     */
    explicit LogManager(MainController* controller);

    /**
      * @brief ~LogManager restores the default QDebug message handler
      */
	~LogManager();

signals:
    /**
     * @brief logChanged emitted when new message are in the log (after a short delay
     * to prevent too many GUI refreshes)
     */
	void logChanged();

public slots:

    /**
     * @brief staticQDebugMessageHandler forwards QDebug messages to qDebugMessageHandler()
     * (because those handlers have to be static)
     * @param type of the message
     * @param context of the message
     * @param msg the message itself
     */
	static void staticQDebugMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    /**
     * @brief qDebugMessageHandler can handle QDebug messages (they are forwarded from the
     * static method staticQDebugMessageHandler())
     * @param type of the message
     * @param context of the message
     * @param msg the message itself
     */
	void qDebugMessageHandler(QtMsgType type, const QMessageLogContext &context, const QString &msg);

    /**
     * @brief getLog returns the log
     * @return the log as a QStringList
     */
	QStringList getLog() const { return m_log;  }

protected:
    /**
     * @brief m_controller a pointer to the main controller
     */
    QPointer<MainController> const m_controller;

    /**
     * @brief m_log the list of logged messages (for size see LogManagerConstants::historyLength)
     */
	QStringList m_log;

    /**
     * @brief m_previousMessageHandler the previous installed QDebug message handler
     */
	QtMessageHandler m_previousMessageHandler;

    /**
     * @brief m_logChangedSignalDelay is a timer to delay the emission of the logChanged signal
     * to prevent the log being updated to often
     */
    QTimer m_logChangedSignalDelay;

    /**
     * @brief s_instance the only instance of this class, used to forward calls from
     * static message handler to non-static class method
     */
    static QPointer<LogManager> s_instance;

    /**
     * @brief s_mutex Mutex to prevent simultaneous writes to the log
     */
    static QMutex s_mutex;

};

#endif // LOGMANAGER_H
