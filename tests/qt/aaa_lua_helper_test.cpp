// tests/qt/aaa_lua_helper_test.cpp -- c152-M
//
// Coverage for the LuaHelper Q_OBJECT exposed to QML :
//   - lint()         : returns "" on valid Lua, non-empty on syntax err.
//   - classifyAsset(): maps file extensions to canonical type buckets.
//   - iconForAsset() : returns a single-glyph icon per bucket.

#include "src/ui/qt/aaa_lua_helper.h"

#include <QCoreApplication>
#include <QtTest/QtTest>

class LuaHelperTest : public QObject
{
    Q_OBJECT

private slots:
    void lint_acceptsValidLua();
    void lint_rejectsSyntaxError();
    void lint_emptySrcIsClean();
    void classifyAsset_imageExtensions();
    void classifyAsset_videoExtensions();
    void classifyAsset_meshExtensions();
    void classifyAsset_audioExtensions();
    void classifyAsset_shaderExtensions();
    void classifyAsset_scriptVsProject();
    void classifyAsset_unknownFallsThroughToOther();
    void classifyAsset_caseInsensitive();
    void iconForAsset_oneGlyphPerBucket();
};

void LuaHelperTest::lint_acceptsValidLua()
{
    aaa::ui::qt6::LuaHelper helper;
    QCOMPARE( helper.lint( QStringLiteral( "local x = 1 + 2 print(x)" ) ),
              QString() );
    QCOMPARE( helper.lint( QStringLiteral(
        "local t = {1, 2, 3} for i,v in ipairs(t) do print(i, v) end" ) ),
              QString() );
}

void LuaHelperTest::lint_rejectsSyntaxError()
{
    aaa::ui::qt6::LuaHelper helper;
    // Unmatched parenthesis.
    QString err = helper.lint( QStringLiteral( "print(\"missing close" ) );
    QVERIFY2( !err.isEmpty(), "expected lint error on malformed string" );

    // Reserved-word-as-name.
    err = helper.lint( QStringLiteral( "local end = 1" ) );
    QVERIFY( !err.isEmpty() );
}

void LuaHelperTest::lint_emptySrcIsClean()
{
    aaa::ui::qt6::LuaHelper helper;
    QCOMPARE( helper.lint( QString() ),       QString() );
    QCOMPARE( helper.lint( QStringLiteral( "   \n\n  " ) ), QString() );
}

void LuaHelperTest::classifyAsset_imageExtensions()
{
    aaa::ui::qt6::LuaHelper helper;
    QCOMPARE( helper.classifyAsset( "foo.png" ),  QStringLiteral( "image" ) );
    QCOMPARE( helper.classifyAsset( "bar.jpg" ),  QStringLiteral( "image" ) );
    QCOMPARE( helper.classifyAsset( "baz.JPEG" ), QStringLiteral( "image" ) );
    QCOMPARE( helper.classifyAsset( "deep/path/file.webp" ),
              QStringLiteral( "image" ) );
    QCOMPARE( helper.classifyAsset( "tex.exr" ), QStringLiteral( "image" ) );
}

void LuaHelperTest::classifyAsset_videoExtensions()
{
    aaa::ui::qt6::LuaHelper helper;
    QCOMPARE( helper.classifyAsset( "movie.mp4" ), QStringLiteral( "video" ) );
    QCOMPARE( helper.classifyAsset( "clip.mov" ),  QStringLiteral( "video" ) );
    QCOMPARE( helper.classifyAsset( "anim.webm" ), QStringLiteral( "video" ) );
}

void LuaHelperTest::classifyAsset_meshExtensions()
{
    aaa::ui::qt6::LuaHelper helper;
    QCOMPARE( helper.classifyAsset( "robot.fbx" ),  QStringLiteral( "mesh" ) );
    QCOMPARE( helper.classifyAsset( "scene.gltf" ), QStringLiteral( "mesh" ) );
    QCOMPARE( helper.classifyAsset( "model.obj" ),  QStringLiteral( "mesh" ) );
    QCOMPARE( helper.classifyAsset( "asset.usd" ),  QStringLiteral( "mesh" ) );
}

void LuaHelperTest::classifyAsset_audioExtensions()
{
    aaa::ui::qt6::LuaHelper helper;
    QCOMPARE( helper.classifyAsset( "tone.wav" ), QStringLiteral( "audio" ) );
    QCOMPARE( helper.classifyAsset( "song.mp3" ), QStringLiteral( "audio" ) );
    QCOMPARE( helper.classifyAsset( "voice.flac" ),
              QStringLiteral( "audio" ) );
}

void LuaHelperTest::classifyAsset_shaderExtensions()
{
    aaa::ui::qt6::LuaHelper helper;
    QCOMPARE( helper.classifyAsset( "blur.metal" ), QStringLiteral( "shader" ) );
    QCOMPARE( helper.classifyAsset( "tint.glsl" ),  QStringLiteral( "shader" ) );
    QCOMPARE( helper.classifyAsset( "post.frag" ),  QStringLiteral( "shader" ) );
    QCOMPARE( helper.classifyAsset( "MSL.msl" ),    QStringLiteral( "shader" ) );
}

void LuaHelperTest::classifyAsset_scriptVsProject()
{
    aaa::ui::qt6::LuaHelper helper;
    // Project files override the generic .lua classification.
    QCOMPARE( helper.classifyAsset( "my_thing.aaaproj.lua" ),
              QStringLiteral( "project" ) );
    QCOMPARE( helper.classifyAsset( "foo.lua" ),
              QStringLiteral( "script" ) );
}

void LuaHelperTest::classifyAsset_unknownFallsThroughToOther()
{
    aaa::ui::qt6::LuaHelper helper;
    QCOMPARE( helper.classifyAsset( "" ),            QStringLiteral( "other" ) );
    QCOMPARE( helper.classifyAsset( "no_extension"), QStringLiteral( "other" ) );
    QCOMPARE( helper.classifyAsset( "weirdfile.xyz" ),
              QStringLiteral( "other" ) );
}

void LuaHelperTest::classifyAsset_caseInsensitive()
{
    aaa::ui::qt6::LuaHelper helper;
    QCOMPARE( helper.classifyAsset( "FOO.PNG" ),  QStringLiteral( "image" ) );
    QCOMPARE( helper.classifyAsset( "Bar.JpG" ),  QStringLiteral( "image" ) );
    QCOMPARE( helper.classifyAsset( "thing.FBX" ),QStringLiteral( "mesh" ) );
}

void LuaHelperTest::iconForAsset_oneGlyphPerBucket()
{
    aaa::ui::qt6::LuaHelper helper;
    // Each known bucket maps to a non-empty single glyph.
    QStringList buckets = {
        "img.png", "vid.mp4", "mesh.fbx", "snd.wav",
        "shd.metal", "script.lua", "proj.aaaproj.lua", "weird.zzz"
    };
    for( auto const& p : buckets )
    {
        QString const icon = helper.iconForAsset( p );
        QVERIFY2( !icon.isEmpty(), qPrintable( "no icon for " + p ) );
    }
}

QTEST_GUILESS_MAIN( LuaHelperTest )
#include "aaa_lua_helper_test.moc"
