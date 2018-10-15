#include "GuiEditorBlock.h"

#include "core/MainController.h"


GuiEditorBlock::GuiEditorBlock(MainController* controller, QString uid)
    : BlockBase(controller, uid)
    , m_code(this, "code", "")
    , m_saved(this, "saved", true)
{
    connect(m_controller->blockManager(), SIGNAL(focusChanged()), this, SLOT(onFocusChanged()));
    connect(&m_code, SIGNAL(valueChanged()), this, SLOT(onCodeChanged()));
}

void GuiEditorBlock::apply() {
    BlockInterface* focusedBlock = m_controller->blockManager()->getFocusedBlock();
    if (!focusedBlock) {
        return;
    }
    focusedBlock->setGuiItemCode(m_code);
    m_saved = true;
}

void GuiEditorBlock::onFocusChanged() {
    BlockInterface* focusedBlock = m_controller->blockManager()->getFocusedBlock();
    if (!focusedBlock) {
        m_code = "No focused block";
        return;
    }

    QString qmlPath = focusedBlock->getBlockInfo().qmlFile;
    qmlPath.remove("qrc");

    QFile loadFile(qmlPath);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open file " + qmlPath + ".";
        return;
    }
    QByteArray content = loadFile.readAll();
    loadFile.close();

    m_code = QString::fromLatin1(content);
    m_saved = true;
}

void GuiEditorBlock::onCodeChanged() {
    m_saved = false;
}
