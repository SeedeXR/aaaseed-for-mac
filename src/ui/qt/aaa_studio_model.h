// src/ui/qt/aaa_studio_model.h
//
// c152-B : full QML-facing surface for the Qt6 AAASeed Studio.
//
// One central StudioModel Q_OBJECT exposes everything that's NOT a
// collection. Three QAbstractListModels expose the lists (console,
// nodes, assets). All four read/write through the public
// aaa::ui::studio::Studio API ; no direct StudioImpl access. The
// adapter polls the Studio at each `refresh()` invocation -- the
// Studio runs single-threaded so this is race-free.

#pragma once

#include <QAbstractListModel>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QTimer>

#include <memory>
#include <string>

namespace aaa { namespace ui { namespace studio {
    class Studio;
} } }

namespace aaa { namespace ui { namespace qt6 {

// ── Console : log lines ────────────────────────────────────────────────────
class ConsoleListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles { LevelRole = Qt::UserRole + 1, TextRole, FrameRole };

    explicit ConsoleListModel( aaa::ui::studio::Studio* studio,
                                QObject* parent = nullptr );

    int rowCount( QModelIndex const& parent = QModelIndex() ) const override;
    QVariant data( QModelIndex const& index, int role ) const override;
    QHash< int, QByteArray > roleNames() const override;

    Q_INVOKABLE void clear();

public slots:
    void refresh();        // called from the master timer in StudioModel

private:
    aaa::ui::studio::Studio* studio_;
    int cached_size_ = 0;
};

// ── Node graph : nodes list ────────────────────────────────────────────────
class NodeListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles { IdRole = Qt::UserRole + 1, LabelRole, PosXRole, PosYRole,
                 ShaderRole, ScriptRole, EnabledRole };

    explicit NodeListModel( aaa::ui::studio::Studio* studio,
                             QObject* parent = nullptr );

    int rowCount( QModelIndex const& parent = QModelIndex() ) const override;
    QVariant data( QModelIndex const& index, int role ) const override;
    QHash< int, QByteArray > roleNames() const override;

    Q_INVOKABLE int   addNode( QString const& label );
    Q_INVOKABLE void  removeNode( int id );
    Q_INVOKABLE void  setNodePos( int id, double x, double y );
    Q_INVOKABLE void  setNodeShader( int id, QString const& shader );
    Q_INVOKABLE void  setNodeScript( int id, QString const& script );
    // c152-I : pin-drag wiring. Returns link id or 0 on failure.
    Q_INVOKABLE int   linkNodes( int fromId, int toId );
    Q_INVOKABLE void  unlinkLink( int linkId );

    Q_INVOKABLE int     linkCount() const;
    Q_INVOKABLE int     linkIdAt( int row )   const;
    Q_INVOKABLE int     linkFromAt( int row ) const;
    Q_INVOKABLE int     linkToAt( int row )   const;
    Q_INVOKABLE double  nodeX( int id ) const;
    Q_INVOKABLE double  nodeY( int id ) const;
    // c152-L : node-id by row (for QML pin-drag hit-tests).
    Q_INVOKABLE int     nodeIdAt( int row ) const;

signals:
    // c152-L : fired whenever the links collection changes (add or
    // remove). QML hooks this to rebuild its wire-rendering Repeater
    // immediately, without waiting for the 33 ms polling tick.
    void linksChanged();

public:

    // c152-K : per-node uniforms editor.
    Q_INVOKABLE QString  nodeUniformsText( int id ) const;
    Q_INVOKABLE int      setNodeUniformsText( int id, QString const& text );
    Q_INVOKABLE void     clearNodeUniforms( int id );

public slots:
    void refresh();

private:
    aaa::ui::studio::Studio* studio_;
    int cached_size_ = 0;
};

// ── Assets : project-relative paths ────────────────────────────────────────
class AssetListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum Roles { PathRole = Qt::UserRole + 1 };

    explicit AssetListModel( aaa::ui::studio::Studio* studio,
                              QObject* parent = nullptr );

    int rowCount( QModelIndex const& parent = QModelIndex() ) const override;
    QVariant data( QModelIndex const& index, int role ) const override;
    QHash< int, QByteArray > roleNames() const override;

    Q_INVOKABLE bool addAssetFromFile( QString const& path );

public slots:
    void refresh();

private:
    aaa::ui::studio::Studio* studio_;
    int cached_size_ = 0;
};

// ── Central model : everything else ────────────────────────────────────────
class StudioModel : public QObject
{
    Q_OBJECT

    // Project surface
    Q_PROPERTY( QString projectName  READ projectName  NOTIFY projectChanged )
    Q_PROPERTY( QString projectPath  READ projectPath  NOTIFY projectChanged )
    Q_PROPERTY( bool    projectDirty READ projectDirty NOTIFY projectChanged )
    Q_PROPERTY( int     nodeCount    READ nodeCount    NOTIFY projectChanged )
    Q_PROPERTY( int     linkCount    READ linkCount    NOTIFY projectChanged )
    Q_PROPERTY( int     assetCount   READ assetCount   NOTIFY projectChanged )

    // Code editor surface
    Q_PROPERTY( QString editorText READ editorText WRITE setEditorText
                NOTIFY editorTextChanged )

    // Selected node (for the Inspector panel)
    Q_PROPERTY( int     selectedNodeId READ selectedNodeId
                                       WRITE setSelectedNodeId
                                       NOTIFY selectionChanged )
    Q_PROPERTY( QString selectedLabel READ selectedLabel
                                       NOTIFY selectionChanged )
    Q_PROPERTY( QString selectedShader READ selectedShader
                                        NOTIFY selectionChanged )

    // Shader catalog (read-only list of names)
    Q_PROPERTY( QStringList shaderCatalog READ shaderCatalog CONSTANT )

    // c152-F : recents persisted via QSettings ; trimmed to 8 entries,
    // existing files only. Emitted on every newProject/openProject/
    // saveProjectAs so the File menu stays in sync.
    Q_PROPERTY( QStringList recentProjects READ recentProjects NOTIFY recentsChanged )

    // List models exposed to QML
    Q_PROPERTY( aaa::ui::qt6::ConsoleListModel* consoleModel READ consoleModel CONSTANT )
    Q_PROPERTY( aaa::ui::qt6::NodeListModel*    nodeModel    READ nodeModel    CONSTANT )
    Q_PROPERTY( aaa::ui::qt6::AssetListModel*   assetModel   READ assetModel   CONSTANT )

public:
    explicit StudioModel( aaa::ui::studio::Studio* studio,
                           QObject* parent = nullptr );
    ~StudioModel() override;

    // ── Property getters ─────────────────────────────────────────────
    QString projectName()  const;
    QString projectPath()  const;
    bool    projectDirty() const;
    int     nodeCount()    const;
    int     linkCount()    const;
    int     assetCount()   const;
    QString editorText()   const;
    int     selectedNodeId() const { return selected_node_id_; }
    QString selectedLabel()  const;
    QString selectedShader() const;
    QStringList shaderCatalog()   const { return shader_catalog_; }
    QStringList recentProjects()  const { return recent_projects_; }

    ConsoleListModel* consoleModel() const { return console_.get(); }
    NodeListModel*    nodeModel()    const { return nodes_.get();   }
    AssetListModel*   assetModel()   const { return assets_.get();  }

    // ── Property setters ─────────────────────────────────────────────
    void setEditorText( QString const& text );
    void setSelectedNodeId( int id );

public slots:
    // Project I/O (file dialogs are opened from QML's FileDialog component).
    void newProject();
    void openProject( QString const& path );
    void saveProject();
    void saveProjectAs( QString const& path );
    bool addAsset( QString const& src_path );
    void removeAsset( int row );

    // Code editor : Cmd+R / Run button. Writes editorText to a temp
    // .lua and (when a runner is attached) loads it.
    void runScript();

    // c152-D : "Play" -- spawn the bundled aaaseed_runtime binary,
    // pointed at the current project file. Auto-saves first if dirty.
    // Returns immediately ; the runtime runs in its own process.
    void playProject();

    // Apply a shader name to the currently-selected node.
    void applyShaderToSelected( QString const& shader_name );

    // Camera convenience — used by the Camera panel.
    void resetCamera();

    // Studio→QML log relay called periodically by `refresh()`.
    void refresh();

    // c152-F : forget the recent at the given index. Used by File →
    // Open Recent → Clear Menu.
    Q_INVOKABLE void clearRecents();

    // c152-N : drop a single recent (by path) from the list.
    Q_INVOKABLE void removeRecent( QString const& path );

    // c152-N : delete the project file from disk. Used by Home →
    // delete-project. Returns true if the file existed and was
    // removed. Always removes from the recents list.
    Q_INVOKABLE bool deleteProjectFile( QString const& path );

    // c152-N : metadata for a recent's tile : returns last-modified
    // ms-since-epoch (0 if missing) and a basename for display.
    Q_INVOKABLE qlonglong projectMTimeMs( QString const& path ) const;
    Q_INVOKABLE QString   projectBasename( QString const& path ) const;
    Q_INVOKABLE bool      projectExists( QString const& path ) const;

    // c152-I : open the bundled starter project from the .app's
    // Resources/sample/ directory. Returns true on success.
    Q_INVOKABLE bool openSample();

    // c152-N : workspace save/load. Each panel auto-persists its own
    // float/collapse/visible state to QSettings under workspace/<id>/.
    // These methods snapshot/restore the WHOLE workspace under a named
    // slot stored at workspaces/<name>/ — enabling user-named layouts
    // (Editing / Coding / Color, etc.).
    Q_INVOKABLE QStringList listWorkspaces() const;
    Q_INVOKABLE bool        saveWorkspace( QString const& name );
    Q_INVOKABLE bool        loadWorkspace( QString const& name );
    Q_INVOKABLE bool        deleteWorkspace( QString const& name );
    Q_INVOKABLE void        resetWorkspace();   // back to defaults

    // ── c152-O : Undo / Redo ─────────────────────────────────────────
    // Snapshot-based : every mutation calls pushUndoSnapshot() to save
    // the previous state. undo() pops, redo() pushes the inverse.
    Q_INVOKABLE void  undo();
    Q_INVOKABLE void  redo();
    Q_INVOKABLE bool  canUndo() const;
    Q_INVOKABLE bool  canRedo() const;
    // Public so sibling NodeListModel / AssetListModel can call us
    // before mutating their backing Studio state.
    void              pushUndoSnapshot();

signals:
    void projectChanged();
    void editorTextChanged();
    void selectionChanged();
    void logLine( int level, QString const& text );
    void recentsChanged();
    // c152-O : fires when the user asks to reset their workspace
    // layout. QML PanelHosts + Main.qml listen and restore in-memory
    // defaults (resetting QSettings alone doesn't re-bind the live
    // properties).
    void workspaceResetRequested();
    // c152-O : fires whenever the undo/redo stack depth changes ;
    // QML uses it to enable/disable Cmd+Z / Cmd+Shift+Z actions.
    void undoRedoStateChanged();

private:
    void loadRecents();
    void saveRecents();
    void pushRecent( QString const& path );

    // c152-O : snapshot/restore helpers. Snapshots are serialized
    // .aaaproj.lua strings ; capped at 64 entries each (newest
    // dropped on overflow). pushUndoSnapshot is declared public above
    // so the list models can call it.
    QString serializeNow() const;
    bool    restoreFrom( QString const& blob );

    aaa::ui::studio::Studio* studio_   = nullptr;
    int                       selected_node_id_ = 0;
    QStringList               shader_catalog_;
    QStringList               recent_projects_;     // c152-F
    QTimer*                   refresh_timer_ = nullptr;

    // c152-O : undo/redo as a pair of bounded stacks of serialized
    // .aaaproj.lua blobs. A guard avoids re-entrant pushes (undo() /
    // redo() do not themselves push new entries).
    QList< QString > undo_stack_;
    QList< QString > redo_stack_;
    bool             suppress_snapshot_ = false;

    std::unique_ptr< ConsoleListModel > console_;
    std::unique_ptr< NodeListModel    > nodes_;
    std::unique_ptr< AssetListModel   > assets_;
};

} } } // namespace aaa::ui::qt6
