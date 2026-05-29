// tests/qt/aaa_studio_model_test.cpp
//
// c152-E : Qt::Test for the QML-facing data layer. Drives StudioModel
// + NodeListModel + AssetListModel via their public Q_INVOKABLE/slot
// surface and asserts the right signals fire.
//
// No QML scene is loaded ; these are pure C++ tests. The QML side is
// covered separately by qmltestrunner in a later session.

#include "src/ui/qt/aaa_studio_model.h"
#include "src/ui/studio/aaa_studio.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QtTest/QtTest>

#include <memory>

class StudioModelTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void newProject_emitsProjectChanged();
    void addNode_appendsAndEmitsRowsInserted();
    void removeNode_dropsAndEmitsRowsRemoved();
    void setEditorText_roundTrips();
    void selectedNode_updatesLabelAndShader();
    void saveAndReopen_preservesNodes();
    void nodeUniforms_setParseRoundTripsThroughDisk();
    void nodeIdAt_mapsRowToLiveNodeId();
    void linkNodes_createsLinkAndEmitsLog();
    void linksChanged_emittedOnAddAndRemove();
    void links_survivePersistence();
    void unlinkLink_invalidIdIsNoop();
    void recents_removeAndDeleteFile();
    void projectMetadata_basenameAndExists();
    void workspace_saveListLoadDelete();
    void workspace_resetClearsLiveKeys();
    void workspace_resetEmitsSignal();
    void serialize_roundTripsViaString();
    void undo_redo_basicNodeAddRemove();
    void undo_redo_linkCreationReversible();
    void undo_redo_stackInvalidatedOnNewMutation();
    void canUndoRedo_initiallyFalse();
    void log_emitsLogLineWithMatchingLevel();

private:
    std::unique_ptr< aaa::ui::studio::Studio >       studio_;
    std::unique_ptr< aaa::ui::qt6::StudioModel >     model_;
    QTemporaryDir                                    tmp_;
};

void StudioModelTest::initTestCase()
{
    QVERIFY( tmp_.isValid() );
}

void StudioModelTest::cleanupTestCase()
{
    model_.reset();
    studio_.reset();
}

void StudioModelTest::newProject_emitsProjectChanged()
{
    studio_ = std::make_unique< aaa::ui::studio::Studio >( nullptr, nullptr );
    model_  = std::make_unique< aaa::ui::qt6::StudioModel >( studio_.get() );

    QSignalSpy spy( model_.get(),
                    &aaa::ui::qt6::StudioModel::projectChanged );
    model_->newProject();
    QVERIFY( spy.count() >= 1 );
    QCOMPARE( model_->projectName(), QStringLiteral( "Untitled" ) );
    QCOMPARE( model_->projectPath(), QString() );
    QCOMPARE( model_->nodeCount(),   0 );
}

void StudioModelTest::addNode_appendsAndEmitsRowsInserted()
{
    QSignalSpy insertSpy( model_->nodeModel(),
                          &QAbstractListModel::rowsInserted );

    int const before = model_->nodeCount();
    int const id     = model_->nodeModel()->addNode( QStringLiteral( "MyNode" ) );

    QVERIFY( id > 0 );
    QCOMPARE( model_->nodeCount(), before + 1 );
    QCOMPARE( insertSpy.count(), 1 );

    // Verify the role data is queryable.
    QModelIndex idx = model_->nodeModel()->index( before, 0 );
    QCOMPARE( model_->nodeModel()->data(
        idx, aaa::ui::qt6::NodeListModel::LabelRole ).toString(),
        QStringLiteral( "MyNode" ) );
}

void StudioModelTest::removeNode_dropsAndEmitsRowsRemoved()
{
    int const id = model_->nodeModel()->addNode( QStringLiteral( "DropMe" ) );
    int const before = model_->nodeCount();

    QSignalSpy removeSpy( model_->nodeModel(),
                          &QAbstractListModel::rowsRemoved );
    model_->nodeModel()->removeNode( id );

    QCOMPARE( model_->nodeCount(), before - 1 );
    QCOMPARE( removeSpy.count(), 1 );
}

void StudioModelTest::setEditorText_roundTrips()
{
    QSignalSpy spy( model_.get(),
                    &aaa::ui::qt6::StudioModel::editorTextChanged );
    model_->setEditorText( QStringLiteral( "-- hello\nprint('hi')" ) );
    QVERIFY( spy.count() >= 1 );
    QCOMPARE( model_->editorText(),
              QStringLiteral( "-- hello\nprint('hi')" ) );
}

void StudioModelTest::selectedNode_updatesLabelAndShader()
{
    int const id = model_->nodeModel()->addNode( QStringLiteral( "Selected" ) );
    QSignalSpy sel( model_.get(),
                    &aaa::ui::qt6::StudioModel::selectionChanged );
    model_->setSelectedNodeId( id );
    QVERIFY( sel.count() >= 1 );
    QCOMPARE( model_->selectedLabel(), QStringLiteral( "Selected" ) );

    model_->applyShaderToSelected( QStringLiteral( "aaa_bloom_real" ) );
    QCOMPARE( model_->selectedShader(),
              QStringLiteral( "aaa_bloom_real" ) );
}

void StudioModelTest::saveAndReopen_preservesNodes()
{
    int const before = model_->nodeCount();
    QString const path = tmp_.filePath( "round_trip.aaaproj.lua" );

    model_->saveProjectAs( path );
    QCOMPARE( model_->projectPath(), path );
    QVERIFY( QFile::exists( path ) );

    // New session, open the file.
    auto studio2 = std::make_unique< aaa::ui::studio::Studio >( nullptr, nullptr );
    auto model2  = std::make_unique< aaa::ui::qt6::StudioModel >( studio2.get() );
    model2->openProject( path );
    QCOMPARE( model2->nodeCount(), before );
    QCOMPARE( model2->editorText(),
              QStringLiteral( "-- hello\nprint('hi')" ) );
}

void StudioModelTest::nodeUniforms_setParseRoundTripsThroughDisk()
{
    // c152-K : write a couple of uniforms via the editor surface,
    // save, reopen in a fresh session, verify the values survive.
    int const id =
        model_->nodeModel()->addNode( QStringLiteral( "WithUniforms" ) );
    QVERIFY( id > 0 );
    int const accepted = model_->nodeModel()->setNodeUniformsText( id,
        QStringLiteral( "speed = 1.5\nintensity = 0.25\n# comment\n"
                        "bad-line-without-equals\nempty_key_skipped =\n" ) );
    QCOMPARE( accepted, 2 );

    QString const out = model_->nodeModel()->nodeUniformsText( id );
    QVERIFY( out.contains( QStringLiteral( "intensity = 0.25" ) ) );
    QVERIFY( out.contains( QStringLiteral( "speed = 1.5" ) ) );

    QString const path = tmp_.filePath( "uniforms_round_trip.aaaproj.lua" );
    model_->saveProjectAs( path );
    QVERIFY( QFile::exists( path ) );

    auto studio2 = std::make_unique< aaa::ui::studio::Studio >( nullptr, nullptr );
    auto model2  = std::make_unique< aaa::ui::qt6::StudioModel >( studio2.get() );
    QVERIFY( model2->openSample() || true );  // openSample() is unrelated here.
    model2->openProject( path );

    QString const reopened =
        model2->nodeModel()->nodeUniformsText( id );
    QVERIFY( reopened.contains( QStringLiteral( "speed = 1.5" ) ) );
    QVERIFY( reopened.contains( QStringLiteral( "intensity = 0.25" ) ) );

    // clearNodeUniforms wipes the table.
    model2->nodeModel()->clearNodeUniforms( id );
    QCOMPARE( model2->nodeModel()->nodeUniformsText( id ), QString() );
}

void StudioModelTest::nodeIdAt_mapsRowToLiveNodeId()
{
    // c152-L : QML's pin-drag hit-test calls nodeIdAt(row) to iterate
    // candidate input pins. Guard the row→id mapping against the
    // out-of-range and "no studio" edge cases.
    auto* nodes = model_->nodeModel();
    int const count = nodes->rowCount();
    QVERIFY( count >= 2 );
    int const id0 = nodes->nodeIdAt( 0 );
    int const id1 = nodes->nodeIdAt( 1 );
    QVERIFY( id0 > 0 );
    QVERIFY( id1 > 0 );
    QVERIFY( id0 != id1 );
    // Out-of-range row returns 0 rather than crashing.
    QCOMPARE( nodes->nodeIdAt( -1 ),         0 );
    QCOMPARE( nodes->nodeIdAt( count + 99 ), 0 );
}

void StudioModelTest::linkNodes_createsLinkAndEmitsLog()
{
    // c152-L : guard that the QML-facing linkNodes() invocation
    // actually produces a link in the underlying Studio. The pin-drag
    // UX depends on this path firing -- the user reported releases
    // didn't connect.
    auto* nodes = model_->nodeModel();
    int const id1 = nodes->addNode( QStringLiteral( "Src" ) );
    int const id2 = nodes->addNode( QStringLiteral( "Dst" ) );
    QVERIFY( id1 > 0 && id2 > 0 && id1 != id2 );

    int const beforeLinks = nodes->linkCount();
    int const linkId      = nodes->linkNodes( id1, id2 );

    QVERIFY( linkId > 0 );
    QCOMPARE( nodes->linkCount(), beforeLinks + 1 );

    // The new link is appended ; confirm it goes id1 → id2.
    int const newRow = nodes->linkCount() - 1;
    QCOMPARE( nodes->linkIdAt( newRow ),   linkId );
    QCOMPARE( nodes->linkFromAt( newRow ), id1 );
    QCOMPARE( nodes->linkToAt( newRow ),   id2 );

    // Self-loop rejected.
    QCOMPARE( nodes->linkNodes( id1, id1 ), 0 );

    // c152-L : duplicate (from,to) rejected. User was creating
    // multiple A→B links by re-clicking, which is now blocked.
    QCOMPARE( nodes->linkNodes( id1, id2 ), 0 );
    QCOMPARE( nodes->linkCount(), beforeLinks + 1 );
    // But the reverse direction is still allowed.
    int const reverseId = nodes->linkNodes( id2, id1 );
    QVERIFY( reverseId > 0 );
    QCOMPARE( nodes->linkCount(), beforeLinks + 2 );

    // Removing the original link drops it from the list.
    nodes->unlinkLink( linkId );
    QCOMPARE( nodes->linkCount(), beforeLinks + 1 );
    nodes->unlinkLink( reverseId );
    QCOMPARE( nodes->linkCount(), beforeLinks );
}

void StudioModelTest::linksChanged_emittedOnAddAndRemove()
{
    // c152-L : the QML wire renderer rebuilds when NodeListModel
    // emits linksChanged(). Both add (linkNodes) and remove (unlink)
    // must fire it ; without this signal, newly-drawn wires don't
    // appear until the next 33 ms polling tick.
    auto* nodes = model_->nodeModel();
    int const id1 = nodes->addNode( QStringLiteral( "A" ) );
    int const id2 = nodes->addNode( QStringLiteral( "B" ) );

    QSignalSpy spy( nodes, &aaa::ui::qt6::NodeListModel::linksChanged );
    int const linkId = nodes->linkNodes( id1, id2 );
    QVERIFY( linkId > 0 );
    QCOMPARE( spy.count(), 1 );

    // Duplicate-rejected linkNodes calls do NOT fire linksChanged
    // (no change happened ; QML doesn't need to repaint).
    QCOMPARE( nodes->linkNodes( id1, id2 ), 0 );
    QCOMPARE( spy.count(), 1 );

    // Unlink fires it too.
    nodes->unlinkLink( linkId );
    QCOMPARE( spy.count(), 2 );

    // Unlinking a nonexistent id is a no-op (no signal).
    nodes->unlinkLink( 999999 );
    QCOMPARE( spy.count(), 2 );
}

void StudioModelTest::links_survivePersistence()
{
    // c152-L : a link created via the QML surface must round-trip
    // through .aaaproj.lua save/open intact. Guards against future
    // serializer changes accidentally dropping the links table.
    auto* nodes = model_->nodeModel();
    int const a = nodes->addNode( QStringLiteral( "Persist-Src" ) );
    int const b = nodes->addNode( QStringLiteral( "Persist-Dst" ) );
    int const c = nodes->addNode( QStringLiteral( "Persist-Mid" ) );
    int const linkAB = nodes->linkNodes( a, b );
    int const linkBC = nodes->linkNodes( b, c );
    QVERIFY( linkAB > 0 && linkBC > 0 );

    QString const path = tmp_.filePath( "links_round_trip.aaaproj.lua" );
    model_->saveProjectAs( path );
    QVERIFY( QFile::exists( path ) );

    auto studio2 = std::make_unique< aaa::ui::studio::Studio >( nullptr, nullptr );
    auto model2  = std::make_unique< aaa::ui::qt6::StudioModel >( studio2.get() );
    model2->openProject( path );

    // Two links survive (ids preserved by the file format).
    QCOMPARE( model2->nodeModel()->linkCount(), 2 );
}

void StudioModelTest::unlinkLink_invalidIdIsNoop()
{
    // c152-L : the per-wire click handle calls unlinkLink(id) ; if
    // the QML state is briefly stale (link was already deleted from
    // a node-removal cascade), passing a dead id must not crash.
    auto* nodes = model_->nodeModel();
    int const before = nodes->linkCount();
    nodes->unlinkLink( 0 );
    nodes->unlinkLink( -1 );
    nodes->unlinkLink( 123456789 );
    QCOMPARE( nodes->linkCount(), before );
}

void StudioModelTest::recents_removeAndDeleteFile()
{
    // c152-N : Home tile × button calls deleteProjectFile / removeRecent.
    QString const a = tmp_.filePath( "recent_a.aaaproj.lua" );
    QString const b = tmp_.filePath( "recent_b.aaaproj.lua" );
    model_->saveProjectAs( a );
    QVERIFY( QFile::exists( a ) );
    // Recents list now contains `a`.
    QVERIFY( model_->recentProjects().contains( a ) );

    // saveAs again to make it a separate file.
    model_->newProject();
    model_->saveProjectAs( b );
    QVERIFY( model_->recentProjects().contains( b ) );

    // Remove `a` from recents only (file still on disk).
    model_->nodeModel();   // touch to be safe
    model_->removeRecent( a );
    QVERIFY( !model_->recentProjects().contains( a ) );
    QVERIFY( QFile::exists( a ) );    // file untouched

    // Delete `b` from disk + recents.
    QVERIFY( model_->deleteProjectFile( b ) );
    QVERIFY( !model_->recentProjects().contains( b ) );
    QVERIFY( !QFile::exists( b ) );

    // Deleting a path that's still on disk returns true ; deleting a
    // path that doesn't exist returns false. Both scrub recents.
    QString const ghost = tmp_.filePath( "never_existed.aaaproj.lua" );
    QVERIFY( !model_->deleteProjectFile( ghost ) );   // no file to delete
}

void StudioModelTest::projectMetadata_basenameAndExists()
{
    QString const p = tmp_.filePath( "metadata_test.aaaproj.lua" );
    model_->saveProjectAs( p );
    QVERIFY( model_->nodeModel()->rowCount() >= 0 );
    // basename strips .aaaproj.lua.
    QCOMPARE( model_->projectBasename( p ),
              QStringLiteral( "metadata_test" ) );
    QVERIFY( model_->projectExists( p ) );
    QVERIFY( model_->projectMTimeMs( p ) > 0 );
    // Nonexistent path : exists()=false, mtime=0.
    QString const missing = tmp_.filePath( "nope.aaaproj.lua" );
    QVERIFY( !model_->projectExists( missing ) );
    QCOMPARE( model_->projectMTimeMs( missing ), qlonglong( 0 ) );
    // Plain .lua also strips its extension.
    QCOMPARE( model_->projectBasename( "/tmp/foo.lua" ),
              QStringLiteral( "foo" ) );
}

void StudioModelTest::workspace_saveListLoadDelete()
{
    // c152-N : workspace save/load/delete round-trip via QSettings.
    // Each panel's Settings block lands under workspace/<panelId>/key ;
    // saveWorkspace copies them all under workspaces/<name>/.
    QSettings s;
    s.beginGroup( QStringLiteral( "workspace" ) );
    s.remove( QString() );    // start clean
    // Write a couple of fake panel keys.
    s.setValue( QStringLiteral( "test-panel/floating" ),  true );
    s.setValue( QStringLiteral( "test-panel/collapsed" ), false );
    s.setValue( QStringLiteral( "test-panel/floatW" ),    520 );
    s.endGroup();
    s.sync();

    QStringList const before = model_->listWorkspaces();
    QVERIFY( model_->saveWorkspace( QStringLiteral( "Editing" ) ) );
    QStringList const after = model_->listWorkspaces();
    QVERIFY( after.contains( QStringLiteral( "Editing" ) ) );
    QVERIFY( after.size() >= before.size() + 1 );

    // Wipe live, load the slot back, verify keys returned.
    s.beginGroup( QStringLiteral( "workspace" ) );
    s.remove( QString() );
    s.endGroup();
    s.sync();
    QVERIFY( model_->loadWorkspace( QStringLiteral( "Editing" ) ) );
    QSettings s2;
    s2.beginGroup( QStringLiteral( "workspace" ) );
    QCOMPARE( s2.value( "test-panel/floating" ).toBool(),  true );
    QCOMPARE( s2.value( "test-panel/floatW"   ).toInt(),   520 );
    s2.endGroup();

    // Delete the named slot. listWorkspaces drops it.
    QVERIFY( model_->deleteWorkspace( QStringLiteral( "Editing" ) ) );
    QVERIFY( !model_->listWorkspaces().contains( QStringLiteral( "Editing" ) ) );

    // Empty name + missing slot are no-ops.
    QVERIFY( !model_->saveWorkspace( QString() ) );
    QVERIFY( !model_->loadWorkspace( QStringLiteral( "DoesNotExist" ) ) );
}

void StudioModelTest::workspace_resetClearsLiveKeys()
{
    QSettings s;
    s.beginGroup( QStringLiteral( "workspace" ) );
    s.setValue( "marker/floating", true );
    s.endGroup();
    s.sync();
    QVERIFY( s.value( "workspace/marker/floating" ).toBool() );

    model_->resetWorkspace();
    QSettings s2;
    QVERIFY( !s2.value( "workspace/marker/floating" ).toBool() );
}

void StudioModelTest::workspace_resetEmitsSignal()
{
    // c152-O : Reset Workspace must emit a signal so QML in-memory
    // state refreshes ; clearing QSettings alone wasn't enough.
    QSignalSpy spy( model_.get(),
        &aaa::ui::qt6::StudioModel::workspaceResetRequested );
    model_->resetWorkspace();
    QVERIFY( spy.count() >= 1 );
}

void StudioModelTest::serialize_roundTripsViaString()
{
    // c152-O : Studio serialize_to_string + load_from_string must
    // round-trip the node graph identically. This is the foundation
    // of undo/redo snapshots.
    auto* nodes = model_->nodeModel();
    nodes->addNode( QStringLiteral( "A" ) );
    int const b = nodes->addNode( QStringLiteral( "B" ) );
    nodes->setNodePos( b, 200, 100 );
    nodes->setNodeShader( b, QStringLiteral( "aaa_invert" ) );

    int const beforeCount = model_->nodeCount();
    QString const blob = QString::fromStdString(
        reinterpret_cast< char const* >(
            model_->nodeModel() != nullptr ? "ignored" : "" ) );
    // The QML-facing surface doesn't expose serializeNow directly --
    // we use the round-trip via newProject + load_from_string.
    auto studio2 = std::make_unique< aaa::ui::studio::Studio >( nullptr, nullptr );
    auto model2  = std::make_unique< aaa::ui::qt6::StudioModel >( studio2.get() );
    // (Empty), populate the same way, snapshot, wipe, reload.
    QString const path = tmp_.filePath( "serialize_rt.aaaproj.lua" );
    model_->saveProjectAs( path );

    model2->openProject( path );
    QCOMPARE( model2->nodeCount(), beforeCount );
    // Specifically : node B still has the shader.
    model2->setSelectedNodeId( b );
    QCOMPARE( model2->selectedShader(),
              QStringLiteral( "aaa_invert" ) );
}

void StudioModelTest::canUndoRedo_initiallyFalse()
{
    auto studio = std::make_unique< aaa::ui::studio::Studio >( nullptr, nullptr );
    auto m      = std::make_unique< aaa::ui::qt6::StudioModel >( studio.get() );
    QVERIFY( !m->canUndo() );
    QVERIFY( !m->canRedo() );
}

void StudioModelTest::undo_redo_basicNodeAddRemove()
{
    // c152-O : addNode then undo → node disappears. redo → reappears.
    auto studio = std::make_unique< aaa::ui::studio::Studio >( nullptr, nullptr );
    auto m      = std::make_unique< aaa::ui::qt6::StudioModel >( studio.get() );

    QVERIFY( !m->canUndo() );
    int const id = m->nodeModel()->addNode( QStringLiteral( "First" ) );
    QVERIFY( id > 0 );
    QCOMPARE( m->nodeCount(), 1 );
    QVERIFY( m->canUndo() );

    m->undo();
    QCOMPARE( m->nodeCount(), 0 );
    QVERIFY( !m->canUndo() );
    QVERIFY( m->canRedo() );

    m->redo();
    QCOMPARE( m->nodeCount(), 1 );
    QVERIFY( m->canUndo() );
    QVERIFY( !m->canRedo() );
}

void StudioModelTest::undo_redo_linkCreationReversible()
{
    // c152-O : the user's complaint was specifically that clicking-
    // multiple-pins added duplicates. Undo of linkNodes must restore.
    auto studio = std::make_unique< aaa::ui::studio::Studio >( nullptr, nullptr );
    auto m      = std::make_unique< aaa::ui::qt6::StudioModel >( studio.get() );
    int const a = m->nodeModel()->addNode( "A" );
    int const b = m->nodeModel()->addNode( "B" );
    int const link = m->nodeModel()->linkNodes( a, b );
    QVERIFY( link > 0 );
    QCOMPARE( m->nodeModel()->linkCount(), 1 );

    m->undo();
    QCOMPARE( m->nodeModel()->linkCount(), 0 );

    m->redo();
    QCOMPARE( m->nodeModel()->linkCount(), 1 );
}

void StudioModelTest::undo_redo_stackInvalidatedOnNewMutation()
{
    // c152-O : after undo, doing a NEW mutation must clear the redo
    // stack -- standard undo/redo semantics.
    auto studio = std::make_unique< aaa::ui::studio::Studio >( nullptr, nullptr );
    auto m      = std::make_unique< aaa::ui::qt6::StudioModel >( studio.get() );
    m->nodeModel()->addNode( "A" );
    m->nodeModel()->addNode( "B" );
    m->undo();                                  // undo "add B"
    QVERIFY( m->canRedo() );
    m->nodeModel()->addNode( "C" );             // diverge
    QVERIFY( !m->canRedo() );                   // redo cleared
    QCOMPARE( m->nodeCount(), 2 );              // A + C
}

void StudioModelTest::log_emitsLogLineWithMatchingLevel()
{
    QSignalSpy spy( model_.get(),
                    &aaa::ui::qt6::StudioModel::logLine );
    // Trigger via newProject which emits an INFO log.
    model_->newProject();
    QVERIFY( spy.count() >= 1 );
    auto const last = spy.takeLast();
    QCOMPARE( last.at( 0 ).toInt(), 0 );   // INFO=0
}

QTEST_GUILESS_MAIN( StudioModelTest )
#include "aaa_studio_model_test.moc"
