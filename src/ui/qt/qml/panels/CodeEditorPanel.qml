// src/ui/qt/qml/panels/CodeEditorPanel.qml
//
// c152-B : Code Editor panel. TextArea bound to studio.editorText ;
// "Run" button calls studio.runScript() (Cmd+R shortcut wired in
// Main.qml's Run menu).

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    implicitWidth: 800
    implicitHeight: 280

    // c152-M : live lint status. Empty when src parses cleanly.
    property string lintError: ""

    Rectangle { anchors.fill: parent; color: "#0e1418" }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 32
            color: "#11181c"
            RowLayout {
                anchors.fill: parent
                anchors.leftMargin: 12
                anchors.rightMargin: 8
                spacing: 8
                Label {
                    text: qsTr("Editor")
                    color: "#a8e6cf"
                    font.bold: true
                }
                Item { Layout.fillWidth: true }
                Label {
                    text: qsTr("%1 char").arg(editorArea.text.length)
                    color: "#7a8c8c"
                    font.pixelSize: 11
                }
                // Lint status dot : green = clean, red = error.
                Rectangle {
                    width: 8; height: 8; radius: 4
                    color: root.lintError.length === 0 ? "#2ecc71" : "#e74c3c"
                    Layout.alignment: Qt.AlignVCenter
                }
                Button {
                    text: qsTr("Run  Cmd+R")
                    enabled: root.lintError.length === 0
                    onClicked: studio.runScript()
                }
            }
        }

        ScrollView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            TextArea {
                id: editorArea
                text: studio.editorText
                color: "#cce7e7"
                font.family: "Menlo, monospace"
                // c152-N : font size + wrap driven by Preferences.
                font.pixelSize: settings.editorFontSize
                background: Rectangle { color: "#0e1418" }
                wrapMode: settings.editorWordWrap
                    ? TextArea.WrapAtWordBoundaryOrAnywhere
                    : TextArea.NoWrap
                selectByMouse: true
                // c152-M : attach the QSyntaxHighlighter at start-up.
                // Re-attaching is safe (setDocument idempotent).
                Component.onCompleted: luaHelper.attach(editorArea.textDocument)
                onTextChanged: {
                    if (studio.editorText !== text)
                        studio.editorText = text
                    // Debounced lint via Timer below.
                    lintTimer.restart()
                }
                Connections {
                    target: studio
                    function onEditorTextChanged() {
                        if (editorArea.text !== studio.editorText)
                            editorArea.text = studio.editorText
                    }
                }
            }
        }

        // ── Lint banner ───────────────────────────────────────────────
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: visible ? 22 : 0
            visible: root.lintError.length > 0
            color: "#3a1717"
            Label {
                anchors.fill: parent
                anchors.leftMargin: 10
                anchors.rightMargin: 10
                verticalAlignment: Text.AlignVCenter
                text: "⚠  " + root.lintError
                color: "#f1c40f"
                font.family: "Menlo, monospace"
                font.pixelSize: 11
                elide: Label.ElideRight
            }
        }
    }

    // c152-N : debounce interval driven by Preferences.
    Timer {
        id: lintTimer
        interval: settings.lintDebounceMs
        repeat: false
        onTriggered: root.lintError = luaHelper.lint(editorArea.text)
    }

    // c157 : drag-and-drop a .lua file onto the editor to load it into
    // the buffer (then Cmd+R runs it). Sits on top of the whole panel ;
    // shows a highlight ring while a file drag hovers.
    DropArea {
        id: editorDrop
        anchors.fill: parent
        keys: ["text/uri-list"]
        onDropped: (drop) => {
            if (drop.hasUrls && drop.urls.length > 0) {
                if (studio.loadEditorFromFile(drop.urls[0].toString()))
                    drop.accept(Qt.CopyAction)
            }
        }
        Rectangle {
            anchors.fill: parent
            visible: editorDrop.containsDrag
            color: "transparent"
            border.color: "#33a6b3"
            border.width: 2
            radius: 4
        }
    }
}
