// src/ui/qt/qml/panels/CameraPanel.qml -- c152-D
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtMultimedia

Item {
    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 6

        Label { text: qsTr("Camera capture")
                color: "#a8e6cf"; font.bold: true }
        Rectangle { Layout.fillWidth: true; height: 1; color: "#1f4e58" }

        RowLayout {
            Layout.fillWidth: true
            ComboBox {
                id: pick
                Layout.fillWidth: true
                model: camera.deviceNames
                currentIndex: camera.activeIndex
                onActivated: camera.activeIndex = currentIndex
            }
            Button {
                text: camera.capturing ? qsTr("Stop") : qsTr("Start")
                enabled: camera.deviceNames.length > 0
                onClicked: camera.capturing ? camera.stop() : camera.start()
            }
            Button { text: qsTr("Refresh"); onClicked: camera.refresh() }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#0a0f12"
            border.color: "#1f4e58"

            VideoOutput {
                id: video
                anchors.fill: parent
                anchors.margins: 4
                fillMode: VideoOutput.PreserveAspectFit
                visible: camera.capturing
                // c152-I : on completion, hand our QVideoSink to the
                // controller's capture session so it writes frames here.
                Component.onCompleted: camera.bindToVideoSink(video.videoSink)
            }

            Label {
                visible: !camera.capturing
                anchors.centerIn: parent
                text: camera.deviceNames.length > 0
                      ? qsTr("Select a device and press Start.")
                      : qsTr("(no cameras detected ; press Refresh)")
                color: "#7a8c8c"
            }
        }
    }
}
