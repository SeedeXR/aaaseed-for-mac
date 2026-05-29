// src/ui/qt/qml/panels/EnginePreviewPanel.qml -- c152-G
//
// Hosts the foreign-NSView engine viewport. We give it the panel's
// global pixel rectangle on every geometry change ; the viewport's
// AAASeedInputView is sized to match.
//
// QML can't render a native NSView ; what we display is a reserved
// rectangle (placeholder Rectangle) at the right position, and the
// AAASeedInputView floats on top of that area via macOS view
// composition.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    // Map this item's coordinates to the parent window's logical
    // (point) coordinates. Recompute on every resize / scroll.
    function pushBounds() {
        if (!viewport) return
        if (!Window.window) return
        var p = mapToItem(null, 0, 0)
        // NSView coordinates use bottom-left origin ; Qt uses top-left.
        // The QQuickWindow's NSView contentView matches Qt's top-left.
        // We pass that directly.
        if (viewport.running) {
            viewport.setBounds(p.x, p.y, viewportRect.width, viewportRect.height)
        }
    }

    onWidthChanged:  pushBounds()
    onHeightChanged: pushBounds()
    onXChanged:      pushBounds()
    onYChanged:      pushBounds()
    // c152-G : the foreign NSView is repositioned every time the panel's
    // own geometry changes. When the user drags the parent window
    // around the screen, the NSView moves with it for free (subviews
    // are positioned relative to the parent NSView). Only when the
    // SplitView dividers move do we need to push bounds.

    Rectangle { anchors.fill: parent; color: "#0a0f12" }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        // Toolbar
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 32
            color: "#11181c"
            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 12
                anchors.rightMargin: 8
                Label {
                    text: qsTr("Engine Preview")
                    color: "#a8e6cf"
                    font.bold: true
                }
                Item { Layout.fillWidth: true }
                Label {
                    text: viewport.running
                          ? qsTr("● live")
                          : qsTr("○ stopped")
                    color: viewport.running ? "#2ecc71" : "#7a8c8c"
                    font.family: "Menlo, monospace"
                }
                Button {
                    text: viewport.running ? qsTr("Stop") : qsTr("Start")
                    onClicked: {
                        if (viewport.running) {
                            viewport.detach()
                        } else {
                            var p = viewportRect.mapToItem(null, 0, 0)
                            viewport.attachToWindow(Window.window,
                                p.x, p.y,
                                viewportRect.width, viewportRect.height)
                            if (studio.projectPath.length > 0)
                                viewport.loadProject(studio.projectPath)
                        }
                    }
                }
                Button {
                    text: qsTr("Reload")
                    enabled: viewport.running && studio.projectPath.length > 0
                    onClicked: viewport.loadProject(studio.projectPath)
                }
            }
        }

        // The reserved rectangle where the NSView sits.
        Item {
            id: viewportRect
            Layout.fillWidth: true
            Layout.fillHeight: true

            // Placeholder rendered behind the NSView when stopped.
            Rectangle {
                anchors.fill: parent
                color: "#13202a"
                visible: !viewport.running
                Label {
                    anchors.centerIn: parent
                    text: studio.projectPath.length > 0
                          ? qsTr("Press Start to attach the engine\n→ %1")
                                .arg(studio.projectPath)
                          : qsTr("Save a project first, then press Start.")
                    color: "#7a8c8c"
                    horizontalAlignment: Text.AlignHCenter
                }
            }
            onWidthChanged:  pushBounds()
            onHeightChanged: pushBounds()
        }
    }

    Component.onDestruction: if (viewport.running) viewport.detach()
}
