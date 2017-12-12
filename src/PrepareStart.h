#ifndef PREPARESTART_H
#define PREPARESTART_H

#include "BezierCurve.h"
#include "MainController.h"
#include "KineticEffect.h"
#include "KineticEffect2D.h"
#include "StretchLayouts.h"
#include "TouchArea.h"
#include "NodeConnectionLines.h"
#include "SpectrumItem.h"
#include "AudioSpectrumItem.h"
#include "SpectralHistoryItem.h"

#include <QtGui>
#include <QApplication>
#include <QtQuick>
#include <QSysInfo>
#include <QFontDatabase>

// amount and complexity of graphical effects (i.e. blur and shadows):
enum TGraphicalEffectsLevel { MIN_EFFECTS = 1, MID_EFFECTS = 2, MAX_EFFECTS = 3 };
#ifdef Q_OS_WIN
static const TGraphicalEffectsLevel GRAPHICAL_EFFECTS_LEVEL = MID_EFFECTS;
#else
static const TGraphicalEffectsLevel GRAPHICAL_EFFECTS_LEVEL = MAX_EFFECTS;
#endif


// checks if additional scaling is needed (i.e. for HiDPI displays)
// and sets the dp QML context property accordingly:
inline void setDpProperty(QQmlApplicationEngine& engine) {
    float scaleFactor = 1.0;

    // get scale factor depending on platform:
    if (QSysInfo::productType() == "android" || QSysInfo::productType() == "ios") {
        float normDPI = 160;
        scaleFactor = (int(QGuiApplication::primaryScreen()->physicalDotsPerInch()) / QGuiApplication::primaryScreen()->devicePixelRatio()) / normDPI;
    } else if (QSysInfo::productType() == "osx") {
        float normDPI = 72;
        scaleFactor = (int(QGuiApplication::primaryScreen()->logicalDotsPerInch()) / QGuiApplication::primaryScreen()->devicePixelRatio()) / normDPI;
    } else {
        float normDPI = 96;
        scaleFactor = (int(QGuiApplication::primaryScreen()->logicalDotsPerInch()) / QGuiApplication::primaryScreen()->devicePixelRatio()) / normDPI;
    }

    // don't shrink content:
    if (scaleFactor < 1) {
        scaleFactor = 1;
    }
    // round scale factor to one decimal position:
    scaleFactor = int(scaleFactor * 10 + 0.5) / 10.0;
    // set QML context property "dp" that can be accessed anywhere from QML:
    engine.rootContext()->setContextProperty("dp", scaleFactor);

    qInfo() << "Device Pixel Ratio (provided by Qt): " << QGuiApplication::primaryScreen()->devicePixelRatio();
    qInfo() << "Custom GUI scale factor (dp unit): " << scaleFactor;
}

inline void registerFonts() {
    QFontDatabase::addApplicationFont(":/fonts/Quicksand-Regular.otf");
    QFontDatabase::addApplicationFont(":/fonts/Quicksand-Italic.otf");
    QFontDatabase::addApplicationFont(":/fonts/Quicksand-Light.otf");
    QFontDatabase::addApplicationFont(":/fonts/Quicksand-LightItalic.otf");
    QFontDatabase::addApplicationFont(":/fonts/Quicksand-Bold.otf");
    QFontDatabase::addApplicationFont(":/fonts/Quicksand-BoldItalic.otf");
    // QFontDatabase::addApplicationFont(":/fonts/Quicksand_Dash.otf");
    QFontDatabase::addApplicationFont(":/fonts/breeze-icons.ttf");
    QFontDatabase::addApplicationFont(":/fonts/BPmono.ttf");
    QFontDatabase::addApplicationFont(":/fonts/BPmonoBold.ttf");
    QFontDatabase::addApplicationFont(":/fonts/BPmonoItalic.ttf");
}

inline void registerCustomQmlTypes() {
    qmlRegisterType<BezierCurve>("CustomGeometry", 1, 0, "BezierCurve");
    qmlRegisterType<KineticEffect>("CustomElements", 1, 0, "KineticEffect");
    qmlRegisterType<KineticEffect2D>("CustomElements", 1, 0, "KineticEffect2D");
    qmlRegisterType<StretchColumn>("CustomElements", 1, 0, "StretchColumn");
    qmlRegisterType<StretchRow>("CustomElements", 1, 0, "StretchRow");
    qmlRegisterType<NodeConnectionLines>("CustomElements", 1, 0, "NodeConnectionLines");
    qmlRegisterType<SpectrumItem>("CustomElements", 1, 0, "SpectrumItem");
    qmlRegisterType<AudioSpectrumItem>("CustomElements", 1, 0, "AudioSpectrumItem");
    qmlRegisterType<SpectralHistoryItem>("CustomElements", 1, 0, "SpectralHistoryItem");
    qRegisterMetaType<TouchAreaEvent>();
    qmlRegisterType<TouchAreaEvent>();
    qmlRegisterType<TouchArea>("CustomElements", 1, 0, "CustomTouchArea");
}

inline void prepareQApplicationAttributes() {
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents, false);
    QApplication::setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents, false);
}

inline void prepareQApplication(QApplication& app) {
    app.setWindowIcon(QIcon(":/images/icon/app_icon_512.png"));
}

inline void prepareQmlEngine(QQmlApplicationEngine& engine) {
    engine.addImportPath("qrc:/");
    setDpProperty(engine);
    engine.rootContext()->setContextProperty("GRAPHICAL_EFFECTS_LEVEL", GRAPHICAL_EFFECTS_LEVEL);
}

#endif // PREPARESTART_H
