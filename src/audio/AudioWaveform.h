#ifndef AUDIOWAVEFORM_H
#define AUDIOWAVEFORM_H

#include <QObject>
#include <QVector>

class MainController;  // forward declaration
class QWaveDecoder;


class AudioWaveform : public QObject
{
    Q_OBJECT

public:
    explicit AudioWaveform();

signals:
    void pointsChanged();
    void availableChanged();
    void contentLoaded();

public slots:
    void analyze(QString filename);
    void loadContent(QString filename);

    const QVector<double>& getPoints() const { return m_points; }

    bool isAvailable() const { return m_available; }

    const QByteArray& content() const { return m_rawData; }

private slots:
    void analyzeBuffer();
    void storeBuffer();

protected:
    QWaveDecoder* m_decoder;

    QVector<double> m_points;

    bool m_available;

    QByteArray m_rawData;
};

#endif // AUDIOWAVEFORM_H
