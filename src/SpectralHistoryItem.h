#ifndef SPECTRALHISTORYITEM_H
#define SPECTRALHISTORYITEM_H

#include "AudioInputAnalyzer.h"

#include <QtQuick/QQuickItem>
#include <QVector2D>
#include <QPointer>


class SpectralHistoryItem : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(float lineWidth READ lineWidth WRITE setLineWidth NOTIFY lineWidthChanged)
    Q_PROPERTY(AudioInputAnalyzer* analyzer READ analyzer WRITE setAnalyzer NOTIFY analyzerChanged)

public:
    SpectralHistoryItem(QQuickItem* parent = 0);
    ~SpectralHistoryItem();

    QSGNode* updatePaintNode(QSGNode*, UpdatePaintNodeData*) override;

signals:
    void colorChanged(const QColor& color);
    void lineWidthChanged(float width);
    void analyzerChanged();

public slots:
    QColor color() const { return m_color; }
    float lineWidth() const { return m_lineWidth; }
    AudioInputAnalyzer* analyzer() const { return m_analyzer; }

    void setColor(const QColor& color);
    void setLineWidth(float width);
    void setAnalyzer(AudioInputAnalyzer* value);

private:
    void updatePoints();

protected:
    QColor m_color;
    float m_lineWidth;
    QPointer<AudioInputAnalyzer> m_analyzer;

    QVector<double> m_points;

    const float m_device_pixel_ratio;
};

#endif // SPECTRALHISTORYITEM_H
