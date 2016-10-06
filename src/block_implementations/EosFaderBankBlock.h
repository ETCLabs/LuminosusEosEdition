#ifndef EOSFADERBANKBLOCK_H
#define EOSFADERBANKBLOCK_H

#include "block_data/BlockBase.h"
#include "eos_specific/EosOSCMessage.h"
#include "utils.h"


class EosFaderBankBlock: public BlockBase
{
    Q_OBJECT

    Q_PROPERTY(int page READ getPage WRITE setPageFromGui NOTIFY pageChanged)
    Q_PROPERTY(QString bankLabel READ getBankLabel NOTIFY bankLabelChanged)
    Q_PROPERTY(QList<QString> faderLabels READ getFaderLabels NOTIFY faderLabelsChanged)
    Q_PROPERTY(QList<qreal> faderLevels READ getFaderLevels NOTIFY faderLevelsChanged)

public:

    static BlockInfo info() {
        static BlockInfo info;
        info.typeName = "Eos Fader Bank";
        info.nameInUi = "Fader Bank";
        info.category << "Eos";
        info.helpText = "A bank of 10 faders.";
        info.qmlFile = "qrc:/qml/Blocks/EosFaderBankBlock.qml";
        info.complete<EosFaderBankBlock>();
        return info;
    }

    explicit EosFaderBankBlock(MainController* controller, QString uid);

    virtual QJsonObject getState() const override;
    virtual void setState(const QJsonObject& state) override;

signals:
    void pageChanged();
    void bankLabelChanged();
    void faderLabelsChanged();
    void faderLevelsChanged();

public slots:
    virtual BlockInfo getBlockInfo() const override { return info(); }

    int getPage() const { return m_page; }
    void setPageFromGui(int value);

    QString getBankLabel() const { return m_bankLabel; }
    void setBankLabel(QString value) { m_bankLabel = value; emit bankLabelChanged(); }

    QList<QString> getFaderLabels() const { return m_faderLabels.toList(); }
    void setFaderLabelFromOsc(int faderIndex, QString label);

    QList<qreal> getFaderLevels() const { return m_faderLevels.toList(); }
    void setFaderLevelFromGui(int faderIndex, qreal value);
    void setFaderLevelFromOsc(int faderIndex, qreal value);

    void sendLoadEvent(int faderIndex, bool value);
    void sendUnloadEvent(int faderIndex);
    void sendStopEvent(int faderIndex, bool value);
    void sendFireEvent(int faderIndex, bool value);

    void sendPageMinusEvent();
    void sendPagePlusEvent();

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
    QString m_bankLabel;

    QVector<QString> m_faderLabels;
    QVector<qreal> m_faderLevels;
};

#endif // EOSFADERBANKBLOCK_H
