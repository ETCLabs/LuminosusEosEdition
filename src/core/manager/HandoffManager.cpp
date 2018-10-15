#include "HandoffManager.h"

#include "core/MainController.h"

#include <QJsonDocument>
#include <QJsonObject>


// create a shorter alias for the constants namespace:
namespace PMC = ProjectManagerConstants;


namespace HandoffManagerConstants {
    static const quint16 PORT = 51235;
    static const QByteArray requestControlMessage = QString("request_control").toLatin1();
}


HandoffManager::HandoffManager(MainController* controller)
    : QObject(controller)
    , m_controller(controller)
    , m_clientSocket(this)
    , m_tcpServer(this)
{
    connect(&m_tcpServer, SIGNAL(newConnection()), this, SLOT(clientConnected()));
    qDebug() << "HandoffManager listening:" << m_tcpServer.listen(QHostAddress::Any, HandoffManagerConstants::PORT);
}

void HandoffManager::takeControl(QString ip) {
    writeData(ip, HandoffManagerConstants::requestControlMessage);
}

void HandoffManager::uploadTo(QString ip) {
    QJsonObject projectState = m_controller->projectManager()->getCurrentProjectState();
    QJsonDocument jsonDocument(projectState);
    writeData(ip, jsonDocument.toJson());
    m_controller->projectManager()->importProjectFile(":/examples/No Project.lpr", /*load=*/ true);
}

bool HandoffManager::connectToHost(QString host) {
    m_clientSocket.connectToHost(host, HandoffManagerConstants::PORT);
    return m_clientSocket.waitForConnected();
}

QByteArray IntToArray(qint32 source) {
    // Use qint32 to ensure that the number have 4 bytes
    // Avoid use of cast, this is the Qt way to serialize objects
    QByteArray temp;
    QDataStream data(&temp, QIODevice::ReadWrite);
    data << source;
    return temp;
}

bool HandoffManager::writeData(QByteArray data) {
    if (m_clientSocket.state() == QAbstractSocket::ConnectedState) {
        m_clientSocket.write(IntToArray(data.size()));  // write size of data
        m_clientSocket.write(data);  // write the data itself
        return m_clientSocket.waitForBytesWritten();
    } else {
        return false;
    }
}

bool HandoffManager::writeData(QString host, QByteArray data) {
    if (connectToHost(host)) {
        return writeData(data);
    }
    return false;
}

void HandoffManager::clientConnected() {
    while (m_tcpServer.hasPendingConnections()) {
        QTcpSocket* socket = m_tcpServer.nextPendingConnection();
        connect(socket, SIGNAL(readyRead()), this, SLOT(processRawPacketsFromClient()));
        connect(socket, SIGNAL(disconnected()), this, SLOT(clientDisconnected()));
        QByteArray* buffer = new QByteArray();
        qint32* s = new qint32(0);
        m_buffers.insert(socket, buffer);
        m_sizes.insert(socket, s);
    }
}

void HandoffManager::clientDisconnected() {
    QTcpSocket* socket = static_cast<QTcpSocket*>(sender());
    QByteArray* buffer = m_buffers.value(socket);
    qint32* s = m_sizes.value(socket);
    socket->deleteLater();
    delete buffer;
    delete s;
}


qint32 ArrayToInt(QByteArray source) {
    qint32 temp;
    QDataStream data(&source, QIODevice::ReadWrite);
    data >> temp;
    return temp;
}

void HandoffManager::processRawPacketsFromClient() {
    QTcpSocket* socket = static_cast<QTcpSocket*>(sender());
    QByteArray* buffer = m_buffers.value(socket);
    qint32* s = m_sizes.value(socket);
    qint32 size = *s;
    while (socket->bytesAvailable() > 0) {
        buffer->append(socket->readAll());
        while ((size == 0 && buffer->size() >= 4) || (size > 0 && buffer->size() >= size)) {
            // while can process data, process it
            if (size == 0 && buffer->size() >= 4) {
                // if size of data has been received completely, then store it on our global variable
                size = ArrayToInt(buffer->mid(0, 4));
                *s = size;
                buffer->remove(0, 4);
            } if (size > 0 && buffer->size() >= size) {
                // if data has been received completely, then emit our SIGNAL with the data
                QByteArray data = buffer->mid(0, size);
                buffer->remove(0, size);
                size = 0;
                *s = size;
                processData(socket->peerAddress().toString(), data);
            }
        }
    }
}

void HandoffManager::processData(QString ip, QByteArray data) {
    if (data == HandoffManagerConstants::requestControlMessage) {
        uploadTo(ip);
    } else {
        receivedProject(data);
    }
}

void HandoffManager::receivedProject(QByteArray data) {
    QJsonDocument jsonDocument = QJsonDocument::fromJson(data);
    if (!jsonDocument.isObject()) {
        qWarning() << "Received project state is invalid.";
        return;
    }
    QJsonObject projectState = jsonDocument.object();
    QString fileName = projectState["fileName"].toString();
    if (fileName.isEmpty()) {
        qWarning() << "Couldn't find filename in project state.";
        return;
    }
    // TODO: check version
    m_controller->dao()->saveFile(PMC::subdirectory, fileName + PMC::fileEnding, projectState);
    m_controller->projectManager()->setCurrentProject(fileName);
}
