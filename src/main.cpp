// Copyright (c) 2016 Electronic Theatre Controls, Inc., http://www.etcconnect.com
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.


#include "core/MainController.h"
#include "qtquick_items/BezierCurve.h"
#include "qtquick_items/KineticEffect.h"
#include "qtquick_items/KineticEffect2D.h"
#include "qtquick_items/StretchLayouts.h"
#include "qtquick_items/TouchArea.h"
#include "qtquick_items/NodeConnectionLines.h"
#include "qtquick_items/SpectrumItem.h"
#include "qtquick_items/AudioSpectrumItem.h"
#include "qtquick_items/AudioBarSpectrumItem.h"
#include "qtquick_items/SpectralHistoryItem.h"
#include "qtquick_items/LineItem.h"
#include "qtquick_items/CustomImagePainter.h"

#include <QtGui>
#include <QApplication>
#include <QtQuick>
#include <QSysInfo>
#include <QFontDatabase>
#include <QCommandLineParser>


// amount and complexity of graphical effects (i.e. blur and shadows):
enum TGraphicalEffectsLevel { MIN_EFFECTS = 1, MID_EFFECTS = 2, MAX_EFFECTS = 3 };
#ifdef Q_OS_WIN
static const TGraphicalEffectsLevel GRAPHICAL_EFFECTS_LEVEL = MID_EFFECTS;
#else
static const TGraphicalEffectsLevel GRAPHICAL_EFFECTS_LEVEL = MID_EFFECTS;
#endif


// checks if additional scaling is needed (i.e. for HiDPI displays)
// and sets the dp QML context property accordingly:
void setDpProperty(QQmlApplicationEngine& engine) {
    double scaleFactor = 1.0;

	// get scale factor depending on platform:
	if (QSysInfo::productType() == "android" || QSysInfo::productType() == "ios") {
        double normDPI = 160;
		scaleFactor = (int(QGuiApplication::primaryScreen()->physicalDotsPerInch()) / QGuiApplication::primaryScreen()->devicePixelRatio()) / normDPI;
	} else if (QSysInfo::productType() == "osx") {
        double normDPI = 72;
		scaleFactor = (int(QGuiApplication::primaryScreen()->logicalDotsPerInch()) / QGuiApplication::primaryScreen()->devicePixelRatio()) / normDPI;
    } else {
        double normDPI = 96;
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


int main(int argc, char* argv[]) {
    // prepare Qt application:
    // (using QApplication instead of smaller QGuiApplication to support QWidget based
    // FileDialogs on Linux)
#ifdef Q_OS_LINUX
    setenv("QSG_RENDER_LOOP", "windows", /*overwrite=*/ 1);
#endif
#ifdef Q_OS_IOS
    // disable QML cache on iOS for now because it doesn't work with Qt 5.10 and iOS 11.2 beta
    // (shader files are not found and black rectangles are visible)
    qputenv("QML_DISABLE_DISK_CACHE", "1");
#endif
    //setenv("QSG_INFO", "1", /*overwrite=*/ 1);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
    QApplication::setAttribute(Qt::AA_SynthesizeTouchForUnhandledMouseEvents, false);
    QApplication::setAttribute(Qt::AA_SynthesizeMouseForUnhandledTouchEvents, false);
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/images/icon/app_icon_512.png"));
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
    QFontDatabase::addApplicationFont(":/fonts/lato.hairline.ttf");

    QCommandLineParser parser;
    parser.addPositionalArgument("template", "Template to import");
    parser.process(app);
    QString templateFile = parser.positionalArguments().size() >= 1 ? parser.positionalArguments().at(0) : "";

    // prepare QML engine:
    qmlRegisterSingletonType(QUrl("qrc:/qml/DefaultStyle.qml"), "CustomStyle", 1, 0, "Style");
    qmlRegisterType<BezierCurve>("CustomGeometry", 1, 0, "BezierCurve");
    qmlRegisterType<KineticEffect>("CustomElements", 1, 0, "KineticEffect");
	qmlRegisterType<KineticEffect2D>("CustomElements", 1, 0, "KineticEffect2D");
	qmlRegisterType<StretchColumn>("CustomElements", 1, 0, "StretchColumn");
    qmlRegisterType<StretchRow>("CustomElements", 1, 0, "StretchRow");
    qmlRegisterType<NodeConnectionLines>("CustomElements", 1, 0, "NodeConnectionLines");
    qmlRegisterType<SpectrumItem>("CustomElements", 1, 0, "SpectrumItem");
    qmlRegisterType<AudioSpectrumItem>("CustomElements", 1, 0, "AudioSpectrumItem");
    qmlRegisterType<AudioBarSpectrumItem>("CustomElements", 1, 0, "AudioBarSpectrumItem");
    qmlRegisterType<SpectralHistoryItem>("CustomElements", 1, 0, "SpectralHistoryItem");
    qmlRegisterType<LineItem>("CustomElements", 1, 0, "LineItem");
    qmlRegisterType<CustomImagePainter>("CustomElements", 1, 0, "ImagePainter");
    qRegisterMetaType<TouchAreaEvent>();
    qmlRegisterType<TouchAreaEvent>();
    qmlRegisterType<TouchArea>("CustomElements", 1, 0, "CustomTouchArea");
    QQmlApplicationEngine engine;
    engine.addImportPath("qrc:/qml/");
	setDpProperty(engine);
	engine.rootContext()->setContextProperty("GRAPHICAL_EFFECTS_LEVEL", GRAPHICAL_EFFECTS_LEVEL);

	// MainController will take care of initalizing GUI, output etc.:
    MainController controller(engine, templateFile);
	QObject::connect(&app, SIGNAL(aboutToQuit()), &controller, SLOT(onExit()));
    QObject::connect(&engine, SIGNAL(quit()), &app, SLOT(quit())); // to make Qt.quit() to work

    return app.exec();
}
