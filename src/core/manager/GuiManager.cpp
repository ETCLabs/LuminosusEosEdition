#include "GuiManager.h"

#include "core/MainController.h"

#include <QQuickWindow>
#include <QQuickItem>
#include <QGuiApplication>
#include <QQmlContext>
#include <QScreen>


GuiManager::GuiManager(MainController* controller, QQmlApplicationEngine& qmlEngine)
    : m_controller(controller)
    , m_qmlEngine(qmlEngine)
    , m_backgroundName(LuminosusConstants::defaultBackgroundName)
    , m_overrideGuiScaling(false)
    , m_overrideGraphicsLevel(false)
    , m_snapToGrid(false)
    , m_window(nullptr)
    , m_workspaceItem(nullptr)
    , m_trashItem(nullptr)
    , m_toastItem(nullptr)
{

}

void GuiManager::writeTo(QJsonObject& appState) const {
    appState["windowGeometry"] = serialize<QRect>(getMainWindow()->geometry());
    bool maximized = (getMainWindow()->width() == QGuiApplication::primaryScreen()->availableSize().width());
    appState["windowMaximized"] = maximized;
    appState["overrideGuiScaling"] = getOverrideGuiScaling();
    appState["guiScaling"] = getGuiScaling();
    appState["overrideGraphicsLevel"] = getOverrideGraphicsLevel();
    appState["graphicsLevel"] = getGraphicsLevel();
    appState["snapToGrid"] = getSnapToGrid();
}

void GuiManager::readFrom(const QJsonObject& appState) {
    restoreWindowGeometry(
                deserialize<QRect>(appState["windowGeometry"].toString()), appState["windowMaximized"].toBool());
    setOverrideGuiScaling(appState["overrideGuiScaling"].toBool());
    if (getOverrideGuiScaling()) {
        setGuiScaling(appState["guiScaling"].toDouble());
    }
    setOverrideGraphicsLevel(appState["overrideGraphicsLevel"].toBool());
    if (getOverrideGraphicsLevel()) {
        setGraphicsLevel(appState["graphicsLevel"].toInt());
    }
    setSnapToGrid(appState["snapToGrid"].toBool());
}

void GuiManager::setBackgroundName(QString value) {
    if (value.isEmpty()) value = LuminosusConstants::defaultBackgroundName;
    m_backgroundName = value;
    emit backgroundNameChanged();
}

void GuiManager::createAndShowWindow() {
    // make this MainController accessable from QML with a context variable
    // the access to all other manager instances is done using this controller
    m_qmlEngine.rootContext()->setContextProperty("controller", m_controller);
    m_qmlEngine.rootContext()->setContextProperty("guiManager", this);

    // load main.qml file:
    m_qmlEngine.load(QUrl("qrc:/qml/main.qml"));
    if (m_qmlEngine.rootObjects().length() <= 0) {
        qCritical() << "Error while loading main.qml file. (No root object created)";
        std::exit(999);
    }
    m_window = qobject_cast<QQuickWindow*>(m_qmlEngine.rootObjects()[0]);
    if (!m_window) {
        qCritical() << "Error while loading main.qml file. (Window could not be created)";
        std::exit(998);
    }

#ifdef Q_OS_LINUX
    // ----------- Anti-Aliasing ---------------
    QSurfaceFormat format(m_window->format());
    if (m_qmlEngine.rootContext()->contextProperty("dp").toFloat() > 1.5
            || m_window->screen()->devicePixelRatio() > 1.5) {
        qInfo() << "AntiAliasing: Not forced (HighDPI screen)";
        format.setSamples(-1);
        // platform decides if MSAA should be used
        // if not: "vertex" anti aliasing will be automatically used, which could result in
        // lower performance because of non-opaque primitves and less batching (~30% more batches)
        // but lower GPU memory usage which is more important on HighDPI
        // see http://doc.qt.io/qt-5/qtquick-visualcanvas-scenegraph-renderer.html#antialiasing
        // and export QSG_ANTIALIASING_METHOD=vertex
    } /*else if (window->screen()->availableSize().width() > 1920) {
       qInfo() << "AntiAliasing: MSAA 4x (High Resolution Screen)";
       format.setSamples(4);
       // -> "vertex" anti aliasing will be automatically disabled
       // opaque primitves are truly opaque
    } */else {
        qInfo() << "AntiAliasing: MSAA 4x (Normal Resolution Screen)";
        format.setSamples(4);
        // -> "vertex" anti aliasing will be automatically disabled
        // opaque primitves are truly opaque
    }
    //qInfo() << "OpenGL Version used: " << format.majorVersion() << "." << format.minorVersion();
    //m_window->setFormat(format);
#endif

    // get pointers to commonly used items:
    m_workspaceItem = m_window->findChild<QQuickItem*>("plane");
    m_trashItem = m_window->findChild<QQuickItem*>("trash");
    m_toastItem = m_window->findChild<QQuickItem*>("toast");

    m_window->setIcon(QIcon(":/images/icon/app_icon_512.png"));
    m_window->show();
}

void GuiManager::restoreWindowGeometry(const QRect windowGeometry, bool maximized) const {
    if (!m_window) return;
    if (maximized) {
        m_window->showMaximized();
    } else if (!windowGeometry.isNull()) {
        m_window->setGeometry(windowGeometry);
    }
}

QRect GuiManager::getWindowGeometryOfItem(const QVariant& item) const {
    if (!item.value<QQuickItem*>() || !item.value<QQuickItem*>()->window()) return {0, 0, 800, 600};
    return item.value<QQuickItem*>()->window()->geometry();
}

void GuiManager::setPropertyWithoutChangingBindings(const QVariant& item, QString name, const QVariant& value) const {
    QQuickItem* qitem = item.value<QQuickItem*>();
    if (!qitem) return;
    qitem->setProperty(name.toLatin1().data(), value);
}

int GuiManager::getGraphicsLevel() const {
    return m_qmlEngine.rootContext()->contextProperty("GRAPHICAL_EFFECTS_LEVEL").toInt();
}

void GuiManager::setGraphicsLevel(int value) {
    m_qmlEngine.rootContext()->setContextProperty("GRAPHICAL_EFFECTS_LEVEL", limit(1, value, 3));
}

double GuiManager::getGuiScaling() const {
    return m_qmlEngine.rootContext()->contextProperty("dp").toDouble();
}

void GuiManager::setGuiScaling(double value) {
    m_controller->projectManager()->saveCurrentProject();
    m_qmlEngine.rootContext()->setContextProperty("dp", value);
    m_controller->projectManager()->reloadCurrentProject();
}

void GuiManager::setQmlContextProperty(QString propertyName, QVariant value) const {
    m_qmlEngine.rootContext()->setContextProperty(propertyName, value);
}

void GuiManager::setKeyboardFocusToWorkspace() {
    if (!m_workspaceItem) return;
    QMetaObject::invokeMethod(m_workspaceItem, "forceActiveFocus");
}

void GuiManager::setWorkspacePosition(double x, double y) const {
    if (!m_workspaceItem) return;
    m_workspaceItem->setX(x);
    m_workspaceItem->setY(y);
    // NOTE: cache planeController if this is used more often?
    QQuickItem* planeController = getGuiItemByObjectName("planeController");
    if (planeController) QMetaObject::invokeMethod(planeController, "onExternalPositionChange");
}

void GuiManager::moveToGrid(QQuickItem* block) {
    if (!block) return;
    const double dp = getGuiScaling();
    const double gridWidth = 30.0*dp;

    const int finalX = qRound(block->x() / gridWidth) * gridWidth;
    const int finalY = qRound((block->y() + block->height()) / gridWidth) * gridWidth - block->height();

    QMetaObject::invokeMethod(block, "moveAnimatedTo", Q_ARG(QVariant, finalX), Q_ARG(QVariant, finalY));
}

bool GuiManager::pointIsInTrashArea(qreal x, qreal y) const {
    if (!m_trashItem) return false;
    QPointF localCoords = m_trashItem->mapFromScene(QPointF(x, y));
    bool isInTrash = m_trashItem->contains(localCoords);
    return isInTrash;
}

void GuiManager::showToast(QString text, bool isWarning) const {
    if (!m_toastItem) return;
    QMetaObject::invokeMethod(m_toastItem, "displayToast", Q_ARG(QVariant, text), Q_ARG(QVariant, isWarning));
}

QQuickItem* GuiManager::getGuiItemByObjectName(QString name) const {
    QQuickItem* item = m_window->findChild<QQuickItem*>(name);
    return item;
}
