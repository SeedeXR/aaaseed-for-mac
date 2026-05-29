// src/ui/qt/qml/panels/BinaryManagerPanel.qml -- c152-D

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Item {
    FileDialog {
        id: pickPath
        title: qsTr("Pick executable")
        onAccepted: pathField.text = selectedFile.toString().replace("file://", "")
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 6

        Label { text: qsTr("Binary Manager (QProcess)")
                color: "#a8e6cf"; font.bold: true }
        Rectangle { Layout.fillWidth: true; height: 1; color: "#1f4e58" }

        // Add-task form
        GridLayout {
            Layout.fillWidth: true
            columns: 2
            columnSpacing: 6
            rowSpacing: 4

            Label { text: qsTr("Label"); color: "#7a8c8c" }
            TextField { id: labelField; Layout.fillWidth: true
                        placeholderText: qsTr("e.g. http-server") }

            Label { text: qsTr("Path"); color: "#7a8c8c" }
            RowLayout {
                Layout.fillWidth: true
                TextField { id: pathField; Layout.fillWidth: true
                            placeholderText: qsTr("/usr/bin/python3") }
                Button { text: qsTr("..."); onClicked: pickPath.open() }
            }

            Label { text: qsTr("Args"); color: "#7a8c8c" }
            TextField { id: argsField; Layout.fillWidth: true
                        placeholderText: qsTr("-m http.server 8080") }
        }
        Button {
            Layout.alignment: Qt.AlignRight
            text: qsTr("Attach")
            enabled: pathField.text.length > 0
            onClicked: {
                tasks.addTask(labelField.text, pathField.text, argsField.text)
                labelField.text = ""
                pathField.text = ""
                argsField.text = ""
            }
        }

        Rectangle { Layout.fillWidth: true; height: 1; color: "#1f4e58" }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: tasks
            delegate: ItemDelegate {
                width: ListView.view ? ListView.view.width : 0
                contentItem: RowLayout {
                    spacing: 6
                    Label {
                        text: model.running ? "●" : "○"
                        color: model.running ? "#2ecc71" : "#e74c3c"
                        font.pixelSize: 14
                        Layout.preferredWidth: 14
                    }
                    Label {
                        text: model.running
                            ? qsTr("%1  (pid %2)").arg(model.label).arg(model.pid)
                            : (model.exitCode !== 0
                                ? qsTr("%1  (exit %2)").arg(model.label).arg(model.exitCode)
                                : model.label)
                        color: "#cce7e7"
                        Layout.fillWidth: true
                        elide: Label.ElideRight
                    }
                    Button {
                        text: model.running ? qsTr("Stop") : qsTr("Start")
                        onClicked: model.running ? tasks.stopTask(index)
                                                  : tasks.startTask(index)
                    }
                    Button {
                        text: qsTr("Remove"); flat: true
                        enabled: !model.running
                        onClicked: tasks.removeTask(index)
                    }
                }
            }
            ScrollBar.vertical: ScrollBar {}
        }
    }
}
