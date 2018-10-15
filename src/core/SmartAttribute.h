#ifndef BLOCKATTRIBUTE_H
#define BLOCKATTRIBUTE_H

#include "core/Matrix.h"

#include <QObject>
#include <QJsonObject>
#include <QColor>

// forward declare:
class BlockInterface;


class SmartAttribute : public QObject
{
    Q_OBJECT

public:
    explicit SmartAttribute(BlockInterface* block, QString name, bool persistent);
    explicit SmartAttribute(QObject* parent, QString name, bool persistent);

public slots:
    virtual void writeTo(QJsonObject& state) const = 0;
    virtual void readFrom(const QJsonObject& state) = 0;

    QString name() const { return m_name; }
    bool persistent() const { return m_persistent; }
    QObject* block() const { return parent(); }

protected:
    QString m_name;
    bool m_persistent;
};

class DoubleAttribute : public SmartAttribute
{
    Q_OBJECT

    Q_PROPERTY(double val READ getValue WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(double min READ getMin WRITE setMin NOTIFY minChanged)
    Q_PROPERTY(double max READ getMax WRITE setMax NOTIFY maxChanged)

public:
    explicit DoubleAttribute(BlockInterface* block, QString name, double initialValue = 0.0, double min = 0.0, double max = 1.0, bool persistent = true);
    explicit DoubleAttribute(QObject* parent, QString name, double initialValue = 0.0, double min = 0.0, double max = 1.0, bool persistent = true);
    operator double() const { return m_value; }
    DoubleAttribute& operator=(double value) { setValue(value); return *this; }

signals:
    void valueChanged();
    void minChanged();
    void maxChanged();

public slots:
    virtual void writeTo(QJsonObject& state) const override;
    virtual void readFrom(const QJsonObject& state) override;

    double getValue() const { return m_value; }
    void setValue(double value);

    double getMin() const { return m_min; }
    void setMin(double value) { m_min = value; emit minChanged(); }

    double getMax() const { return m_max; }
    void setMax(double value) { m_max = value; emit maxChanged(); }

protected:
    double m_value;
    double m_min;
    double m_max;
};


class IntegerAttribute : public SmartAttribute
{
    Q_OBJECT

    Q_PROPERTY(int val READ getValue WRITE setValue NOTIFY valueChanged)
    Q_PROPERTY(int min READ getMin WRITE setMin NOTIFY minChanged)
    Q_PROPERTY(int max READ getMax WRITE setMax NOTIFY maxChanged)

public:
    explicit IntegerAttribute(BlockInterface* block, QString name, int initialValue = 0, int min = 0, int max = 100, bool persistent = true);
    explicit IntegerAttribute(QObject* parent, QString name, int initialValue = 0, int min = 0, int max = 100, bool persistent = true);
    operator int() const { return m_value; }
    IntegerAttribute& operator=(int value) { setValue(value); return *this; }

signals:
    void valueChanged();
    void minChanged();
    void maxChanged();

public slots:
    virtual void writeTo(QJsonObject& state) const override;
    virtual void readFrom(const QJsonObject& state) override;

    int getValue() const { return m_value; }
    void setValue(int value);

    int getMin() const { return m_min; }
    void setMin(int value) { m_min = value; emit minChanged(); }

    int getMax() const { return m_max; }
    void setMax(int value) { m_max = value; emit maxChanged(); }

protected:
    int m_value;
    int m_min;
    int m_max;
};


class StringAttribute : public SmartAttribute
{
    Q_OBJECT

    Q_PROPERTY(QString val READ getValue WRITE setValue NOTIFY valueChanged)

public:
    explicit StringAttribute(BlockInterface* block, QString name, QString initialValue = "", bool persistent = true);
    explicit StringAttribute(QObject* parent, QString name, QString initialValue = "", bool persistent = true);
    operator QString() const { return m_value; }
    StringAttribute& operator=(QString value) { setValue(value); return *this; }

signals:
    void valueChanged();

public slots:
    virtual void writeTo(QJsonObject& state) const override;
    virtual void readFrom(const QJsonObject& state) override;

    QString getValue() const { return m_value; }
    void setValue(QString value) { m_value = value; emit valueChanged(); }  // TODO: check if equal

protected:
    QString m_value;
};


class BoolAttribute : public SmartAttribute
{
    Q_OBJECT

    Q_PROPERTY(bool val READ getValue WRITE setValue NOTIFY valueChanged)

public:
    explicit BoolAttribute(BlockInterface* block, QString name, bool initialValue = false, bool persistent = true);
    explicit BoolAttribute(QObject* parent, QString name, bool initialValue = false, bool persistent = true);
    operator bool() const { return m_value; }
    BoolAttribute& operator=(bool value) { setValue(value); return *this; }

signals:
    void valueChanged();

public slots:
    virtual void writeTo(QJsonObject& state) const override;
    virtual void readFrom(const QJsonObject& state) override;

    bool getValue() const { return m_value; }
    void setValue(bool value) { m_value = value; emit valueChanged(); }  // TODO: check if equal

protected:
    bool m_value;
};


// RGB is a windows macro, undefine it first:
#ifdef RGB
#undef RGB
#endif

class RgbAttribute : public SmartAttribute
{
    Q_OBJECT

    Q_PROPERTY(double red READ red WRITE setRed NOTIFY valueChanged)
    Q_PROPERTY(double green READ green WRITE setGreen NOTIFY valueChanged)
    Q_PROPERTY(double blue READ blue WRITE setBlue NOTIFY valueChanged)
    Q_PROPERTY(double hue READ hue WRITE setHue NOTIFY valueChanged)
    Q_PROPERTY(double sat READ sat WRITE setSat NOTIFY valueChanged)
    Q_PROPERTY(double val READ val WRITE setVal NOTIFY valueChanged)
    Q_PROPERTY(QColor qcolor READ getQColor WRITE setQColor NOTIFY valueChanged)
    Q_PROPERTY(double max READ max NOTIFY valueChanged)
    Q_PROPERTY(QColor glow READ getGlow NOTIFY valueChanged)

public:
    explicit RgbAttribute(BlockInterface* block, QString name, const RGB& initialValue = {0, 0, 0}, bool persistent = true);
    explicit RgbAttribute(QObject* parent, QString name, const RGB& initialValue = {0, 0, 0}, bool persistent = true);
    operator RGB() const { return m_value; }
    RgbAttribute& operator=(const RGB& value) { setValue(value); return *this; }

signals:
    void valueChanged();

public slots:
    virtual void writeTo(QJsonObject& state) const override;
    virtual void readFrom(const QJsonObject& state) override;

    const RGB& getValue() const { return m_value; }
    void setValue(const RGB& value) { m_value = value; emit valueChanged(); }  // TODO: check if equal

    double red() const { return m_value.r; }
    void setRed(double value) { m_value.r = value; emit valueChanged(); }
    double green() const { return m_value.g; }
    void setGreen(double value) { m_value.g = value; emit valueChanged(); }
    double blue() const { return m_value.b; }
    void setBlue(double value) { m_value.b = value; emit valueChanged(); }

    double hue() const;
    void setHue(double value);
    double sat() const;
    void setSat(double value);
    double val() const { return m_value.max(); }
    void setVal(double value);

    QColor getQColor() const { return QColor::fromRgbF(m_value.r, m_value.g, m_value.b); }
    void setQColor(QColor value);

    QColor getGlow() const;

    double max() const { return m_value.max(); }

    void mixHtp(const RGB& other) { m_value.mixHtp(other); emit valueChanged(); }

protected:
    RGB m_value;
    HSV m_tempHsv;
};


class HsvAttribute : public SmartAttribute
{
    Q_OBJECT

    Q_PROPERTY(QColor qcolor READ getQColor WRITE setQColor NOTIFY valueChanged)
    Q_PROPERTY(double hue READ hue WRITE setHue NOTIFY valueChanged)
    Q_PROPERTY(double sat READ sat WRITE setSat NOTIFY valueChanged)
    Q_PROPERTY(double val READ val WRITE setVal NOTIFY valueChanged)

public:
    explicit HsvAttribute(BlockInterface* block, QString name, const HSV& initialValue = {0, 0, 0}, bool persistent = true);
    explicit HsvAttribute(QObject* parent, QString name, const HSV& initialValue = {0, 0, 0}, bool persistent = true);
    operator HSV() const { return m_value; }
    HsvAttribute& operator=(const HSV& value) { setValue(value); return *this; }

signals:
    void valueChanged();

public slots:
    virtual void writeTo(QJsonObject& state) const override;
    virtual void readFrom(const QJsonObject& state) override;

    const HSV& getValue() const { return m_value; }
    void setValue(const HSV& value) { m_value = value; emit valueChanged(); }  // TODO: check if equal

    double hue() const { return m_value.h; }
    void setHue(double value) { m_value.h = value; emit valueChanged(); }
    double sat() const { return m_value.s; }
    void setSat(double value) { m_value.s = value; emit valueChanged(); }
    double val() const { return m_value.v; }
    void setVal(double value) { m_value.v = value; emit valueChanged(); }

    QColor getQColor() const { return QColor::fromHsvF(m_value.h, m_value.s, m_value.v); }
    void setQColor(QColor value);

    void mixHtp(const HSV& other);

protected:
    HSV m_value;
};

#endif // BLOCKATTRIBUTE_H
