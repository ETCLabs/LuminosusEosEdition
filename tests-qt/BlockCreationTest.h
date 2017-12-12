#ifndef BLOCKCREATIONTEST_H
#define BLOCKCREATIONTEST_H

#include <QtTest>

#include "../src/PrepareStart.h"
#include "../src/block_implementations/SliderBlock.h"


class BlockCreationTest : public QObject {

    Q_OBJECT

private slots:
    // functions executed by QtTest before and after test suite
    void initTestCase() {
        // prepare Qt application:
        // (using QApplication instead of smaller QGuiApplication to support QWidget based
        // FileDialogs on Linux)
        prepareQApplicationAttributes();
        char* argv[1];
        int argc = 0;
        m_app = new QApplication(argc, argv);
        prepareQApplication(*m_app);

        registerFonts();

        // prepare QML engine:
        registerCustomQmlTypes();
        m_engine = new QQmlApplicationEngine();
        prepareQmlEngine(*m_engine);

        // MainController will take care of initalizing GUI, output etc.:
        m_controller = new MainController(*m_engine);
        QObject::connect(m_app, SIGNAL(aboutToQuit()), m_controller, SLOT(onExit()));
        QObject::connect(m_engine, SIGNAL(quit()), m_app, SLOT(quit()));  // to make Qt.quit() to work
    }

    void cleanupTestCase() {
//        delete m_controller;
//        delete m_engine;
//        delete m_app;
    }

    // functions executed by QtTest before and after each test
    void init() {}
    void cleanup() {}

    // test functions - all functions prefixed with "test" will be ran as tests

    void testBlockCreation_data() {
        QTest::addColumn<QString>("blockTypeName");

        // the test data for testBlockCreation() are all available block names:
        for (QString blockType: m_controller->blockManager()->blockList()->getAllBlockTypes()) {
            QTest::newRow(blockType.toLatin1()) << blockType;
        }
    }

    void testBlockCreation() {
        QFETCH(QString, blockTypeName);

        BlockInterface* block = m_controller->blockManager()->addNewBlock(blockTypeName);

        QTEST_ASSERT(block != nullptr);
        QCOMPARE(block->getBlockInfo().typeName, blockTypeName);
    }

    void testBlockCreationNotExisting() {
        QString blockTypeName = "fooBar";
        BlockInterface* block = m_controller->blockManager()->addNewBlock(blockTypeName);

        QCOMPARE(block, nullptr);
    }

protected:
    QApplication* m_app = nullptr;
    QQmlApplicationEngine* m_engine = nullptr;
    MainController* m_controller;

};

#endif // BLOCKCREATIONTEST_H
