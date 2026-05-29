// src/ui/qt/qml/panels/AssetsPanel.qml

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Item {
    id: root

    FileDialog {
        id: dlg
        title: qsTr("Add Asset")
        onAccepted: studio.addAsset(selectedFile.toString().replace("file://", ""))
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 8
        spacing: 6

        RowLayout {
            Layout.fillWidth: true
            Label {
                text: qsTr("Assets (%1)").arg(studio.assetCount)
                color: "#a8e6cf"
                font.bold: true
            }
            Item { Layout.fillWidth: true }
            Button {
                text: qsTr("+ Add")
                onClicked: dlg.open()
            }
        }
        Label {
            visible: studio.projectPath.length === 0
            color: "#f1c40f"
            text: qsTr("(save the project first to enable Assets)")
            wrapMode: Label.WordWrap
            Layout.fillWidth: true
        }
        Rectangle { Layout.fillWidth: true; height: 1; color: "#1f4e58" }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: studio.assetModel
            delegate: ItemDelegate {
                id: assetRow
                width: ListView.view ? ListView.view.width : 0
                // c152-M : route icon + color through luaHelper's
                // asset classifier so .png shows 🖼, .fbx shows ⬢, etc.
                contentItem: Row {
                    spacing: 8
                    Label {
                        text: luaHelper.iconForAsset(model.path)
                        color: {
                            var t = luaHelper.classifyAsset(model.path)
                            if (t === "image")  return "#33a6b3"
                            if (t === "video")  return "#9b59b6"
                            if (t === "mesh")   return "#f1c40f"
                            if (t === "audio")  return "#e74c3c"
                            if (t === "shader") return "#2ecc71"
                            if (t === "script") return "#a8e6cf"
                            return "#7a8c8c"
                        }
                        font.pixelSize: 14
                        width: 22
                        horizontalAlignment: Text.AlignHCenter
                    }
                    Label { text: model.path; color: "#cce7e7"; elide: Label.ElideMiddle }
                }
                // c152-I : start a drag of this asset's path. Drop on
                // a node in the Node Graph to bind it.
                Drag.active: dragHandle.drag.active
                Drag.dragType: Drag.Automatic
                Drag.supportedActions: Qt.CopyAction
                Drag.mimeData: {
                    "application/x-aaa-asset": model.path
                }
                MouseArea {
                    id: dragHandle
                    anchors.fill: parent
                    drag.target: assetRow
                    onPressed: assetRow.grabToImage(function(r) {
                        assetRow.Drag.imageSource = r.url
                    })
                }
            }
            ScrollBar.vertical: ScrollBar {}
        }
    }
}
