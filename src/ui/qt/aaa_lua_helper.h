// src/ui/qt/aaa_lua_helper.h -- c152-M
//
// Two QML-facing utilities :
//   - LuaSyntaxHighlighter : QSyntaxHighlighter subclass that paints
//     keywords / strings / comments / numbers / function calls in
//     distinct colors inside the Code Editor's TextArea.
//   - LuaHelper : Q_OBJECT registered as a context property. Exposes
//     `attach(qtd)` to wire the highlighter, `lint(src)` to validate
//     a Lua snippet, and `classifyAsset(path)` to tell the UI what
//     kind of asset a file is (image, video, mesh, audio, shader,
//     script, other) -- used by AssetsPanel icons and node-drop
//     binding routing.

#pragma once

#include <QObject>
#include <QQuickTextDocument>
#include <QString>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>

namespace aaa { namespace ui { namespace qt6 {

class LuaSyntaxHighlighter : public QSyntaxHighlighter
{
    Q_OBJECT
public:
    explicit LuaSyntaxHighlighter( QTextDocument* parent = nullptr );

protected:
    void highlightBlock( QString const& text ) override;

private:
    struct Rule { QString pattern; QTextCharFormat fmt; bool is_regex; };
    QVector< Rule > rules_;
    QTextCharFormat comment_fmt_;
};

class LuaHelper : public QObject
{
    Q_OBJECT

public:
    explicit LuaHelper( QObject* parent = nullptr );

    // QML hooks the editor's TextArea.textDocument to the highlighter.
    Q_INVOKABLE void attach( QQuickTextDocument* doc );

    // Returns the Lua error message if `src` doesn't parse, or "" if
    // it parses cleanly. Uses luaL_loadstring -- syntax only ; doesn't
    // execute.
    Q_INVOKABLE QString lint( QString const& src );

    // Classifies a file path by extension. Returns one of :
    //   "image"   .png .jpg .jpeg .gif .bmp .tiff .webp .heic
    //   "video"   .mp4 .mov .avi .mkv .webm
    //   "mesh"    .fbx .obj .gltf .glb .dae .stl .ply
    //   "audio"   .wav .mp3 .aac .flac .ogg .m4a
    //   "shader"  .metal .glsl .frag .vert .comp .msl
    //   "script"  .lua
    //   "project" .aaaproj.lua
    //   "other"   anything else (or empty path)
    Q_INVOKABLE QString classifyAsset( QString const& path ) const;

    // Convenience : returns a single-character glyph that visually
    // represents the type. Used by AssetsPanel for inline icons
    // without shipping image assets.
    Q_INVOKABLE QString iconForAsset( QString const& path ) const;

private:
    LuaSyntaxHighlighter* highlighter_ = nullptr;
};

} } } // namespace aaa::ui::qt6
