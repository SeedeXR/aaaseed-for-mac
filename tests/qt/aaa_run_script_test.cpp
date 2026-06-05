// tests/qt/aaa_run_script_test.cpp
//
// c156 : unit coverage for the Run Script (Cmd+R) editor-execution path.
// Guiless ; spawning is DISABLED via setRunSpawnEnabled(false) so the test
// exercises the full pipeline (syntax check -> no-on_frame hint -> temp-file
// write -> spawn decision) without launching a real engine window.

#include "src/ui/qt/aaa_studio_model.h"
#include "src/ui/studio/aaa_studio.h"

#include <QDir>
#include <QFile>
#include <QSignalSpy>
#include <QStringList>
#include <QUrl>
#include <QtTest/QtTest>

#include <functional>
#include <memory>

// c158 : parse-check the generated graph MEU.
extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

using namespace aaa::ui::qt6;

namespace
{
//	Collect every logLine text emitted during a callable.
QStringList collect_logs( StudioModel& m, std::function< void() > fn )
{
    QStringList out;
    auto conn = QObject::connect( &m, &StudioModel::logLine,
        [&out]( int, QString const& s ) { out << s; } );
    fn();
    QObject::disconnect( conn );
    return out;
}

bool any_contains( QStringList const& l, char const* needle )
{
    for( auto const& s : l )
        if( s.contains( QString::fromUtf8( needle ) ) ) return true;
    return false;
}
}

class RunScriptTest : public QObject
{
    Q_OBJECT

    std::unique_ptr< aaa::ui::studio::Studio > studio_;
    std::unique_ptr< StudioModel >             model_;

private slots:
    void init()
    {
        studio_ = std::make_unique< aaa::ui::studio::Studio >(
            /*backend=*/ nullptr, /*runner=*/ nullptr );
        model_  = std::make_unique< StudioModel >( studio_.get() );
        model_->setRunSpawnEnabled( false );   //	headless seam
    }

    void cleanup()
    {
        model_.reset();
        studio_.reset();
    }

    void write_editor_run_script_round_trips()
    {
        model_->setEditorText( QStringLiteral( "-- hello run\nreturn 42\n" ) );
        QString const path = model_->writeEditorRunScript();
        QVERIFY( !path.isEmpty() );
        QFile f( path );
        QVERIFY( f.open( QIODevice::ReadOnly ) );
        QCOMPARE( QString::fromUtf8( f.readAll() ),
                  QStringLiteral( "-- hello run\nreturn 42\n" ) );
    }

    void empty_buffer_warns_and_stops()
    {
        model_->setEditorText( QString() );
        auto const logs = collect_logs( *model_, [this]{ model_->runScript(); } );
        QVERIFY( any_contains( logs, "editor is empty" ) );
        QVERIFY( !any_contains( logs, "Syntax OK" ) );
    }

    void module_only_script_gets_hint_and_runs()
    {
        //	The user's original perlin.lua shape : valid Lua, no on_frame.
        model_->setEditorText( QStringLiteral(
            "local m = {}\nfunction m.noise(x) return 0 end\nreturn m\n" ) );
        auto const logs = collect_logs( *model_, [this]{ model_->runScript(); } );
        QVERIFY( any_contains( logs, "Syntax OK" ) );
        QVERIFY( any_contains( logs, "defines no aaa.on_frame" ) );   //	the hint
        QVERIFY( any_contains( logs, "spawn disabled (test mode)" ) ); //	run path engaged
    }

    void meu_script_runs_without_hint()
    {
        model_->setEditorText( QStringLiteral(
            "function aaa.on_frame(w, h, f)\n"
            "  aaa.draw_fullscreen_quad()\n"
            "end\n" ) );
        auto const logs = collect_logs( *model_, [this]{ model_->runScript(); } );
        QVERIFY( any_contains( logs, "Syntax OK" ) );
        QVERIFY( !any_contains( logs, "defines no aaa.on_frame" ) );
        QVERIFY( any_contains( logs, "spawn disabled (test mode)" ) );
    }

    void syntax_error_stops_before_run()
    {
        model_->setEditorText( QStringLiteral( "function broken(\n" ) );
        auto const logs = collect_logs( *model_, [this]{ model_->runScript(); } );
        QVERIFY( any_contains( logs, "Syntax error" ) );
        QVERIFY( !any_contains( logs, "spawn disabled" ) );
    }

    void load_editor_from_file_round_trips()
    {
        //	c157 : drag-drop transport. Both a plain path and a file://
        //	URL load the file into the editor buffer.
        QString const tmp = QDir::temp().filePath(
            QStringLiteral( "aaa_drop_test.lua" ) );
        {
            QFile f( tmp );
            QVERIFY( f.open( QIODevice::WriteOnly | QIODevice::Truncate ) );
            f.write( "-- dropped\nfunction aaa.on_frame(w,h,f) end\n" );
        }

        QVERIFY( model_->loadEditorFromFile( tmp ) );
        QVERIFY( model_->editorText().contains( QStringLiteral( "-- dropped" ) ) );

        model_->setEditorText( QString() );
        QVERIFY( model_->loadEditorFromFile(
            QUrl::fromLocalFile( tmp ).toString() ) );
        QVERIFY( model_->editorText().contains( QStringLiteral( "on_frame" ) ) );

        QFile::remove( tmp );
    }

    void load_editor_rejects_non_lua_and_missing()
    {
        auto logs = collect_logs( *model_, [this]{
            QVERIFY( !model_->loadEditorFromFile(
                QStringLiteral( "/tmp/not_a_script.txt" ) ) );
        } );
        QVERIFY( any_contains( logs, "only .lua files" ) );

        logs = collect_logs( *model_, [this]{
            QVERIFY( !model_->loadEditorFromFile(
                QStringLiteral( "/no/such/file.lua" ) ) );
        } );
        QVERIFY( any_contains( logs, "cannot read" ) );
    }

    void intuitive_hook_receives_script_and_skips_spawn()
    {
        //	c158 : when the hook accepts (Display mode == intuitive), the
        //	script goes to the embedded preview and the spawn path never
        //	runs.
        QString captured;
        model_->setIntuitiveRunHook( [&captured]( QString const& p ) {
            captured = p;
            return true;
        } );
        model_->setEditorText( QStringLiteral(
            "function aaa.on_frame(w,h,f) aaa.draw_fullscreen_quad() end\n" ) );
        auto const logs = collect_logs( *model_, [this]{ model_->runScript(); } );

        QVERIFY( !captured.isEmpty() );
        QFile f( captured );
        QVERIFY( f.open( QIODevice::ReadOnly ) );
        QVERIFY( QString::fromUtf8( f.readAll() )
                     .contains( QStringLiteral( "draw_fullscreen_quad" ) ) );
        QVERIFY( any_contains( logs, "routed to the Engine Preview" ) );
        QVERIFY( !any_contains( logs, "spawn disabled" ) );
        model_->setIntuitiveRunHook( {} );   //	clear for other tests
    }

    void run_graph_warns_on_empty_graph()
    {
        auto const logs = collect_logs( *model_, [this]{
            QVERIFY( !model_->runGraph() );
        } );
        QVERIFY( any_contains( logs, "no node carries a shader" ) );
    }

    void run_graph_generates_valid_lua_with_node_shaders()
    {
        //	Two shader-bearing nodes -> the generated MEU must reference
        //	both, define on_frame, and PARSE as valid Lua 5.1 (guards the
        //	QString::arg / Lua '%' interplay).
        int const a = model_->nodeModel()->addNode( QStringLiteral( "NoiseNode" ) );
        int const b = model_->nodeModel()->addNode( QStringLiteral( "BloomNode" ) );
        model_->nodeModel()->setNodeShader( a, QStringLiteral( "aaa_noise_real" ) );
        model_->nodeModel()->setNodeShader( b, QStringLiteral( "aaa_bloom_real" ) );

        QString const script = model_->generateGraphScript();
        QVERIFY( script.contains( QStringLiteral( "aaa_noise_real" ) ) );
        QVERIFY( script.contains( QStringLiteral( "aaa_bloom_real" ) ) );
        QVERIFY( script.contains( QStringLiteral( "aaa.on_frame" ) ) );
        QVERIFY( script.contains( QStringLiteral( "% #nodes" ) ) );   //	modulo intact

        lua_State* L = luaL_newstate();
        QVERIFY( L != nullptr );
        QByteArray const utf8 = script.toUtf8();
        int const rc = luaL_loadbuffer( L, utf8.constData(),
                                        std::size_t( utf8.size() ), "graph" );
        QVERIFY2( rc == 0, lua_isstring( L, -1 ) ? lua_tostring( L, -1 )
                                                 : "(no message)" );
        lua_close( L );

        //	runGraph routes through the same dispatch (hook captures it).
        QString captured;
        model_->setIntuitiveRunHook( [&captured]( QString const& p ) {
            captured = p; return true;
        } );
        QVERIFY( model_->runGraph() );
        QVERIFY( !captured.isEmpty() );
        model_->setIntuitiveRunHook( {} );
    }

    void locate_runtime_is_safe()
    {
        //	Environment-dependent : in the dev tree the sibling runtime
        //	may exist. Either way : no crash, and a non-empty result must
        //	point at a real file.
        QString const p = StudioModel::locateRuntimeBinary();
        if( !p.isEmpty() )
            QVERIFY( QFile::exists( p ) );
    }
};

QTEST_GUILESS_MAIN( RunScriptTest )
#include "aaa_run_script_test.moc"
