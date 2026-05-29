// src/ui/qt/qml/panels/ShaderCatalogPanel.qml
//
// c152-B : browse the MSL shader catalog (169 .metal files in
// src/shaders/msl/). Click a row to apply to the currently-selected
// node. Live filter at the top.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Item {
    id: root

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 6

        Label {
            text: qsTr("Shader Catalog (%1)").arg(studio.shaderCatalog.length)
            color: "#a8e6cf"
            font.bold: true
        }
        TextField {
            id: filterField
            Layout.fillWidth: true
            placeholderText: qsTr("Filter...")
        }
        Rectangle { Layout.fillWidth: true; height: 1; color: "#1f4e58" }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: studio.shaderCatalog
            delegate: ItemDelegate {
                width: ListView.view ? ListView.view.width : 0
                visible: filterField.text.length === 0 ||
                         modelData.toLowerCase().includes(filterField.text.toLowerCase())
                height: visible ? implicitHeight : 0
                contentItem: Label {
                    text: modelData
                    color: studio.selectedShader === modelData ? "#f1c40f" : "#cce7e7"
                    font.family: "Menlo, monospace"
                    font.pixelSize: 12
                }
                onClicked: {
                    if (studio.selectedNodeId > 0)
                        studio.applyShaderToSelected(modelData)
                }
            }
            ScrollBar.vertical: ScrollBar {}
        }
        Label {
            Layout.fillWidth: true
            color: "#7a8c8c"
            font.pixelSize: 10
            wrapMode: Label.WordWrap
            text: studio.selectedNodeId > 0
                ? qsTr("Click to bind to selected node.")
                : qsTr("Select a node in the graph to bind a shader.")
        }
    }
}
