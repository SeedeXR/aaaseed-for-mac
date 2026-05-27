# AAADoc tools

Scripts that generate the `.md` API references from the `.lua` source files in this repo.

## Files

- `lua_to_md.pl` -- main generator. Takes a `.lua` doc file and produces a corresponding `.md`. Reads the section markers (`-- # ...` / `-- ## ...` / `-- ### ...`), generates a TOC with scientific numbering, fences function entries in `lua` code blocks, preserves prose paragraphs and hard-line breaks.
- `section_promote.pl` -- one-time helper. Promotes legacy section markers to the Option C convention:
  - Triplet `--\n--<tab>NAME\n--` (level 1) -> adds `# ` prefix to the middle line.
  - Single-line `-- NAME` with strict UPPERCASE content -> rewrites to `--## NAME`.
  - Idempotent: re-running on already-promoted lines is a no-op.
- `regen_all.sh` -- regenerate the three sibling `.md` files in one call.

## Section convention (Option C)

In the `.lua` doc files:

```
-- # SECTION                 -- level 1 (top section), maps to MD `## 1. SECTION`
-- ## sub-section            -- level 2, maps to MD `### 1.1. sub-section`
-- ### sub-sub-section       -- level 3, maps to MD `#### 1.1.1. sub-sub-section`

-- free comment              -- prose, kept above the next code block
```

The number of `#` after the leading `--` directly maps to the heading depth (offset by +1 in MD because the file title is h1).

The triplet visual decoration is preserved for readability:

```
--
--	# SECTION
--
```

Free comments (no `#` prefix) appear in the source as plain `-- ...` lines and become MD prose above the relevant code block.

## Usage

Edit the `.lua` source. Then regenerate:

```sh
./tools/regen_all.sh
```

Or regenerate one file:

```sh
perl tools/lua_to_md.pl lua_aaaseed_draw.lua lua_aaaseed_draw.md
```

## Convention rule

The `.lua` files are the source of truth. Do NOT edit the `.md` files directly -- they are regenerated from the `.lua` and your edits will be overwritten. The first lines of each generated `.md` document this.
