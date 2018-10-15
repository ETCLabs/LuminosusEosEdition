#ifndef AUDIOVOLUMEBLOCK_H
#define AUDIOVOLUMEBLOCK_H

#include "core/block_data/OneOutputBlock.h"
#include "audio/AudioInputAnalyzer.h"
#include "utils.h"

#include <QPointer>


class AudioVolumeBlock : public OneOutputBlock {

    Q_OBJECT

    Q_PROPERTY(AudioInputAnalyzer* analyzer READ getAnalyzer NOTIFY inputChanged)
    Q_PROPERTY(double volume READ getVolume NOTIFY volumeChanged)
    Q_PROPERTY(double spectralFlux READ getSpectralFlux NOTIFY volumeChanged)
    Q_PROPERTY(bool agcEnabled READ getAgcEnabled WRITE setAgcEnabled NOTIFY agcEnabledChanged)
    Q_PROPERTY(double volumeGain READ getVolumeGain NOTIFY volumeChanged)
    Q_PROPERTY(double fluxGain READ getFluxGain NOTIFY volumeChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Audio Volume";
        info.nameInUi = "Volume";
        info.category << "Sound2Light";
        info.availabilityRequirements = {AvailabilityRequirement::AudioInput};
        info.helpText = "Outputs the volume and spectral flux of an audio input.\n"
                        "The spectral flux is the sum of all increases in energy of frequencies.\n"
                        "The input device can be chosen above.";
        info.qmlFile = "qrc:/qml/Blocks/Audio/AudioVolumeBlock.qml";
        info.complete<AudioVolumeBlock>();
        return info;
    }

    explicit AudioVolumeBlock(MainController* m_controller, QString m_uid);
    ~AudioVolumeBlock();

    virtual void getAdditionalState(QJsonObject& state) const override;

    virtual void setAdditionalState(const QJsonObject& state) override;

signals:
    void inputChanged();
    void volumeChanged();
    void agcEnabledChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void updateOutput();

    QString getInputName() const;
    void setInputByName(QString name);

    AudioInputAnalyzer* getAnalyzer() const { return m_analyzer; }

    double getVolume() const { return m_analyzer ? m_analyzer->getMaxLevel() : 0.0; }
    double getSpectralFlux() const { return m_analyzer ? m_analyzer->getCurrentSpectralFlux() : 0.0; }

    bool getAgcEnabled() const { return m_agcEnabled; }
    void setAgcEnabled(bool value) { m_agcEnabled = value; emit agcEnabledChanged(); }

    double getVolumeGain() const { return (m_agcEnabled && m_analyzer) ? m_analyzer->getAgcValue() : 1.0; }
    double getFluxGain() const { return (m_agcEnabled && m_analyzer) ? m_analyzer->getSpectralFluxAgcValue() : 1.0; }

protected:
    QPointer<AudioInputAnalyzer> m_analyzer;
    QPointer<NodeBase> m_fluxNode;
    bool m_agcEnabled;

};

#endif // AUDIOVOLUMEBLOCK_H
