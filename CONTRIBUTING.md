# Contributing Guidelines

Thank you for your interest in contributing to this project! We welcome contributions from developers of all skill levels.

## Development Setup


### Prerequisites

1. **Visual Studio 2022 Build Tools** (Desktop development with C++): https://visualstudio.microsoft.com/downloads/
2. **CMake + Ninja**: `scoop install cmake ninja`
3. **Qt 6.7+** with [aqtinstall](https://github.com/miurahr/aqtinstall): `pip install aqtinstall` then `aqt install-qt windows desktop 6.7.3 win64_msvc2022_64 -O C:\Qt` (or use the official Qt Online Installer). Point CMake at it with `-DCMAKE_PREFIX_PATH=C:\Qt\6.7.3\msvc2022_64`.

The `scoop` CLI itself must also be installed — the app is a GUI wrapper around it.

### Build environment

C++/Qt has no virtualenv; isolation comes from **out-of-source builds**: everything generated lives under `build/` (git-ignored) and the "environment" is pinned by `CMakePresets.json`. To reset the environment completely, delete `build/` and configure again.

```bash
make build     # configure (if needed) + compile
make test      # build + run the QtTest suite
make run       # build + launch the app
make release   # optimized build
make help      # list all targets
```

### Project structure

- `src/core/` — vendored YAS core (process runner, queue, models, controller). Kept identical across all YAS repos; if you fix a bug here, please mention it so the fix can be replicated suite-wide.
- `src/` — the scoop adapter (command builders + output parsers) and `main.cpp`.
- `qml/core/` — shared design system and app shell (also vendored).
- `qml/` — app entry (`Main.qml`: brand accent + tag).
- `tests/` — QtTest unit tests; adapter parsers are tested against recorded CLI output.

## How to Contribute

1. **Reporting Bugs**: Open an issue using the Bug Report template.
2. **Suggesting Features**: Open an issue using the Feature Request template.
3. **Pull Requests**:
   - Fork the repository.
   - Create a feature branch (`git checkout -b feature/amazing-feature`).
   - Commit your changes (`git commit -m 'feat: add some amazing feature'`).
   - Push to the branch (`git push origin feature/amazing-feature`).
   - Open a Pull Request.

## Code Style

- Write clean, readable, and documented code.
- Follow the visual style and tokens outlined in `DESIGN.md`.
- Keep changes concise and focused.
