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

#include "FileSystemManager.h"
#include <QObject>
#include <QDir>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QString>
#include <QStandardPaths>
#include <QDebug>


FileSystemManager::FileSystemManager()
	: QObject()
{
    if (QSysInfo::productType() == "android") {
        //m_dataRoot = "file:///sdcard/luminosus_data/";
        m_dataRoot = "";
    } else {
        m_dataRoot = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        if (!m_dataRoot.endsWith("/")) {
            m_dataRoot += "/";
        }
    }
    QDir().mkpath(m_dataRoot);
    QDir().mkpath(m_dataRoot + "projects");
	QDir().mkpath(m_dataRoot + "combinations");
    qDebug() << "App data directory: " << m_dataRoot;
}

bool FileSystemManager::saveFile(QString dir, QString filename, QByteArray content) const {
    QString path = QString(m_dataRoot);
    if (dir.length()) {
        path += dir + "/";
        QDir().mkpath(path);
    }
    QFile saveFile(path + filename);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning() << "Couldn't write to file " + path + filename + ": " << saveFile.errorString();
        return false;
    }
    saveFile.write(content);
    saveFile.close();
    return true;
}

bool FileSystemManager::saveFile(QString dir, QString filename, QJsonObject content) const {
    QByteArray raw = QJsonDocument(content).toJson();
    return saveFile(dir, filename, raw);
}

bool FileSystemManager::saveFile(QString dir, QString filename, QJsonArray content) const {
    QByteArray raw = QJsonDocument(content).toJson();
    return saveFile(dir, filename, raw);
}

QByteArray FileSystemManager::loadFile(QString dir, QString filename) const {
    QString path = getDataDir(dir) + filename;
    if (!QDir().exists(path)) {
        qWarning() << "File " + path + " does not exist.";
        return QByteArray();
    }
    QFile loadFile(path);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open file " + path + ".";
        return QByteArray();
    }
    QByteArray content = loadFile.readAll();
    loadFile.close();
    return content;
}

QJsonObject FileSystemManager::loadJsonObject(QString dir, QString filename) const {
    QByteArray content = loadFile(dir, filename);
    QJsonDocument loadDoc(QJsonDocument::fromJson(content));
    return loadDoc.object();
}

QJsonArray FileSystemManager::loadJsonArray(QString dir, QString filename) const {
    QByteArray content = loadFile(dir, filename);
    QJsonDocument loadDoc(QJsonDocument::fromJson(content));
    return loadDoc.array();
}

bool FileSystemManager::fileExists(QString dir, QString filename) const {
    QString path = getDataDir(dir) + filename;
	return QDir().exists(path);
}

QStringList FileSystemManager::getFilenames(QString dir, QString filter) const {
	QString path = getDataDir(dir);
	return QDir(path).entryList(QStringList {filter});
}

void FileSystemManager::deleteFile(QString dir, QString filename) const {
    QString path = getDataDir(dir) + filename;
    if (!QDir().exists(path)) return;
    QFile::remove(path);
}

void FileSystemManager::importFile(QString inputPath, QString dir, bool overwrite) const {
    QString outputPath = getDataDir(dir) + QFileInfo(inputPath).fileName();
    if (QDir().exists(outputPath)) {
        if (overwrite) {
            QFile::remove(outputPath);
        } else {
            return;
        }
    }
    QFile::copy(inputPath, outputPath);
    // if inputPath is a resource file (":/...") fix the permissions:
    if (inputPath.startsWith(":")) {
        QFile(outputPath).setPermissions(QFile::ReadOwner|QFile::WriteOwner
                                         |QFile::ReadGroup|QFile::ReadOther);
    }
}

void FileSystemManager::exportFile(QString dir, QString filename, QString outputPath, bool overwrite) const {
    QString inputPath = getDataDir(dir) + filename;
    if (QDir().exists(outputPath)) {
        if (overwrite) {
            QFile::remove(outputPath);
        } else {
            return;
        }
    }
    QFile::copy(inputPath, outputPath);
}

bool FileSystemManager::appendToFile(QString dir, QString filename, QString text) const {
    QString path = QString(m_dataRoot);
    if (dir.length()) {
        path += dir + "/";
        QDir().mkpath(path);
    }
    QFile saveFile(path + filename);
    if (!saveFile.open(QIODevice::Append)) {
        qWarning() << "Couldn't write to file " + path + filename + ": " << saveFile.errorString();
        return false;
    }
    saveFile.write(text.toUtf8());
    saveFile.close();
    return true;
}

QString FileSystemManager::getDataDir(QString dir) const {
    if (dir.length()) {
        return m_dataRoot + dir + "/";
    }
    return m_dataRoot;
}
