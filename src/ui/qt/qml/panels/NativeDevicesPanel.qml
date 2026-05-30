// src/ui/qt/qml/panels/NativeDevicesPanel.qml -- c154
//
// Surfaces the c153 native feature sub-libs (MIDI / native audio / native
// video / Syphon / clipboard / net + display) via the `nativeDevices`
// context property. Read-mostly : it shows the SAME device truth the engine
// sees, so the intuitive Studio UI and the native engine stay consistent.
// Purely additive -- it sits in its own "Devices" tab and does not affect the
// Qt-Multimedia Sound/Camera/Tasks panels.
import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    // Small reusable labelled list section.
    component DeviceSection: ColumnLayout {
        property string heading: ""
        property var entries: []
        Layout.fillWidth: true
        spacing: 2
        Label { text: heading; color: "#a8e6cf"; font.bold: true }
        Repeater {
            model: entries
            delegate: Label {
                Layout.fillWidth: true
                leftPadding: 10
                text: "- " + modelData
                color: "#cce7e7"
                elide: Text.ElideRight
            }
        }
        Label {
            visible: entries.length === 0
            leftPadding: 10
            text: qsTr("(none detected)")
            color: "#7a8c8c"
        }
    }

    ScrollView {
        anchors.fill: parent
        anchors.margins: 8
        clip: true
        ColumnLayout {
            width: parent.parent.width - 16
            spacing: 8

            RowLayout {
                Layout.fillWidth: true
                Label { text: qsTr("Native engine devices")
                        color: "#a8e6cf"; font.bold: true; Layout.fillWidth: true }
                Button { text: qsTr("Refresh"); onClicked: nativeDevices.refresh() }
            }
            Rectangle { Layout.fillWidth: true; height: 1; color: "#1f4e58" }

            DeviceSection { heading: qsTr("MIDI inputs");   entries: nativeDevices.midiInputs }
            DeviceSection { heading: qsTr("MIDI outputs");  entries: nativeDevices.midiOutputs }
            DeviceSection { heading: qsTr("Audio outputs"); entries: nativeDevices.audioOutputs }
            DeviceSection { heading: qsTr("Audio inputs");  entries: nativeDevices.audioInputs }
            DeviceSection { heading: qsTr("Video capture"); entries: nativeDevices.videoDevices }

            RowLayout {
                Layout.fillWidth: true
                Label { text: qsTr("Syphon servers"); color: "#a8e6cf"
                        font.bold: true; Layout.fillWidth: true }
                Button { text: qsTr("Poll"); onClicked: nativeDevices.refreshSyphon() }
            }
            DeviceSection { entries: nativeDevices.syphonServers }

            Rectangle { Layout.fillWidth: true; height: 1; color: "#1f4e58" }
            Label { text: qsTr("Displays: %1").arg(nativeDevices.screenCount)
                    color: "#cce7e7" }

            // Clipboard round-trip demo (aaaseed_clipboard / NSPasteboard).
            RowLayout {
                Layout.fillWidth: true
                TextField {
                    id: clipField
                    Layout.fillWidth: true
                    placeholderText: qsTr("text to copy")
                    color: "#cce7e7"
                }
                Button { text: qsTr("Copy");  onClicked: nativeDevices.clipboardCopy(clipField.text) }
                Button { text: qsTr("Paste"); onClicked: clipField.text = nativeDevices.clipboardPaste() }
            }
        }
    }
}
