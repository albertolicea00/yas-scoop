# CLAUDE.md — YAS Scoop

## What
Native GUI wrapper for **Scoop** (`scoop`). Part of YAS suite.
Status: **scaffolded & unit-tested** — vendored core + adapter + QML shell compile, 3/3 QtTest suites pass (verified cross-compiling on macOS). Pending: build + QA on the real target platform.

## Stack
- C++20 + Qt 6.7+ (Qt Quick / QML), CMake ≥ 3.24, MSVC toolchain
- Native windowing via Qt QPA plugin: **windows** (Win32).
- CLI execution: `QProcess`. **scoop is a PowerShell script, not an exe** — invoke via `powershell.exe -NoProfile -Command scoop ...` or the `scoop.cmd` shim in `%USERPROFILE%\scoop\shims`.
- Architecture: **vendored core copy** (identical across suite, NO shared library by design) + `scoop` adapter. Master template: `../yas-core/` local folder (not published). Core fixes must be replicated across repos.

## Target platform
Windows 10/11. x64 + arm64.

## scoop specifics
- User-level by default — **no elevation needed**. Simplest Windows target of the suite; good first Windows app.
- Key commands: `scoop search`, `scoop info`, `scoop list`, `scoop install/uninstall/update`, `scoop update *`, `scoop hold/unhold`, `scoop cleanup`, `scoop bucket list/add`, `scoop status`.
- Buckets (main, extras, versions, custom git repos) are core UX — UI needs bucket management.
- Local bucket manifests are JSON on disk (`buckets/*/bucket/*.json`) — read directly for fast search/metadata instead of scraping CLI output.
- Detect scoop root: `%USERPROFILE%\scoop` or `SCOOP` env var.

## Design (see DESIGN.md)
- UI shell: **Teams-style** — icon rail | list panel | detail pane (no in-app title header). Light/dark mode persisted via `YasManager` context property (`src/core/thememanager.*`, QSettings), toggle at rail bottom. Both palettes live in `qml/core/Theme.qml` (`Theme.dark`).
- Dark theme. Base `#222629`, accent **Teal `#008080`**, highlight `#0080801A`, text `#F8F8F2` / `#ACADAD`.
- App tag: **SCOOP**. Fonts: Outfit/Inter (UI), Fira Code or JetBrains Mono (CLI output).

## Conventions
- Conventional Commits (no co-author attribution), feature branches, PRs per CONTRIBUTING.md. Never push to origin without explicit ask.
- Planned layout (mirrors yas-brew, the reference scaffold): `src/core/` (vendored), `src/scoopadapter.*`, `src/main.cpp`, `qml/core/` (vendored) + `qml/Main.qml`, `tests/`, `assets/fonts/`, `icons/` (exists), CMakeLists.txt + CMakePresets.json.
- Packaging: portable zip + scoop manifest in extras bucket (dogfooding).

## Key files
README.md · DESIGN.md · CONTRIBUTING.md · EULA.md · SECURITY.md · icons/
