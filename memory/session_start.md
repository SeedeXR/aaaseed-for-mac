# Session Start — Read before touching anything

> Any agent (human or AI) picking up the AAASeed Mac port for the first time, or after a break, reads this file end-to-end before issuing the first tool call or writing the first line of code.

---

## TL;DR — what this project is, in 50 words

A native macOS port of AAASeed (Mâa Berriet's Windows C++/Lua/OpenGL real-time visual engine, 1996-present). **Target : Apple Silicon (M-series), Apple Metal GPU directly — no Vulkan / SPIR-V / SPIRV-Cross / MoltenVK / ANGLE.** Windows MSVC continues to build the same engine source verbatim. New code lands NATIVELY on Mac ; Windows code paths inside `#if AAA_OS_WINDOWS()` are preserved.

## Project doctrine in one screen

1. **Apple Metal direct.** `MTL::Device::newLibrary(NS::String*, ...)` for shaders ; metal-cpp throughout. No abstraction via Vulkan IR. See `philosophy.md` Part 2.86.
2. **Apple Silicon (M-series) primary.** arm64 only, macOS 13+. TBDR + unified memory in the cost model. Intel-Mac dropped for v1.
3. **Windows must keep working.** Mac branches under `#elif AAA_OS_MAC()` only. Never alter `#if AAA_OS_WINDOWS()` bodies. The engine vendor tree compiles bi-platform from the same source. See `philosophy.md` Part 2.97.
4. **Performance profiling is embedded.** Every render change ships with `os_signpost` (CPU), `pushDebugGroup` (GPU), and a `perf`-labelled CTest budget. See `philosophy.md` Part 2.95 and `agent_profile.md`.
5. **Test pyramid required.** Unit + integration + regression. No render code merges without all three. Stub-only ports are provisional.
6. **Zero hallucination on engine symbols.** Grep the vendor tree before referencing any identifier. The `AAA_OS_DARWIN` mistake (continuation 23-24) is the canonical anti-example.

---

## The three mindsets (non-negotiable)

### 1. Multi-agent mindset
You are one of many specialized agents that will work on this project across many sessions. Your job is:
- Use sub-agents (`Explore`, `Plan`, `general-purpose`) for parallel research; do not serially read every file.
- Leave the project in a state where the **next agent can pick up from the files alone**, with no chat history.
- Treat the `memory/` and `instructions/` documents as the project's shared brain. Read, update, and never silently desync them.
- When you finish, append (do not overwrite) an entry in `memory/handover_session.md`.

### 2. Token-efficiency mindset
- Read excerpts via `Read` with `offset+limit`, not whole multi-thousand-line files.
- Dispatch parallel sub-agents in **one tool-call message** when their work is independent.
- Write dense, structured prose: bullets, tables, no filler.
- Cache findings into `memory/` so the next session does not redo your reading.

### 3. Zero-hallucination mindset
- Cite only what you have actually read or run. File path, line number, function name, API symbol — all verified or marked `unconfirmed`.
- For Apple APIs: cite the link list in `instructions/links.md` and the books in `books/`. If unsure, say `I'm unsure of the exact name; let me check the Apple docs` and check.
- For Windows source: cite the file in `aaaseed-windows/aaaseed_exe-main/Src/` with the path verbatim.
- Anti-pattern: confidently named but wrong API. Refuse this every time.

---

## Reading order (do not skip)

Read these files in this order. Each one builds on the previous.

1. **`README.md`** (repo root) — clone-and-build entry point. Confirm the toolchain checklist passes on this machine.
2. **`VENDORING.md`** (repo root) — the `vendor/` ↔ upstream contract and allowed-edit policy. Critical: you will be editing inside `vendor/aaaseed-engine/` from time to time, so know the rules.
3. **`memory/project_context.md`** — what we are building, deliverables, evaluation criteria.
4. **`memory/agent_profile.md`** — your identity, your mindsets, your testing discipline.
5. **`memory/philosophy.md`** — design philosophy: AAASeed's Windows building style, Apple Silicon mental model, and the **self-contained-repo norm** (Part 2.9).
6. **`memory/mindmap.md`** — the current architecture map.
7. **`memory/porting.md`** — detailed porting reference.
8. **`memory/todo.md`** — what is open, what is done, what was cancelled.
9. **`memory/handover_session.md`** — last session's notes and open threads.
10. **`instructions/instruction.md`** — end-to-end execution runbook.
11. **`instructions/links.md`** — external references.

Then, if the task involves engine source:

12. **`vendor/aaaseed-engine/CLAUDE.md`** — Windows build repo top-level orientation (lives inside vendor since the engine snapshot includes Mâa's own CLAUDE files).
13. **`vendor/aaaseed-runtime/CLAUDE.md`** — runtime repo top-level orientation.
14. The relevant subsystem's local `CLAUDE.md` (e.g. `vendor/aaaseed-engine/Src/gol/CLAUDE.md`).

Only then start the work.

---

## Skim, then drill

You do not need to memorize the books in `books/`. Skim:

- **`books/AAASeed_Introduction.md`** for engine concepts when you encounter unfamiliar terms (MEU, GaBu, GaBuZoMeu, c_obj_ui, c_layers, params).
- **`books/Metal by Tutorials (Fourth Edition).md`** as the Metal reference — open it when you need MTL* details, MSL syntax, render-pass descriptor patterns.
- **`books/GPU_Programming_on_Apple_Silicon_Using_CPP.md`** for compute pipelines, Metal-cpp idiom, unified memory cost model.

Treat them as encyclopedias: skim the TOC, deep-read the relevant chapter, move on.

---

## Anti-patterns to refuse

- **Do not** start coding before reading `memory/handover_session.md`. The previous session may have left an explicit warning or a half-finished branch.
- **Do not** introduce `#ifdef _WIN32` / `#ifdef __APPLE__` inside engine subsystem `.cpp` files. Use the platform layer (`Src/platform/win32/`, `src/platform/macos/`) or the GOL backend abstraction. Engine `.cpp` files should not know which OS they are on.
- **Do not** refactor an unrelated subsystem during a focused port step. Match the receiving codebase's style; resist cleanup urges.
- **Do not** delete preserved-draft `if false { ... }` blocks. They are intentional per `aaaseed-windows/aaaseed_exe-main/CLAUDE.md`.
- **Do not** modify `err.h` or `aaa_type.h` without asking — both trigger full rebuilds.
- **Do not** rename existing Lua API entries. Add new ones; never break old callers.
- **Do not** swap Lua 5.1 for 5.4 in this port.
- **Do not** rewrite the UI. Replace its host (Win32 → Cocoa) but keep the GaBu look-and-feel and behavior identical.
- **Do not** claim a porting step done without unit + integration + regression tests in place.

---

## First actions checklist (before writing code)

1. Confirm the repo is self-contained: `ls aaaseed-for-mac/vendor/aaaseed-engine/Src/aaa_build_config.h` returns a path. If not, see `VENDORING.md` re-vendoring procedure.
2. Confirm the build still works on this machine: `cmake --build --preset macos-arm64-debug && ctest --preset macos-arm64-debug`. Last known green is in the most recent `handover_session.md` entry.
3. Read the latest `handover_session.md` entry's "Open threads" section. That is your starting point unless Alex says otherwise.
4. Confirm with Alex what task this session targets. If unclear, ask a focused multiple-choice question; do not silently pick.
5. Check `memory/todo.md` for the highest-priority open item.
6. If the item touches engine source, locate it inside `vendor/aaaseed-engine/Src/<subsys>/`, read its `CLAUDE.md`, and only then start.

---

## When you do not know

The correct order:

1. **Read** — the relevant `CLAUDE.md`, then the relevant source file, then the relevant book chapter.
2. **Search** — `grep -r` in the codebase for prior art.
3. **Ask** — a focused, multiple-choice question to Alex.
4. **Spike** — write the smallest possible test program that resolves the question.
5. **Only then** — implement.

Never invent. Never bluff. The cost of a wrong fact in a memory file compounds across sessions.

---

## When you finish

1. Mark the relevant `todo.md` items `[x]` and link the file/PR that delivered them.
2. Append a new entry at the top of `memory/handover_session.md` using the template at the bottom of that file.
3. Update `memory/mindmap.md` if any subsystem boundary changed.
4. If you discovered something non-obvious (an undocumented Windows behavior, an Apple-API gotcha, a project-specific convention), record it in the appropriate file:
   - Engine-level project fact → `memory/porting.md` or `memory/mindmap.md`.
   - Personal-to-Alex preference / correction → save as an auto-memory entry (see `agent_profile.md`).
5. Run the test suites you advertised. Do not declare done if tests are red.

---

## Final reminder

This project is a **port**, not a rewrite. Mâa spent 30 years building the engine. Your job is to bring it across to a new platform with as little semantic drift as possible, while leveraging what Apple Silicon does best (unified memory, TBDR, Metal command-buffer scheduling). When in doubt: read the original code, ask, then act.
