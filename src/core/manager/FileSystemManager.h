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

#ifndef DATAHELPER_H
#define DATAHELPER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QString>
#include <QByteArray>

#include <iostream>


/**
 * @brief The FileSystemManager class simplifies the file system access.
 * It can also read and write QJson objects to and from files.
 */
class FileSystemManager : public QObject {

    Q_OBJECT

public:
	/**
	 * @brief FileSystemManager creates an object and initializes all directories.
	 */
    FileSystemManager();

	/**
	 * @brief saveFile saves QByteArray object to a file in the data dir
	 * It overwrites the file if it already exists.
	 * @param dir sub dir inside the app data dir
	 * @param filename for the file that will be written
	 * @param content to be written
	 * @return true if the file was successfully written
	 */
    bool saveFile(QString dir, QString filename, QByteArray content) const;
	/**
	 * @brief saveFile saves QJsonObject object to a file in the data dir
	 * It overwrites the file if it already exists.
	 * @param dir sub dir inside the app data dir
	 * @param filename for the file that will be written
	 * @param content to be written
	 * @return true if the file was successfully written
	 */
    bool saveFile(QString dir, QString filename, QJsonObject content) const;
	/**
	 * @brief saveFile saves QJsonArray object to a file in the data dir
	 * It overwrites the file if it already exists.
	 * @param dir sub dir inside the app data dir
	 * @param filename for the file that will be written
	 * @param content to be written
	 * @return true if the file was successfully written
	 */
    bool saveFile(QString dir, QString filename, QJsonArray content) const;

	/**
	 * @brief loadFile loads a QByteArray from a file.
	 * It returns an empty object if the file does not exist.
	 * @param dir sub dir inside the app data dir
	 * @param filename of the file to be read
	 * @return the object read from the file or an emtpy object if the file does not exists
	 */
    QByteArray loadFile(QString dir, QString filename) const;
	/**
	 * @brief loadJsonObject loads a QJsonObject from a file.
	 * It returns an empty object if the file does not exist.
	 * @param dir sub dir inside the app data dir
	 * @param filename of the file to be read
	 * @return the object read from the file or an emtpy object if the file does not exists
	 */
    QJsonObject loadJsonObject(QString dir, QString filename) const;
	/**
	 * @brief loadJsonArray loads a QJsonArray from a file.
	 * It returns an empty object if the file does not exist.
	 * @param dir sub dir inside the app data dir
	 * @param filename of the file to be read
	 * @return the object read from the file or an emtpy object if the file does not exists
	 */
    QJsonArray loadJsonArray(QString dir, QString filename) const;

	/**
	 * @brief fileExists checks if a file exits in the app data dir
	 * @param dir sub dir inside the app data dir
	 * @param filename of the file
	 * @return true if the file exists
	 */
    bool fileExists(QString dir, QString filename) const;

	/**
	 * @brief getFilenames returns a list of all files and dirs in the path
	 * that match the filter pattern
	 * @param dir sub dir inside the app data dir
	 * @param filter string to match the files (i.e. "*.txt")
	 * @return list of files and dirs
	 */
	QStringList getFilenames(QString dir, QString filter) const;

	/**
	 * @brief deleteFile delets a file in the app data dir
	 * @param dir sub dir inside the app data dir
	 * @param filename of the file to be deleted
	 */
    void deleteFile(QString dir, QString filename) const;

    /**
     * @brief importFile imports a file from the filesystem to the app data directory
     * @param inputPath path to the file in the filesystem
     * @param dir inside the app data dir
     * @param overwrite true to overwrite a possible existing file
     */
    void importFile(QString inputPath, QString dir, bool overwrite = true) const;

    /**
     * @brief exportFile
     * @param dir
     * @param filename
     * @param outputPath
     * @param overwrite
     */
    void exportFile(QString dir, QString filename, QString outputPath, bool overwrite = true) const;

    /**
     * @brief appendToFile appends data to a file
     * It creates the file if it doesn't already exists.
     * @param dir sub dir inside the app data dir
     * @param filename for the file that will be written
     * @param text to be appended
     * @return true if the file was successfully written
     */
    bool appendToFile(QString dir, QString filename, QString text) const;

public slots:
	/**
	 * @brief getDataDir returns the full path to a sub dir inside the app data die
	 * @param dir sub dir inside the app data dir
	 * @return full path to the dir
	 */
    QString getDataDir(QString dir) const;  // used in GUI for projects list

protected:
	/**
	 * @brief m_dataRoot is the path to the app data directory
	 */
    QString m_dataRoot;

};

#endif // DATAHELPER_H
