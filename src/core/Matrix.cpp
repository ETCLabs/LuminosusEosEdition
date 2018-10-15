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

#include "core/Matrix.h"

#include <QDebug>
#include <cmath>

// ---------------------------- HSV ----------------------------

HsvMatrix::HsvMatrix()
    : HsvMatrix(1, 1)
{ }

HsvMatrix::HsvMatrix(int width, int height)
    : m_data(qMax(1, width), QVector<HSV>(qMax(1, height)))
    , m_width(qMax(1, width))
    , m_height(qMax(1, height))
{

}

bool HsvMatrix::hasSameSizeAs(const HsvMatrix& other) const {
    return m_width == other.m_width && m_height == other.m_height;
}

bool HsvMatrix::isSmallerThan(const HsvMatrix& other) const {
    return m_width < other.m_width || m_height < other.m_height;
}

void HsvMatrix::rescale(int width, int height) {
    if (width == m_width && height == m_height) return;

    // width and height must be at least 1:
    width = qMax(1, width);
    height = qMax(1, height);

    // change width:
    m_data.resize(width);

    // for all columns:
    for (int x = 0; x < width; ++x) {
        // change height:
        m_data[x].resize(height);
    }
    m_width = width;
    m_height = height;

    // postcondition check because this operation
    if (m_width < 1 || m_height < 1 || m_data.size() != m_width || m_data[0].size() != m_height) {
        if (m_data.size() < 1) {
            qCritical() << "Matrix resize failed.";
        }
        qCritical() << "Matrix resize failed (target: " << width << "x" << height
                    << ", actual:" << m_data.size() << "x" << m_data[0].size() << ")";
    }
}

void HsvMatrix::rescale(const Size& s) {
    rescale(s.width, s.height);
}

void HsvMatrix::expandTo(int width, int height) {
    rescale(qMax(m_width, width), qMax(m_height, height));
}

void HsvMatrix::expandTo(const Size& s) {
    expandTo(s.width, s.height);
}

void HsvMatrix::setFrom(const HsvMatrix& other) {
    int minWidth = qMin(m_width, other.m_width);
    int minHeight = qMin(m_height, other.m_height);
    for (int x=0; x<minWidth; ++x) {
        for (int y=0; y<minHeight; ++y) {
            m_data[x][y] = other.m_data[x][y];
        }
    }
}

void HsvMatrix::fadeTo(const HsvMatrix& other, double pos) {
    for (int x=0; x < m_width; ++x) {
        for (int y=0; y < m_height; ++y) {
            HSV& col = m_data[x][y];
            const HSV& colOther = other.at(x, y);
            col.h = col.h * (1 - pos) + colOther.h * pos;
            col.s = col.s * (1 - pos) + colOther.s * pos;
            col.v = col.v * (1 - pos) + colOther.v * pos;
        }
    }
}

// ---------------------------- RGB ----------------------------

RgbMatrix::RgbMatrix()
    : RgbMatrix(1, 1)
{ }

RgbMatrix::RgbMatrix(int width, int height)
    : m_data(qMax(1, width), QVector<RGB>(qMax(1, height)))
    , m_width(qMax(1, width))
    , m_height(qMax(1, height))
{

}

bool RgbMatrix::hasSameSizeAs(const RgbMatrix& other) const {
    return m_width == other.m_width && m_height == other.m_height;
}

bool RgbMatrix::isSmallerThan(const RgbMatrix& other) const {
    return m_width < other.m_width || m_height < other.m_height;
}

void RgbMatrix::rescale(int width, int height) {
    if (width == m_width && height == m_height) return;

    // width and height must be at least 1:
    width = qMax(1, width);
    height = qMax(1, height);

    // change width:
    m_data.resize(width);

    // for all columns:
    for (int x = 0; x < width; ++x) {
        // change height:
        m_data[x].resize(height);
    }
    m_width = width;
    m_height = height;

    // postcondition check because this operation
    if (m_width < 1 || m_height < 1 || m_data.size() != m_width || m_data[0].size() != m_height) {
        if (m_data.size() < 1) {
            qCritical() << "Matrix resize failed.";
        }
        qCritical() << "Matrix resize failed (target: " << width << "x" << height
                    << ", actual:" << m_data.size() << "x" << m_data[0].size() << ")";
    }
}

void RgbMatrix::rescale(const Size& s) {
    rescale(s.width, s.height);
}

void RgbMatrix::expandTo(int width, int height) {
    rescale(qMax(m_width, width), qMax(m_height, height));
}

void RgbMatrix::expandTo(const Size& s) {
    expandTo(s.width, s.height);
}

void RgbMatrix::setFrom(const RgbMatrix& other) {
    int minWidth = qMin(m_width, other.m_width);
    int minHeight = qMin(m_height, other.m_height);
    for (int x=0; x<minWidth; ++x) {
        for (int y=0; y<minHeight; ++y) {
            m_data[x][y] = other.m_data[x][y];
        }
    }
}

void RgbMatrix::addHtp(const RgbMatrix& other) {
    int minWidth = qMin(m_width, other.m_width);
    int minHeight = qMin(m_height, other.m_height);
    for (int x=0; x<minWidth; ++x) {
        for (int y=0; y<minHeight; ++y) {
            m_data[x][y].mixHtp(other.m_data[x][y]);
        }
    }
}

QDataStream& operator<<(QDataStream& out, const HsvMatrix& matrix) {
    out << matrix.m_data;
    out << matrix.m_width;
    out << matrix.m_height;
    return out;
}

QDataStream& operator>>(QDataStream& in, HsvMatrix& matrix) {
    in >> matrix.m_data;
    in >> matrix.m_width;
    in >> matrix.m_height;

    // make sure matrix has correct size even if not restored correctly:
    matrix.m_width = qMax(matrix.m_width, 1);
    matrix.m_height = qMax(matrix.m_height, 1);

    // change width:
    matrix.m_data.resize(matrix.m_width);

    // for all columns:
    for (int x = 0; x < matrix.m_width; ++x) {
        // change height:
        matrix.m_data[x].resize(matrix.m_height);
    }

    return in;
}

QDataStream&operator<<(QDataStream& out, const HSV& col) {
    out << col.h;
    out << col.s;
    out << col.v;
    return out;
}

QDataStream&operator>>(QDataStream& in, HSV& col) {
    in >> col.h;
    in >> col.s;
    in >> col.v;
    return in;
}

HSV::HSV(const RGB& rgb) {
    double r, g, b, maxc, minc, delta;
    r = rgb.r;
    g = rgb.g;
    b = rgb.b;
    maxc = std::max(std::max(r, g), b);
    minc = std::min(std::min(r, g), b);
    if (minc == maxc) {
        h = 0;
        s = 0;
        v = maxc;
        return;
    }
    delta = maxc - minc;
    s = delta / maxc;
    if (r == maxc) {
        h = (g-b) / delta;
    } else if (g == maxc) {
        h = 2 + (b-r) / delta;
    } else {
        h = 4 + (r-g) / delta;
    }
    h = std::fmod(h/6, 1);
    if (h < 0) {
        h += 1;
    }
    v = maxc;
}

RGB::RGB(const HSV& hsv) {
    double h, s, v, f, p, q, t;
    h = hsv.h;
    s = hsv.s;
    v = hsv.v;
    if (s == 0.0) {
        r = v;
        g = v;
        b = v;
        return;
    }
    int i = int(h*6);
    f = (h*6)-i;
    p = v*(1-s);
    q = v*(1-s*f);
    t = v*(1-s*(1-f));
    i = i % 6;
    switch (i) {
        case 0: r = v, g = t, b = p; break;
        case 1: r = q, g = v, b = p; break;
        case 2: r = p, g = v, b = t; break;
        case 3: r = p, g = q, b = v; break;
        case 4: r = t, g = p, b = v; break;
        case 5: r = v, g = p, b = q; break;
        default: r = 0, g = 0, b = 0; break;
    }
}
