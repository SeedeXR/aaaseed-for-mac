// src/ui/qt/aaa_lua_helper.cpp -- c152-M

#include "src/ui/qt/aaa_lua_helper.h"

#include <QFileInfo>
#include <QRegularExpression>
#include <QRegularExpressionMatchIterator>
#include <QTextDocument>

extern "C" {
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
}

namespace aaa { namespace ui { namespace qt6 {

// ── LuaSyntaxHighlighter ──────────────────────────────────────────────────

namespace {

constexpr char const* kLuaKeywords[] =
{
    "and", "break", "do", "else", "elseif", "end", "false", "for",
    "function", "goto", "if", "in", "local", "nil", "not", "or",
    "repeat", "return", "then", "true", "until", "while",
};

constexpr char const* kLuaStdNames[] =
{
    // Common stdlib + a few AAASeed-specific globals worth highlighting.
    "print", "ipairs", "pairs", "next", "tostring", "tonumber",
    "select", "type", "pcall", "xpcall", "error", "assert",
    "require", "rawget", "rawset", "rawequal", "setmetatable", "getmetatable",
    "math", "string", "table", "io", "os",
    "self",
};

} // namespace

LuaSyntaxHighlighter::LuaSyntaxHighlighter( QTextDocument* parent )
    : QSyntaxHighlighter( parent )
{
    // Color palette : dark theme, distinct hues per token category.
    QTextCharFormat keywordFmt;
    keywordFmt.setForeground( QColor( "#9b59b6" ) );    // violet
    keywordFmt.setFontWeight( QFont::Bold );
    QString kwPattern = "\\b(?:";
    for( int i = 0; i < int( sizeof( kLuaKeywords ) / sizeof( kLuaKeywords[ 0 ] ) ); ++i )
    {
        if( i ) kwPattern += "|";
        kwPattern += QString::fromLatin1( kLuaKeywords[ i ] );
    }
    kwPattern += ")\\b";
    rules_.append( { kwPattern, keywordFmt, true } );

    QTextCharFormat stdFmt;
    stdFmt.setForeground( QColor( "#33a6b3" ) );        // teal
    QString stdPattern = "\\b(?:";
    for( int i = 0; i < int( sizeof( kLuaStdNames ) / sizeof( kLuaStdNames[ 0 ] ) ); ++i )
    {
        if( i ) stdPattern += "|";
        stdPattern += QString::fromLatin1( kLuaStdNames[ i ] );
    }
    stdPattern += ")\\b";
    rules_.append( { stdPattern, stdFmt, true } );

    // Numbers : integer + float + hex.
    QTextCharFormat numFmt;
    numFmt.setForeground( QColor( "#f39c12" ) );        // amber
    rules_.append(
        { "\\b(?:0x[0-9a-fA-F]+|\\d+\\.?\\d*(?:[eE][+-]?\\d+)?)\\b",
          numFmt, true } );

    // Function call : name followed by `(`.
    QTextCharFormat fnFmt;
    fnFmt.setForeground( QColor( "#f1c40f" ) );         // yellow
    rules_.append( { "\\b([A-Za-z_][A-Za-z_0-9]*)\\s*(?=\\()", fnFmt, true } );

    // Local variable definitions : `local foo = ...` highlights `foo`.
    QTextCharFormat varFmt;
    varFmt.setForeground( QColor( "#e74c3c" ) );        // red-orange
    rules_.append( { "\\blocal\\s+([A-Za-z_][A-Za-z_0-9]*)", varFmt, true } );

    // Strings : "double" and 'single' quoted, with backslash escapes.
    QTextCharFormat strFmt;
    strFmt.setForeground( QColor( "#2ecc71" ) );        // green
    rules_.append( { "\"(?:\\\\.|[^\"\\\\])*\"", strFmt, true } );
    rules_.append( { "'(?:\\\\.|[^'\\\\])*'",   strFmt, true } );

    // Long-bracket string `[[ ... ]]` (single-line only here ; long
    // multi-line strings need block state -- skipped for v1).
    rules_.append( { "\\[\\[.*\\]\\]", strFmt, true } );

    // Single-line comments : `-- text to end of line`. Done LAST so
    // it overrides above rules when matched.
    comment_fmt_.setForeground( QColor( "#7a8c8c" ) );  // grey
    comment_fmt_.setFontItalic( true );
}

void LuaSyntaxHighlighter::highlightBlock( QString const& text )
{
    // Apply each rule in order.
    for( auto const& r : rules_ )
    {
        QRegularExpression re( r.pattern );
        auto it = re.globalMatch( text );
        while( it.hasNext() )
        {
            QRegularExpressionMatch m = it.next();
            // If the rule captures a sub-group, color just that ;
            // otherwise color the whole match.
            int const groups = m.lastCapturedIndex();
            if( groups >= 1 )
                setFormat( m.capturedStart( 1 ), m.capturedLength( 1 ), r.fmt );
            else
                setFormat( m.capturedStart(), m.capturedLength(), r.fmt );
        }
    }
    // Comments override : everything from `--` to end of line.
    int const commentStart = text.indexOf( QStringLiteral( "--" ) );
    if( commentStart >= 0 )
        setFormat( commentStart, text.length() - commentStart, comment_fmt_ );
}

// ── LuaHelper ─────────────────────────────────────────────────────────────

LuaHelper::LuaHelper( QObject* parent )
    : QObject( parent )
{
}

void LuaHelper::attach( QQuickTextDocument* doc )
{
    if( !doc ) return;
    if( !highlighter_ )
        highlighter_ = new LuaSyntaxHighlighter( doc->textDocument() );
    else
        highlighter_->setDocument( doc->textDocument() );
}

QString LuaHelper::lint( QString const& src )
{
    if( src.trimmed().isEmpty() ) return QString();
    lua_State* L = luaL_newstate();
    if( !L ) return QStringLiteral( "lua: out of memory" );
    int const rc = luaL_loadstring( L, src.toUtf8().constData() );
    QString err;
    if( rc != 0 && lua_isstring( L, -1 ) )
        err = QString::fromUtf8( lua_tostring( L, -1 ) );
    lua_close( L );
    return err;
}

QString LuaHelper::classifyAsset( QString const& path ) const
{
    if( path.isEmpty() ) return QStringLiteral( "other" );
    QString const lower = path.toLower();
    // Project files come before the generic .lua check.
    if( lower.endsWith( QStringLiteral( ".aaaproj.lua" ) ) )
        return QStringLiteral( "project" );
    QString const suffix =
        QStringLiteral( "." ) + QFileInfo( lower ).suffix();
    // Map suffix → category.
    static const QMap< QString, QString > kMap = {
        // images
        { ".png",  "image" }, { ".jpg", "image" }, { ".jpeg", "image" },
        { ".gif",  "image" }, { ".bmp", "image" }, { ".tiff", "image" },
        { ".tif",  "image" }, { ".webp","image" }, { ".heic", "image" },
        { ".heif", "image" }, { ".exr", "image" }, { ".hdr",  "image" },
        // video
        { ".mp4",  "video" }, { ".mov", "video" }, { ".avi", "video" },
        { ".mkv",  "video" }, { ".webm","video" }, { ".m4v", "video" },
        // 3D / meshes
        { ".fbx",  "mesh" },  { ".obj", "mesh" },  { ".gltf","mesh" },
        { ".glb",  "mesh" },  { ".dae", "mesh" },  { ".stl", "mesh" },
        { ".ply",  "mesh" },  { ".usd", "mesh" },  { ".usdz","mesh" },
        // audio
        { ".wav",  "audio" }, { ".mp3", "audio" }, { ".aac", "audio" },
        { ".flac", "audio" }, { ".ogg", "audio" }, { ".m4a", "audio" },
        { ".aif",  "audio" }, { ".aiff","audio" },
        // shaders
        { ".metal","shader" },{ ".glsl","shader" },{ ".frag","shader" },
        { ".vert", "shader" },{ ".comp","shader" },{ ".msl", "shader" },
        { ".hlsl", "shader" },{ ".wgsl","shader" },
        // scripts
        { ".lua",  "script" },{ ".py",  "script" },{ ".js",  "script" },
    };
    auto it = kMap.find( suffix );
    return it != kMap.end() ? it.value() : QStringLiteral( "other" );
}

QString LuaHelper::iconForAsset( QString const& path ) const
{
    QString const t = classifyAsset( path );
    if( t == QStringLiteral( "image" ) )   return QStringLiteral( "🖼" );
    if( t == QStringLiteral( "video" ) )   return QStringLiteral( "▶" );
    if( t == QStringLiteral( "mesh" ) )    return QStringLiteral( "⬢" );
    if( t == QStringLiteral( "audio" ) )   return QStringLiteral( "♪" );
    if( t == QStringLiteral( "shader" ) )  return QStringLiteral( "✦" );
    if( t == QStringLiteral( "script" ) )  return QStringLiteral( "λ" );
    if( t == QStringLiteral( "project" ) ) return QStringLiteral( "◆" );
    return QStringLiteral( "·" );
}

} } } // namespace aaa::ui::qt6
