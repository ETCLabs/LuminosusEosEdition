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

#ifndef UTILS_H
#define UTILS_H

#include <QtMath>
#include <QDataStream>
#include <QByteArray>

#include <algorithm>
#include <chrono>


// This file includes some often used utility functions.

// Limits / saturates a value with min and max bounds:
template<typename T1, typename T2, typename T3>
T2 limit(T1 min, T2 value, T3 max) {
	return qMax(T2(min), qMin(value, T2(max)));
}

template<typename T>
T limitToOne(T value) {
    return std::max(0.0, std::min(value, 1.0));
}

template<typename T, typename E>
bool contains(const T& list, const E& el) {
    return list.end() != std::find(list.begin(), list.end(), el);
}

template<typename T, typename E>
bool mapContains(const T& list, const E& el) {
    return list.end() != list.find(el);
}

template<typename T, typename E>
bool appendUnique(T& vec, E& el) {
    if (!contains(vec, el)) {
        vec.push_back(el);
        return true;
    }
    return false;
}

template<typename T, typename E>
void removeUnique(T& vec, E& el) {
    auto pos = std::find(vec.begin(), vec.end(), el);
    if (pos != vec.end()) {
        vec.erase(pos);
    }
}

namespace HighResTime {  // -------------------

typedef std::chrono::time_point<std::chrono::system_clock> time_point_t;

inline time_point_t now() {
    return std::chrono::system_clock::now();
}

inline double elapsedSecSince(time_point_t start) {
    time_point_t now = HighResTime::now();
    std::chrono::duration<double> elapsed_seconds_duration = now - start;
    double elapsedSeconds = elapsed_seconds_duration.count();
    return elapsedSeconds;
}

inline double diff(time_point_t end, time_point_t start) {
    std::chrono::duration<double> elapsed_seconds_duration = end - start;
    return elapsed_seconds_duration.count();
}

inline double getElapsedSecAndUpdate(time_point_t& lastTime) {
    time_point_t now = HighResTime::now();
    double elapsedSeconds = diff(now, lastTime);
    lastTime = now;
    return elapsedSeconds;
}

}  // end namespace HighResTime -----------------

template<typename T>
QString serialize(const T& x)
{
	QByteArray ba;
	QDataStream s(&ba, QIODevice::WriteOnly);
	s << x;
	QString str = QString::fromLatin1(ba.toBase64());
	return str;
}

template<typename T>
T deserialize(const QString& str)
{
	QByteArray ba = QByteArray::fromBase64(str.toLatin1());
	QDataStream s(&ba, QIODevice::ReadOnly);
	T res; s >> res;
	return res;
}

inline bool isEqualInsensitive(const QString& str1, const QString& str2) {
	int result = str1.compare(str2, Qt::CaseInsensitive);
	return result == 0;
}

#endif // UTILS_H

