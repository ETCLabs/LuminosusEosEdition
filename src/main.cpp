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

#include "PrepareStart.h"

#include <QApplication>
#include <QtQuick>


int main(int argc, char *argv[]) {
    // prepare Qt application:
    // (using QApplication instead of smaller QGuiApplication to support QWidget based
    // FileDialogs on Linux)
    prepareQApplicationAttributes();
    QApplication app(argc, argv);
    prepareQApplication(app);

    registerFonts();

    // prepare QML engine:
    registerCustomQmlTypes();
    QQmlApplicationEngine engine;
    prepareQmlEngine(engine);

    // MainController will take care of initalizing GUI, output etc.:
    MainController controller(engine);
    QObject::connect(&app, SIGNAL(aboutToQuit()), &controller, SLOT(onExit()));
    QObject::connect(&engine, SIGNAL(quit()), &app, SLOT(quit())); // to make Qt.quit() to work

    // start main loop:
    return app.exec();
}
