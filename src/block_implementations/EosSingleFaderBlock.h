#ifndef EOSSINGLEFADERBLOCK_H
#define EOSSINGLEFADERBLOCK_H

#include "block_data/OneInputBlock.h"
#include "eos_specific/EosOSCMessage.h"
#include "utils.h"


class EosSingleFaderBlock: public OneInputBlock
{
    Q_OBJECT

    Q_PROPERTY(int page READ getPage WRITE setPageFromGui NOTIFY pageChanged)
    Q_PROPERTY(int faderNumber READ getFaderNumber WRITE setFaderNumber NOTIFY faderNumberChanged)
    Q_PROPERTY(QString bankLabel READ getBankLabel NOTIFY bankLabelChanged)
    Q_PROPERTY(QString faderLabel READ getFaderLabel NOTIFY faderLabelChanged)
    Q_PROPERTY(double faderLevel READ getFaderLevel NOTIFY faderLevelChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Eos Single Fader";
        info.nameInUi = "Single Fader";
        info.category << "Eos";
        info.helpText = "Controls a single fader of the connected console.";
        info.qmlFile = "qrc:/qml/Blocks/EosSingleFaderBlock.qml";
        info.complete<EosSingleFaderBlock>();
        return info;
    }

    explicit EosSingleFaderBlock(MainController* controller, QString uid);

    virtual QJsonObject getState() const override;
    virtual void setState(const QJsonObject& state) override;

signals:
    void pageChanged();
    void faderNumberChanged();
    void bankLabelChanged();
    void faderLabelChanged();
    void faderLevelChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    void onValueChanged();
    void onStopValueChanged();
    void onFireValueChanged();

    int getPage() const { return m_page; }
    void setPageFromGui(int value);

    int getFaderNumber() const { return m_faderNumber; }
    void setFaderNumber(int value);

    QString getBankLabel() const { return m_bankLabel; }
    void setBankLabel(QString value) { m_bankLabel = value; emit bankLabelChanged(); }

    QString getFaderLabel() const { return m_faderLabels[m_faderNumber - 1]; }
    void setFaderLabelFromOsc(int faderIndex, QString label);

    double getFaderLevel() const { return m_faderLevels[m_faderNumber - 1]; }
    void setFaderLevelFromGui(qreal value);
    void setFaderLevelFromOsc(int faderIndex, qreal value);

    void sendLoadEvent(bool value);
    void sendUnloadEvent();
    void sendStopEvent(bool value);
    void sendFireEvent(bool value);

protected slots:
    void onEosConnectionEstablished();
    void sendConfigMessage();
    void onIncomingEosMessage(const EosOSCMessage& msg);
    void onConnectionReset();

private:
    int getMaxFaderPage();

protected:
    const QString m_bankIndex;

    int m_page;
    int m_faderNumber;
    QString m_bankLabel;

    QVector<QString> m_faderLabels;
    QVector<qreal> m_faderLevels;

    QPointer<NodeBase> m_stopNode;
    QPointer<NodeBase> m_fireNode;

    double m_lastStopNodeValue;
    double m_lastFireNodeValue;
};

#endif // EOSSINGLEFADERBLOCK_H
