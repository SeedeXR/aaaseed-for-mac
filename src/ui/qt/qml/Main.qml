// src/ui/qt/qml/Main.qml
//
// c152-B : real Studio main window.
//
// Layout : ApplicationWindow with native macOS menubar. Body is a
// vertical split : top is a horizontal SplitView with the side
// (Inspector/Assets/Shader Catalog tabs) + central area (NodeGraph
// canvas + Code Editor stacked) ; bottom is the Console.
//
// All panel files live in panels/ and bind to the `studio` context
// property exposed from main().

import QtQuick
import QtQuick.Window
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtCore

import "panels"

ApplicationWindow {
    id: root
    visible: true
    width: 1440
    height: 900
    minimumWidth: 1100
    minimumHeight: 700
    title: "AAASeed Studio -- " + studio.projectName +
           (studio.projectDirty ? " *" : "")
    color: "#11181c"   // GaBuZoMeu dark blue

    // c152-H : the Welcome overlay covers the whole window at startup.
    // We dismiss it the FIRST time the user takes any project action
    // (New / Open / Recent / drag-drop) ; once dismissed it stays
    // dismissed for the session. Otherwise the projectPath===0 + nodeCount===0
    // condition stayed true after `New Project` (the project is empty
    // and unsaved) and the welcome appeared to block the button. Bug
    // reported by the user. The flag is reset on next launch -- showing
    // the welcome again is the right behavior at app start.
    property bool welcomeDismissed: false

    // c152-O : per-panel visibility lives at top level so the View
    // menu can restore any × closed panel. Each PanelHost's
    // closeRequested signal flips the matching showXxx to false ;
    // workspaceResetRequested flips them all back to true.
    property bool showSidePanels:   true
    property bool showNodeGraph:    true
    property bool showEnginePreview:true
    property bool showCodeEditor:   true
    property bool showRightPanels:  true
    property bool showConsole:      true
    property bool showInspector:    true
    property bool showAssets:       true
    property bool showShaders:      true
    property bool showCamera:       true
    property bool showSound:        true
    property bool showTasks:        true

    Settings {
        category: "workspace"
        property alias welcomeDismissed:  root.welcomeDismissed
        property alias showSidePanels:    root.showSidePanels
        property alias showNodeGraph:     root.showNodeGraph
        property alias showEnginePreview: root.showEnginePreview
        property alias showCodeEditor:    root.showCodeEditor
        property alias showRightPanels:   root.showRightPanels
        property alias showConsole:       root.showConsole
        property alias showInspector:     root.showInspector
        property alias showAssets:        root.showAssets
        property alias showShaders:       root.showShaders
        property alias showCamera:        root.showCamera
        property alias showSound:         root.showSound
        property alias showTasks:         root.showTasks
        // Window geometry survives launches.
        property alias winX:      root.x
        property alias winY:      root.y
        property alias winW:      root.width
        property alias winH:      root.height
    }

    // c152-O : Reset Layout → restore all panel visibility too. Wire
    // to studio.workspaceResetRequested so a single signal touches
    // both Main flags and every PanelHost.
    Connections {
        target: studio
        function onWorkspaceResetRequested() {
            root.showSidePanels    = true
            root.showNodeGraph     = true
            root.showEnginePreview = true
            root.showCodeEditor    = true
            root.showRightPanels   = true
            root.showConsole       = true
            root.showInspector     = true
            root.showAssets        = true
            root.showShaders       = true
            root.showCamera        = true
            root.showSound         = true
            root.showTasks         = true
        }
    }

    // ── Native Qt menubar ─────────────────────────────────────────────
    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            Action { text: qsTr("New Project");      shortcut: "Ctrl+N";       onTriggered: { studio.newProject(); root.welcomeDismissed = true } }
            Action { text: qsTr("Open Project...");  shortcut: "Ctrl+O";       onTriggered: openDialog.open() }
            Menu {
                id: recentMenu
                title: qsTr("Open Recent")
                enabled: studio.recentProjects.length > 0
                Instantiator {
                    model: studio.recentProjects
                    delegate: MenuItem {
                        text: modelData
                        onTriggered: { studio.openProject(modelData); root.welcomeDismissed = true }
                    }
                    onObjectAdded: (index, object) => recentMenu.insertItem(index, object)
                    onObjectRemoved: (index, object) => recentMenu.removeItem(object)
                }
                MenuSeparator { visible: studio.recentProjects.length > 0 }
                Action {
                    text: qsTr("Clear Menu")
                    enabled: studio.recentProjects.length > 0
                    onTriggered: studio.clearRecents()
                }
            }
            MenuSeparator {}
            Action { text: qsTr("Save");             shortcut: "Ctrl+S";       enabled: studio.projectPath.length > 0; onTriggered: studio.saveProject() }
            Action { text: qsTr("Save As...");       shortcut: "Ctrl+Shift+S"; onTriggered: saveAsDialog.open() }
            MenuSeparator {}
            Action { text: qsTr("Add Asset...");     onTriggered: addAssetDialog.open() }
            MenuSeparator {}
            Action { text: qsTr("Quit");             shortcut: StandardKey.Quit; onTriggered: Qt.quit() }
        }
        Menu {
            title: qsTr("Edit")
            // c152-O : standard Cmd+Z / Cmd+Shift+Z (Ctrl on Linux/Windows).
            Action {
                text: qsTr("Undo")
                shortcut: StandardKey.Undo
                enabled: studio.canUndo()
                onTriggered: studio.undo()
            }
            Action {
                text: qsTr("Redo")
                shortcut: StandardKey.Redo
                enabled: studio.canRedo()
                onTriggered: studio.redo()
            }
            MenuSeparator {}
            Action { text: qsTr("Add Node");         onTriggered: studio.nodeModel.addNode("Node " + (studio.nodeCount + 1)) }
            Action { text: qsTr("Reset Camera");     onTriggered: studio.resetCamera() }
            MenuSeparator {}
            Action {
                text: qsTr("Preferences…")
                shortcut: "Ctrl+,"
                onTriggered: prefsDialog.open()
            }
        }
        Menu {
            title: qsTr("Run")
            Action { text: qsTr("Run Script");       shortcut: "Ctrl+R"; onTriggered: studio.runScript() }
            Action { text: qsTr("▶ Play Project");   shortcut: "Ctrl+P"; onTriggered: studio.playProject() }
        }
        Menu {
            title: qsTr("View")
            Action {
                text: qsTr("Home")
                shortcut: "Ctrl+H"
                onTriggered: root.welcomeDismissed = false
            }
            Action {
                text: qsTr("Toggle Fullscreen")
                shortcut: "Ctrl+Meta+F"
                onTriggered: root.visibility = (root.visibility === Window.FullScreen)
                    ? Window.Windowed : Window.FullScreen
            }
            MenuSeparator {}
            // c152-O : every panel has its own toggle so the View menu
            // can restore anything closed via the × button.
            MenuItem { text: qsTr("Side Tabs Strip")
                       checkable: true; checked: root.showSidePanels
                       onTriggered: root.showSidePanels = !root.showSidePanels }
            MenuItem { text: qsTr("  Inspector")
                       checkable: true; checked: root.showInspector
                       onTriggered: root.showInspector = !root.showInspector }
            MenuItem { text: qsTr("  Assets")
                       checkable: true; checked: root.showAssets
                       onTriggered: root.showAssets = !root.showAssets }
            MenuItem { text: qsTr("  Shader Catalog")
                       checkable: true; checked: root.showShaders
                       onTriggered: root.showShaders = !root.showShaders }
            MenuItem { text: qsTr("Node Graph")
                       checkable: true; checked: root.showNodeGraph
                       onTriggered: root.showNodeGraph = !root.showNodeGraph }
            MenuItem { text: qsTr("Engine Preview")
                       checkable: true; checked: root.showEnginePreview
                       onTriggered: root.showEnginePreview = !root.showEnginePreview }
            MenuItem { text: qsTr("Code Editor")
                       checkable: true; checked: root.showCodeEditor
                       onTriggered: root.showCodeEditor = !root.showCodeEditor }
            MenuItem { text: qsTr("Right Tabs Strip")
                       checkable: true; checked: root.showRightPanels
                       onTriggered: root.showRightPanels = !root.showRightPanels }
            MenuItem { text: qsTr("  Camera")
                       checkable: true; checked: root.showCamera
                       onTriggered: root.showCamera = !root.showCamera }
            MenuItem { text: qsTr("  Sound")
                       checkable: true; checked: root.showSound
                       onTriggered: root.showSound = !root.showSound }
            MenuItem { text: qsTr("  Tasks")
                       checkable: true; checked: root.showTasks
                       onTriggered: root.showTasks = !root.showTasks }
            MenuItem { text: qsTr("Console")
                       checkable: true; checked: root.showConsole
                       onTriggered: root.showConsole = !root.showConsole }
            MenuSeparator {}
            Action {
                text: qsTr("Reset Workspace")
                onTriggered: studio.resetWorkspace()
            }
            Action {
                text: qsTr("Show All Panels")
                onTriggered: {
                    root.showSidePanels=true; root.showInspector=true
                    root.showAssets=true; root.showShaders=true
                    root.showNodeGraph=true; root.showEnginePreview=true
                    root.showCodeEditor=true; root.showRightPanels=true
                    root.showCamera=true; root.showSound=true
                    root.showTasks=true; root.showConsole=true
                }
            }
        }
        // c152-N : Window menu — save & load named workspaces.
        Menu {
            id: windowMenu
            title: qsTr("Window")
            Action {
                text: qsTr("Save Workspace As…")
                onTriggered: saveWorkspaceDialog.open()
            }
            Menu {
                id: loadWorkspaceMenu
                title: qsTr("Load Workspace")
                enabled: studio.listWorkspaces().length > 0
                Instantiator {
                    model: studio.listWorkspaces()
                    delegate: MenuItem {
                        text: modelData
                        onTriggered: studio.loadWorkspace(modelData)
                    }
                    onObjectAdded: (index, object) => loadWorkspaceMenu.insertItem(index, object)
                    onObjectRemoved: (index, object) => loadWorkspaceMenu.removeItem(object)
                }
            }
            Menu {
                id: deleteWorkspaceMenu
                title: qsTr("Delete Workspace")
                enabled: studio.listWorkspaces().length > 0
                Instantiator {
                    model: studio.listWorkspaces()
                    delegate: MenuItem {
                        text: modelData
                        onTriggered: studio.deleteWorkspace(modelData)
                    }
                    onObjectAdded: (index, object) => deleteWorkspaceMenu.insertItem(index, object)
                    onObjectRemoved: (index, object) => deleteWorkspaceMenu.removeItem(object)
                }
            }
            MenuSeparator {}
            Action {
                text: qsTr("Reset Layout to Defaults")
                onTriggered: studio.resetWorkspace()
            }
        }
    }

    // c152-O : modal name prompt for Save Workspace As… The TextField
    // was previously a child of Dialog (not contentItem) so it never
    // rendered. Now properly inside a ColumnLayout that IS the
    // contentItem. Pressing Enter or clicking OK saves.
    Dialog {
        id: saveWorkspaceDialog
        title: qsTr("Save Workspace")
        modal: true
        anchors.centerIn: parent
        width: 360
        standardButtons: Dialog.Save | Dialog.Cancel
        // Auto-focus the field when the dialog opens.
        onOpened: workspaceNameField.forceActiveFocus()
        contentItem: ColumnLayout {
            spacing: 8
            Label {
                Layout.fillWidth: true
                text: qsTr("Name this workspace layout so you can reload it later.")
                wrapMode: Label.Wrap
                color: "#cce7e7"
            }
            TextField {
                id: workspaceNameField
                Layout.fillWidth: true
                Layout.minimumWidth: 280
                placeholderText: qsTr("Workspace name (e.g. Editing)")
                onAccepted: saveWorkspaceDialog.accept()   // Enter = OK
            }
            Label {
                Layout.fillWidth: true
                visible: workspaceNameField.text.trim().length === 0
                text: qsTr("Required.")
                color: "#e74c3c"
                font.pixelSize: 11
            }
        }
        onAccepted: {
            var n = workspaceNameField.text.trim()
            if (n.length > 0) studio.saveWorkspace(n)
            workspaceNameField.text = ""
        }
    }

    // ── Native file dialogs ───────────────────────────────────────────
    FileDialog {
        id: openDialog
        title: qsTr("Open AAASeed Project")
        nameFilters: [ "AAASeed projects (*.lua)" ]
        onAccepted: {
            studio.openProject(selectedFile.toString().replace("file://", ""))
            root.welcomeDismissed = true
        }
    }
    FileDialog {
        id: saveAsDialog
        title: qsTr("Save AAASeed Project As")
        nameFilters: [ "AAASeed projects (*.lua)" ]
        fileMode: FileDialog.SaveFile
        defaultSuffix: "lua"
        onAccepted: studio.saveProjectAs(selectedFile.toString().replace("file://", ""))
    }
    FileDialog {
        id: addAssetDialog
        title: qsTr("Add Asset")
        onAccepted: studio.addAsset(selectedFile.toString().replace("file://", ""))
    }

    // c152-F : drag-and-drop ; accept .aaaproj.lua URIs from Finder and
    // open them via studio.openProject. The DropArea fills the window
    // but doesn't intercept mouse events for the children.
    DropArea {
        anchors.fill: parent
        keys: [ "text/uri-list" ]
        onDropped: (drop) => {
            for (var i = 0; i < drop.urls.length; ++i) {
                var p = drop.urls[i].toString().replace("file://", "")
                if (p.toLowerCase().endsWith(".lua") ||
                    p.toLowerCase().endsWith(".aaaproj"))
                {
                    studio.openProject(p)
                    root.welcomeDismissed = true
                    drop.acceptProposedAction()
                    return
                }
            }
        }
    }

    // ── Body ──────────────────────────────────────────────────────────
    SplitView {
        anchors.fill: parent
        orientation: Qt.Vertical

        // Top : horizontal split (side tabs | central area)
        SplitView {
            id: topSplit
            SplitView.fillHeight: true
            orientation: Qt.Horizontal

            // ── LEFT : Side tabs (Inspector / Assets / Shader Catalog) ──
            // c152-M : rounded dark frame ; current-tab content also
            // wrapped in PanelHost so it can be detached individually.
            Item {
                visible: root.showSidePanels
                SplitView.preferredWidth: 320
                SplitView.minimumWidth: 220

                Rectangle {
                    anchors.fill: parent
                    color: "#0e1418"
                    radius: 8
                    border.color: "#1f4e58"
                    border.width: 1
                }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 1
                    spacing: 0
                    TabBar {
                        id: leftTabs
                        Layout.fillWidth: true
                        TabButton { text: qsTr("Inspector")      }
                        TabButton { text: qsTr("Assets")         }
                        TabButton { text: qsTr("Shaders")        }
                    }
                    StackLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        currentIndex: leftTabs.currentIndex
                        PanelHost {
                            panelId: "inspector"
                            title:   qsTr("Inspector")
                            visible: root.showInspector
                            contentComponent: Component { InspectorPanel { } }
                            onCloseRequested: root.showInspector = false
                        }
                        PanelHost {
                            panelId: "assets"
                            title:   qsTr("Assets")
                            visible: root.showAssets
                            contentComponent: Component { AssetsPanel { } }
                            onCloseRequested: root.showAssets = false
                        }
                        PanelHost {
                            panelId: "shader-catalog"
                            title:   qsTr("Shader Catalog")
                            visible: root.showShaders
                            contentComponent: Component { ShaderCatalogPanel { } }
                            onCloseRequested: root.showShaders = false
                        }
                    }
                }
            }

            // ── CENTRE : NodeGraph + Engine Preview above Code Editor ──
            SplitView {
                id: centreSplit
                SplitView.fillWidth: true
                orientation: Qt.Vertical

                // Top : horizontal split between Node Graph (left) and
                // Engine Preview (right). User can drag the divider to
                // hide the preview entirely.
                SplitView {
                    SplitView.fillHeight: true
                    SplitView.minimumHeight: 200
                    orientation: Qt.Horizontal

                    Item {
                        visible: root.showNodeGraph
                        SplitView.fillWidth: true
                        SplitView.minimumWidth: 240
                        PanelHost {
                            anchors.fill: parent
                            panelId: "node-graph"
                            title:   qsTr("Node Graph")
                            contentComponent: Component { NodeGraphPanel { } }
                            onCloseRequested: root.showNodeGraph = false
                        }
                    }
                    Item {
                        visible: root.showEnginePreview
                        SplitView.preferredWidth: 480
                        SplitView.minimumWidth: 120
                        PanelHost {
                            anchors.fill: parent
                            panelId: "engine-preview"
                            title:   qsTr("Engine Preview")
                            contentComponent: Component { EnginePreviewPanel { } }
                            onCloseRequested: root.showEnginePreview = false
                        }
                    }
                }
                Item {
                    visible: root.showCodeEditor
                    SplitView.preferredHeight: 240
                    SplitView.minimumHeight: 120
                    PanelHost {
                        anchors.fill: parent
                        panelId: "code-editor"
                        title:   qsTr("Code Editor")
                        contentComponent: Component { CodeEditorPanel { } }
                        onCloseRequested: root.showCodeEditor = false
                    }
                }
            }

            // ── RIGHT : Camera / Sound / Tasks ─────────────────────────
            Item {
                visible: root.showRightPanels
                SplitView.preferredWidth: 320
                SplitView.minimumWidth: 240

                Rectangle {
                    anchors.fill: parent
                    color: "#0e1418"
                    radius: 8
                    border.color: "#1f4e58"
                    border.width: 1
                }

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 1
                    spacing: 0
                    TabBar {
                        id: rightTabs
                        Layout.fillWidth: true
                        TabButton { text: qsTr("Camera")  }
                        TabButton { text: qsTr("Sound")   }
                        TabButton { text: qsTr("Tasks")   }
                    }
                    StackLayout {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        currentIndex: rightTabs.currentIndex
                        PanelHost {
                            panelId: "camera"
                            title:   qsTr("Camera")
                            visible: root.showCamera
                            contentComponent: Component { CameraPanel { } }
                            onCloseRequested: root.showCamera = false
                        }
                        PanelHost {
                            panelId: "sound"
                            title:   qsTr("Sound")
                            visible: root.showSound
                            contentComponent: Component { SoundPanel { } }
                            onCloseRequested: root.showSound = false
                        }
                        PanelHost {
                            panelId: "tasks"
                            title:   qsTr("Tasks")
                            visible: root.showTasks
                            contentComponent: Component { BinaryManagerPanel { } }
                            onCloseRequested: root.showTasks = false
                        }
                    }
                }
            }
        }

        // Bottom : Console
        Item {
            visible: root.showConsole
            SplitView.preferredHeight: 180
            SplitView.minimumHeight: 60
            PanelHost {
                anchors.fill: parent
                panelId: "console"
                title:   qsTr("Console")
                contentComponent: Component { ConsolePanel { } }
                onCloseRequested: root.showConsole = false
            }
        }
    }

    // c152-N : Home / project gallery -- replaces the old Welcome
    // overlay. Visible when no project has been actioned this session.
    HomeScreen {
        id: home
        anchors.fill: parent
        visible: !root.welcomeDismissed
        onNewProjectRequested: { studio.newProject(); root.welcomeDismissed = true }
        onOpenProjectDialogRequested: openDialog.open()
        onOpenSampleRequested: {
            if (studio.openSample()) root.welcomeDismissed = true
        }
        onOpenProject: (path) => {
            studio.openProject(path)
            root.welcomeDismissed = true
        }
    }

    // c152-N : Preferences dialog. Cmd+, opens it (wired in Edit menu).
    SettingsDialog { id: prefsDialog }

    // ── Status bar (footer) ───────────────────────────────────────────
    footer: Rectangle {
        height: 28
        color: "#11181c"
        RowLayout {
            anchors.fill: parent
            anchors.leftMargin: 12
            anchors.rightMargin: 12
            spacing: 16
            Label { text: studio.projectName + (studio.projectDirty ? " *" : "")
                    color: "#a8e6cf" }
            Label { text: studio.projectPath.length > 0 ? studio.projectPath
                                                         : "(unsaved)"
                    color: "#7a8c8c"; Layout.fillWidth: true; elide: Text.ElideMiddle }
            Label { text: "nodes : " + studio.nodeCount;  color: "#f1c40f" }
            Label { text: "links : " + studio.linkCount;  color: "#f1c40f" }
            Label { text: "assets : " + studio.assetCount; color: "#f1c40f" }
        }
    }
}
