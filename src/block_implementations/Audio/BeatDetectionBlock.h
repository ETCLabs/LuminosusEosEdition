#ifndef BEATDETECTIONBLOCK_H
#define BEATDETECTIONBLOCK_H

#include "core/block_data/OneOutputBlock.h"
#include "audio/AudioInputAnalyzer.h"
#include "utils.h"

#include <QPointer>


class BeatDetectionBlock : public OneOutputBlock {

    Q_OBJECT

    Q_PROPERTY(AudioInputAnalyzer* analyzer READ getAnalyzer NOTIFY inputChanged)
    Q_PROPERTY(double bpm READ getBpm NOTIFY bpmChanged)
    Q_PROPERTY(bool bpmIsValid READ getBpmIsValid NOTIFY bpmChanged)
    Q_PROPERTY(int minBpm READ getMinBpm WRITE setMinBpm NOTIFY minBpmChanged)
    Q_PROPERTY(double factor READ getFactor WRITE setFactor NOTIFY factorChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.nameInUi = "BPM Detection";
        info.typeName = "Beat Detection";
        info.category << "Sound2Light";
        info.availabilityRequirements = {AvailabilityRequirement::AudioInput};
        info.helpText = "Tries to detect the tempo of the music in BPM.\n\n"
                        "The output can be connected to any block accepting a time input, "
                        "i.e. Generator Blocks or an Eos Effect Speed Block.\n"
                        "Can also be used in connection with an OSC Out Block and "
                        "the string '<bpm>' in its path.\n\n"
                        "The input device can be chosen above.";
        info.qmlFile = "qrc:/qml/Blocks/Audio/BeatDetectionBlock.qml";
        info.complete<BeatDetectionBlock>();
        return info;
    }

    explicit BeatDetectionBlock(MainController* m_controller, QString m_uid);
    ~BeatDetectionBlock();

    virtual void getAdditionalState(QJsonObject& state) const override;

    virtual void setAdditionalState(const QJsonObject& state) override;

signals:
    void inputChanged();
    void bpmChanged();
    void minBpmChanged();
    void factorChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void updateOutput();

    void onBpmChanged();

    QString getInputName() const;
    void setInputByName(QString name);

    AudioInputAnalyzer* getAnalyzer() const { return m_analyzer; }

    double getBpm() const { return m_analyzer ? m_analyzer->getBpm(m_minBpm) : 0; }
    bool getBpmIsValid() const { return m_analyzer ? m_analyzer->getBpmIsValid() : false; }

    int getMinBpm() const { return m_minBpm; }
    void setMinBpm(int value) { m_minBpm = value; emit minBpmChanged(); }

    double getFactor() const { return m_factor; }
    void setFactor(double value);

protected:
    QPointer<AudioInputAnalyzer> m_analyzer;
    int m_minBpm;
    double m_factor;

};

#endif // BEATDETECTIONBLOCK_H
