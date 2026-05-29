// src/ui/qt/qml/panels/InspectorPanel.qml
//
// c152-B : Inspector. Shows fields of the currently-selected node.
// Edits route back into studio.nodeModel via setNodeShader /
// setNodePos. When no node is selected, shows a help message.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 8

        Label {
            text: studio.selectedNodeId > 0
                ? qsTr("Node #%1").arg(studio.selectedNodeId)
                : qsTr("(no selection)")
            color: "#a8e6cf"
            font.bold: true
            font.pixelSize: 14
        }
        Rectangle { Layout.fillWidth: true; height: 1; color: "#1f4e58" }

        Label {
            visible: studio.selectedNodeId === 0
            Layout.fillWidth: true
            wrapMode: Label.WordWrap
            color: "#7a8c8c"
            text: qsTr("Click a node in the graph to edit its fields.")
        }

        // ── Selected-node form ──
        GridLayout {
            Layout.fillWidth: true
            columns: 2
            visible: studio.selectedNodeId > 0
            rowSpacing: 6
            columnSpacing: 8

            Label { text: qsTr("Label");  color: "#7a8c8c" }
            Label {
                text: studio.selectedLabel
                color: "#cce7e7"
                Layout.fillWidth: true
                elide: Label.ElideRight
            }
            Label { text: qsTr("Shader"); color: "#7a8c8c" }
            ComboBox {
                id: shaderCombo
                Layout.fillWidth: true
                model: studio.shaderCatalog
                editable: true
                editText: studio.selectedShader
                onAccepted: studio.applyShaderToSelected(editText)
                onActivated: studio.applyShaderToSelected(currentText)
                Connections {
                    target: studio
                    function onSelectionChanged() {
                        shaderCombo.editText = studio.selectedShader
                    }
                }
            }
        }

        // c152-K : per-node uniforms editor. One "key = value" line each ;
        // floats only. Save on focus-out or Cmd+Return.
        ColumnLayout {
            visible: studio.selectedNodeId > 0
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 4

            RowLayout {
                Layout.fillWidth: true
                Label { text: qsTr("Uniforms (key = value, one per line)")
                        color: "#7a8c8c" }
                Item { Layout.fillWidth: true }
                Label {
                    id: uniformsStatus
                    color: "#33a6b3"
                    font.pixelSize: 11
                    text: ""
                }
            }
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: "#0d1418"
                border.color: "#1f4e58"
                border.width: 1
                radius: 3
                ScrollView {
                    anchors.fill: parent
                    anchors.margins: 4
                    clip: true
                    TextArea {
                        id: uniformsArea
                        font.family: "Menlo, monospace"
                        font.pixelSize: 12
                        color: "#cce7e7"
                        wrapMode: TextEdit.NoWrap
                        selectByMouse: true
                        placeholderText: qsTr("speed = 1.0\nintensity = 0.5")
                        background: Item {}
                        text: studio.selectedNodeId > 0
                            ? studio.nodeModel.nodeUniformsText(studio.selectedNodeId)
                            : ""
                        property bool _suspendUpdates: false
                        Connections {
                            target: studio
                            function onSelectionChanged() {
                                uniformsArea._suspendUpdates = true
                                uniformsArea.text = studio.selectedNodeId > 0
                                    ? studio.nodeModel.nodeUniformsText(studio.selectedNodeId)
                                    : ""
                                uniformsArea._suspendUpdates = false
                                uniformsStatus.text = ""
                            }
                        }
                        Keys.onPressed: (e) => {
                            if (e.key === Qt.Key_Return &&
                                (e.modifiers & Qt.ControlModifier ||
                                 e.modifiers & Qt.MetaModifier))
                            {
                                uniformsArea.commit()
                                e.accepted = true
                            }
                        }
                        onFocusChanged: if (!focus && !_suspendUpdates) commit()
                        function commit() {
                            if (studio.selectedNodeId === 0) return
                            var n = studio.nodeModel.setNodeUniformsText(
                                studio.selectedNodeId, text)
                            uniformsStatus.text = qsTr("%1 uniform%2 saved")
                                .arg(n).arg(n === 1 ? "" : "s")
                        }
                    }
                }
            }
            RowLayout {
                Layout.fillWidth: true
                Item { Layout.fillWidth: true }
                Button {
                    text: qsTr("Clear")
                    onClicked: {
                        studio.nodeModel.clearNodeUniforms(studio.selectedNodeId)
                        uniformsArea.text = ""
                        uniformsStatus.text = qsTr("cleared")
                    }
                }
                Button {
                    text: qsTr("Apply")
                    highlighted: true
                    onClicked: uniformsArea.commit()
                }
            }
        }

        Button {
            visible: studio.selectedNodeId > 0
            Layout.alignment: Qt.AlignRight
            text: qsTr("Delete Node")
            onClicked: {
                studio.nodeModel.removeNode(studio.selectedNodeId)
                studio.selectedNodeId = 0
            }
        }

        Item { Layout.fillHeight: true; visible: studio.selectedNodeId === 0 }
    }
}
