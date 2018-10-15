#include "core/Nodes.h"

#include "core/block_data/BlockInterface.h"
#include "core/MainController.h"  // for LuminosusConstants

// ------------------------ NodeBase -----------------------------------------------------------

// initialize static member attributes:
QPointer<NodeBase> NodeBase::s_focusedNode = nullptr;

NodeBase::NodeBase(BlockInterface* block, int index, bool isOutput)
    : QObject(block)
    , m_block(block)
    , m_index(index)
    , m_isOutput(isOutput)
    , m_uid(block->getUid().append("|").append(QString::number(index)))
    , m_guiItem(nullptr)
    , m_connectedNodes(0)
    , m_nodesSharingActiveState(0)
    , m_nodesSharingRequestedSize(0)
    , m_isActive(true)
    , m_htp(true)
    , m_impulseActive(false)
    , m_requestedSize(1, 1)
    , m_data()
{
    m_impulseTimer.setInterval(100);
    m_impulseTimer.setSingleShot(true);
    connect(&m_impulseTimer, SIGNAL(timeout()), this, SLOT(setOutputBackToZero()));
    connect(block, SIGNAL(positionChanged()), this, SLOT(updateConnectionLines()));
}


// --------------------------- Logic ----------------------------------

// ----------- user interaction:

void NodeBase::onTouched() {
    if (!focusExists()) {
        // if no focus exists, focus this node:
        focus();
    } else if (/*this node*/ isFocused()) {
        // if this node was focused, then this is the second tap
        // -> disconnect from all nodes and defocus:
        disconnectAll();
        defocus();
    } else {
        // -> another node is focused, defocus it and connect to it:
        NodeBase* otherNode = s_focusedNode;
        if (!otherNode) return;
        otherNode->defocus();
        connectTo(otherNode);
    }
}

// ----------- direct interaction:

void NodeBase::connectTo(NodeBase* otherNode) {

    // ----------------- Preparation ----------------
    if (!otherNode) {
        qCritical("Nullptr in connectTo of NodeBase.");
        return;
    }
    if (otherNode->m_block == this->m_block) {
        // nodes are on same block -> ignore:
        return;
    }
    if (otherNode->m_isOutput == this->m_isOutput) {
        // both nodes are inputs or both are ouputs
        // -> steal the connections from them:
        return stealConnectionsFrom(otherNode);
    }
    if (m_connectedNodes.contains(otherNode)) {
        // nodes are already connected
        // -> disconnect them:
        return disconnectFrom(otherNode);
    }

    // -> this is a normal connection process:
    NodeBase* inputNode = nullptr;
    NodeBase* outputNode = nullptr;
    if (m_isOutput) {
        outputNode = this;
        inputNode = otherNode;
    } else {
        inputNode = this;
        outputNode = otherNode;
    }

    if (cycleCheck(outputNode, inputNode)) {
        qDebug() << "Node cycle detected.";
        return;
    }

    // ------------ Connection Process --------------

    outputNode->m_connectedNodes.append(inputNode);
    inputNode->m_connectedNodes.append(outputNode);
    // TODO: create Bezier Curve

    outputNode->updateRequestedSize();
    // -> output calculates total requested size from all connected nodes (including new input node)
    // -> output emits signal to block that requested Size changed
    // -> block calls setData() method with data with new requested Size
    // -> dataWasModifiedByBlock() method notifies all nodes connected to the output
    // that new data is available, including the new input (by calling their updateData() method)

    // call updateData of inputNode in case outputNode didn't change the data (i.e. a slider):
    inputNode->updateData();

    outputNode->updateActiveState();
    // -> output checks if any of its connected input nodes is active
    // -> output emits signal to block that active state changed
    // -> if block was inactive before, it starts sending data with setData() method
    // -> setData() method notifies all nodes connected to the output, that new data is available,
    // including the new input (by calling their updateData() method)

    outputNode->connectionChanged();
    inputNode->connectionChanged();

    outputNode->updateConnectionLines();
}

void NodeBase::disconnectFrom(NodeBase* otherNode) {

    // ----------------- Preparation ----------------
    if (!otherNode) {
        qCritical("Nullptr in disconnectFrom of NodeBase.");
        return;
    }

    NodeBase* inputNode = nullptr;
    NodeBase* outputNode = nullptr;
    if (m_isOutput) {
        outputNode = this;
        inputNode = otherNode;
    } else {
        inputNode = this;
        outputNode = otherNode;
    }

    if (!(outputNode->m_connectedNodes.contains(inputNode)
            && inputNode->m_connectedNodes.contains(outputNode))) {
        qCritical("Tried to disconnect two not properly connected nodes.");
        return;
    }

    outputNode->m_connectedNodes.removeOne(inputNode);
    inputNode->m_connectedNodes.removeOne(outputNode);

    // check if requested Size changed in output node because of disconnect:
    outputNode->updateRequestedSize();
    // check if active State changed in output node because of disconnect:
    outputNode->updateActiveState();

    // check if data changed for input node because of disconnect:
    inputNode->updateData();

    outputNode->connectionChanged();
    inputNode->connectionChanged();

    outputNode->updateConnectionLines();
}

void NodeBase::disconnectAll() {
    for (NodeBase* otherNode: QVector<QPointer<NodeBase>>(m_connectedNodes)) {
        if (!otherNode) continue;
        disconnectFrom(otherNode);
    }
}

void NodeBase::stealConnectionsFrom(NodeBase* otherNode) {
    const QVector<QPointer<NodeBase>>& connections = otherNode->getConnectedNodes();
    for (QPointer<NodeBase> node: connections) {
        connectTo(node);
    }
    otherNode->disconnectAll();
}


// ----------- focus:

void NodeBase::focus() {
    s_focusedNode = this;
    emit focusedChanged();
}

void NodeBase::defocus() {
    if (s_focusedNode == this) {
        s_focusedNode = nullptr;
        emit focusedChanged();
    }
}

void NodeBase::setGuiItem(QQuickItem* item) {
    if (!item) {
        qCritical("Nullptr in setGuiItem of NodeBase.");
        return;
    }
    m_guiItem = item;
}

void NodeBase::updateConnectionLines() {
    // updating the lines can only be done by the input nodes:
    if (!isOutput()) {
        for (NodeBase* outputNode: m_connectedNodes) {
            if (!outputNode) continue;
            outputNode->updateConnectionLines();
        }
        return;
    }
    // this is an output node:
    emit connectionLinesChanged();
}


// --------------------------- Getter + Setter ----------------------------------


// --------- Generic Getter ---------

BlockInterface*NodeBase::getBlock() const {
    return m_block;
}

// -------- Setter of Input Node ---------

void NodeBase::setActive(bool value) {
    if (m_isOutput) {
        qCritical() << "Method setActive() is only available for input nodes.";
        return;
    }
    if (value == m_isActive) return;  // value isn't changing
    m_isActive = value;

    emit isActiveChanged();

    for (NodeBase* outputNode: m_connectedNodes) {
        if (!outputNode) continue;
        outputNode->updateActiveState();
    }
}

void NodeBase::setRequestedSize(Size value) {
    if (m_isOutput) {
        qCritical() << "Method setRequestedSize() is only available for input nodes.";
        return;
    }
    if (value.width == m_requestedSize.width && value.height == m_requestedSize.height) return;
    if (value.width == 0 || value.height == 0) {
        qWarning() << "Width and height of requested size of input node must always be greater 0.";
        return;
    }
    m_requestedSize = value;
    m_data.rescaleTo(value.width, value.height);
    emit dataChanged();

    for (NodeBase* outputNode: m_connectedNodes) {
        if (!outputNode) continue;
        outputNode->updateRequestedSize();
    }
}

void NodeBase::setHtpMode(bool value) {
    if (m_isOutput) {
        qCritical() << "Method setHtpMode() is only available for input nodes.";
        return;
    }
    m_htp = value;
    emit htpModeChanged();
    if (value && getConnectedNodes().size() > 1) {
        updateData();
    }
}

void NodeBase::enableImpulseDetection() {
    if (m_isOutput) {
        qCritical() << "Method enableImpulseDetection() is only available for input nodes.";
        return;
    }

    connect(this, SIGNAL(dataChanged()), this, SLOT(checkForImpulse()));
}

// --------- Getter of Output Node ----------

QVector<QString> NodeBase::getConnections() const {
    QVector<QString> connections;
    if (!m_isOutput) {
        qCritical() << "Method getConnections() is only available for output nodes.";
        return connections;
    }
    for (NodeBase* inputNode: m_connectedNodes) {
        if (!inputNode) continue;
        // "outputUid->inputUid"
        QString connection = getUid().append("->").append(inputNode->getUid());
        connections.append(connection);
    }
    return connections;
}

// -------- Setter of Output Node --------

void NodeBase::dataWasModifiedByBlock() {
    if (!m_isOutput) {
        qCritical() << "Method dataWasModifiedByBlock() is only available for output nodes.";
        return;
    }
    for (NodeBase* inputNode: m_connectedNodes) {
        if (!inputNode) continue;
        inputNode->updateData(this);
    }
}

void NodeBase::sendImpulse() {
    setValue(1.0);
    m_impulseTimer.start();
}

// Convenience Methods:

void NodeBase::setHsv(double h, double s, double v) {
    m_data.setHsv(h, s, v);
    dataWasModifiedByBlock();
}

void NodeBase::setHsvAt(size_t x, size_t y, double h, double s, double v) {
    m_data.setHsvAt(x, y, h, s, v);
    dataWasModifiedByBlock();
}

void NodeBase::setRgb(double r, double g, double b) {
    m_data.setRgb(r, g, b);
    dataWasModifiedByBlock();
}

void NodeBase::setRgbAt(size_t x, size_t y, double r, double g, double b) {
    m_data.setRgbAt(x, y, r, g, b);
    dataWasModifiedByBlock();
}

void NodeBase::setValue(double v) {
    if (v == m_data.getValue()) return;
    m_data.setValue(v);
    dataWasModifiedByBlock();
}

void NodeBase::setAbsoluteValue(double v) {
    if (v == m_data.getAbsoluteValue()) return;
    m_data.setAbsoluteValue(v);
    dataWasModifiedByBlock();
}

void NodeBase::addNodeSharingRequestedSize(NodeBase* input) {
    if (!m_isOutput) {
        qCritical() << "Method dataWasModifiedByBlock() is only available for output nodes.";
        return;
    }
    if (!input) {
        qCritical() << "addNodeSharingRequestedSize() received nullptr.";
        return;
    }
    if (input->isOutput()) {
        qCritical() << "addNodeSharingRequestedSize() can only be used for inputs.";
        return;
    }
    m_nodesSharingRequestedSize.append(input);
}

void NodeBase::checkForImpulse() {
    double value = getValue();
    if (!m_impulseActive && value >= LuminosusConstants::triggerThreshold) {
        m_impulseActive = true;
        emit impulseBegin();
    } else if (m_impulseActive && value < LuminosusConstants::triggerThreshold) {
        m_impulseActive = false;
        emit impulseEnd();
    }
}

void NodeBase::setOutputBackToZero() {
    setValue(0.0);
}

bool NodeBase::cycleCheck(NodeBase* outputNode, NodeBase* inputNode) {
    if (!outputNode || !inputNode) return false;
    QString uid = outputNode->getBlock()->getUid();
    return cycleCheckRecursive(uid, inputNode, 0);
}

bool NodeBase::cycleCheckRecursive(QString uid, NodeBase* inputNode, int depth) {
    if (!inputNode) return false;
    if (depth > 1000) {
        // too many recursion steps, somthing is wrong
        return true;
    }

    // check if the block of this input node is the block we are looking for:
    BlockInterface* block = inputNode->getBlock();
    if (!block) return false;
    if (block->getUid() == uid) {
        // -> cycle detected!
        // cancel condition of recursion
        return true;
    }

    // continue cycle check the nodes, that are connected to the output nodes
    // of this block:

    QList<QPointer<NodeBase>> nodes = block->getNodes();

    for (NodeBase* node: nodes) {
        if (!node) continue;
        if (!node->isOutput()) continue;
        // -> node is output
        auto connectedInputNodes = node->getConnectedNodes();
        for (NodeBase* nextInput: connectedInputNodes) {
            if (!nextInput) continue;
            bool cycle = cycleCheckRecursive(uid, nextInput, ++depth);
            if (cycle) return true;
        }
    }
    return false;
}


// ------ internal logic of Input Node:

void NodeBase::updateData(NodeBase* ltpSource) {
    if (m_isOutput) {
        qCritical() << "Method updateData() is only available for input nodes.";
        return;
    }

    if (m_htp || !ltpSource) {
        bool isFirst = true;
        for (NodeBase* outputNode: m_connectedNodes) {
            if (!outputNode) continue;
            const ColorMatrix& data = outputNode->constData();
            if (data.width() < m_requestedSize.width
                    || data.height() < m_requestedSize.height) {
                qWarning() << "Data of an output is too small for this input node.";
                continue;
            }

            if (isFirst) {
                m_data = data;
                isFirst = false;
            } else {
                m_data.addHtp(data);
            }
        }
    } else {
        // LTP
        // if (!ltpSource) return; -> is checked in first if clause
        const ColorMatrix& data = ltpSource->constData();
        if (data.width() < m_requestedSize.width
                || data.height() < m_requestedSize.height) {
            qWarning() << "Data of output is too small for this input node.";
            return;
        }
        m_data = data;
    }

    emit dataChanged();
}

// ------ internal logic of Output Node:

void NodeBase::updateRequestedSize() {
    if (!m_isOutput) {
        qCritical() << "Method updateRequestedSize() is only available for output nodes.";
        return;
    }

    m_requestedSize = Size{1, 1};
    for (NodeBase* inputNode: m_connectedNodes) {
        if (!inputNode) continue;
        m_requestedSize.width = qMax(m_requestedSize.width, inputNode->getRequestedSize().width);
        m_requestedSize.height = qMax(m_requestedSize.height, inputNode->getRequestedSize().height);
    }
    m_data.rescaleTo(m_requestedSize.width, m_requestedSize.height);
    emit requestedSizeChanged();

    for (NodeBase* inputNode: m_nodesSharingRequestedSize) {
        if (!inputNode) continue;
        inputNode->setRequestedSize(m_requestedSize);
    }
}

void NodeBase::updateActiveState() {
    if (!m_isOutput) {
        qCritical() << "Method updateActiveState() is only available for output nodes.";
        return;
    }

    m_isActive = false;
    for (NodeBase* inputNode: m_connectedNodes) {
        if (!inputNode) continue;
        m_isActive = m_isActive || inputNode->isActive();
    }
    emit isActiveChanged();

    for (NodeBase* inputNode: m_nodesSharingActiveState) {
        if (!inputNode) continue;
        inputNode->setActive(m_isActive);
    }
}
