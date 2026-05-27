#!/bin/bash
# Regenerate the three .md files from their .lua source siblings.
# Run from any directory (paths are absolute relative to script location).

set -e
HERE="$(cd "$(dirname "$0")" && pwd)"
DOC="$(cd "$HERE/.." && pwd)"

for name in lua_aaaseed_draw lua_aaaseed_interface lua_aaaseed_helpers lua_aaaseed_GABU_OBJ lua_aaaseed_SHADING lua_aaaseed_STACK lua_aaaseed_QUEUE lua_aaaseed_CHANGER lua_aaaseed_IMG lua_aaaseed_VIDEO lua_aaaseed_PHASOR lua_aaaseed_TEXT lua_aaaseed_FBO; do
    perl "$HERE/lua_to_md.pl" "$DOC/$name.lua" "$DOC/$name.md"
done
