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

#ifndef HSV_ARRAY
#define HSV_ARRAY

#include <vector>
#include <cmath>

/**
 * @brief colorVector is the type to store one RGB or HSV value
 */
typedef std::vector<double> colorVector;
/**
 * @brief colorData2d is a 2D matrix type of colorVector values
 */
typedef std::vector< std::vector< colorVector > > colorData2d;


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
	ColorMatrix& operator=(const ColorMatrix&) & = default; // Copy assignment operator
	ColorMatrix& operator=(ColorMatrix&&) & = default;      // Move assignment operator
	~ColorMatrix() = default;								// Destructor

	/**
	 * @brief rescaleTo changes the size of the matrix
	 * @param sx new width
	 * @param sy new height
	 */
	void rescaleTo(std::size_t sx, std::size_t sy);

	/**
	 * @brief getSX returns the width of the matrix
	 * @return width
	 */
	std::size_t getSX() const {
		// FIXME: hsv could be invalid
        return hsvData.size();
    }

	/**
	 * @brief getSY returns the height of the matrix
	 * @return height
	 */
    std::size_t getSY() const {
		// FIXME: hsv could be invalid
        return hsvData[0].size();
    }

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
    void setHsv(const colorData2d& newHsv) {
        hsvData = newHsv;
        hsvValid = true;
        rgbValid = false;
        valueValid = false;
    }

	/**
	 * @brief setHsvAt sets the value of a single element by HSV values
	 * @param x index
	 * @param y index
	 * @param h hue value [0-1]
	 * @param s saturation value [0-1]
	 * @param v brightness value [0-1]
	 */
	void setHsvAt(std::size_t x, std::size_t y, double h, double s, double v);

	/**
	 * @brief getHsv returns the data of this matrix as HSV values
	 * (may be a conversion of RGB values)
	 * @return array of HSV values
	 */
	colorData2d getHsv() const {
        if (!hsvValid) updateHsv();
        return hsvData;
    }

	/**
	 * @brief getHsvAt returns the data of a single element as HSV values
	 * @param x index
	 * @param y index
	 * @return HSV values
	 */
	colorVector getHsvAt(std::size_t x, std::size_t y) const {
        if (!hsvValid) updateHsv();
        return hsvData[x][y];
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
	void setRgb(const colorData2d& newRgb);

	/**
	 * @brief setRgbAt sets the value of a single element by HSV values
	 * @param x index
	 * @param y index
	 * @param r red value [0-1]
	 * @param g green value [0-1]
	 * @param b blue value [0-1]
	 */
	void setRgbAt(std::size_t x, std::size_t y, double r, double g, double b);

	/**
	 * @brief getRgb returns the data of this matrix as RGB values
	 * (may be a conversion of HSV values)
	 * @return array of HSV values
	 */
	colorData2d getRgb() const {
        if (!rgbValid) updateRgb();
        return rgbData;
    }

	/**
	 * @brief getRgbAt returns the data of a single element as HSV values
	 * @param x index
	 * @param y index
	 * @return RGB values
	 */
	colorVector getRgbAt(std::size_t x, std::size_t y) const {
        if (!rgbValid) updateRgb();
        return rgbData[x][y];
	}
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

	/**
	 * @brief hsvData stores the data as HSV values (this is not always up to date)
	 */
	mutable colorData2d hsvData;
	/**
	 * @brief hsvValid is true, if the HSV values are up to date
	 */
	mutable bool hsvValid;
	/**
	 * @brief rgbData stores the data as RGB values (this is not always up to date)
	 */
	mutable colorData2d rgbData;
	/**
	 * @brief rgbValid is true, if the RGB values are up to date
	 */
	mutable bool rgbValid;
	/**
	 * @brief value stores the first value of the data (because it is very often used)
	 * (this is not always up to date)
	 */
	mutable double value;
	/**
	 * @brief valueValid is true, if "value" is up to date
	 */
	mutable bool valueValid;
};



#endif // HSV_ARRAY

