// src/ui/qt/qml/panels/SoundPanel.qml -- c152-D
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 6

        RowLayout {
            Layout.fillWidth: true
            Label { text: qsTr("Audio devices")
                    color: "#a8e6cf"; font.bold: true }
            Item { Layout.fillWidth: true }
            Button { text: qsTr("Test 440 Hz")
                     onClicked: sound.playTestTone() }
            Button {
                text: sound.monitoring ? qsTr("Stop Listening")
                                       : qsTr("Listen Live")
                onClicked: sound.monitoring ? sound.stopMonitoring()
                                            : sound.startMonitoring()
            }
            Button { text: qsTr("Refresh"); onClicked: sound.refresh() }
        }

        // c152-K : live input meter. Bar fills 0..100% from peak amp.
        RowLayout {
            Layout.fillWidth: true
            visible: sound.monitoring
            Label { text: qsTr("Input :")
                    color: "#7a8c8c"
                    Layout.preferredWidth: 60 }
            Rectangle {
                Layout.fillWidth: true
                height: 14
                color: "#0d1418"
                border.color: "#1f4e58"
                border.width: 1
                radius: 3
                Rectangle {
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    anchors.margins: 2
                    width: Math.max(0, (parent.width - 4) * sound.inputLevel)
                    color: sound.inputLevel > 0.85 ? "#e74c3c"
                         : sound.inputLevel > 0.6  ? "#f1c40f"
                                                   : "#33a6b3"
                    radius: 2
                    Behavior on width { NumberAnimation { duration: 60 } }
                }
            }
            Label {
                text: (sound.inputLevel * 100).toFixed(0) + "%"
                color: "#cce7e7"
                font.family: "Menlo, monospace"
                Layout.preferredWidth: 50
                horizontalAlignment: Text.AlignRight
            }
        }
        Rectangle { Layout.fillWidth: true; height: 1; color: "#1f4e58" }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: sound
            delegate: ItemDelegate {
                width: ListView.view ? ListView.view.width : 0
                contentItem: RowLayout {
                    spacing: 8
                    Label { text: "[" + model.kind + "]"
                            color: model.kind === "IN" ? "#f1c40f"
                                  : model.kind === "OUT" ? "#a8e6cf"
                                  : "#9b59b6"
                            font.family: "Menlo, monospace"
                            Layout.preferredWidth: 60 }
                    Label { text: model.name; color: "#cce7e7"
                            Layout.fillWidth: true; elide: Label.ElideRight }
                    Label { visible: model.isDefault
                            text: "★"; color: "#f1c40f" }
                }
            }
            ScrollBar.vertical: ScrollBar {}
        }
    }
}
