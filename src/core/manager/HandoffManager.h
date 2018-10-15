#ifndef HANDOFFMANAGER_H
#define HANDOFFMANAGER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QVector>
#include <QHash>
#include <QTimer>

// forward declaration to prevent dependency loop
class MainController;


class HandoffManager : public QObject
{
    Q_OBJECT

public:
    explicit HandoffManager(MainController* controller);

signals:

public slots:
    void takeControl(QString ip);
    void uploadTo(QString ip);

private slots:
    // client:
    bool connectToHost(QString host);
    bool writeData(QByteArray data);
    bool writeData(QString host, QByteArray data);

    // server:
    void clientConnected();
    void clientDisconnected();
    void processRawPacketsFromClient();
    void processData(QString ip, QByteArray data);
    void receivedProject(QByteArray data);

protected:
    MainController* const m_controller;  //!< pointer to MainController instance

    // client:
   QTcpSocket m_clientSocket;

    // server:
    QTcpServer m_tcpServer;
    QHash<QTcpSocket*, QByteArray*> m_buffers;  // we need a buffer to store data until block has been completely received
    QHash<QTcpSocket*, qint32*> m_sizes;  // we need to store the size to verify if a block has been received completely
};

#endif // HANDOFFMANAGER_H
