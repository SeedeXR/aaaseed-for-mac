// src/ui/qt/qml/panels/ConsolePanel.qml
//
// c152-B : Console panel. Listens to `studio.logLine(level, text)`
// signals and appends to a local ListModel ; this avoids needing a
// per-tick poll of Studio's console_log deque. The Studio's
// internal console keeps its own copy for the .aaaproj save path.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root
    implicitWidth: 600
    implicitHeight: 200

    ListModel { id: logModel }

    function levelToColor(level) {
        // 0 INFO, 1 WARN, 2 ERR, 3 LUA -- mirror the ImGui palette.
        switch (level) {
            case 0: return "#a8e6cf"   // light teal
            case 1: return "#f1c40f"   // yellow
            case 2: return "#e74c3c"   // red
            case 3: return "#9b59b6"   // magenta (lua)
        }
        return "#cce7e7"
    }
    function levelToTag(level) {
        switch (level) {
            case 0: return "INFO"
            case 1: return "WARN"
            case 2: return "ERR "
            case 3: return "LUA "
        }
        return "----"
    }

    Connections {
        target: studio
        function onLogLine(level, text) {
            logModel.append({ level: level, text: text })
            // Trim to 4096 lines to match Studio::log's MaxLogLines.
            while (logModel.count > 4096) logModel.remove(0)
            // Auto-scroll to the bottom.
            view.positionViewAtEnd()
        }
    }

    Rectangle { anchors.fill: parent; color: "#0a0f12" }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0

        RowLayout {
            Layout.fillWidth: true
            Layout.preferredHeight: 28
            Rectangle { Layout.fillWidth: true; height: parent.height
                color: "#11181c"
                Label {
                    anchors.left: parent.left; anchors.leftMargin: 12
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("Console (%1)").arg(logModel.count)
                    color: "#a8e6cf"
                }
                Button {
                    anchors.right: parent.right; anchors.rightMargin: 8
                    anchors.verticalCenter: parent.verticalCenter
                    text: qsTr("Clear"); flat: true
                    onClicked: logModel.clear()
                }
            }
        }

        ListView {
            id: view
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: logModel
            delegate: Row {
                spacing: 8
                leftPadding: 12
                Label { text: root.levelToTag(level); color: root.levelToColor(level)
                        font.family: "Menlo, monospace"; font.pixelSize: 12 }
                Label { text: model.text; color: "#cce7e7"
                        font.family: "Menlo, monospace"; font.pixelSize: 12 }
            }
            ScrollBar.vertical: ScrollBar {}
        }
    }
}
