#ifndef AUDIOBARSPECTRUMITEM_H
#define AUDIOBARSPECTRUMITEM_H

#include "audio/AudioInputAnalyzer.h"

#include <QtQuick/QQuickItem>
#include <QVector2D>
#include <QPointer>


class AudioBarSpectrumItem : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(float lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged)
    Q_PROPERTY(AudioInputAnalyzer* analyzer READ analyzer WRITE setAnalyzer NOTIFY analyzerChanged)
    Q_PROPERTY(bool agcEnabled READ getAgcEnabled WRITE setAgcEnabled NOTIFY agcEnabledChanged)
    Q_PROPERTY(double manualGain READ getManualGain WRITE setManualGain NOTIFY manualGainChanged)

public:
    explicit AudioBarSpectrumItem(QQuickItem* parent = 0);
    ~AudioBarSpectrumItem();

    QSGNode *updatePaintNode(QSGNode*, UpdatePaintNodeData*);

signals:
    void colorChanged(const QColor& color);
    void lineWidthChanged(float width);
    void analyzerChanged();
    void agcEnabledChanged();
    void manualGainChanged();

public slots:
    QColor color() const { return m_color; }
    float lineWidth() const { return m_lineWidth; }
    AudioInputAnalyzer* analyzer() const { return m_analyzer; }

    void setColor(const QColor& color);
    void setLineWidth(float width);
    void setAnalyzer(AudioInputAnalyzer* value);

    bool getAgcEnabled() const { return m_agcEnabled; }
    void setAgcEnabled(bool value) { m_agcEnabled = value; emit agcEnabledChanged(); }

    double getManualGain() const { return m_manualGain; }
    void setManualGain(double value) { m_manualGain = limit(0.2, value, 8); emit manualGainChanged(); }

private:
    static QPointF normalFromTangent(const QPointF& tangent);

    QColor      m_color;
    float       m_lineWidth;
    QPointer<AudioInputAnalyzer> m_analyzer;
    bool m_agcEnabled;
    double m_manualGain;

    const float m_device_pixel_ratio;
};

#endif // AUDIOBARSPECTRUMITEM_H
