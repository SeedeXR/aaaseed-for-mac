// src/ui/qt/qml/SettingsDialog.qml -- c152-N
//
// Application Preferences dialog. Bound to the `settings` context
// property (SettingsModel). Reachable via the Edit menu and Cmd+,.

import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Dialog {
    id: dlg
    title: qsTr("Preferences")
    modal: true
    width: 520
    height: 460
    standardButtons: Dialog.Close

    background: Rectangle {
        color: "#13202a"
        radius: 8
        border.color: "#1f4e58"
    }

    FolderDialog {
        id: defaultDirDialog
        title: qsTr("Default project folder")
        onAccepted: settings.defaultProjectDir =
            selectedFolder.toString().replace("file://", "")
    }

    contentItem: ScrollView {
        anchors.fill: parent
        clip: true
        ColumnLayout {
            width: dlg.width - 40
            spacing: 16

            // ── Appearance ──────────────────────────────────────────
            GroupBox {
                Layout.fillWidth: true
                title: qsTr("Appearance")
                background: Rectangle {
                    color: "#0e1418"
                    radius: 6
                    border.color: "#1f4e58"
                }
                label: Label {
                    text: parent.title; color: "#a8e6cf"
                    font.bold: true; padding: 6
                }
                GridLayout {
                    anchors.fill: parent
                    columns: 2
                    columnSpacing: 12
                    rowSpacing: 8

                    Label { text: qsTr("Theme"); color: "#cce7e7" }
                    ComboBox {
                        Layout.fillWidth: true
                        model: [ "dark", "light", "system" ]
                        currentIndex: Math.max(0,
                            model.indexOf(settings.theme))
                        onActivated: settings.theme = currentText
                    }
                }
            }

            // ── Editor ──────────────────────────────────────────────
            GroupBox {
                Layout.fillWidth: true
                title: qsTr("Editor")
                background: Rectangle {
                    color: "#0e1418"
                    radius: 6
                    border.color: "#1f4e58"
                }
                label: Label {
                    text: parent.title; color: "#a8e6cf"
                    font.bold: true; padding: 6
                }
                GridLayout {
                    anchors.fill: parent
                    columns: 2
                    columnSpacing: 12
                    rowSpacing: 8

                    Label { text: qsTr("Font size"); color: "#cce7e7" }
                    SpinBox {
                        from: 9; to: 28
                        value: settings.editorFontSize
                        onValueModified: settings.editorFontSize = value
                    }

                    Label { text: qsTr("Tab width"); color: "#cce7e7" }
                    SpinBox {
                        from: 1; to: 8
                        value: settings.editorTabWidth
                        onValueModified: settings.editorTabWidth = value
                    }

                    Label { text: qsTr("Word wrap"); color: "#cce7e7" }
                    Switch {
                        checked: settings.editorWordWrap
                        onToggled: settings.editorWordWrap = checked
                    }

                    Label { text: qsTr("Show line numbers"); color: "#cce7e7" }
                    Switch {
                        checked: settings.editorShowLineNumbers
                        onToggled: settings.editorShowLineNumbers = checked
                    }

                    Label { text: qsTr("Lint debounce (ms)"); color: "#cce7e7" }
                    SpinBox {
                        from: 50; to: 2000; stepSize: 50
                        value: settings.lintDebounceMs
                        onValueModified: settings.lintDebounceMs = value
                    }
                }
            }

            // ── Project ─────────────────────────────────────────────
            GroupBox {
                Layout.fillWidth: true
                title: qsTr("Project")
                background: Rectangle {
                    color: "#0e1418"
                    radius: 6
                    border.color: "#1f4e58"
                }
                label: Label {
                    text: parent.title; color: "#a8e6cf"
                    font.bold: true; padding: 6
                }
                GridLayout {
                    anchors.fill: parent
                    columns: 2
                    columnSpacing: 12
                    rowSpacing: 8

                    Label { text: qsTr("Auto-save"); color: "#cce7e7" }
                    Switch {
                        checked: settings.autoSave
                        onToggled: settings.autoSave = checked
                    }

                    Label { text: qsTr("Auto-save every (s)"); color: "#cce7e7" }
                    SpinBox {
                        enabled: settings.autoSave
                        from: 5; to: 3600; stepSize: 5
                        value: settings.autoSaveIntervalSec
                        onValueModified: settings.autoSaveIntervalSec = value
                    }

                    Label { text: qsTr("Default project folder"); color: "#cce7e7" }
                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 6
                        TextField {
                            Layout.fillWidth: true
                            text: settings.defaultProjectDir
                            onEditingFinished:
                                settings.defaultProjectDir = text
                        }
                        Button {
                            text: qsTr("Browse…")
                            onClicked: defaultDirDialog.open()
                        }
                    }
                }
            }

            // ── Reset ───────────────────────────────────────────────
            RowLayout {
                Layout.fillWidth: true
                Item { Layout.fillWidth: true }
                Button {
                    text: qsTr("Reset to defaults")
                    onClicked: settings.resetToDefaults()
                }
            }

            Item { Layout.preferredHeight: 12 }
        }
    }
}
