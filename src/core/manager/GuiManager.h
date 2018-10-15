#ifndef GUIMANAGER_H
#define GUIMANAGER_H

#include <QObject>
#include <QJsonObject>
#include <QQmlApplicationEngine>
#include <QRect>

// forward declaration to prevent dependency loop
class MainController;
class QQuickWindow;
class QQuickItem;


namespace GuiManagerConstants {

}


class GuiManager : public QObject
{

    Q_OBJECT

    Q_PROPERTY(QString backgroundName READ getBackgroundName WRITE setBackgroundName NOTIFY backgroundNameChanged)
    Q_PROPERTY(bool overrideGuiScaling READ getOverrideGuiScaling WRITE setOverrideGuiScaling NOTIFY overrideGuiScalingChanged)
    Q_PROPERTY(bool overrideGraphicsLevel READ getOverrideGraphicsLevel WRITE setOverrideGraphicsLevel NOTIFY overrideGraphicsLevelChanged)
    Q_PROPERTY(bool snapToGrid READ getSnapToGrid WRITE setSnapToGrid NOTIFY snapToGridChanged)

public:

    explicit GuiManager(MainController* controller, QQmlApplicationEngine& qmlEngine);

    // ------------------- Persistence --------------------

    /**
     * @brief writeTo writes the GUI related settings into the app state object
     * @param appState the application state
     */
    void writeTo(QJsonObject& appState) const;

    /**
     * @brief readFrom reads the GUI related settings from the app state object
     * @param appState the application state
     */
    void readFrom(const QJsonObject& appState);

signals:
    void openBlockSettings();  //!< can be emitted to tell the GUI to open the Block Settings area
    void backgroundNameChanged();
    void overrideGuiScalingChanged();
    void overrideGraphicsLevelChanged();
    void snapToGridChanged();

public slots:

    /**
     * @brief qmlEngine is a Getter for the only QQmlApplicationEngine instance to use in this application
     * @return  the QQmlApplicationEngine object used to generate the GUI
     */
    QQmlApplicationEngine* qmlEngine() { return &m_qmlEngine; }

    QString getBackgroundName() const { return m_backgroundName; }
    void setBackgroundName(QString value);

    bool getSnapToGrid() const { return m_snapToGrid; }
    void setSnapToGrid(bool value) { m_snapToGrid = value; emit snapToGridChanged(); }

    // ------------------------- Window ----------------------

    /**
     * @brief createAndShowWindow creates the main window, loads QML file and shows it
     */
    void createAndShowWindow();

    /**
     * @brief getMainWindow returns pointer to main window
     * @return a pointer to the main window or null if it hasn't been created yet
     */
    QQuickWindow* getMainWindow() const { return m_window; }

    /**
     * @brief restoreWindowGeometry sets the window position and geometry based on an appState
     * @param windowGeometry size and position of window
     * @param maximized true if maximized
     */
    void restoreWindowGeometry(const QRect windowGeometry, bool maximized) const;
    /**
     * @brief getWindowGeometryOfItem returns the geometry of the window that contains the item
     * @param item a GUI item
     * @return position and size of a window
     */
    QRect getWindowGeometryOfItem(const QVariant& item) const;


    // ------------------------- Context ----------------------

    /**
     * @brief setPropertyWithoutChangingBindings sets a property of a QQuickItem to a value
     * without changing its bindings (in opposite to setting it from QML directly)
     * @param item that contains the property
     * @param name of the property
     * @param value to set the property to
     */
    void setPropertyWithoutChangingBindings(const QVariant& item, QString name, const QVariant& value) const;

    /**
     * @brief getOverrideGuiScaling returns if the GUI scaling factor is overriden by the user
     * @return true if it is overriden
     */
    bool getOverrideGuiScaling() const { return m_overrideGuiScaling; }
    /**
     * @brief setOverrideGuiScaling sets if the GUI scaling factor is overriden by the user
     * @param value true if it is overriden
     */
    void setOverrideGuiScaling(bool value) { m_overrideGuiScaling = value; emit overrideGuiScalingChanged(); }
    /**
     * @brief getOverrideGraphicsLevel returns if the graphics level is overriden by the user
     * @return true if it is overriden
     */
    bool getOverrideGraphicsLevel() const { return m_overrideGraphicsLevel; }
    /**
     * @brief setOverrideGraphicsLevel sets if the graphics level is overriden by the user
     * @param value true if it is overriden
     */
    void setOverrideGraphicsLevel(bool value) { m_overrideGraphicsLevel = value; emit overrideGraphicsLevelChanged(); }
    /**
     * @brief getGraphicsLevel returns the graphics level
     * @return the current graphics level [1, 2, 3]
     */
    int getGraphicsLevel() const;
    /**
     * @brief setGraphicsLevel sets the graphics level
     * @param value the new graphics level [1, 2, 3]
     */
    void setGraphicsLevel(int value);
    /**
     * @brief getGuiScaling returns the GUI scaling factor (-> dp unit) in addition to the Qt
     * internal scaling
     * @return the factor the GUI is scaled with
     */
    double getGuiScaling() const;
    /**
     * @brief setGuiScaling sets the GUI scaling factor (-> dp unit) in addition to the Qt
     * internal scaling
     * @param value factor the GUI should be scaled with
     */
    void setGuiScaling(double value);
    /**
     * @brief setQmlContextProperty convenience method to set a property in the current QML context
     *  to a specific value
     * @param propertyName name of the property to set
     * @param value to set the property to
     */
    void setQmlContextProperty(QString propertyName, QVariant value) const;

    // ------------------------- Workspace ----------------------

    /**
     * @brief getWorkspaceItem returns a pointer to the QML workspace item
     * @return pointer to QML item or nullptr
     */
    QQuickItem* getWorkspaceItem() const { return m_workspaceItem; }
    /**
     * @brief setKeyboardFocusToWorkspace sets the Qt keyboard focus to the workspace plane,
     * can be used to unfocus a GUI item but preserve keyboard shortcuts
     */
    void setKeyboardFocusToWorkspace();
    /**
     * @brief setWorkspacePosition sets the position of the workspace
     * @param x position in px
     * @param y position in px
     */
    void setWorkspacePosition(double x, double y) const;

    /**
     * @brief moveToGrid aligns a QML block item to a virtual grid
     * @param guiItem item to align
     */
    void moveToGrid(QQuickItem* block);

    // ------------------------- Other ----------------------

    /**
     * @brief pointIsInTrashArea checks if a point in the GUI is in the area of the trash icon
     * @param x position in screen coordinates
     * @param y position in screen coordinates
     * @return true if the point is in the trash area
     */
    bool pointIsInTrashArea(qreal x, qreal y) const;
    /**
     * @brief showToast shows a toast message at the bottom of the screen
     * @param text to show
     * @param isWarning true if this is a warning message and should have a red background
     */
    void showToast(QString text, bool isWarning = false) const;

    /**
     * @brief getGuiItemByObjectName finds a GUI item by its objectName
     * @param name objectName specified in QML
     * @return pointer to object or nullptr
     */
    QQuickItem* getGuiItemByObjectName(QString name) const;

private slots:

protected:
    MainController* const m_controller;  //!< a pointer to the MainController

    QQmlApplicationEngine& m_qmlEngine;  //!< QQmlApplicationEngine object, created in main.cpp

    QString m_backgroundName;  //!< the file name of the background image
    bool m_overrideGuiScaling;  //!< true if GUI scaling is overriden by user
    bool m_overrideGraphicsLevel;  //!< true if graphics level is overriden by user
    bool m_snapToGrid;  //!< true if blocks should snap to a virtual grid

    // ------------------------ Cached Values -------------------------

    QQuickWindow* m_window;  //!< pointer to main window

    QQuickItem* m_workspaceItem;  //!< pointer to workspace item ("plane")
    QQuickItem* m_trashItem;  //!< pointer to trash item
    QQuickItem* m_toastItem;  //!< pointer to Toast item

};

#endif // GUIMANAGER_H
