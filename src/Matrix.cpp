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

#include <cmath>

#include "Matrix.h"
#include "utils.h"

ColorMatrix::ColorMatrix()
    : m_hsvData(1, std::vector<std::vector<double>>(1, std::vector<double>(3, 0)))
    , m_hsvIsValid(false)
    , m_rgbData(1, std::vector<std::vector<double>>(1, std::vector<double>(3, 0)))
    , m_rgbIsValid(false)
    , m_value(0)
    , m_valueIsValid(true)
    , m_absoluteMaximum(1)
    , m_absoluteMaximumIsProvided(false)
{}

void ColorMatrix::rescaleTo(std::size_t sx, std::size_t sy) {
    if (sx == getSX() && sy == getSY()) {
        return;
    }
    if (sx < 1 || sy < 1) {
		return;
	}
	// change width:
    m_hsvData.resize(sx);
    m_rgbData.resize(sx);
	// for all columns:
	for (std::size_t x=0; x<sx; x++) {
		// change height:
        m_hsvData[x].resize(sy);
        m_rgbData[x].resize(sy);
		// for all color values:
		for (std::size_t y=0; y<sy; y++) {
			// set length of color values to three values (RGB or HSV):
            m_hsvData[x][y].resize(3);
            m_rgbData[x][y].resize(3);
		}
    }
}

void ColorMatrix::mixHtp(const ColorMatrix& other) {
    // Special Case:
    // check if in both Matrices only the value attribute is valid:
    if (!m_hsvIsValid && !m_rgbIsValid && !other.m_hsvIsValid && !other.m_rgbIsValid) {
        m_value = std::max(m_value, other.m_value);
        return;
    }

    // do normal RGB HTP mix:
    if (!m_rgbIsValid) updateRgb();
    if (!other.m_rgbIsValid) updateRgb();
    std::size_t sx = getSX();
    std::size_t sy = getSY();
    for (std::size_t x=0; x<sx; x++) {
        for (std::size_t y=0; y<sy; y++) {
            m_rgbData[x][y][0] = std::max(m_rgbData[x][y][0], other.m_rgbData[x][y][0]);
            m_rgbData[x][y][1] = std::max(m_rgbData[x][y][1], other.m_rgbData[x][y][1]);
            m_rgbData[x][y][2] = std::max(m_rgbData[x][y][2], other.m_rgbData[x][y][2]);
        }
    }
    m_hsvIsValid = false;
    m_valueIsValid = false;

    // absolute maximum:
    if (other.m_absoluteMaximumIsProvided) {
        if (m_absoluteMaximumIsProvided) {
            m_absoluteMaximum = std::max(m_absoluteMaximum, other.m_absoluteMaximum);
        } else {
            m_absoluteMaximum = other.m_absoluteMaximum;
            m_absoluteMaximumIsProvided = true;
        }
    }
}

void ColorMatrix::setHsv(double h, double s, double v) {
	std::size_t sx = getSX();
	std::size_t sy = getSY();
	for (std::size_t x=0; x<sx; x++) {
		for (std::size_t y=0; y<sy; y++) {
            m_hsvData[x][y][0] = h;
            m_hsvData[x][y][1] = s;
            m_hsvData[x][y][2] = v;
		}
	}
    m_hsvIsValid = true;
    m_rgbIsValid = false;
    m_valueIsValid = false;
}

void ColorMatrix::setHsvAt(std::size_t x, std::size_t y, double h, double s, double v) {
	// TODO: check if x and y are in the boundaries
    m_hsvData[x][y][0] = h;
    m_hsvData[x][y][1] = s;
    m_hsvData[x][y][2] = v;
    m_hsvIsValid = true;
    m_rgbIsValid = false;
    m_valueIsValid = false;
}

void ColorMatrix::setRgb(double r, double g, double b) {
	std::size_t sx = getSX();
	std::size_t sy = getSY();
	for (std::size_t x=0; x<sx; x++) {
		for (std::size_t y=0; y<sy; y++) {
            m_rgbData[x][y][0] = r;
            m_rgbData[x][y][1] = g;
            m_rgbData[x][y][2] = b;
		}
	}
    m_hsvIsValid = false;
    m_rgbIsValid = true;
    m_valueIsValid = false;
}

void ColorMatrix::setRgb(const colorData2d &newRgb) {
    m_rgbData = newRgb;
    m_hsvIsValid = false;
    m_rgbIsValid = true;
    m_valueIsValid = false;
}

void ColorMatrix::setRgbAt(std::size_t x, std::size_t y, double r, double g, double b) {
	// TODO: check if x and y are in the boundaries
    m_rgbData[x][y][0] = r;
    m_rgbData[x][y][1] = g;
    m_rgbData[x][y][2] = b;
    m_hsvIsValid = false;
    m_rgbIsValid = true;
    m_valueIsValid = false;
}

void ColorMatrix::setValue(double v) {
    m_value = limit(0, v, 1);
    m_hsvIsValid = false;
    m_rgbIsValid = false;
    m_valueIsValid = true;
}

double ColorMatrix::getValue() const {
    if (!m_valueIsValid) {
        if (m_hsvIsValid) {
            m_value = m_hsvData[0][0][2];
		} else {
            m_value = std::max(std::max(m_rgbData[0][0][0], m_rgbData[0][0][1]), m_rgbData[0][0][0]);
		}
        m_valueIsValid = true;
	}
    return m_value;
}

void ColorMatrix::setAbsoluteMaximum(double value) {
    m_absoluteMaximum = value;
    m_absoluteMaximumIsProvided = true;
}

void ColorMatrix::setAbsoluteValue(double v) {
    m_absoluteMaximum = v;
    m_value = 1;
    m_hsvIsValid = false;
    m_rgbIsValid = false;
    m_valueIsValid = true;
    m_absoluteMaximumIsProvided = true;
}

double ColorMatrix::getAbsoluteValue(double defaultMax) const {
    if (m_absoluteMaximumIsProvided) {
        return m_value * m_absoluteMaximum;
    } else {
        return m_value * defaultMax;
    }
}

void ColorMatrix::updateHsv() const {
    if (m_rgbIsValid) {
		rgbToHsv();
	} else {
        const_cast<ColorMatrix*>(this)->setHsv(0, 0, m_value);
	}
    m_hsvIsValid = true;
}

void ColorMatrix::updateRgb() const {
    if (m_hsvIsValid) {
		hsvToRgb();
	} else {
        const_cast<ColorMatrix*>(this)->setRgb(m_value, m_value, m_value);
	}
    m_rgbIsValid = true;
}

void ColorMatrix::rgbToHsv() const {
	std::size_t sx = getSX();
	std::size_t sy = getSY();
	for (std::size_t x=0; x<sx; ++x) {
		for (std::size_t y=0; y<sy; ++y) {
			double r, g, b, maxc, minc, h, s, delta;
            r = m_rgbData[x][y][0];
            g = m_rgbData[x][y][1];
            b = m_rgbData[x][y][2];
			maxc = std::max(std::max(r, g), b);
			minc = std::min(std::min(r, g), b);
			if (minc == maxc) {
                m_hsvData[x][y][0] = 0;
                m_hsvData[x][y][1] = 0;
                m_hsvData[x][y][2] = maxc;
				continue;
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
            m_hsvData[x][y][0] = h;
            m_hsvData[x][y][1] = s;
            m_hsvData[x][y][2] = maxc;
        }
    }
}


void ColorMatrix::hsvToRgb() const {
    std::size_t sx = getSX();
    std::size_t sy = getSY();
    for (std::size_t x=0; x<sx; ++x) {
        for (std::size_t y=0; y<sy; ++y) {
            double h, s, v, f, p, q, t, r, g, b;
            h = m_hsvData[x][y][0];
            s = m_hsvData[x][y][1];
            v = m_hsvData[x][y][2];
            if (s == 0.0) {
                m_rgbData[x][y][0] = v;
                m_rgbData[x][y][1] = v;
                m_rgbData[x][y][2] = v;
                continue;
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
            m_rgbData[x][y][0] = r;
            m_rgbData[x][y][1] = g;
            m_rgbData[x][y][2] = b;
        }
    }
}
