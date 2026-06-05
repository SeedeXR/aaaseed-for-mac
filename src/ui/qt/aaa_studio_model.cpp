// src/ui/qt/aaa_studio_model.cpp
//
// c152-B : implementation of the QML-facing data layer. Reads through
// the public aaa::ui::studio::Studio API only. A 30 fps QTimer refresh
// pulls Studio→QML changes from the (single-threaded) Studio side ;
// QML→Studio mutations are direct method calls and emit their own
// change signals.

#include "src/ui/qt/aaa_studio_model.h"

#include "src/ui/studio/aaa_studio.h"

#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>
#include <QTextStream>
#include <QTimer>
#include <QUrl>
#include <QtGlobal>

#include <algorithm>
#include <filesystem>
#include <utility>

// c156 : Run Script liveness probe (kill(pid, 0)) for the hot-reload-
// instead-of-respawn path.
#include <signal.h>

namespace aaa { namespace ui { namespace qt6 {

// ───── ConsoleListModel ────────────────────────────────────────────────────

ConsoleListModel::ConsoleListModel( aaa::ui::studio::Studio* studio,
                                      QObject* parent )
    : QAbstractListModel( parent )
    , studio_( studio )
{
}

int ConsoleListModel::rowCount( QModelIndex const& parent ) const
{
    if( parent.isValid() ) return 0;
    return cached_size_;
}

QVariant ConsoleListModel::data( QModelIndex const& index, int role ) const
{
    // Read directly from Studio's console_log through the public path :
    // log() writes ; we can't read the deque without a getter. For
    // Phase 0 we expose a Q_INVOKABLE refresh that re-counts and the
    // QML side displays only the count. To get text we'd need a
    // dedicated getter. Adding one is cheap -- keep the model alive
    // and we extend Studio with `console_entries()` once needed.
    Q_UNUSED( index )
    Q_UNUSED( role )
    return QVariant();
}

QHash< int, QByteArray > ConsoleListModel::roleNames() const
{
    QHash< int, QByteArray > r;
    r[ LevelRole ] = "level";
    r[ TextRole  ] = "text";
    r[ FrameRole ] = "frame";
    return r;
}

void ConsoleListModel::clear()
{
    // No public clear() on Studio yet ; reserved for a future call.
}

void ConsoleListModel::refresh()
{
    // No console-size getter on Studio yet -- the count comes from the
    // model's own emission of logLine signals via StudioModel. For now
    // we keep this stub and let QML drive its own buffer.
}

// ───── NodeListModel ───────────────────────────────────────────────────────

NodeListModel::NodeListModel( aaa::ui::studio::Studio* studio, QObject* parent )
    : QAbstractListModel( parent )
    , studio_( studio )
{
}

int NodeListModel::rowCount( QModelIndex const& parent ) const
{
    if( parent.isValid() ) return 0;
    return cached_size_;
}

QVariant NodeListModel::data( QModelIndex const& index, int role ) const
{
    if( !studio_ ) return QVariant();
    auto const& nodes = studio_->nodes();
    int const row = index.row();
    if( row < 0 || row >= static_cast< int >( nodes.size() ) ) return QVariant();
    auto const& n = nodes[ row ];
    switch( role )
    {
        case IdRole:      return static_cast< int >( n.id );
        case LabelRole:   return QString::fromStdString( n.label );
        case PosXRole:    return n.pos_x;
        case PosYRole:    return n.pos_y;
        case ShaderRole:  return QString::fromStdString( n.shader_name );
        case ScriptRole:  return QString::fromStdString( n.script_path );
        case EnabledRole: return n.enabled;
        default:          return QVariant();
    }
}

QHash< int, QByteArray > NodeListModel::roleNames() const
{
    QHash< int, QByteArray > r;
    r[ IdRole      ] = "nodeId";
    r[ LabelRole   ] = "label";
    r[ PosXRole    ] = "x";
    r[ PosYRole    ] = "y";
    r[ ShaderRole  ] = "shader";
    r[ ScriptRole  ] = "script";
    r[ EnabledRole ] = "enabled";
    return r;
}

namespace
{
    // c152-O : grab the owning StudioModel via QObject::parent() so
    // sibling list models can push undo snapshots before mutating.
    void snap( QObject* self )
    {
        if( auto* sm = qobject_cast< StudioModel* >( self->parent() ) )
            sm->pushUndoSnapshot();
    }
}

int NodeListModel::addNode( QString const& label )
{
    if( !studio_ ) return 0;
    snap( this );
    refresh();   // sync before mutating to keep beginInsertRows happy
    beginInsertRows( QModelIndex(), cached_size_, cached_size_ );
    int const id = static_cast< int >( studio_->add_node(
        label.toStdString() ) );
    ++cached_size_;
    endInsertRows();
    return id;
}

void NodeListModel::removeNode( int id )
{
    if( !studio_ ) return;
    auto const& nodes = studio_->nodes();
    int row = -1;
    for( int i = 0; i < static_cast< int >( nodes.size() ); ++i )
        if( nodes[ i ].id == static_cast< std::uint32_t >( id ) ) { row = i; break; }
    if( row < 0 ) return;
    snap( this );
    beginRemoveRows( QModelIndex(), row, row );
    studio_->remove_node( static_cast< std::uint32_t >( id ) );
    --cached_size_;
    endRemoveRows();
}

void NodeListModel::setNodePos( int id, double x, double y )
{
    if( !studio_ ) return;
    auto* n = studio_->find_node( static_cast< std::uint32_t >( id ) );
    if( !n ) return;
    n->pos_x = static_cast< float >( x );
    n->pos_y = static_cast< float >( y );
    // Notify QML : find the row and emit dataChanged.
    auto const& nodes = studio_->nodes();
    for( int i = 0; i < static_cast< int >( nodes.size() ); ++i )
    {
        if( nodes[ i ].id == static_cast< std::uint32_t >( id ) )
        {
            QModelIndex const idx = index( i, 0 );
            emit dataChanged( idx, idx, { PosXRole, PosYRole } );
            return;
        }
    }
}

void NodeListModel::setNodeShader( int id, QString const& shader )
{
    if( !studio_ ) return;
    auto* n = studio_->find_node( static_cast< std::uint32_t >( id ) );
    if( !n ) return;
    if( n->shader_name != shader.toStdString() ) snap( this );
    n->shader_name = shader.toStdString();
    auto const& nodes = studio_->nodes();
    for( int i = 0; i < static_cast< int >( nodes.size() ); ++i )
    {
        if( nodes[ i ].id == static_cast< std::uint32_t >( id ) )
        {
            QModelIndex const idx = index( i, 0 );
            emit dataChanged( idx, idx, { ShaderRole } );
            return;
        }
    }
}

void NodeListModel::setNodeScript( int id, QString const& script )
{
    if( !studio_ ) return;
    auto* n = studio_->find_node( static_cast< std::uint32_t >( id ) );
    if( !n ) return;
    if( n->script_path != script.toStdString() ) snap( this );
    n->script_path = script.toStdString();
    auto const& nodes = studio_->nodes();
    for( int i = 0; i < static_cast< int >( nodes.size() ); ++i )
    {
        if( nodes[ i ].id == static_cast< std::uint32_t >( id ) )
        {
            QModelIndex const idx = index( i, 0 );
            emit dataChanged( idx, idx, { ScriptRole } );
            return;
        }
    }
}

int NodeListModel::linkNodes( int fromId, int toId )
{
    if( !studio_ ) return 0;
    snap( this );
    auto const linkId = studio_->link_nodes(
        static_cast< std::uint32_t >( fromId ),
        static_cast< std::uint32_t >( toId ) );
    if( linkId > 0 )
    {
        // c152-L : surface to the Console so the user can confirm the
        // wire was created (helps diagnose pin-drag UX issues).
        studio_->log( aaa::ui::studio::ConsoleEntry::INFO,
            "Linked node " + std::to_string( fromId ) +
            " → " + std::to_string( toId ) );
        emit linksChanged();
    }
    return static_cast< int >( linkId );
}

void NodeListModel::unlinkLink( int linkId )
{
    if( !studio_ ) return;
    auto const before = studio_->links().size();
    snap( this );
    studio_->unlink( static_cast< std::uint32_t >( linkId ) );
    if( studio_->links().size() != before )
        emit linksChanged();
}

int NodeListModel::linkCount() const
{
    return studio_ ? static_cast< int >( studio_->links().size() ) : 0;
}

int NodeListModel::linkIdAt( int row ) const
{
    if( !studio_ ) return 0;
    auto const& ls = studio_->links();
    if( row < 0 || row >= static_cast< int >( ls.size() ) ) return 0;
    return static_cast< int >( ls[ row ].id );
}

int NodeListModel::linkFromAt( int row ) const
{
    if( !studio_ ) return 0;
    auto const& ls = studio_->links();
    if( row < 0 || row >= static_cast< int >( ls.size() ) ) return 0;
    return static_cast< int >( ls[ row ].from_pin.node_id );
}

int NodeListModel::linkToAt( int row ) const
{
    if( !studio_ ) return 0;
    auto const& ls = studio_->links();
    if( row < 0 || row >= static_cast< int >( ls.size() ) ) return 0;
    return static_cast< int >( ls[ row ].to_pin.node_id );
}

double NodeListModel::nodeX( int id ) const
{
    if( !studio_ ) return 0;
    auto const* n = studio_->find_node( static_cast< std::uint32_t >( id ) );
    return n ? n->pos_x : 0.0;
}

double NodeListModel::nodeY( int id ) const
{
    if( !studio_ ) return 0;
    auto const* n = studio_->find_node( static_cast< std::uint32_t >( id ) );
    return n ? n->pos_y : 0.0;
}

int NodeListModel::nodeIdAt( int row ) const
{
    if( !studio_ ) return 0;
    auto const& ns = studio_->nodes();
    if( row < 0 || row >= static_cast< int >( ns.size() ) ) return 0;
    return static_cast< int >( ns[ row ].id );
}

// c152-K : per-node uniforms editor. Surface format is one entry per
// line as "key = value" ; blank lines + lines without "=" are skipped.
// Both reads and writes are stable-sorted alphabetically so the editor
// doesn't flicker when the underlying unordered_map rehashes.
QString NodeListModel::nodeUniformsText( int id ) const
{
    if( !studio_ ) return {};
    auto const* n = studio_->find_node( static_cast< std::uint32_t >( id ) );
    if( !n ) return {};
    QStringList keys;
    keys.reserve( static_cast< int >( n->uniforms.size() ) );
    for( auto const& kv : n->uniforms )
        keys.append( QString::fromStdString( kv.first ) );
    keys.sort();
    QStringList lines;
    for( auto const& k : keys )
    {
        auto it = n->uniforms.find( k.toStdString() );
        if( it == n->uniforms.end() ) continue;
        lines.append( QStringLiteral( "%1 = %2" )
            .arg( k ).arg( static_cast< double >( it->second ) ) );
    }
    return lines.join( QStringLiteral( "\n" ) );
}

int NodeListModel::setNodeUniformsText( int id, QString const& text )
{
    if( !studio_ ) return 0;
    snap( this );
    studio_->clear_node_uniforms( static_cast< std::uint32_t >( id ) );
    int accepted = 0;
    auto const lines = text.split( QChar( '\n' ), Qt::SkipEmptyParts );
    for( auto const& raw : lines )
    {
        QString const line = raw.trimmed();
        if( line.isEmpty() || line.startsWith( '#' ) ) continue;
        int const eq = line.indexOf( '=' );
        if( eq <= 0 ) continue;
        QString const key = line.left( eq ).trimmed();
        if( key.isEmpty() ) continue;
        bool ok = false;
        double const value = line.mid( eq + 1 ).trimmed().toDouble( &ok );
        if( !ok ) continue;
        studio_->set_node_uniform(
            static_cast< std::uint32_t >( id ),
            key.toStdString(), static_cast< float >( value ) );
        ++accepted;
    }
    QModelIndex const top = index( 0, 0 );
    QModelIndex const bot = index( cached_size_ - 1, 0 );
    if( top.isValid() && bot.isValid() )
        emit dataChanged( top, bot );
    return accepted;
}

void NodeListModel::clearNodeUniforms( int id )
{
    if( !studio_ ) return;
    snap( this );
    studio_->clear_node_uniforms( static_cast< std::uint32_t >( id ) );
}

void NodeListModel::refresh()
{
    if( !studio_ ) return;
    int const live = static_cast< int >( studio_->nodes().size() );
    if( live == cached_size_ ) return;
    beginResetModel();
    cached_size_ = live;
    endResetModel();
}

// ───── AssetListModel ──────────────────────────────────────────────────────

AssetListModel::AssetListModel( aaa::ui::studio::Studio* studio, QObject* parent )
    : QAbstractListModel( parent )
    , studio_( studio )
{
}

int AssetListModel::rowCount( QModelIndex const& parent ) const
{
    if( parent.isValid() ) return 0;
    return cached_size_;
}

QVariant AssetListModel::data( QModelIndex const& index, int role ) const
{
    if( !studio_ ) return QVariant();
    auto const& assets = studio_->assets();
    int const row = index.row();
    if( row < 0 || row >= static_cast< int >( assets.size() ) ) return QVariant();
    if( role == PathRole )
        return QString::fromStdString( assets[ row ] );
    return QVariant();
}

QHash< int, QByteArray > AssetListModel::roleNames() const
{
    QHash< int, QByteArray > r;
    r[ PathRole ] = "path";
    return r;
}

bool AssetListModel::addAssetFromFile( QString const& path )
{
    if( !studio_ ) return false;
    refresh();
    beginInsertRows( QModelIndex(), cached_size_, cached_size_ );
    std::string const rel = studio_->add_asset_from_file( path.toStdString() );
    bool const ok = !rel.empty();
    if( ok ) ++cached_size_;
    endInsertRows();
    return ok;
}

void AssetListModel::refresh()
{
    if( !studio_ ) return;
    int const live = static_cast< int >( studio_->assets().size() );
    if( live == cached_size_ ) return;
    beginResetModel();
    cached_size_ = live;
    endResetModel();
}

// ───── StudioModel ─────────────────────────────────────────────────────────

StudioModel::StudioModel( aaa::ui::studio::Studio* studio, QObject* parent )
    : QObject( parent )
    , studio_( studio )
    , console_( std::make_unique< ConsoleListModel >( studio, this ) )
    , nodes_  ( std::make_unique< NodeListModel    >( studio, this ) )
    , assets_ ( std::make_unique< AssetListModel   >( studio, this ) )
{
    // Pre-populate the shader catalog from disk so the QML side can
    // show it instantly. The path matches the ImGui Shader Catalog
    // panel (AAA_SHADERS_MSL_DIR compile define).
    namespace fs = std::filesystem;
    fs::path const root( "/Users/alexmkwizu/Documents/SoftwareProjects/aaaseed-mac/aaaseed-for-mac/src/shaders/msl" );
    std::error_code ec;
    if( fs::is_directory( root, ec ) )
    {
        std::vector< std::string > tmp;
        for( auto const& entry : fs::directory_iterator( root, ec ) )
            if( entry.is_regular_file() && entry.path().extension() == ".metal" )
                tmp.push_back( entry.path().stem().string() );
        std::sort( tmp.begin(), tmp.end() );
        for( auto const& s : tmp )
            shader_catalog_ << QString::fromStdString( s );
    }

    refresh_timer_ = new QTimer( this );
    refresh_timer_->setInterval( 33 );   // ~30 fps
    QObject::connect( refresh_timer_, &QTimer::timeout,
                      this, &StudioModel::refresh );
    refresh_timer_->start();

    // c152-F : load recent-projects list from QSettings ; drop entries
    // whose files no longer exist.
    loadRecents();
}

void StudioModel::loadRecents()
{
    QSettings s;
    QStringList raw = s.value( QStringLiteral( "recentProjects" ) ).toStringList();
    recent_projects_.clear();
    for( auto const& p : raw )
        if( QFile::exists( p ) ) recent_projects_ << p;
    if( recent_projects_ != raw ) saveRecents();
    emit recentsChanged();
}

void StudioModel::saveRecents()
{
    QSettings s;
    s.setValue( QStringLiteral( "recentProjects" ), recent_projects_ );
}

void StudioModel::pushRecent( QString const& path )
{
    if( path.isEmpty() ) return;
    recent_projects_.removeAll( path );
    recent_projects_.prepend( path );
    while( recent_projects_.size() > 8 ) recent_projects_.removeLast();
    saveRecents();
    emit recentsChanged();
}

void StudioModel::clearRecents()
{
    recent_projects_.clear();
    saveRecents();
    emit recentsChanged();
}

void StudioModel::removeRecent( QString const& path )
{
    int const removed = recent_projects_.removeAll( path );
    if( removed > 0 )
    {
        saveRecents();
        emit recentsChanged();
    }
}

bool StudioModel::deleteProjectFile( QString const& path )
{
    bool removed = false;
    QFileInfo fi( path );
    if( fi.exists() && fi.isFile() )
        removed = QFile::remove( path );
    // Always drop from recents whether the file existed or not.
    removeRecent( path );
    // Best-effort : also drop the sidecar Assets/ folder if it was
    // created next to the project. Only remove if empty -- safer.
    QDir parent = fi.dir();
    QDir assets( parent.filePath( QStringLiteral( "Assets" ) ) );
    if( assets.exists() && assets.isEmpty() )
        assets.removeRecursively();
    if( removed )
        emit logLine( /*INFO=*/0,
            QStringLiteral( "Deleted project : %1" ).arg( path ) );
    return removed;
}

qlonglong StudioModel::projectMTimeMs( QString const& path ) const
{
    QFileInfo fi( path );
    if( !fi.exists() ) return 0;
    return fi.lastModified().toMSecsSinceEpoch();
}

QString StudioModel::projectBasename( QString const& path ) const
{
    QString name = QFileInfo( path ).fileName();
    // Strip ".aaaproj.lua" suffix for display.
    if( name.endsWith( QStringLiteral( ".aaaproj.lua" ),
                        Qt::CaseInsensitive ) )
        name.chop( QStringLiteral( ".aaaproj.lua" ).size() );
    else if( name.endsWith( QStringLiteral( ".lua" ),
                             Qt::CaseInsensitive ) )
        name.chop( 4 );
    return name;
}

bool StudioModel::projectExists( QString const& path ) const
{
    return QFileInfo( path ).exists();
}

// ── c152-N : workspace named slots ───────────────────────────────────────
//
// Storage layout :
//   workspace/<panelId>/floating   = bool       (live state)
//   workspace/<panelId>/collapsed  = bool
//   workspace/<panelId>/...
//   workspaces/<name>/<panelId>/floating = bool (named snapshot)
//   workspaces/<name>/<panelId>/...
//
// Save copies every key under "workspace/*" → "workspaces/<name>/*".
// Load reverses it. Reset clears "workspace/*".

QStringList StudioModel::listWorkspaces() const
{
    QSettings s;
    s.beginGroup( QStringLiteral( "workspaces" ) );
    QStringList const groups = s.childGroups();
    s.endGroup();
    return groups;
}

bool StudioModel::saveWorkspace( QString const& name )
{
    if( name.trimmed().isEmpty() ) return false;
    QSettings s;
    // Snapshot every key under workspace/.
    s.beginGroup( QStringLiteral( "workspace" ) );
    QStringList const keys = s.allKeys();
    QMap< QString, QVariant > snapshot;
    for( auto const& k : keys )
        snapshot.insert( k, s.value( k ) );
    s.endGroup();
    // Wipe the target slot, then write fresh.
    s.beginGroup( QStringLiteral( "workspaces/" ) + name );
    s.remove( QString() );
    for( auto it = snapshot.constBegin(); it != snapshot.constEnd(); ++it )
        s.setValue( it.key(), it.value() );
    s.endGroup();
    emit logLine( /*INFO=*/0,
        QStringLiteral( "Saved workspace : %1 (%2 keys)" )
            .arg( name ).arg( snapshot.size() ) );
    return true;
}

bool StudioModel::loadWorkspace( QString const& name )
{
    if( name.trimmed().isEmpty() ) return false;
    QSettings s;
    s.beginGroup( QStringLiteral( "workspaces/" ) + name );
    QStringList const keys = s.allKeys();
    if( keys.isEmpty() )
    {
        s.endGroup();
        emit logLine( /*WARN=*/1,
            QStringLiteral( "Workspace not found : %1" ).arg( name ) );
        return false;
    }
    QMap< QString, QVariant > snapshot;
    for( auto const& k : keys )
        snapshot.insert( k, s.value( k ) );
    s.endGroup();
    // Overwrite live workspace.
    s.beginGroup( QStringLiteral( "workspace" ) );
    s.remove( QString() );
    for( auto it = snapshot.constBegin(); it != snapshot.constEnd(); ++it )
        s.setValue( it.key(), it.value() );
    s.endGroup();
    s.sync();
    emit logLine( /*INFO=*/0,
        QStringLiteral( "Loaded workspace : %1 (re-open the app to see all changes)" )
            .arg( name ) );
    return true;
}

bool StudioModel::deleteWorkspace( QString const& name )
{
    if( name.trimmed().isEmpty() ) return false;
    QSettings s;
    s.beginGroup( QStringLiteral( "workspaces/" ) + name );
    s.remove( QString() );
    s.endGroup();
    return true;
}

void StudioModel::resetWorkspace()
{
    QSettings s;
    s.beginGroup( QStringLiteral( "workspace" ) );
    s.remove( QString() );
    s.endGroup();
    s.sync();
    // c152-O : also nudge every PanelHost + Main.qml to reset live
    // in-memory state. Without this signal, the QSettings clear
    // wouldn't take effect until next launch.
    emit workspaceResetRequested();
    emit logLine( /*INFO=*/0,
        QStringLiteral( "Workspace reset to defaults." ) );
}

// ── c152-O : Undo / Redo ──────────────────────────────────────────────

namespace { constexpr int kMaxUndoStack = 64; }

void StudioModel::pushUndoSnapshot()
{
    if( !studio_ || suppress_snapshot_ ) return;
    QString const blob = serializeNow();
    undo_stack_.append( blob );
    if( undo_stack_.size() > kMaxUndoStack )
        undo_stack_.removeFirst();
    // A new mutation invalidates any redo history.
    redo_stack_.clear();
    emit undoRedoStateChanged();
}

QString StudioModel::serializeNow() const
{
    if( !studio_ ) return QString();
    return QString::fromStdString( studio_->serialize_to_string() );
}

bool StudioModel::restoreFrom( QString const& blob )
{
    if( !studio_ ) return false;
    suppress_snapshot_ = true;
    bool const ok = studio_->load_from_string( blob.toStdString() );
    suppress_snapshot_ = false;
    if( ok )
    {
        // Force list models + Q_PROPERTYs to refresh.
        if( console_ ) console_->refresh();
        if( nodes_   ) nodes_->refresh();
        if( assets_  ) assets_->refresh();
        if( nodes_   ) emit nodes_->linksChanged();
        emit editorTextChanged();
        emit projectChanged();
        emit selectionChanged();
    }
    return ok;
}

void StudioModel::undo()
{
    if( undo_stack_.isEmpty() ) return;
    QString const current = serializeNow();
    QString const target  = undo_stack_.takeLast();
    if( restoreFrom( target ) )
    {
        redo_stack_.append( current );
        if( redo_stack_.size() > kMaxUndoStack )
            redo_stack_.removeFirst();
        emit logLine( /*INFO=*/0, QStringLiteral( "Undo." ) );
    }
    emit undoRedoStateChanged();
}

void StudioModel::redo()
{
    if( redo_stack_.isEmpty() ) return;
    QString const current = serializeNow();
    QString const target  = redo_stack_.takeLast();
    if( restoreFrom( target ) )
    {
        undo_stack_.append( current );
        if( undo_stack_.size() > kMaxUndoStack )
            undo_stack_.removeFirst();
        emit logLine( /*INFO=*/0, QStringLiteral( "Redo." ) );
    }
    emit undoRedoStateChanged();
}

bool StudioModel::canUndo() const { return !undo_stack_.isEmpty(); }
bool StudioModel::canRedo() const { return !redo_stack_.isEmpty(); }

bool StudioModel::openSample()
{
    // Probe both bundled (production) + dev-tree paths.
    QString const self = QCoreApplication::applicationDirPath();
    QStringList const candidates = {
        self + QStringLiteral( "/../Resources/sample/starter.aaaproj.lua" ),
        QStringLiteral( "/Users/alexmkwizu/Documents/SoftwareProjects/aaaseed-mac/aaaseed-for-mac/bundle/macos/sample/starter.aaaproj.lua" ),
    };
    for( auto const& p : candidates )
    {
        if( QFile::exists( p ) )
        {
            // Copy to a writable scratch location ; the user can Save As
            // to keep their version.
            QString const scratch = QDir::tempPath() +
                QStringLiteral( "/aaaseed_starter.aaaproj.lua" );
            QFile::remove( scratch );
            if( !QFile::copy( p, scratch ) )
            {
                emit logLine( /*ERR=*/2,
                    QStringLiteral( "Open Sample : copy failed." ) );
                return false;
            }
            openProject( scratch );
            return true;
        }
    }
    emit logLine( /*ERR=*/2,
        QStringLiteral( "Open Sample : starter.aaaproj.lua not found." ) );
    return false;
}

StudioModel::~StudioModel() = default;

QString StudioModel::projectName()  const { return studio_ ? QString::fromStdString( studio_->project_name() )  : QString(); }
QString StudioModel::projectPath()  const { return studio_ ? QString::fromStdString( studio_->project_path() )  : QString(); }
bool    StudioModel::projectDirty() const { return studio_ ? studio_->project_dirty() : false; }
int     StudioModel::nodeCount()    const { return studio_ ? static_cast< int >( studio_->nodes().size() )  : 0; }
int     StudioModel::linkCount()    const { return studio_ ? static_cast< int >( studio_->links().size() )  : 0; }
int     StudioModel::assetCount()   const { return studio_ ? static_cast< int >( studio_->assets().size() ) : 0; }
QString StudioModel::editorText()   const { return studio_ ? QString::fromStdString( studio_->editor_text() ) : QString(); }

QString StudioModel::selectedLabel() const
{
    if( !studio_ || selected_node_id_ == 0 ) return QString();
    auto const* n = studio_->find_node( static_cast< std::uint32_t >( selected_node_id_ ) );
    return n ? QString::fromStdString( n->label ) : QString();
}

QString StudioModel::selectedShader() const
{
    if( !studio_ || selected_node_id_ == 0 ) return QString();
    auto const* n = studio_->find_node( static_cast< std::uint32_t >( selected_node_id_ ) );
    return n ? QString::fromStdString( n->shader_name ) : QString();
}

void StudioModel::setEditorText( QString const& text )
{
    if( !studio_ ) return;
    studio_->set_editor_text( text.toStdString() );
    emit editorTextChanged();
}

void StudioModel::setSelectedNodeId( int id )
{
    if( selected_node_id_ == id ) return;
    selected_node_id_ = id;
    emit selectionChanged();
}

void StudioModel::newProject()
{
    if( !studio_ ) return;
    studio_->new_project();
    selected_node_id_ = 0;
    emit projectChanged();
    emit editorTextChanged();
    emit selectionChanged();
    emit logLine( /*INFO=*/0, QStringLiteral( "New project." ) );
}

void StudioModel::openProject( QString const& path )
{
    if( !studio_ || path.isEmpty() ) return;
    if( studio_->open_project( path.toStdString() ) )
    {
        selected_node_id_ = 0;
        pushRecent( path );
        emit projectChanged();
        emit editorTextChanged();
        emit selectionChanged();
        emit logLine( 0, QStringLiteral( "Opened : %1" ).arg( path ) );
    }
    else
    {
        emit logLine( /*ERR=*/2, QStringLiteral( "Open failed : %1" ).arg( path ) );
    }
}

void StudioModel::saveProject()
{
    if( !studio_ ) return;
    auto const path = studio_->project_path();
    if( path.empty() )
    {
        emit logLine( /*WARN=*/1, QStringLiteral( "No path set ; use Save As..." ) );
        return;
    }
    if( studio_->save_project_to( path ) )
    {
        emit projectChanged();
        emit logLine( 0, QStringLiteral( "Saved : %1" )
            .arg( QString::fromStdString( path ) ) );
    }
}

void StudioModel::saveProjectAs( QString const& path )
{
    if( !studio_ || path.isEmpty() ) return;
    if( studio_->save_project_to( path.toStdString() ) )
    {
        pushRecent( path );
        emit projectChanged();
        emit logLine( 0, QStringLiteral( "Saved As : %1" ).arg( path ) );
    }
}

bool StudioModel::addAsset( QString const& src_path )
{
    if( !assets_ ) return false;
    bool const ok = assets_->addAssetFromFile( src_path );
    if( ok )
    {
        emit projectChanged();
        emit logLine( 0, QStringLiteral( "Imported asset : %1" ).arg( src_path ) );
    }
    return ok;
}

void StudioModel::removeAsset( int row )
{
    // No public remove-by-row on Studio yet ; this is a v2 polish item.
    // (Removing in QML doesn't yet round-trip to the on-disk Assets/
    // dir.) Logged as TODO in ui/notes/qt6-migration.md.
    Q_UNUSED( row )
    emit logLine( /*WARN=*/1,
        QStringLiteral( "Asset remove from QML pending : v2." ) );
}

void StudioModel::playProject()
{
    if( !studio_ ) return;
    // Auto-save dirty projects before playing so the runtime sees the
    // latest edits. Untitled projects must be Save As'd first.
    auto path = studio_->project_path();
    if( path.empty() )
    {
        emit logLine( /*WARN=*/1,
            QStringLiteral( "Save the project before pressing Play." ) );
        return;
    }
    if( studio_->project_dirty() )
        studio_->save_project_to( path );

    // Locate the runtime binary. Two cases :
    //   - Production : we're inside AAASeed-Studio.app/Contents/MacOS/
    //     and the runtime ships next to us as aaaseed_runtime.app inside
    //     the same .app's Contents/Resources/runtime/ (bundled by
    //     ship-qt-dmg.sh).
    //   - Dev : we're at out/<preset>/bin/AAASeed-Studio and the
    //     runtime is the sibling out/<preset>/bin/aaaseed_runtime.app.
    QString const runtime_path = locateRuntimeBinary();
    if( runtime_path.isEmpty() )
    {
        emit logLine( /*ERR=*/2,
            QStringLiteral( "aaaseed_runtime not found ; reinstall the .app." ) );
        return;
    }

    // Spawn detached so killing the Studio doesn't kill the playback.
    QStringList argv;
    argv << QStringLiteral( "--project" )
         << QString::fromStdString( path );
    qint64 pid = 0;
    if( QProcess::startDetached( runtime_path, argv, QString(), &pid ) )
    {
        emit logLine( /*INFO=*/0,
            QStringLiteral( "Play : runtime spawned, pid=%1" ).arg( pid ) );
    }
    else
    {
        emit logLine( /*ERR=*/2,
            QStringLiteral( "Play : failed to spawn %1" ).arg( runtime_path ) );
    }
}

// c152-E : transient lua_State opened just to syntax-check the
// editor buffer. No bindings registered, no scripts run -- only
// luaL_loadstring + error position parsed back to the QML console.
// Forward decl since aaa_studio.cpp owns the lua includes.
namespace
{
    // Minimal local lua header re-include for this single function.
    // Avoids dragging the include into the .h.
}

// We need the lua headers locally for this slot ; pull them in.
extern "C" {
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
}

void StudioModel::runScript()
{
    if( !studio_ ) return;

    // c152-E : Studio has no engine Runner attached (the Runner lives
    // in aaaseed_runtime, spawned by Play). The closest "Run" can do
    // here is syntax-check the buffer + report errors to the Console.
    // For full execution, the user presses Play (Cmd+P).
    QString const text = QString::fromStdString( studio_->editor_text() );
    if( text.isEmpty() )
    {
        emit logLine( /*WARN=*/1,
            QStringLiteral( "Run : editor is empty." ) );
        return;
    }

    QByteArray const utf8 = text.toUtf8();
    lua_State* L = luaL_newstate();
    if( L == nullptr )
    {
        emit logLine( /*ERR=*/2,
            QStringLiteral( "Run : luaL_newstate() failed." ) );
        return;
    }
    luaL_openlibs( L );

    int const rc = luaL_loadbuffer( L, utf8.constData(),
                                     static_cast< std::size_t >( utf8.size() ),
                                     "editor" );
    if( rc != 0 )
    {
        QString err = QString::fromUtf8( lua_isstring( L, -1 )
            ? lua_tostring( L, -1 )
            : "(no message)" );
        emit logLine( /*ERR=*/2,
            QStringLiteral( "Syntax error : %1" ).arg( err ) );
        lua_close( L );
        return;
    }
    // Don't pcall the chunk -- running it standalone (no engine
    // bindings) would error on first aaa.* call. Just confirm the
    // syntax is OK ; full execution happens in the runtime on Play.
    lua_close( L );
    emit logLine( /*LUA=*/3,
        QStringLiteral( "Syntax OK (%1 bytes)." ).arg( utf8.size() ) );

    // c155 : the classic "my script didn't work" trap -- a pure library
    // module (no `aaa.on_frame`) loads in the runtime and renders
    // NOTHING. Heuristic string check ; cheap, no false negatives for
    // real MEUs (they must literally contain "aaa.on_frame" to render).
    if( !text.contains( QStringLiteral( "aaa.on_frame" ) ) )
    {
        emit logLine( /*WARN=*/1,
            QStringLiteral( "Hint : this script defines no aaa.on_frame(w, h, frame) "
                            "-- it will load but render nothing. See "
                            "Samples/perlin_noise for the minimal pattern." ) );
    }

    // c156 : actually RUN the buffer in the engine. Write it to a stable
    // temp .lua and dispatch (c158 : preview hook first, then the
    // spawn / hot-reload path).
    QString const tmp = writeEditorRunScript();
    if( tmp.isEmpty() )
    {
        emit logLine( /*ERR=*/2,
            QStringLiteral( "Run : could not write the editor script to a "
                            "temp file." ) );
        return;
    }
    dispatchRunFile( tmp );
}

// c158 : shared run-dispatch tail. Preview hook -> live-runtime
// hot-reload -> spawn. `tmp` is the already-written script file.
void StudioModel::dispatchRunFile( QString const& tmp )
{
    // Intuitive route : main() installs a hook that hands the script to
    // the embedded Engine Preview when Display mode == "intuitive".
    if( intuitive_run_hook_ && intuitive_run_hook_( tmp ) )
    {
        emit logLine( /*INFO=*/0,
            QStringLiteral( "Run : script routed to the Engine Preview "
                            "(Display menu : Intuitive)." ) );
        return;
    }

    if( editor_run_pid_ > 0 && ::kill( pid_t( editor_run_pid_ ), 0 ) == 0 )
    {
        emit logLine( /*INFO=*/0,
            QStringLiteral( "Run : script updated -- the running engine "
                            "window hot-reloads it." ) );
        return;
    }

    if( !run_spawn_enabled_ )
    {
        emit logLine( /*INFO=*/0,
            QStringLiteral( "Run : spawn disabled (test mode) ; script "
                            "written to %1." ).arg( tmp ) );
        return;
    }

    QString const runtime = locateRuntimeBinary();
    if( runtime.isEmpty() )
    {
        emit logLine( /*ERR=*/2,
            QStringLiteral( "Run : aaaseed_runtime not found next to the "
                            "Studio -- build the runtime target (dev) or "
                            "reinstall the .app. Script written to %1." )
                .arg( tmp ) );
        return;
    }

    qint64 pid = 0;
    if( QProcess::startDetached( runtime,
            { QStringLiteral( "--script" ), tmp }, QString(), &pid ) )
    {
        editor_run_pid_ = pid;
        emit logLine( /*INFO=*/0,
            QStringLiteral( "Run : engine runtime spawned (pid=%1) with the "
                            "editor script. Save/Cmd+R again to hot-reload." )
                .arg( pid ) );
    }
    else
    {
        emit logLine( /*ERR=*/2,
            QStringLiteral( "Run : failed to spawn %1" ).arg( runtime ) );
    }
}

// c158 : generate a MEU from the node graph. Every node with a shader
// gets a 3-second time slice ; numeric-keyed node uniforms (keys "0"..
// "15") are applied as float slots. "" when no node carries a shader.
QString StudioModel::generateGraphScript() const
{
    if( !studio_ ) return QString();

    QString entries;
    int     count = 0;
    for( auto const& node : studio_->nodes() )
    {
        if( node.shader_name.empty() ) continue;
        ++count;

        QString uniforms;
        for( auto const& kv : node.uniforms )
        {
            bool ok = false;
            int const slot = QString::fromStdString( kv.first ).toInt( &ok );
            if( ok && slot >= 0 && slot < 16 )
                uniforms += QStringLiteral( "[%1]=%2," )
                                .arg( slot )
                                .arg( double( kv.second ) );
        }

        QString label = QString::fromStdString( node.label );
        label.replace( QStringLiteral( "\\" ), QStringLiteral( "\\\\" ) )
             .replace( QStringLiteral( "\"" ), QStringLiteral( "\\\"" ) );

        entries += QStringLiteral(
            "  { name = \"%1\", shader = \"%2\", u = { %3 } },\n" )
                .arg( label )
                .arg( QString::fromStdString( node.shader_name ) )
                .arg( uniforms );
    }
    if( count == 0 ) return QString();

    //	NB on '%' : QString::arg only substitutes '%' followed by a DIGIT,
    //	so Lua's modulo ("% #nodes") and string.format markers ("%d", "%s")
    //	pass through untouched ; only the explicit %1 / %2 are replaced.
    return QStringLiteral(
        "-- generated from the node graph (c158) ; %1 node(s)\n"
        "local nodes = {\n%2}\n"
        "local per = 3.0\n"
        "function aaa.on_frame(w, h, frame)\n"
        "  local t = aaa.time()\n"
        "  local i = (math.floor(t / per) % #nodes) + 1\n"
        "  local n = nodes[i]\n"
        "  aaa.use_shader(n.shader)\n"
        "  aaa.set_uniform_int(0, 1)\n"
        "  aaa.set_uniform_float(0, t - math.floor(t))\n"
        "  aaa.set_uniform_vec4(0, 1, 1, 1, 1)\n"
        "  aaa.set_uniform_vec4(1, 0, 0, 0, 0)\n"
        "  for slot, v in pairs(n.u) do aaa.set_uniform_float(slot, v) end\n"
        "  aaa.draw_hud_text(string.format(\"graph %d/%d : %s (%s)\","
        " i, #nodes, n.name, n.shader))\n"
        "  aaa.draw_fullscreen_quad()\n"
        "end\n"
        "aaa.log(\"graph script : \" .. tostring(#nodes) .. \" node(s)\")\n" )
            .arg( count )
            .arg( entries );
}

bool StudioModel::runGraph()
{
    QString const script = generateGraphScript();
    if( script.isEmpty() )
    {
        emit logLine( /*WARN=*/1,
            QStringLiteral( "Run Graph : no node carries a shader. Assign "
                            "one in the Inspector / Shader Catalog first." ) );
        return false;
    }

    QString const path =
        QDir::temp().filePath( QStringLiteral( "aaaseed_editor_run.lua" ) );
    QFile f( path );
    if( !f.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
    {
        emit logLine( /*ERR=*/2,
            QStringLiteral( "Run Graph : cannot write %1" ).arg( path ) );
        return false;
    }
    f.write( script.toUtf8() );
    f.close();

    emit logLine( /*INFO=*/0,
        QStringLiteral( "Run Graph : generated a MEU from the graph "
                        "(3 s per node)." ) );
    dispatchRunFile( path );
    return true;
}

// c157 : drag-and-drop a .lua onto the Code Editor panel.
bool StudioModel::loadEditorFromFile( QString const& path_or_url )
{
    if( !studio_ ) return false;

    QString path = path_or_url;
    if( path.startsWith( QStringLiteral( "file://" ) ) )
        path = QUrl( path ).toLocalFile();

    if( !path.endsWith( QStringLiteral( ".lua" ), Qt::CaseInsensitive ) )
    {
        emit logLine( /*WARN=*/1,
            QStringLiteral( "Drop : only .lua files load into the editor "
                            "(got %1)." ).arg( path ) );
        return false;
    }

    QFile f( path );
    if( !f.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        emit logLine( /*ERR=*/2,
            QStringLiteral( "Drop : cannot read %1" ).arg( path ) );
        return false;
    }

    setEditorText( QString::fromUtf8( f.readAll() ) );
    emit logLine( /*INFO=*/0,
        QStringLiteral( "Drop : loaded %1 into the editor. Press Cmd+R "
                        "to run it." ).arg( path ) );
    return true;
}

// c156 : write the editor buffer to the stable Run Script temp path.
QString StudioModel::writeEditorRunScript()
{
    if( !studio_ ) return QString();
    QString const path =
        QDir::temp().filePath( QStringLiteral( "aaaseed_editor_run.lua" ) );
    QFile f( path );
    if( !f.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
        return QString();
    QByteArray const utf8 =
        QString::fromStdString( studio_->editor_text() ).toUtf8();
    if( f.write( utf8 ) != utf8.size() )
    {
        f.close();
        return QString();
    }
    f.close();
    return path;
}

// c156 : shared runtime locator (Run Script + Play).
QString StudioModel::locateRuntimeBinary()
{
    QString const self = QCoreApplication::applicationDirPath();
    QStringList const candidates = {
        // Bundled inside the Studio .app
        self + QStringLiteral( "/../Resources/runtime/aaaseed_runtime.app/Contents/MacOS/aaaseed_runtime" ),
        // Dev-tree sibling
        self + QStringLiteral( "/aaaseed_runtime.app/Contents/MacOS/aaaseed_runtime" ),
        self + QStringLiteral( "/aaaseed_runtime" ),
    };
    for( auto const& c : candidates )
    {
        QFileInfo fi( c );
        if( fi.exists() && fi.isExecutable() )
            return c;
    }
    return QString();
}

void StudioModel::applyShaderToSelected( QString const& shader_name )
{
    if( !nodes_ || selected_node_id_ == 0 ) return;
    nodes_->setNodeShader( selected_node_id_, shader_name );
    emit selectionChanged();
}

void StudioModel::resetCamera()
{
    if( !studio_ ) return;
    studio_->set_camera( aaa::ui::studio::CameraState{} );
}

void StudioModel::refresh()
{
    // Pull-mode tick. Refresh the list models ; if their sizes
    // changed, dataChanged signals fire and QML rebinds. Nothing
    // touches the Studio for collections that didn't change.
    if( console_ ) console_->refresh();
    if( nodes_   ) nodes_->refresh();
    if( assets_  ) assets_->refresh();

    // Re-emit projectChanged so dirty / count properties stay live
    // without manual touches. Cheap signal -- QML bindings only fire
    // when the read-value actually changes.
    emit projectChanged();
}

} } } // namespace aaa::ui::qt6
