// src/ui/qt/qml/panels/PanelHost.qml -- c152-N (revised from c152-M)
//
// Implements the panel anatomy described in docs/docking-spec.md :
//
//   ┌─────────────────────────────────────────────┐
//   │ ⠿  Title                       ⬡   ─   ×    │
//   ├─────────────────────────────────────────────│
//   │                                             │
//   │                  Content                    │
//   │                                             │
//   └─────────────────────────────────────────────┘
//
// Header elements :
//   ⠿  grip            : left-drag → detach, right-click → context menu
//   ⬡  snap-back       : visible ONLY when floating ; one click → re-dock
//   ─  collapse        : collapse-to-header / expand toggle
//   ×  close           : hide panel (restore from View menu)
//
// State persisted per `panelId` under workspace/<panelId>/ via QSettings :
//   floating  collapsed  visible  floatX/Y/W/H
//
// The contentComponent is instantiated via Loader in either the inline
// frame or the floating Window depending on `floating`. Because the
// underlying data lives in C++ models, recreating the view is cheap.

import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtCore

Item {
    id: host

    // ── Required + public properties ───────────────────────────────────
    required property string panelId
    required property string title
    required property Component contentComponent

    // c152-O : signals the parent to close us. Parent (Main.qml) owns
    // the show<X> visibility flag so the close can be undone via the
    // View menu. The host no longer tracks its own panelVisible.
    signal closeRequested()

    // Persisted state (workspace/<panelId>/...).
    property bool  floating:  false
    property bool  collapsed: false
    property int   floatX:   200
    property int   floatY:   140
    property int   floatW:   480
    property int   floatH:   360

    Settings {
        category: "workspace/" + host.panelId
        property alias floating:  host.floating
        property alias collapsed: host.collapsed
        property alias floatX:    host.floatX
        property alias floatY:    host.floatY
        property alias floatW:    host.floatW
        property alias floatH:    host.floatH
    }

    // c152-O : listen for global workspace-reset to restore defaults.
    Connections {
        target: studio
        function onWorkspaceResetRequested() {
            host.floating  = false
            host.collapsed = false
            host.floatX    = 200
            host.floatY    = 140
            host.floatW    = 480
            host.floatH    = 360
        }
    }

    // c152-O : Drag-to-dock. If the floating window's top-left is
    // close enough to the inline slot's screen position, snap back.
    // Threshold ≈ 80 px. We use mapToGlobal on `inlineFrame` to find
    // the slot's screen-space rect.
    property real _snapPx: 80
    function maybeSnapBackFromWindow() {
        if (!host.floating) return
        if (!inlineFrame.visible && !inlineFrame.parent) return
        // Slot's top-left in global coordinates.
        var slot = inlineFrame.mapToGlobal(0, 0)
        var dx = floatWindow.x - slot.x
        var dy = floatWindow.y - slot.y
        if (dx * dx + dy * dy < _snapPx * _snapPx) {
            host.floating = false
        }
    }

    // ─── Header bar component (shared between docked & floating views) ─
    component Header : Rectangle {
        id: headerRoot
        property bool isFloatingHeader: false
        height: 24
        color: "#0e1418"

        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 6
            anchors.rightMargin: 4
            spacing: 6

            // ── Grip ⠿ : six dots in a 2×3 grid, left-drag detaches,
            // right-click opens context menu.
            Item {
                id: grip
                Layout.preferredWidth: 16
                Layout.preferredHeight: 18
                Layout.alignment: Qt.AlignVCenter
                property bool armed: false
                property real pressX: 0
                property real pressY: 0
                Column {
                    anchors.centerIn: parent
                    spacing: 2
                    Repeater {
                        model: 3
                        Row {
                            spacing: 3
                            Rectangle { width: 2; height: 2; radius: 1
                                color: gripMA.containsMouse ? "#a8e6cf" : "#7a8c8c" }
                            Rectangle { width: 2; height: 2; radius: 1
                                color: gripMA.containsMouse ? "#a8e6cf" : "#7a8c8c" }
                        }
                    }
                }
                MouseArea {
                    id: gripMA
                    anchors.fill: parent
                    anchors.margins: -4
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    hoverEnabled: true
                    cursorShape: grip.armed ? Qt.ClosedHandCursor
                                            : Qt.OpenHandCursor
                    onPressed: (m) => {
                        grip.pressX = m.x
                        grip.pressY = m.y
                        grip.armed  = true
                        if (m.button === Qt.RightButton) {
                            contextMenu.popup()
                            grip.armed = false
                        }
                    }
                    onReleased: { grip.armed = false }
                    onPositionChanged: (m) => {
                        if (!grip.armed) return
                        // Threshold : > 6 px = detach.
                        var dx = m.x - grip.pressX
                        var dy = m.y - grip.pressY
                        if (dx * dx + dy * dy < 36) return
                        // Trigger detach.
                        if (!host.floating) {
                            // Spawn near the global cursor for a more
                            // "follow the cursor" feel.
                            var p = mapToGlobal(m.x, m.y)
                            host.floatX = p.x - 40
                            host.floatY = p.y - 12
                            host.floating = true
                        }
                        grip.armed = false
                    }
                    ToolTip.delay: 500
                    ToolTip.visible: containsMouse && !pressed
                    ToolTip.text: qsTr("Drag to detach · right-click for menu")
                }
            }

            Label {
                Layout.fillWidth: true
                Layout.alignment: Qt.AlignVCenter
                text: host.title
                color: "#a8e6cf"
                font.pixelSize: 11
                font.bold: true
                elide: Label.ElideRight
            }

            // ── ⬡ Snap-back (only when floating) ─────────────────────
            HeaderButton {
                visible: host.floating && headerRoot.isFloatingHeader
                glyph: "⬡"
                tip: qsTr("Snap back to docked position")
                onClicked: host.floating = false
            }
            // ── ⤢ Detach (only when docked) ──────────────────────────
            HeaderButton {
                visible: !host.floating
                glyph: "⤢"
                tip: qsTr("Detach to floating window")
                onClicked: host.floating = true
            }
            // ── ─ Collapse / expand ──────────────────────────────────
            HeaderButton {
                visible: !host.floating
                glyph: host.collapsed ? "+" : "—"
                tip: host.collapsed ? qsTr("Expand panel")
                                    : qsTr("Collapse panel")
                onClicked: host.collapsed = !host.collapsed
            }
            // ── × Close ──────────────────────────────────────────────
            HeaderButton {
                glyph: "×"
                tip: qsTr("Hide panel (restore from View menu)")
                onClicked: {
                    if (host.floating) host.floating = false
                    host.closeRequested()
                }
            }
        }

        // Double-click toggles collapse.
        MouseArea {
            anchors.fill: parent
            anchors.leftMargin: 30   // leave the grip area to the grip
            propagateComposedEvents: true
            onDoubleClicked: host.collapsed = !host.collapsed
            onPressed: (m) => m.accepted = false
        }
    }

    // Single header button component -- shared style.
    component HeaderButton : Item {
        id: btnRoot
        property string glyph
        property string tip
        signal clicked
        Layout.preferredWidth: 20
        Layout.preferredHeight: 18
        Rectangle {
            anchors.fill: parent
            color: btnMA.containsMouse ? "#1f4e58" : "transparent"
            radius: 3
        }
        Label {
            anchors.centerIn: parent
            text: btnRoot.glyph
            color: "#a8e6cf"
            font.pixelSize: 11
        }
        MouseArea {
            id: btnMA
            anchors.fill: parent
            hoverEnabled: true
            cursorShape: Qt.PointingHandCursor
            onClicked: btnRoot.clicked()
            ToolTip.delay: 500
            ToolTip.visible: containsMouse
            ToolTip.text: btnRoot.tip
        }
    }

    // ─── Right-click context menu (per spec §6) ───────────────────────
    Menu {
        id: contextMenu
        MenuItem {
            text: host.floating ? qsTr("Snap back") : qsTr("Detach panel")
            onTriggered: host.floating = !host.floating
        }
        MenuItem {
            text: host.collapsed ? qsTr("Expand") : qsTr("Collapse")
            onTriggered: host.collapsed = !host.collapsed
        }
        MenuSeparator {}
        MenuItem {
            text: qsTr("Close")
            onTriggered: {
                if (host.floating) host.floating = false
                host.panelVisible = false
            }
        }
    }

    // ─── Inline frame (when docked) ───────────────────────────────────
    Rectangle {
        id: inlineFrame
        anchors.fill: parent
        visible: !host.floating
        color: "#13202a"
        radius: 8
        border.color: "#1f4e58"
        border.width: 1
        clip: true

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 1
            spacing: 0

            Header { Layout.fillWidth: true; isFloatingHeader: false }

            // Content slot (docked, expanded). Hidden when collapsed.
            Loader {
                id: inlineLoader
                Layout.fillWidth: true
                Layout.fillHeight: true
                visible: !host.collapsed
                sourceComponent: (host.floating || host.collapsed)
                    ? null : host.contentComponent
            }
        }
    }

    // ─── Floating window (when detached) ──────────────────────────────
    Window {
        id: floatWindow
        visible: host.floating
        width:   host.floatW
        height:  host.floatH
        x:       host.floatX
        y:       host.floatY
        title:   "AAASeed Studio — " + host.title
        color:   "#13202a"
        flags:   Qt.Window | Qt.WindowSystemMenuHint |
                 Qt.WindowMinMaxButtonsHint | Qt.WindowCloseButtonHint
        minimumWidth:  240
        minimumHeight: 180

        // Closing the window docks back (Photoshop semantics).
        onClosing: { host.floating = false }

        // c152-O : "drag-to-dock" — whenever the user moves the
        // floating window so its top-left lands within ~80 px of the
        // inline slot's screen position, snap it back. Position
        // updates fire continuously while dragging.
        onXChanged: {
            if (!visible) return
            host.floatX = x
            host.maybeSnapBackFromWindow()
        }
        onYChanged: {
            if (!visible) return
            host.floatY = y
            host.maybeSnapBackFromWindow()
        }
        onWidthChanged:  if (visible) host.floatW = width
        onHeightChanged: if (visible) host.floatH = height

        ColumnLayout {
            anchors.fill: parent
            spacing: 0
            Header { Layout.fillWidth: true; isFloatingHeader: true }
            Loader {
                id: floatingLoader
                Layout.fillWidth: true
                Layout.fillHeight: true
                sourceComponent: host.floating ? host.contentComponent : null
            }
        }
    }

    // ─── Placeholder shown in the inline slot while floating ──────────
    Rectangle {
        anchors.fill: parent
        visible: host.floating
        color: "#13202a"
        radius: 8
        border.color: "#1f4e58"
        border.width: 1
        ColumnLayout {
            anchors.centerIn: parent
            spacing: 6
            Label {
                Layout.alignment: Qt.AlignHCenter
                text: host.title
                color: "#7a8c8c"
                font.pixelSize: 12
                font.italic: true
            }
            Label {
                Layout.alignment: Qt.AlignHCenter
                text: qsTr("(floating)")
                color: "#33a6b3"
                font.pixelSize: 10
            }
            Button {
                Layout.alignment: Qt.AlignHCenter
                text: qsTr("⬡  Snap back")
                onClicked: host.floating = false
            }
        }
    }
}
