#ifndef NODES_H
#define NODES_H

#include "NodeData.h"

#include <QObject>
#include <QPointer>
#include <QVector>
#include <QQuickItem>
#include <QTimer>

// RGB is a windows macro, undefine it first:
#ifdef RGB
#undef RGB
#endif

// Forward declaration to reduce dependencies
class BlockInterface;


/**
 * @brief The NodeBase class is used for Input Node and Output Node objects.
 * It is responsible for the connection handling, active status and matrix size.
 */
class NodeBase : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool htpMode READ getHtpMode WRITE setHtpMode NOTIFY htpModeChanged)
    Q_PROPERTY(bool focused READ isFocused NOTIFY focusedChanged)
    Q_PROPERTY(bool active READ isActive NOTIFY isActiveChanged)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectionChanged)

public:
    /**
     * @brief NodeBase creates a NodeBase object
     * @param block a pointer to the block that contains this Node
     * @param index of this Node in the Block
     * @param isOutput defines if this is an Output Node
     */
    explicit NodeBase(BlockInterface* block, int index, bool isOutput);

signals:
    // ------------ signals for Block:
    /**
     * @brief connectionChanged is emitted when the connection to other nodes changed
     */
    void connectionChanged();

    // ------------ signals of Input Node:
    /**
     * @brief dataChanged is emitted if this is an Input Node and the data changed
     */
    void dataChanged();

    // ------------ signals of Output Node:
    /**
     * @brief requestedSizeChanged is emitted if this is an Output Node and the requested matrix size changed
     */
    void requestedSizeChanged();
    /**
     * @brief isActiveChanged is emitted if this is an Output Node and the active state changed
     */
    void isActiveChanged();

    /**
     * @brief htpChanged is emitted when HTP / LTP mode changes
     */
    void htpModeChanged();

    /**
     * @brief focusedChanged is emitted when focused state of this node changes
     */
    void focusedChanged();

    void connectionLinesChanged();

    void impulseBegin();

    void impulseEnd();

public slots:

    // -------------------------------- Logic -------------------------

    // ------------ user interaction:
    /**
     * @brief onTouched checks what to do when this Node was touched in the GUI
     */
    void onTouched();

    // ------------ direct interaction:
    /**
     * @brief connectTo connects this Node to another one
     * @param otherNode the Node to connect to
     */
    void connectTo(NodeBase* otherNode);
    /**
     * @brief disconnectFrom disconnects this node from another node
     * @param otherNode the Node to disconnect from
     */
    void disconnectFrom(NodeBase* otherNode);
    /**
     * @brief disconnectAll disconnects this Node from all connected Nodes
     */
    void disconnectAll();
    /**
     * @brief stealConnectionsFrom redirects all connections from another Node to this one
     * @param otherNode the Node to "steal" the connections from
     */
    void stealConnectionsFrom(NodeBase* otherNode);

    // ------------ focus:
    /**
     * @brief focus focuses this Node (internal use only)
     */
    void focus();
    /**
     * @brief defocus defocuses this node if it was focused (internal use only)
     */
    void defocus();

    // ------------ GUI:

    /**
     * @brief setGuiItem provides a pointer to the GUI item of this Node
     * @param item a pointer to a InputNode or OutputNode QML item
     */
    void setGuiItem(QQuickItem* item);
    /**
     * @brief updateConnectionLines updates the visual rendering of the connecting lines
     */
    void updateConnectionLines();

    // ---------------------------- Generic Getter -----------------------
    /**
     * @brief getUid returns the UID of this Node
     * @return the UID
     */
    QString getUid() const { return m_uid; }
    /**
     * @brief isOutput returns if this is an Output Node
     * @return true if this is an Output Node
     */
    bool isOutput() const { return m_isOutput; }
    /**
     * @brief isConnected return if this Node is connected to at least one other
     * @return true if it is connected
     */
    bool isConnected() const { return !m_connectedNodes.isEmpty(); }
    /**
     * @brief getConnectedNodes returns the connected list
     * @return a list of all nodes this node is connected to
     */
    const QVector<QPointer<NodeBase>>& getConnectedNodes() { return m_connectedNodes; }
    /**
     * @brief focusExists return if a Node is focused
     * @return true if a Node is currently focused
     */
    static bool focusExists() { return s_focusedNode != nullptr; }
    /**
     * @brief isFocused return if this Node is focused
     * @return true if this Node is focused
     */
    bool isFocused() const { return s_focusedNode == this; }
    /**
     * @brief getFocusedNode return the focused Node
     * @return a pointer to the focused node or nullptr if no Node is focused
     */
    static NodeBase* getFocusedNode() { return s_focusedNode; }
    /**
     * @brief getBlock returns the block that contains this Node
     * @return a pointer to the block
     */
    BlockInterface* getBlock() const;
    /**
     * @brief getGuiItem return the GUI item of this Node
     * @return a pointer to a QML item or nullptr if there is no corresponding GUI item
     */
    QQuickItem* getGuiItem() const { return m_guiItem; }
    /**
     * @brief getIndex returns the index of this node
     * @return index
     */
    int getIndex() const { return m_index; }



    // ------------------------- Getter of Input Node --------------------------
    /**
     * @brief constData returns a const reference to the Node data (Input Node only)
     * @return  a const ColorMatrix reference
     */
    const ColorMatrix& constData() const { return m_data; }

    // ---------- Convenience Method:
    /**
     * @brief getValue returns the "value" of the ColorMatrix (Input Node only)
     * @return a value between 0 and 1
     */
    double getValue() const { return m_data.getValue(); }
    /**
     * @brief getAbsoluteValue returns the "absolute value" of the ColorMatrix (Input Node only)
     * @return a value
     */
    double getAbsoluteValue() const { return m_data.getAbsoluteValue(); }
    /**
     * @brief getHtpMode returns if HTP merging is used (Input Node only)
     * @return true, if HTP merging is used, false if LTP is used
     */
    bool getHtpMode() const { return m_htp; }

    // ------------------------- Setter of Input Node --------------------------
    /**
     * @brief setActive sets the "active state" (Input Node only)
     * @param value true if this Node should be active
     */
    void setActive(bool value);
    /**
     * @brief setRequestedSize sets the requested matrix size (Input Node only)
     * @param value
     */
    void setRequestedSize(Size value);
    /**
     * @brief setHtpMode sets the merging mode (Input Node only)
     * @param value true to use HTP, false to use LTP
     */
    void setHtpMode(bool value);
    /**
     * @brief toggleHtpMode toggle the merging mode (Input Node only)
     */
    void toggleHtpMode() { setHtpMode(!m_htp); }

    /**
     * @brief enableImpulseDetection enables the impulse mode where impulseBegin and impulseEnd
     * is emitted (not enabled by default because of detection overhead)
     */
    void enableImpulseDetection();


    // ------------------------ Getter of Output Node --------------------------
    /**
     * @brief isActive returns if this Node is in active state (Output Node only)
     * @return true if it is active
     */
    bool isActive() const { return m_isActive; }
    /**
     * @brief data returns a writable reference to the ColorMatrix object of this Node;
     * after modifying it, dataWasModifiedByBlock() should be called; (Output Node only)
     * @return a reference to a ColorMatrix object
     */
    ColorMatrix& data() { return m_data; }
    /**
     * @brief getRequestedSize return the resquested matrix size (Output Node only)
     * @return requested matrix size
     */
    const Size& getRequestedSize() const { return m_requestedSize; }
    /**
     * @brief getConnections returns a list of connections for persistence (Output Node only)
     * @return a list of "outputUid->inputUid" strings
     */
    QVector<QString> getConnections() const;

    // ------------------------ Setter of Output Node --------------------------
    /**
     * @brief dataWasModifiedByBlock is to be called, after the data of data() was
     * modified to notify the connected Nodes about the change (Output Node only)
     */
    void dataWasModifiedByBlock();
    /**
     * @brief sendImpulse sets the output for ~1/10s to 1.0 and then back to 0.0
     */
    void sendImpulse();

    // ---------- Convenience Methods:
    /**
     * @brief setHsv convenience method to set a color by HSV values (Output Node only)
     * @param h hue between 0 and 1
     * @param s saturation between 0 and 1
     * @param v value between 0 and 1
     */
    void setHsv(double h, double s, double v);
    /**
     * @brief setHsvAt convenience method to set a color at a certain position by HSV values
     * (Output Node only)
     * @param x position
     * @param y position
     * @param h hue between 0 and 1
     * @param s saturation between 0 and 1
     * @param v value between 0 and 1
     */
    void setHsvAt(size_t x, size_t y, double h, double s, double v);
    /**
     * @brief setRgb convenience method to set a color by RGB values (Output Node only)
     * @param r red between 0 and 1
     * @param g green between 0 and 1
     * @param b blue between 0 and 1
     */
    void setRgb(double r, double g, double b);
    /**
     * @brief setRgbAt convenience method to set a color at a certain position by RGB values
     * (Output Node only)
     * @param x position
     * @param y position
     * @param r red between 0 and 1
     * @param g green between 0 and 1
     * @param b blue between 0 and 1
     */
    void setRgbAt(size_t x, size_t y, double r, double g, double b);
    /**
     * @brief setValue convenience method to the "value" of the this Node (Output Node only)
     * @param v value between 0 and 1
     */
    void setValue(double v);
    /**
     * @brief setAbsoluteValue convenience method to the "absolute value" of the this Node
     * (Output Node only)
     * @param v an abritrary value
     */
    void setAbsoluteValue(double v);

    /**
     * @brief addNodeSharingRequestedSize adds a noe to the list of sharing size nodes
     * @param input an input node
     */
    void addNodeSharingRequestedSize(NodeBase* input);

private slots:
    /**
     * @brief checkForImpulse emits impulse signals, is calles on value change if impulse mode is active
     */
    void checkForImpulse();
    /**
     * @brief setOutputBackToZero is used ~1/10s after sendImpulse() was called to set the output
     * back to 0.0
     */
    void setOutputBackToZero();

protected:

    bool cycleCheck(NodeBase* outputNode, NodeBase* inputNode);

    bool cycleCheckRecursive(QString uid, NodeBase* inputNode, int depth);

    // ------------------------ internal logic of Input Node:
    /**
     * @brief updateData is called to notify the Node that the data of a connected output changed;
     * it merges the data of all connected nodes and emits dataChanged(); (Input Node only)
     * @param ltpSource a pointer to the Node thats data was changed last (for LTP merging)
     * or a nullptr if this is unknown
     */
    void updateData(NodeBase* ltpSource = nullptr);

    // ------------------------ internal logic of Output Node:
    /**
     * @brief updateRequestedSize is called to notify an Output Node that the requested matrix size
     * of a connected Input Node changed; it calculates the max. matrix size
     * and emits requestedSizeChanged(); (Output Node only)
     */
    void updateRequestedSize();
    /**
     * @brief updateActiveState  is called to notify an Output Node that the "active state"
     * of a connected Input Node changed; it checks if at least one connected input is active and
     * emits isActiveChanged(); (Output Node only)
     */
    void updateActiveState();

protected:
    // const infos:
    QPointer<BlockInterface> const m_block;  //!< a pointer to the block that contains this Node
    const int m_index;  //!< index of this node in the Block
    const bool m_isOutput;  //!< true if this is an Output Node
    const QString m_uid;  //!< UID of this Node
    QPointer<QQuickItem> m_guiItem;  //!< pointer to the GUI item for this Node if it exists

    // state:
    QVector<QPointer<NodeBase>> m_connectedNodes;  //!< list of connected Nodes
    QVector<QPointer<NodeBase>> m_nodesSharingActiveState;  //!< list of Nodes sharing active state
    QVector<QPointer<NodeBase>> m_nodesSharingRequestedSize;  //!< list of Node sharing matrix size
    bool m_isActive;  //!< true if this Node is in active state
    bool m_htp;  //!< true if this Node uses HTP merging, false if LTP
    bool m_impulseActive;  //!< true if value is above threshold and impulseBegin was sent (only in impulse mode)
    QTimer m_impulseTimer;  //!< used for sendImpulse() to set the value back to 0.0 after a short time

    // data:
    Size m_requestedSize;  //!< requested matrix size
    ColorMatrix m_data;  //!< ColorMatrix data object

    // static infos:
    static QPointer<NodeBase> s_focusedNode;  //!< contains a pointer to the focused Node or nullptr
};


/**
 * @brief An object of the HsvDataModifier struct can be used to modify the HSV data of
 * an output node.
 * Attention: It should only be used as a stack variable because during the lifetime of this modifier object
 * the data in the output node is not consistent!
 */
struct HsvDataModifier {

    explicit HsvDataModifier(NodeBase* node)
        : m_node(node)
        , m_matrix(node->data())
        , width(m_matrix.width())
        , height(m_matrix.height())
    {
        m_matrix.m_hsvIsValid = true;
        m_matrix.m_rgbIsValid = false;
        m_matrix.m_valueIsValid = false;
    }

    ~HsvDataModifier() {
        m_node->dataWasModifiedByBlock();
    }

    void set(int x, int y, double h, double s, double v) {
        m_matrix.m_hsvData.at(x, y) = HSV(h, s, v);
    }

    void set(int x, int y, const HSV& val) {
        m_matrix.m_hsvData.at(x, y) = val;
    }

    HSV get(int x, int y) const {
        return m_matrix.m_hsvData.at(x, y);
    }

    void setFrom(const HsvMatrix& matrix) {
        m_matrix.m_hsvData.setFrom(matrix);
    }

protected:
    NodeBase* const m_node;
    ColorMatrix& m_matrix;

public:
    const int width;
    const int height;
};


/**
 * @brief An object of the RgbDataModifier struct can be used to modify the RGB data of
 * an output node.
 * Attention: It should only be used as a stack variable because during the lifetime of this modifier object
 * the data in the output node is not consistent!
 */
struct RgbDataModifier {

    explicit RgbDataModifier(NodeBase* node)
        : m_node(node)
        , m_matrix(node->data())
        , width(m_matrix.width())
        , height(m_matrix.height())
    {
        m_matrix.m_hsvIsValid = false;
        m_matrix.m_rgbIsValid = true;
        m_matrix.m_valueIsValid = false;
    }

    ~RgbDataModifier() {
        m_node->dataWasModifiedByBlock();
    }

    void set(int x, int y, double r, double g, double b) {
        m_matrix.m_rgbData.at(x, y) = RGB(r, g, b);
    }

    void set(int x, int y, const RGB& val) {
        m_matrix.m_rgbData.at(x, y) = val;
    }

    RGB get(int x, int y) const {
        return m_matrix.m_rgbData.at(x, y);
    }

protected:
    NodeBase* const m_node;
    ColorMatrix& m_matrix;

public:
    const int width;
    const int height;
};

#endif // NODES_H
