// src/ui/qt/qml/HomeScreen.qml -- c152-N
//
// Replaces the c152-F Welcome overlay with a proper home / project
// gallery :
//   - Big "New Project" + "Open Sample" + "Open Project…" actions.
//   - Tile grid of recent projects, each with name + path + mtime + ×.
//   - × on a tile prompts to delete the file (or just dismiss from recents).
// Visible when `root.welcomeDismissed` is false ; dismissed by any
// project action.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: home

    signal openProject(string path)
    signal newProjectRequested()
    signal openProjectDialogRequested()
    signal openSampleRequested()
    signal saveAsDialogRequested()

    color: Qt.rgba(0.06, 0.09, 0.11, 0.98)   // near-opaque dark teal

    // c152-N : custom delete-confirmation dialog (QtQuick.Controls
    // Dialog is more portable than QtQuick.Dialogs MessageDialog
    // across Qt 6.x point releases).
    Dialog {
        id: deleteConfirm
        property string targetPath: ""
        property string targetName: ""
        title: qsTr("Delete project?")
        modal: true
        anchors.centerIn: parent
        standardButtons: Dialog.Yes | Dialog.No
        Label {
            text: qsTr("Permanently delete \"%1\"?\n\nThis removes the project file from disk.")
                .arg(deleteConfirm.targetName)
            color: "#cce7e7"
            wrapMode: Label.Wrap
        }
        onAccepted: studio.deleteProjectFile(targetPath)
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 36
        spacing: 16

        // ── Header ───────────────────────────────────────────────────
        RowLayout {
            Layout.fillWidth: true
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2
                Label {
                    text: qsTr("AAASeed Studio")
                    color: "#a8e6cf"
                    font.pixelSize: 36
                    font.bold: true
                }
                Label {
                    text: qsTr("Author live visuals with Lua + MSL shaders.")
                    color: "#7a8c8c"
                    font.pixelSize: 13
                }
            }
            // Top-right action buttons.
            RowLayout {
                spacing: 8
                Button {
                    text: qsTr("✦  New Project")
                    onClicked: home.newProjectRequested()
                }
                Button {
                    text: qsTr("Open Project…")
                    onClicked: home.openProjectDialogRequested()
                }
                Button {
                    text: qsTr("Open Sample")
                    enabled: studio.openSample !== undefined
                    onClicked: home.openSampleRequested()
                }
            }
        }

        Rectangle { Layout.fillWidth: true; height: 1; color: "#1f4e58" }

        // ── Recent projects header ───────────────────────────────────
        RowLayout {
            Layout.fillWidth: true
            Label {
                text: qsTr("Recent Projects")
                color: "#a8e6cf"
                font.pixelSize: 16
                font.bold: true
            }
            Label {
                text: qsTr("(%1)").arg(studio.recentProjects.length)
                color: "#7a8c8c"
                font.pixelSize: 13
            }
            Item { Layout.fillWidth: true }
            Button {
                enabled: studio.recentProjects.length > 0
                text: qsTr("Clear All")
                onClicked: studio.clearRecents()
            }
        }

        // ── Project tile grid ────────────────────────────────────────
        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true

            Flickable { /* ScrollView wants a flickable */
                Flow {
                    width: home.width - 72
                    spacing: 12
                    Repeater {
                        model: studio.recentProjects
                        delegate: Rectangle {
                            width: 260; height: 120
                            radius: 8
                            color: tileMA.containsMouse
                                   ? "#1a2d36" : "#13202a"
                            border.color: tileMA.containsMouse
                                   ? "#33a6b3" : "#1f4e58"
                            border.width: 1

                            ColumnLayout {
                                anchors.fill: parent
                                anchors.margins: 12
                                spacing: 4

                                RowLayout {
                                    Layout.fillWidth: true
                                    Label {
                                        Layout.fillWidth: true
                                        text: studio.projectBasename(modelData)
                                        color: studio.projectExists(modelData)
                                            ? "#cce7e7" : "#7a8c8c"
                                        font.pixelSize: 15
                                        font.bold: true
                                        elide: Label.ElideMiddle
                                    }
                                    // Per-tile × close (delete).
                                    Rectangle {
                                        width: 20; height: 18
                                        radius: 4
                                        color: closeMA.containsMouse
                                               ? "#e74c3c" : "transparent"
                                        Label {
                                            anchors.centerIn: parent
                                            text: "×"
                                            color: closeMA.containsMouse
                                                   ? "#ffffff" : "#7a8c8c"
                                            font.pixelSize: 14
                                            font.bold: true
                                        }
                                        MouseArea {
                                            id: closeMA
                                            anchors.fill: parent
                                            hoverEnabled: true
                                            cursorShape: Qt.PointingHandCursor
                                            onClicked: (m) => {
                                                deleteConfirm.targetPath = modelData
                                                deleteConfirm.targetName =
                                                    studio.projectBasename(modelData)
                                                deleteConfirm.open()
                                                m.accepted = true
                                            }
                                            ToolTip.delay: 350
                                            ToolTip.visible: containsMouse
                                            ToolTip.text: qsTr("Delete project")
                                        }
                                    }
                                }
                                Label {
                                    Layout.fillWidth: true
                                    text: modelData
                                    color: "#7a8c8c"
                                    font.pixelSize: 10
                                    font.family: "Menlo, monospace"
                                    elide: Label.ElideMiddle
                                    wrapMode: Label.NoWrap
                                }
                                Item { Layout.fillHeight: true }
                                RowLayout {
                                    Layout.fillWidth: true
                                    Label {
                                        text: studio.projectExists(modelData)
                                            ? new Date(studio.projectMTimeMs(modelData))
                                                  .toLocaleString(Qt.locale(), "MMM d, yyyy · h:mm AP")
                                            : qsTr("(missing)")
                                        color: studio.projectExists(modelData)
                                            ? "#33a6b3" : "#e74c3c"
                                        font.pixelSize: 10
                                    }
                                    Item { Layout.fillWidth: true }
                                    Label {
                                        text: "◆"
                                        color: "#9b59b6"
                                        font.pixelSize: 11
                                    }
                                }
                            }

                            MouseArea {
                                id: tileMA
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: studio.projectExists(modelData)
                                    ? Qt.PointingHandCursor : Qt.ForbiddenCursor
                                enabled: studio.projectExists(modelData)
                                onClicked: home.openProject(modelData)
                            }
                        }
                    }
                    Rectangle {
                        visible: studio.recentProjects.length === 0
                        width: 540; height: 80
                        color: "transparent"
                        border.color: "#1f4e58"
                        border.width: 1
                        radius: 8
                        Label {
                            anchors.centerIn: parent
                            text: qsTr("No recent projects yet — click ✦ New Project above to start one.")
                            color: "#7a8c8c"
                            font.italic: true
                        }
                    }
                }
            }
        }
    }
}
