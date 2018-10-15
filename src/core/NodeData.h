#ifndef NODEDATA_H
#define NODEDATA_H

#include "core/Matrix.h"

#include <QSize>
#include <vector>
#include <cmath>

/**
 * @brief The ColorMatrix struct can store a 2D matrix of color values.
 *
 * The data can be accessed as RGB or HSV values (conversions are cached).
 * It can also be effeciently used to get and store a single 1D value,
 * that is only converted when necessary to RGB or HSV values.
 * The size of the matrix can be changed anytime.
 */
struct ColorMatrix {

    /**
     * @brief ColorMatrix creates a 1x1 matrix with value 0
     */
    ColorMatrix();
    ColorMatrix(const ColorMatrix&) = default;				// copy
    ColorMatrix(ColorMatrix&&) = default;					// move
    ColorMatrix& operator=(const ColorMatrix& other) & = default; // Copy assignment operator
    ColorMatrix& operator=(ColorMatrix&&) & = default;      // Move assignment operator
    ~ColorMatrix() = default;								// Destructor

    /**
     * @brief mixHtp merges this marix with another one using HTP mode
     * @param other another ColorMatrix object
     */
    void addHtp(const ColorMatrix& other);


    friend struct HsvDataModifier;
    friend struct RgbDataModifier;


    // -------------------- Size and Offset ---------------------

    /**
     * @brief rescaleTo changes the size of the matrix
     * @param sx new width
     * @param sy new height
     */
    void rescaleTo(int sx, int sy);

    /**
     * @brief width returns the width of the matrix
     * @return width
     */
    int width() const {
        return m_width;
    }

    /**
     * @brief height returns the height of the matrix
     * @return height
     */
    int height() const {
        return m_height;
    }

    QSize getSize() const {
        return QSize(int(width()), int(height()));
    }

//    /**
//     * @brief getOffsetX returns the offset on x-axis
//     * @return offset
//     */
//    std::size_t getOffsetX() const { return m_offsetX; }

//    /**
//     * @brief setOffsetX sets the offset on x-axis
//     * @param value new offset value
//     */
//    void setOffsetX(std::size_t value) { m_offsetX = std::max(std::size_t(0), value); }

//    /**
//     * @brief getOffsetY returns the offset on y-axis
//     * @return offset
//     */
//    std::size_t getOffsetY() const { return m_offsetY; }

//    /**
//     * @brief setOffsetY sets the offset on y-axis
//     * @param value new offset value
//     */
//    void setOffsetY(std::size_t value) { m_offsetY = std::max(std::size_t(0), value); }

    // ---------- HSV -------------

    /**
     * @brief setHsv sets all values to a single HSV value
     * @param h hue value [0-1]
     * @param s saturation value [0-1]
     * @param v brightness value [0-1]
     */
    void setHsv(double h, double s, double v);

    /**
     * @brief setHsv sets all values to new HSV values
     * @param newHsv an array of HSV values  [0-1]
     */
    void setHsv(const HsvMatrix& newHsv) {
        m_hsvData = newHsv;
        m_hsvIsValid = true;
        m_rgbIsValid = false;
        m_valueIsValid = false;
    }

    /**
     * @brief setHsvAt sets the value of a single element by HSV values
     * @param x index
     * @param y index
     * @param h hue value [0-1]
     * @param s saturation value [0-1]
     * @param v brightness value [0-1]
     */
    void setHsvAt(int x, int y, double h, double s, double v);

    /**
     * @brief getHsv returns the data of this matrix as HSV values
     * (may be a conversion of RGB values)
     * @return array of HSV values
     */
    const HsvMatrix& getHsv() const {
        if (!m_hsvIsValid) updateHsv();
        return m_hsvData;
    }

    /**
     * @brief getHsvAt returns the data of a single element as HSV values
     * @param x index
     * @param y index
     * @return HSV values
     */
    HSV getHsvAt(int x, int y) const {
        if (!m_hsvIsValid) updateHsv();
        return m_hsvData.at(x, y);
    }

    // ---------- RGB -------------

    /**
     * @brief setRgb sets all values to a single RGB value
     * @param r red value [0-1]
     * @param g green value [0-1]
     * @param b blue value [0-1]
     */
    void setRgb(double r, double g, double b);

    /**
     * @brief setRgb sets all values to new RGB values
     * @param newRgb an array of HSV values  [0-1]
     */
    void setRgb(const RgbMatrix& newRgb);

    /**
     * @brief setRgbAt sets the value of a single element by HSV values
     * @param x index
     * @param y index
     * @param r red value [0-1]
     * @param g green value [0-1]
     * @param b blue value [0-1]
     */
    void setRgbAt(int x, int y, double r, double g, double b);

    /**
     * @brief getRgb returns the data of this matrix as RGB values
     * (may be a conversion of HSV values)
     * @return array of HSV values
     */
    const RgbMatrix& getRgb() const {
        if (!m_rgbIsValid) updateRgb();
        return m_rgbData;
    }

    /**
     * @brief getRgbAt returns the data of a single element as HSV values
     * @param x index
     * @param y index
     * @return RGB values
     */
    RGB getRgbAt(int x, int y) const {
        if (!m_rgbIsValid) updateRgb();
        return m_rgbData.at(x, y);
    }

//    /**
//     * @brief getOffsetRgbValue returns the rgb value at the "offset position"
//     * @return RGB values
//     */
//    RGB getOffsetRgbValue() const;

    // ---------- Value -------------

    /**
     * @brief setValue sets the "value" of this object
     * (RGB and HSV values are only calculated on demand)
     * @param v value [0-1]
     */
    void setValue(double v);

    /**
     * @brief getValue returns the "value" (may be got from the first element of color data)
     * @return value [0-1]
     */
    double getValue() const;

    /**
     * @brief getOffsetValue returns the value at the "offset position"
     * @return value [0-1]
     */
    double getOffsetValue() const;

    /**
     * @brief setAbsoluteMaximum sets the absolute maximum value used to multiply the relative
     * values with to get the absolute values
     * @param value absolute maximum value
     */
    void setAbsoluteMaximum(double value);

    /**
     * @brief setAbsoluteValue sets an absolute value
     * @param v an arbitrary value
     */
    void setAbsoluteValue(double v);

    /**
     * @brief getAbsoluteValue returns the absolute value of this matrix
     * @param defaultMax default absolute maximum value if no one is set (default = 1)
     * @return an absolute value
     */
    double getAbsoluteValue(double defaultMax = 1) const;

    /**
     * @brief getAbsoluteMaximum returns the absolute maximum value
     * @return the maximal absolute value
     */
    double getAbsoluteMaximum() const { return m_absoluteMaximum; }

    /**
     * @brief absoluteMaximumIsProvided return if an absolute maximum value is provided
     * @return true if absolute maximum value is provided
     */
    bool absoluteMaximumIsProvided() const { return m_absoluteMaximumIsProvided; }

    /**
     * @brief resetAbsoluteMaximum resets absolute maximum value
     */
    void resetAbsoluteMaximum() { m_absoluteMaximumIsProvided = false; }

protected:
    /**
     * @brief updateHsv sets the HSV values from RGB data or the value
     */
    void updateHsv() const;

    /**
     * @brief updateRgb sets the RGB values from HSV data or "value"
     */
    void updateRgb() const;

    /**
     * @brief rgbToHsv converts the RGB values in rgbData to HSV and writes them into hsvData
     */
    void rgbToHsv() const;

    /**
     * @brief hsvToRgb converts the HSV values in hsvData to RGB and writes them into rgbData
     */
    void hsvToRgb() const;

    // -------------- member attributes ----------------

    /**
     * @brief m_width is the width of the matrix [1...INT_MAX]
     */
    int m_width;
    /**
     * @brief m_height is the height of the matrix [1...INT_MAX]
     */
    int m_height;

    /**
     * @brief m_hsvData stores the data as HSV values (this is not always up to date)
     */
    mutable HsvMatrix m_hsvData;
    /**
     * @brief m_hsvIsValid is true, if the HSV values are up to date
     */
    mutable bool m_hsvIsValid;
    /**
     * @brief m_rgbData stores the data as RGB values (this is not always up to date)
     */
    mutable RgbMatrix m_rgbData;
    /**
     * @brief m_rgbIsValid is true, if the RGB values are up to date
     */
    mutable bool m_rgbIsValid;
    /**
     * @brief m_value stores the first value of the data (because it is very often used)
     * (this is not always up to date)
     */
    mutable double m_value;
    /**
     * @brief m_valueIsValid is true, if "m_value" is up to date
     */
    mutable bool m_valueIsValid;

    /**
     * @brief m_absoluteMaximum stores the maximum absolute value to be multiplied with
     * the relative values (it is only valid if m_absoluteMaximumProvided is true)
     */
    mutable double m_absoluteMaximum;

    /**
     * @brief m_absoluteMaximumIsProvided is true, if the absoluteValue is set and valid
     */
    mutable bool m_absoluteMaximumIsProvided;

//    /**
//     * @brief m_offsetX offset on x-axis to read from (i.e. because the values before that
//     * were already used / displayed)
//     */
//    std::size_t m_offsetX;

//    /**
//     * @brief m_offsetY offset on y-axis to read from (i.e. because the values before that
//     * were already used / displayed)
//     */
//    std::size_t m_offsetY;
};


#endif // NODEDATA_H
