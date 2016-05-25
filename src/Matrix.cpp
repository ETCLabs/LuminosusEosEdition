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

ColorMatrix::ColorMatrix()
	: hsvData(1, std::vector<std::vector<double>>(1, std::vector<double>(3, 0)))
	, hsvValid(true)
	, rgbData(1, std::vector<std::vector<double>>(1, std::vector<double>(3, 0)))
	, rgbValid(true)
	, value(0)
	, valueValid(true)
{}

void ColorMatrix::rescaleTo(std::size_t sx, std::size_t sy) {
	// FIXME: is this correct?
	if (hsvData.size() != sx || hsvData[0].size() != sy
			|| rgbData.size() != sx || rgbData[0].size() != sy
			|| sx < 1 || sy < 1) {
		return;
	}
	// change width:
	hsvData.resize(sx);
	rgbData.resize(sx);
	// for all columns:
	for (std::size_t x=0; x<sx; x++) {
		// change height:
		hsvData[x].resize(sy);
		rgbData[x].resize(sy);
		// for all color values:
		for (std::size_t y=0; y<sy; y++) {
			// set length of color values to three values (RGB or HSV):
			hsvData[x][y].resize(3);
			rgbData[x][y].resize(3);
		}
	}
}

void ColorMatrix::setHsv(double h, double s, double v) {
	std::size_t sx = getSX();
	std::size_t sy = getSY();
	for (std::size_t x=0; x<sx; x++) {
		for (std::size_t y=0; y<sy; y++) {
			hsvData[x][y][0] = h;
			hsvData[x][y][1] = s;
			hsvData[x][y][2] = v;
		}
	}
	hsvValid = true;
	rgbValid = false;
	valueValid = false;
}

void ColorMatrix::setHsvAt(std::size_t x, std::size_t y, double h, double s, double v) {
	// TODO: check if x and y are in the boundaries
	hsvData[x][y][0] = h;
	hsvData[x][y][1] = s;
	hsvData[x][y][2] = v;
	hsvValid = true;
	rgbValid = false;
	valueValid = false;
}

void ColorMatrix::setRgb(double r, double g, double b) {
	std::size_t sx = getSX();
	std::size_t sy = getSY();
	for (std::size_t x=0; x<sx; x++) {
		for (std::size_t y=0; y<sy; y++) {
			rgbData[x][y][0] = r;
			rgbData[x][y][1] = g;
			rgbData[x][y][2] = b;
		}
	}
	hsvValid = false;
	rgbValid = true;
	valueValid = false;
}

void ColorMatrix::setRgb(const colorData2d &newRgb) {
	rgbData = newRgb;
	hsvValid = false;
	rgbValid = true;
	valueValid = false;
}

void ColorMatrix::setRgbAt(std::size_t x, std::size_t y, double r, double g, double b) {
	// TODO: check if x and y are in the boundaries
	rgbData[x][y][0] = r;
	rgbData[x][y][1] = g;
	rgbData[x][y][2] = b;
	hsvValid = false;
	rgbValid = true;
	valueValid = false;
}

void ColorMatrix::setValue(double v) {
	value = v;
	hsvValid = false;
	rgbValid = false;
	valueValid = true;
}

double ColorMatrix::getValue() const {
	if (!valueValid) {
		if (hsvValid) {
			value = hsvData[0][0][2];
		} else {
			value = std::max(std::max(rgbData[0][0][0], rgbData[0][0][1]), rgbData[0][0][0]);
		}
		valueValid = true;
	}
	return value;
}

void ColorMatrix::updateHsv() const {
	if (rgbValid) {
		rgbToHsv();
	} else {
		const_cast<ColorMatrix*>(this)->setHsv(0, value, 0);
	}
	hsvValid = true;
}

void ColorMatrix::updateRgb() const {
	if (hsvValid) {
		hsvToRgb();
	} else {
		const_cast<ColorMatrix*>(this)->setRgb(value, value, value);
	}
	rgbValid = true;
}

void ColorMatrix::rgbToHsv() const {
	std::size_t sx = getSX();
	std::size_t sy = getSY();
	for (std::size_t x=0; x<sx; ++x) {
		for (std::size_t y=0; y<sy; ++y) {
			double r, g, b, maxc, minc, h, s, delta;
			r = rgbData[x][y][0];
			g = rgbData[x][y][1];
			b = rgbData[x][y][2];
			maxc = std::max(std::max(r, g), b);
			minc = std::min(std::min(r, g), b);
			if (minc == maxc) {
				hsvData[x][y][0] = 0;
				hsvData[x][y][1] = 0;
				hsvData[x][y][2] = maxc;
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
            hsvData[x][y][0] = h;
            hsvData[x][y][1] = s;
            hsvData[x][y][2] = maxc;
        }
    }
}


void ColorMatrix::hsvToRgb() const {
    std::size_t sx = getSX();
    std::size_t sy = getSY();
    for (std::size_t x=0; x<sx; ++x) {
        for (std::size_t y=0; y<sy; ++y) {
            double h, s, v, f, p, q, t, r, g, b;
            h = hsvData[x][y][0];
            s = hsvData[x][y][1];
            v = hsvData[x][y][2];
            if (s == 0.0) {
                rgbData[x][y][0] = v;
                rgbData[x][y][1] = v;
                rgbData[x][y][2] = v;
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
            rgbData[x][y][0] = r;
            rgbData[x][y][1] = g;
            rgbData[x][y][2] = b;
        }
    }
}
