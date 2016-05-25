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

#ifndef OSCMESSAGE_H
#define OSCMESSAGE_H

#include <QtGlobal>
#include <QVector>
#include <QVariant>
#include <QByteArray>


/**
 * @brief The OSCMessage class represents an OSC Message.
 *
 * currently supported argument types:
 * - bool (FALSE, TRUE)
 * - int (INT32, INT64, optional from STRING)
 * - double (FLOAT32, FLOAT64, optional from STRING)
 * - string (STRING)
 */
class OSCMessage
{

public:
	/**
	 * @brief OSCMessage creates an empty message
	 */
	OSCMessage();

	/**
	 * @brief OSCMessage creates a message from the raw OSC data
	 * @param data raw OSC packet data (without frame)
	 * @param convertNumberStrings true to convert strings containing only digits to numbers
	 */
	OSCMessage(const QByteArray& data, bool convertNumberStrings = false);

	/**
	 * @brief setData sets the data from the raw OSC data
	 * @param data raw OSC packet data (without frame)
	 * @param convertNumberStrings true to convert strings containing only digits to numbers
	 */
	void setData(const QByteArray& data, bool convertNumberStrings = false);

	/**
	 * @brief isValid returns if the message is valid and not emtpy
	 * @return true if valid and not empty
	 */
	bool isValid() const { return m_isValid; }

	/**
	 * @brief path returns the path as a list of strings
	 * @return the path separated at the slashes
	 */
	const QStringList& path() const { return m_path; }

	/**
	 * @brief pathPart returns a specific part of the path
	 * @param index of the part to be returned
	 * @return part of the path as string
	 */
	QString pathPart(int index) const;

	/**
	 * @brief pathStartsWith returns true if the path starts with a given string
	 * @param value string to check
	 * @return true if the path starts with a given string
	 */
	bool pathStartsWith(QString value) const;

	/**
	 * @brief pathString returns the path as a single string
	 * @return path
	 */
	const QString& pathString() const { return m_pathString; }

	/**
	 * @brief arguments returns the arguments as a vector of QVariants
	 * @return arguments as QVariants
	 */
	const QVector<QVariant>& arguments() const { return m_arguments; }

	/**
	 * @brief isTrue returns if the arguments probably mean true or "do that"
	 * @return true if there are no arguments or if the first argument is true or 1
	 */
	bool isTrue();

	/**
	 * @brief value converts the arguments to a single number and returns it
	 * @return the value of the first argument if it exists and is a number, otherwise return 0.0
	 */
	qreal value();

	// ------------- Debug ----------------------

	/**
	 * @brief getArgumentsAsDebugString returns the arguments as a human readable string
	 * (for debugging)
	 * @return a srting with the arguments
	 */
	QString getArgumentsAsDebugString();

	/**
	 * @brief printToQDebug prints the message in human readable form to QDebug
	 * (for debugging only)
	 */
	void printToQDebug() const;

private:
	/**
	 * @brief getPathFromMessage extracts the path string from the raw OSC packet data
	 * @param data the raw OSC packet data (without frame)
	 * @return path string
	 */
	static QString getPathFromMessage(const QByteArray& data);

protected:
	/**
	 * @brief m_pathString stores the path as a string
	 */
	QString				m_pathString;
	/**
	 * @brief m_path stores the path as a list of strings (separated at slashes)
	 */
	QStringList			m_path;
	/**
	 * @brief m_arguments is the list of arguments as QVariants
	 */
	QVector<QVariant>	m_arguments;
	/**
	 * @brief m_isValid is true, if the message is valid (a path exists)
	 */
	bool				m_isValid;
};

#endif // OSCMESSAGE_H
