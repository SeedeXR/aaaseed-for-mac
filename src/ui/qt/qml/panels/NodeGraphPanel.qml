// src/ui/qt/qml/panels/NodeGraphPanel.qml -- c152-J
//
// FigJam-style infinite canvas :
//   - Drag empty space to pan
//   - Mouse wheel zooms 0.25× to 4×
//   - Pan/zoom persisted via Qt.labs.settings
//   - Right-click on a node opens a context menu
//   - Reset View button restores 1× / centred
//
// Node interaction (unchanged from c152-I) :
//   - Drag node body to move
//   - Drag from output pin (right) to input pin (left) to link
//   - Click a node to select ; Delete / Backspace removes it
//   - Asset-typed DropArea on each node accepts asset paths

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Shapes
import QtCore

Item {
    id: root
    focus: true
    implicitWidth: 800
    implicitHeight: 500
    clip: true

    // ── Constants ─────────────────────────────────────────────────────
    readonly property real nodeW: 160
    readonly property real nodeH: 80
    readonly property real titleH: 22
    readonly property real pinR: 9        // c152-L : was 6, bumped for clickability
    readonly property real toolbarY: 48

    // ── Pan / zoom state (persisted) ──────────────────────────────────
    property real panX:  0
    property real panY:  0
    property real zoom:  1.0
    Settings {
        category: "nodeGraph"
        property alias panX: root.panX
        property alias panY: root.panY
        property alias zoom: root.zoom
    }

    // ── Link-drag state ───────────────────────────────────────────────
    property int  dragFromId: 0
    // c152-L : "out" means we're dragging from the source's output pin
    // (left-to-right, source → target). "in" means we're dragging from
    // the source's input pin (the wire actually represents
    // target → source). Always link as output → input.
    property string dragFromSide: ""
    property real dragEndX: 0
    property real dragEndY: 0
    property bool dragging: dragFromId !== 0
    // c152-L : node id whose pin is the candidate drop target.
    // Used to glow the matching pin yellow during the drag.
    property int  hoverInputId: 0
    // c152-L : link id currently hovered (over its mid-point handle).
    // Wires highlight red when hovered ; clicking deletes the link.
    property int  hoverLinkId: 0

    // c152-L : hit-test canvas-space cursor against every node.
    // Strategy: pick the node whose CENTER is closest to the cursor,
    // as long as it's within a generous tolerance (≈ 2× a node
    // width). Skips the drag source so we never self-loop. This is
    // intentionally forgiving -- exact pin-targeting was too tight.
    function pickInputPinAt(wx, wy) {
        var tol = root.nodeW * 2     // 320 px tolerance
        var bestD2 = tol * tol
        var best = 0
        var n = studio.nodeCount
        for (var i = 0; i < n; ++i) {
            var nid = studio.nodeModel.nodeIdAt(i)
            if (nid <= 0 || nid === root.dragFromId) continue
            var nx = studio.nodeModel.nodeX(nid)
            var ny = studio.nodeModel.nodeY(nid)
            // Node-CENTER coords in canvas-local space.
            var cx = nx + root.nodeW / 2
            var cy = ny + root.toolbarY + root.nodeH / 2
            var dx = wx - cx
            var dy = wy - cy
            var d2 = dx * dx + dy * dy
            if (d2 < bestD2) { bestD2 = d2; best = nid }
        }
        return best
    }

    // ── Context menu target ───────────────────────────────────────────
    property int  ctxMenuNodeId: 0
    property int  ctxMenuLinkId: 0
    // World-space cursor when the canvas context menu was opened ; used
    // by "Add Node Here" to spawn the node under the right-click.
    property real ctxMenuWorldX: 0
    property real ctxMenuWorldY: 0

    // ── Background ─ plain dark surface, no grid dots (c152-M) ───────
    Rectangle { anchors.fill: parent; color: "#0d1418" }

    // c152-L : wire renderer. Lives at root level (NOT inside the
    // 100000×100000 scaled canvas Item) so its backing buffer stays
    // viewport-sized. Computes screen coords from world coords just
    // like the dot grid above. Imperative Canvas2D is more reliable
    // than Shape+Repeater for dynamically-added paths.
    Canvas {
        id: wireCanvas
        anchors.fill: parent
        antialiasing: true
        // Track all inputs that should trigger a repaint.
        property real panX:    root.panX
        property real panY:    root.panY
        property real zoom:    root.zoom
        property int  tick:    canvas ? canvas.linksTick : 0
        property real dragEnd: root.dragEndX + root.dragEndY
        property int  dragFromId: root.dragFromId
        property string dragFromSide: root.dragFromSide
        property int  hoverLink: root.hoverLinkId
        onPanXChanged:         requestPaint()
        onPanYChanged:         requestPaint()
        onZoomChanged:         requestPaint()
        onTickChanged:         requestPaint()
        onDragEndChanged:      requestPaint()
        onDragFromIdChanged:   requestPaint()
        onDragFromSideChanged: requestPaint()
        onHoverLinkChanged:    requestPaint()
        onWidthChanged:        requestPaint()
        onHeightChanged:       requestPaint()
        // World → screen.
        function sx(wx) { return panX + wx * zoom }
        function sy(wy) { return panY + wy * zoom }
        onPaint: {
            var ctx = getContext("2d")
            ctx.reset()
            ctx.lineCap  = "round"
            ctx.lineJoin = "round"

            // ── Persistent wires : source.out → target.in ────────────
            var n = studio.nodeModel.linkCount()
            for (var i = 0; i < n; ++i) {
                var linkId = studio.nodeModel.linkIdAt(i)
                var fromId = studio.nodeModel.linkFromAt(i)
                var toId   = studio.nodeModel.linkToAt(i)
                if (fromId <= 0 || toId <= 0) continue
                // World coords of pin centers.
                var fx = studio.nodeModel.nodeX(fromId)
                var fy = studio.nodeModel.nodeY(fromId)
                var tx = studio.nodeModel.nodeX(toId)
                var ty = studio.nodeModel.nodeY(toId)
                var sxA = sx(fx + root.nodeW)                          // source.outPin
                var syA = sy(fy + root.toolbarY + root.nodeH / 2)
                var sxB = sx(tx)                                       // target.inPin
                var syB = sy(ty + root.toolbarY + root.nodeH / 2)
                // Control-point offset scales with zoom so the bezier
                // bulge stays proportional.
                var bulge = 80 * zoom
                // Hovered wire glows red + thicker so the user knows
                // a click will delete it.
                var hovered = (linkId === root.hoverLinkId)
                ctx.strokeStyle = hovered ? "#e74c3c" : "#33a6b3"
                ctx.lineWidth   = hovered ? 4.0       : 2.5
                ctx.beginPath()
                ctx.moveTo(sxA, syA)
                ctx.bezierCurveTo(sxA + bulge, syA, sxB - bulge, syB, sxB, syB)
                ctx.stroke()
            }

            // ── In-flight drag wire (solid yellow) ──────────────────
            if (root.dragging) {
                var fxId = root.dragFromId
                var srcWX, srcWY, srcDir
                if (root.dragFromSide === "in") {
                    srcWX = studio.nodeModel.nodeX(fxId)
                    srcWY = studio.nodeModel.nodeY(fxId) + root.toolbarY + root.nodeH / 2
                    srcDir = -1
                } else {
                    srcWX = studio.nodeModel.nodeX(fxId) + root.nodeW
                    srcWY = studio.nodeModel.nodeY(fxId) + root.toolbarY + root.nodeH / 2
                    srcDir = 1
                }
                var dragSrcX = sx(srcWX)
                var dragSrcY = sy(srcWY)
                var dragDstX = sx(root.dragEndX)
                var dragDstY = sy(root.dragEndY)
                var dragBulge = 60 * zoom
                ctx.strokeStyle = "#f1c40f"
                ctx.lineWidth   = 3.0
                ctx.beginPath()
                ctx.moveTo(dragSrcX, dragSrcY)
                ctx.bezierCurveTo(
                    dragSrcX + srcDir * dragBulge, dragSrcY,
                    dragDstX - srcDir * dragBulge, dragDstY,
                    dragDstX, dragDstY)
                ctx.stroke()
            }
        }
    }

    // Canvas Item -- node + link content transforms together. The
    // grid above is SCREEN-space (computed each repaint from panX/Y +
    // zoom), so it doesn't need to live inside this transform.
    Item {
        id: canvas
        x: root.panX
        y: root.panY
        width:  100000
        height: 100000
        transformOrigin: Item.TopLeft
        scale: root.zoom

        // ── Link layer ────────────────────────────────────────────────
        // linksTick is bumped (a) every 33 ms by Timer to refresh wire
        // positions during node drags, and (b) immediately whenever
        // NodeListModel::linksChanged fires so a newly-created wire
        // shows up the instant it's created.
        property int linksTick: 0
        Timer { running: true; interval: 33; repeat: true
                onTriggered: canvas.linksTick++ }
        Connections {
            target: studio.nodeModel
            function onLinksChanged() { canvas.linksTick++ }
        }

        function outPinX(nx) { return nx + root.nodeW }
        function outPinY(ny) { return ny + root.nodeH / 2 + root.toolbarY }
        function inPinX(nx)  { return nx }
        function inPinY(ny)  { return ny + root.nodeH / 2 + root.toolbarY }

        // (Wires are now rendered by `wireCanvas` at root level --
        // see below. Living inside this scaled 100000×100000 Item
        // hit allocation/rendering quirks ; root level is reliable.)

        // c152-K / c152-L : per-wire mid-point handle. The wire itself
        // is drawn by `wireCanvas` (root-level Canvas) which can't
        // receive clicks ; we overlay a clickable disc with a ×
        // glyph at each wire's midpoint. Left-click deletes ; right-
        // click opens a context menu. Hover sets root.hoverLinkId so
        // the wire glows red in wireCanvas.
        Repeater {
            model: {
                canvas.linksTick
                studio.linkCount
                var n = studio.nodeModel.linkCount()
                var arr = []
                for (var i = 0; i < n; ++i) {
                    var fromId = studio.nodeModel.linkFromAt(i)
                    var toId   = studio.nodeModel.linkToAt(i)
                    arr.push({
                        id:   studio.nodeModel.linkIdAt(i),
                        midX: 0.5 * (canvas.outPinX(studio.nodeModel.nodeX(fromId)) +
                                     canvas.inPinX(studio.nodeModel.nodeX(toId))),
                        midY: 0.5 * (canvas.outPinY(studio.nodeModel.nodeY(fromId)) +
                                     canvas.inPinY(studio.nodeModel.nodeY(toId)))
                    })
                }
                return arr
            }
            delegate: Item {
                x: modelData.midX - 16
                y: modelData.midY - 16
                width:  32
                height: 32
                // Always-visible disc with × glyph.
                Rectangle {
                    anchors.centerIn: parent
                    width:  hover.containsMouse ? 22 : 14
                    height: width
                    radius: width / 2
                    color: hover.containsMouse ? "#e74c3c" : "#13202a"
                    border.color: hover.containsMouse ? "#f1c40f" : "#33a6b3"
                    border.width: 1.5
                    Behavior on width { NumberAnimation { duration: 80 } }
                    Label {
                        anchors.centerIn: parent
                        text: "×"
                        color: "#cce7e7"
                        font.pixelSize: hover.containsMouse ? 16 : 11
                        font.bold: true
                    }
                }
                MouseArea {
                    id: hover
                    anchors.fill: parent       // 32×32 click target
                    hoverEnabled: true
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    cursorShape: Qt.PointingHandCursor
                    onEntered: root.hoverLinkId = modelData.id
                    onExited:  if (root.hoverLinkId === modelData.id)
                                   root.hoverLinkId = 0
                    onClicked: (m) => {
                        if (m.button === Qt.RightButton) {
                            root.ctxMenuLinkId = modelData.id
                            linkCtxMenu.popup()
                        } else {
                            studio.nodeModel.unlinkLink(modelData.id)
                            root.hoverLinkId = 0
                        }
                    }
                    ToolTip.delay: 300
                    ToolTip.visible: containsMouse
                    ToolTip.text: qsTr("Click to delete link  ·  right-click for menu")
                }
            }
        }

        // ── Nodes ─────────────────────────────────────────────────────
        Repeater {
            model: studio.nodeModel
            delegate: Item {
                id: nodeItem
                x: model.x
                y: model.y + root.toolbarY
                width: root.nodeW
                height: root.nodeH

                property bool isSelected: studio.selectedNodeId === model.nodeId

                Rectangle {
                    anchors.fill: parent
                    radius: 6
                    color: nodeItem.isSelected ? "#2da4b3" : "#1f4e58"
                    border.color: nodeItem.isSelected ? "#f1c40f" : "#a8e6cf"
                    border.width: nodeItem.isSelected ? 2 : 1

                    Rectangle {
                        anchors.top: parent.top
                        anchors.left: parent.left
                        anchors.right: parent.right
                        height: root.titleH
                        color: "#11181c"
                        radius: 4
                        Label {
                            anchors.fill: parent
                            anchors.leftMargin: 8
                            verticalAlignment: Text.AlignVCenter
                            text: model.label
                            color: "#cce7e7"
                            font.bold: true
                            elide: Text.ElideRight
                        }
                    }
                    Label {
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 6
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: model.shader.length > 0
                            ? model.shader
                            : (model.script.length > 0
                                ? "script: " + model.script.split("/").pop()
                                : qsTr("(no binding)"))
                        color: model.shader.length > 0 || model.script.length > 0
                            ? "#f1c40f" : "#7a8c8c"
                        font.pixelSize: 11
                        font.family: "Menlo, monospace"
                        elide: Label.ElideMiddle
                        width: parent.width - 8
                        horizontalAlignment: Text.AlignHCenter
                    }

                    // Body : left-click drag, right-click menu. Inset
                    // farther from the left/right edges so pin clicks
                    // win cleanly. anchors.margins covers ALL sides;
                    // we shrink the horizontal sides extra via
                    // explicit left/right margin overrides.
                    MouseArea {
                        anchors.fill: parent
                        anchors.leftMargin:   16
                        anchors.rightMargin:  16
                        anchors.topMargin:    6
                        anchors.bottomMargin: 6
                        acceptedButtons: Qt.LeftButton | Qt.RightButton
                        drag.target: nodeItem
                        drag.axis: Drag.XAndYAxis
                        onPressed: (m) => {
                            studio.selectedNodeId = model.nodeId
                            root.forceActiveFocus()
                            if (m.button === Qt.RightButton) {
                                root.ctxMenuNodeId = model.nodeId
                                ctxMenu.popup()
                            }
                        }
                        onPositionChanged: if (drag.active) {
                            studio.nodeModel.setNodePos(model.nodeId,
                                nodeItem.x, nodeItem.y - root.toolbarY)
                        }
                    }

                    // Asset drop target.
                    DropArea {
                        anchors.fill: parent
                        keys: [ "application/x-aaa-asset" ]
                        onEntered: parent.border.color = "#9b59b6"
                        onExited:  parent.border.color =
                            nodeItem.isSelected ? "#f1c40f" : "#a8e6cf"
                        onDropped: (drop) => {
                            var path = drop.getDataAsString("application/x-aaa-asset")
                            if (path && path.length > 0) {
                                if (path.toLowerCase().endsWith(".lua"))
                                    studio.nodeModel.setNodeScript(model.nodeId, path)
                                else
                                    studio.nodeModel.setNodeShader(model.nodeId, path)
                                drop.acceptProposedAction()
                            }
                        }
                    }
                }

                // Input pin (left) -- also draggable. When you drag
                // FROM an input pin to another node, the resulting
                // link goes from THAT other node TO this one (out→in).
                Rectangle {
                    id: inPin
                    x: -root.pinR
                    y: root.nodeH / 2 - root.pinR
                    width: root.pinR * 2; height: root.pinR * 2; radius: root.pinR
                    property bool isHoverTarget:
                        root.dragging &&
                        root.dragFromId !== model.nodeId &&
                        root.hoverInputId === model.nodeId
                    color: isHoverTarget
                        ? "#f1c40f"
                        : (root.dragFromId === model.nodeId &&
                           root.dragFromSide === "in"
                            ? "#f1c40f" : "#1f4e58")
                    border.color: isHoverTarget ? "#f1c40f" : "#a8e6cf"
                    border.width: isHoverTarget ? 2 : 1.5
                    scale: isHoverTarget ? 1.4 : 1.0
                    Behavior on scale { NumberAnimation { duration: 90 } }
                    MouseArea {
                        anchors.fill: parent; anchors.margins: -14
                        hoverEnabled: true
                        cursorShape: Qt.CrossCursor
                        onPressed: (m) => {
                            root.dragFromId   = model.nodeId
                            root.dragFromSide = "in"
                            var pt = mapToItem(canvas, m.x, m.y)
                            root.dragEndX = pt.x
                            root.dragEndY = pt.y
                            root.hoverInputId = 0
                            m.accepted = true
                        }
                        onPositionChanged: (m) => {
                            if (!root.dragging) return
                            var pt = mapToItem(canvas, m.x, m.y)
                            root.dragEndX = pt.x
                            root.dragEndY = pt.y
                            root.hoverInputId = root.pickInputPinAt(pt.x, pt.y)
                        }
                        onReleased: (m) => {
                            if (root.dragging) {
                                var pt = mapToItem(canvas, m.x, m.y)
                                var targetId = root.pickInputPinAt(pt.x, pt.y)
                                if (targetId > 0) {
                                    // From input-side drag : link is
                                    // target → dragFromId.
                                    studio.nodeModel.linkNodes(targetId, root.dragFromId)
                                }
                            }
                            root.hoverInputId = 0
                            root.dragFromId   = 0
                            root.dragFromSide = ""
                        }
                    }
                }
                // Output pin (right) -- drag to wire forward.
                Rectangle {
                    x: root.nodeW - root.pinR
                    y: root.nodeH / 2 - root.pinR
                    width: root.pinR * 2; height: root.pinR * 2; radius: root.pinR
                    color: (root.dragFromId === model.nodeId &&
                            root.dragFromSide === "out") ? "#f1c40f" : "#33a6b3"
                    border.color: "#a8e6cf"
                    border.width: 1.5
                    MouseArea {
                        anchors.fill: parent; anchors.margins: -14
                        hoverEnabled: true
                        cursorShape: Qt.CrossCursor
                        onPressed: (m) => {
                            root.dragFromId   = model.nodeId
                            root.dragFromSide = "out"
                            root.dragEndX = canvas.outPinX(studio.nodeModel.nodeX(model.nodeId))
                            root.dragEndY = canvas.outPinY(studio.nodeModel.nodeY(model.nodeId))
                            root.hoverInputId = 0
                            m.accepted = true
                        }
                        onPositionChanged: (m) => {
                            if (!root.dragging) return
                            var pt = mapToItem(canvas, m.x, m.y)
                            root.dragEndX = pt.x
                            root.dragEndY = pt.y
                            root.hoverInputId = root.pickInputPinAt(pt.x, pt.y)
                        }
                        onReleased: (m) => {
                            if (root.dragging) {
                                var pt = mapToItem(canvas, m.x, m.y)
                                var targetId = root.pickInputPinAt(pt.x, pt.y)
                                if (targetId > 0)
                                    studio.nodeModel.linkNodes(root.dragFromId, targetId)
                            }
                            root.hoverInputId = 0
                            root.dragFromId   = 0
                            root.dragFromSide = ""
                        }
                    }
                }
            }
        }
    }

    // ── Empty-canvas drag = pan ; right-click = "Add Node Here" menu ──
    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        z: -1   // below the node MouseAreas
        property real panStartX: 0
        property real panStartY: 0
        property real mouseStartX: 0
        property real mouseStartY: 0
        property bool panning: false

        onPressed: (m) => {
            if (m.button === Qt.RightButton) {
                // World-space coordinate under cursor.
                root.ctxMenuWorldX = (m.x - root.panX) / root.zoom
                root.ctxMenuWorldY = (m.y - root.panY) / root.zoom - root.toolbarY
                canvasCtxMenu.popup()
                return
            }
            panStartX = root.panX
            panStartY = root.panY
            mouseStartX = m.x
            mouseStartY = m.y
            panning = true
            studio.selectedNodeId = 0
            root.forceActiveFocus()
            cursorShape = Qt.ClosedHandCursor
        }
        onPositionChanged: (m) => {
            if (panning) {
                root.panX = panStartX + (m.x - mouseStartX)
                root.panY = panStartY + (m.y - mouseStartY)
            }
        }
        onReleased: { panning = false; cursorShape = Qt.OpenHandCursor }
        cursorShape: Qt.OpenHandCursor

        // Mouse wheel zoom -- keeps the point under the cursor anchored.
        onWheel: (w) => {
            var factor = w.angleDelta.y > 0 ? 1.10 : 1.0 / 1.10
            var newZoom = Math.max(0.25, Math.min(4.0, root.zoom * factor))
            if (newZoom === root.zoom) return
            // Anchor under-cursor : world point W = (mouse - pan) / zoom.
            // After zoom : new pan = mouse - W * newZoom.
            var wx = (w.x - root.panX) / root.zoom
            var wy = (w.y - root.panY) / root.zoom
            root.zoom = newZoom
            root.panX = w.x - wx * newZoom
            root.panY = w.y - wy * newZoom
            w.accepted = true
        }
    }

    // ── Toolbar (overlays the canvas) ─────────────────────────────────
    Row {
        id: toolbar
        x: 8; y: 8
        spacing: 8
        z: 10
        Button { text: qsTr("+ Node")
                 onClicked: studio.nodeModel.addNode("Node " + (studio.nodeCount + 1)) }
        Button { text: qsTr("Clear All")
                 onClicked: {
                     while (studio.nodeCount > 0) {
                         var id = studio.nodeModel.nodeIdAt(0)
                         if (id <= 0) break
                         studio.nodeModel.removeNode(id)
                     }
                     studio.selectedNodeId = 0
                 }
        }
        Button { text: qsTr("Reset View")
                 onClicked: { root.panX = 0; root.panY = 0; root.zoom = 1.0 } }
        Label {
            anchors.verticalCenter: parent.verticalCenter
            text: qsTr("Nodes : %1   Links : %2   Zoom : %3×")
                    .arg(studio.nodeCount).arg(studio.linkCount)
                    .arg(root.zoom.toFixed(2))
            color: "#7a8c8c"
        }
        Label {
            anchors.verticalCenter: parent.verticalCenter
            text: studio.selectedNodeId > 0
                  ? qsTr("Selected #%1 (Del to remove, right-click for menu)")
                      .arg(studio.selectedNodeId)
                  : qsTr("Drag empty canvas to pan, scroll to zoom")
            color: "#9b59b6"
            font.italic: true
        }
    }

    // ── Right-click context menu on nodes ─────────────────────────────
    Menu {
        id: ctxMenu
        MenuItem { text: qsTr("Set Shader...")
                   onTriggered: shaderDialog.open() }
        MenuItem { text: qsTr("Set Script...")
                   onTriggered: scriptDialog.open() }
        MenuItem { text: qsTr("Clear Bindings")
                   onTriggered: {
                       studio.nodeModel.setNodeShader(root.ctxMenuNodeId, "")
                       studio.nodeModel.setNodeScript(root.ctxMenuNodeId, "")
                   }
        }
        MenuSeparator {}
        MenuItem { text: qsTr("Disconnect All Links")
                   onTriggered: {
                       var nid = root.ctxMenuNodeId
                       var n = studio.nodeModel.linkCount()
                       var toRemove = []
                       for (var i = 0; i < n; ++i) {
                           if (studio.nodeModel.linkFromAt(i) === nid ||
                               studio.nodeModel.linkToAt(i) === nid) {
                               toRemove.push(studio.nodeModel.linkIdAt(i))
                           }
                       }
                       for (var j = 0; j < toRemove.length; ++j)
                           studio.nodeModel.unlinkLink(toRemove[j])
                   }
        }
        MenuSeparator {}
        MenuItem { text: qsTr("Delete Node")
                   onTriggered: {
                       studio.nodeModel.removeNode(root.ctxMenuNodeId)
                       studio.selectedNodeId = 0
                   }
        }
    }

    // c152-K : right-click on empty canvas. Spawns a node at the
    // cursor (in world space, so panning/zoom is honored).
    Menu {
        id: canvasCtxMenu
        MenuItem {
            text: qsTr("Add Node Here")
            onTriggered: {
                var newId = studio.nodeModel.addNode(
                    "Node " + (studio.nodeCount + 1))
                if (newId > 0) {
                    studio.nodeModel.setNodePos(newId,
                        root.ctxMenuWorldX - root.nodeW / 2,
                        root.ctxMenuWorldY - root.nodeH / 2)
                    studio.selectedNodeId = newId
                }
            }
        }
        MenuSeparator {}
        MenuItem {
            text: qsTr("Reset View")
            onTriggered: { root.panX = 0; root.panY = 0; root.zoom = 1.0 }
        }
    }

    // c152-K : right-click on a wire's mid-point handle.
    Menu {
        id: linkCtxMenu
        MenuItem {
            text: qsTr("Delete Link")
            onTriggered: studio.nodeModel.unlinkLink(root.ctxMenuLinkId)
        }
    }

    // Shader picker dialog.
    Dialog {
        id: shaderDialog
        title: qsTr("Bind shader to node #") + root.ctxMenuNodeId
        modal: true
        anchors.centerIn: parent
        width: 400
        standardButtons: Dialog.Close
        ScrollView {
            anchors.fill: parent
            ListView {
                model: studio.shaderCatalog
                delegate: ItemDelegate {
                    width: ListView.view.width
                    text: modelData
                    onClicked: {
                        studio.nodeModel.setNodeShader(root.ctxMenuNodeId, modelData)
                        shaderDialog.close()
                    }
                }
            }
        }
    }
    // Script picker (file dialog).
    Dialog {
        id: scriptDialog
        title: qsTr("Bind script to node #") + root.ctxMenuNodeId
        modal: true
        anchors.centerIn: parent
        width: 400
        height: 160
        standardButtons: Dialog.Cancel
        ColumnLayout {
            anchors.fill: parent
            Label { text: qsTr("Pick a project asset row from the Assets panel + drag onto a node, or set a path here :")
                    color: "#cce7e7"; wrapMode: Label.WordWrap; Layout.fillWidth: true }
            TextField {
                id: scriptPathField
                Layout.fillWidth: true
                placeholderText: "Assets/mything.lua"
            }
            Button {
                text: qsTr("Set Script")
                onClicked: {
                    studio.nodeModel.setNodeScript(root.ctxMenuNodeId, scriptPathField.text)
                    scriptDialog.close()
                }
            }
        }
    }

    // ── Keyboard ──────────────────────────────────────────────────────
    Keys.onPressed: (event) => {
        if ((event.key === Qt.Key_Delete ||
             event.key === Qt.Key_Backspace) &&
            studio.selectedNodeId > 0)
        {
            studio.nodeModel.removeNode(studio.selectedNodeId)
            studio.selectedNodeId = 0
            event.accepted = true
        }
        else if (event.key === Qt.Key_0 && event.modifiers & Qt.ControlModifier)
        {
            root.panX = 0; root.panY = 0; root.zoom = 1.0
            event.accepted = true
        }
    }
}
