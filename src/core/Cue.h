#ifndef CUE_H
#define CUE_H

#include <QObject>
#include <QPointer>

#include "core/block_data/BlockInterface.h"
#include "block_implementations/Theater/PresetBlock.h"


class Cue : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QObject* sceneBlock READ sceneBlock NOTIFY sceneBlockChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(double fadeIn READ fadeIn WRITE setFadeIn NOTIFY fadeInChanged)
    Q_PROPERTY(bool follow READ follow WRITE setFollow NOTIFY followChanged)
    Q_PROPERTY(double hold READ hold WRITE setHold NOTIFY holdChanged)

public:
    explicit Cue(MainController* controller, BlockInterface* block, PresetBlock* sceneBlock);

    QJsonObject getState() const;
    void setState(const QJsonObject& state);

signals:
    void sceneBlockChanged();
    void descriptionChanged();
    void fadeInChanged();
    void followChanged();
    void holdChanged();

public slots:

    PresetBlock* sceneBlock() const;
    void setSceneBlock(PresetBlock* sceneBlock);

    QString description() const { return m_description; }
    void setDescription(QString value) { m_description = value; emit descriptionChanged(); }

    double fadeIn() const { return m_fadeIn; }
    void setFadeIn(double value) { m_fadeIn = value; emit fadeInChanged(); }

    bool follow() const { return m_follow; }
    void setFollow(bool value) { m_follow = value; emit followChanged(); }

    double hold() const { return m_hold; }
    void setHold(double value) { m_hold = value; emit holdChanged(); }


protected:
    MainController* const m_controller;
    BlockInterface* const m_block;

    QPointer<PresetBlock> m_sceneBlock;

    QString m_description;
    double m_fadeIn;
    bool m_follow;
    double m_hold;
};

#endif // CUE_H
