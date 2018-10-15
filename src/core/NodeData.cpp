#include "NodeData.h"

#include <QDebug>
#include <cmath>


ColorMatrix::ColorMatrix()
    : m_width(1)
    , m_height(1)
    , m_hsvData(1, 1)
    , m_hsvIsValid(false)
    , m_rgbData(1, 1)
    , m_rgbIsValid(false)
    , m_value(0)
    , m_valueIsValid(true)
    , m_absoluteMaximum(1)
    , m_absoluteMaximumIsProvided(false)
//    , m_offsetX(0)
//    , m_offsetY(0)
{}

void ColorMatrix::addHtp(const ColorMatrix& other) {
    // Special Case:
    // check if in both Matrices only the value attribute is valid:
    if (!m_hsvIsValid && !m_rgbIsValid && !other.m_hsvIsValid && !other.m_rgbIsValid) {
        m_value = std::max(m_value, other.m_value);
        return;
    }

    // do normal RGB HTP mix:
    if (!m_rgbIsValid) updateRgb();
    if (!other.m_rgbIsValid) other.updateRgb();
    m_rgbData.addHtp(other.m_rgbData);
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

void ColorMatrix::rescaleTo(int sx, int sy) {
    if (sx == m_width && sy == m_height) return;
    if (sx < 1 || sy < 1) {
        // sx and sy must be at least 1, aborting:
        qWarning() << "Requested ColorMatrix size is invalid.";
        return;
    }
    m_hsvData.rescale(sx, sy);
    m_rgbData.rescale(sx, sy);
    m_width = sx;
    m_height = sy;

    // postcondition check:
    if (m_hsvData.width() != m_width || m_hsvData.height() != m_height
            || m_rgbData.width() != m_width || m_rgbData.height() != m_height) {
        // -> size is not correct:
        qCritical() << "ColorMatrix resize failed.";
    }
}

void ColorMatrix::setHsv(double h, double s, double v) {
    int sx = width();
    int sy = height();
    HSV newVal(h, s, v);
    for (int x=0; x<sx; x++) {
        for (int y=0; y<sy; y++) {
            m_hsvData.at(x, y) = newVal;
        }
    }
    m_hsvIsValid = true;
    m_rgbIsValid = false;
    m_valueIsValid = false;
}

void ColorMatrix::setHsvAt(int x, int y, double h, double s, double v) {
    m_hsvData.at(x, y) = HSV(h, s, v);
    m_hsvIsValid = true;
    m_rgbIsValid = false;
    m_valueIsValid = false;
}

void ColorMatrix::setRgb(double r, double g, double b) {
    int sx = width();
    int sy = height();
    RGB newVal(r, g, b);
    for (int x=0; x<sx; x++) {
        for (int y=0; y<sy; y++) {
            m_rgbData.at(x, y) = newVal;
        }
    }
    m_hsvIsValid = false;
    m_rgbIsValid = true;
    m_valueIsValid = false;
}

void ColorMatrix::setRgb(const RgbMatrix &newRgb) {
    m_rgbData = newRgb;
    m_hsvIsValid = false;
    m_rgbIsValid = true;
    m_valueIsValid = false;
}

void ColorMatrix::setRgbAt(int x, int y, double r, double g, double b) {
    m_rgbData.at(x, y) = RGB(r, g, b);
    m_hsvIsValid = false;
    m_rgbIsValid = true;
    m_valueIsValid = false;
}

//RGB ColorMatrix::getOffsetRgbValue() const {
//    return getRgbAt(std::min(m_offsetX, getSX()), std::min(m_offsetY, getSY()));
//}

void ColorMatrix::setValue(double v) {
    m_value = v;
    m_hsvIsValid = false;
    m_rgbIsValid = false;
    m_valueIsValid = true;
}

double ColorMatrix::getValue() const {
    if (!m_valueIsValid) {
        if (m_hsvIsValid) {
            m_value = m_hsvData.at(0, 0).v;
        } else {
            m_value = m_rgbData.at(0, 0).max();
        }
        m_valueIsValid = true;
    }
    return m_value;
}

//double ColorMatrix::getOffsetValue() const {
//    colorVector rgb = getRgbAt(std::min(m_offsetX, m_rgbData.size() - 1), std::min(m_offsetY, m_rgbData[0].size() - 1));
//    return std::max(std::max(rgb[0], rgb[1]), rgb[2]);
//}

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
    int sx = width();
    int sy = height();
    for (int x=0; x<sx; ++x) {
        for (int y=0; y<sy; ++y) {
            double r, g, b, maxc, minc, h, s, delta;
            const RGB& rgb = m_rgbData.at(x, y);
            r = rgb.r;
            g = rgb.b;
            b = rgb.g;
            maxc = std::max(std::max(r, g), b);
            minc = std::min(std::min(r, g), b);
            if (minc == maxc) {
                m_hsvData.at(x, y) = HSV(0, 0, maxc);
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
            m_hsvData.at(x, y) = HSV(h, s, maxc);
        }
    }
}


void ColorMatrix::hsvToRgb() const {
    int sx = width();
    int sy = height();
    for (int x=0; x<sx; ++x) {
        for (int y=0; y<sy; ++y) {
            double h, s, v, f, p, q, t, r, g, b;
            const HSV& hsv = m_hsvData.at(x, y);
            h = hsv.h;
            s = hsv.s;
            v = hsv.v;
            if (s == 0.0) {
                m_rgbData.at(x, y) = RGB(v, v, v);
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
            m_rgbData.at(x, y) = RGB(r, g, b);
        }
    }
}
