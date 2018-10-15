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

#ifndef EOSOSCMESSAGE_H
#define EOSOSCMESSAGE_H

#include "osc/OSCMessage.h"

#include <QtGlobal>
#include <QVector>
#include <QVariant>
#include <QByteArray>


/**
 * @brief The EosOSCMessage class prepares OSC messages from an Eos console.
 */
class EosOSCMessage
{

public:
    /**
     * @brief EosOSCMessage creates an empty message.
     * Exists to support Qt types that need a default constructor.
     */
    EosOSCMessage();

    /**
     * @brief EosOSCMessage creates an EosOSCMessage object from a normal OSC message.
     * @param msg a normal OSC message from an Eos console
     */
    explicit EosOSCMessage(const OSCMessage& msg);

    /**
     * @brief add tries to complete an uncomplete OSC List Convention message with another message
     * @param msg possible part of OSC List Convention message
     * @return true if message could be appended
     */
    bool add(const OSCMessage& msg);

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
     * @brief arguments returns the arguments as a vector of QVariants
     * @return arguments as QVariants
     */
    const QVector<QVariant>& arguments() const { return m_arguments; }

    /**
     * @brief isTrue returns if the arguments probably mean true or "do that"
     * @return true if there are no arguments or if the first argument is true or 1
     */
    bool isTrue() const;

    /**
     * @brief value converts the arguments to a single number and returns it
     * @return the value of the first argument if it exists and is a number, otherwise return 0.0
     */
    qreal numericValue() const;

    /**
     * @brief stringValue returns a string containing the first argument if existent
     * @return first argument as a string
     */
    QString stringValue() const;

    /**
     * @brief userIdProvided returns if a Eos OSC user id was provided
     * @return true if provided
     */
    bool userIdProvided() const { return m_userId != -1; }

    /**
     * @brief userId returns the Eos OSC user id of this message
     * @return a user id or -1 if not provided
     */
    int userId() const { return m_userId; }

    /**
     * @brief isComplete return if this message is not an uncomplete OSC List Convention message
     * @return true if it is complete
     */
    bool isComplete() const { return m_arguments.size() >= m_listConventionArgumentCount; }

protected:
    /**
     * @brief m_path stores the path as a list of strings (separated at slashes)
     */
    QStringList m_path;
    /**
     * @brief m_arguments is the list of arguments as QVariants
     */
    QVector<QVariant> m_arguments;

    /**
     * @brief m_userId Eos OSC user id or -1 if not provided
     */
    int m_userId;

    /**
     * @brief m_listConventionArgumentCount count of arguments of the complete message
     */
    int m_listConventionArgumentCount;
};

// register this class to the Qt MetaType system to be able to use it i.e. in QVariant:
Q_DECLARE_METATYPE(EosOSCMessage)

#endif // EOSOSCMESSAGE_H
