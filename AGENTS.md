# Project Directives & AI Agent Rules for RPG-Engine

This repository uses Antigravity IDE and AI Coding Assistants. All agents interacting with this repository MUST follow the operational guidelines below.

---

## 1. Planning & Approval Workflow (Quy Trình Lập Kế Hoạch & Phê Duyệt)

### Mandatory Planning Mode
- **When to Plan**: For any non-trivial task — including new features, combat/story/shop balance changes, story graph updates, multi-file edits, or architectural refactoring — the Agent **MUST** first create or update `implementation_plan.md` in the artifact directory.
- **Stop for Approval**: The Agent **MUST STOP** execution turn immediately after creating/updating the plan and wait for manual user review and approval (via the "Proceed" button or explicit message).
- **No Pre-Execution**: Under NO circumstances should the Agent edit source files, create project files, or run modifying commands before explicit approval is given.

### Exemptions (Direct Execution Allowed)
- The Agent may proceed directly without a formal plan for:
  - Answering investigatory questions (e.g., "how does `SaveManager` work?", "where is `StoryGraph` defined?").
  - Simple, single-line typos or syntax fixes.
  - Minor follow-up adjustments to an already-approved plan.

---

## 2. Autonomous Execution (Quyền Tự Động Thực Thi)

- Once the user explicitly approves the implementation plan, the Agent is granted **full autonomy** to complete the entire plan end-to-end:
  - Applying code edits across all target files.
  - Compiling the project via `build.bat` or `g++`.
  - Running automated test binaries (`test_story.exe`, `test_save.exe`, `test_shop.exe`, `test_ui.exe`, `test_app.exe`).
  - Fixing any build/test issues proactively without prompting for permission on minor fixes.
- Upon completion, the Agent summarizes results in `walkthrough.md`.

---

## 3. Tech Stack & Build Conventions

- **Language**: C++17 / C++20 Standard.
- **Testing & Verification Standard (MANDATORY)**:
  - The Agent **MUST ALWAYS** prioritize running tests and builds via **Docker** (`rpg-engine:dev` with CMake + Ninja + CTest) for maximum speed (~1-2s):
    ```powershell
    docker run --rm -v "${PWD}:/app" -w /app rpg-engine:dev bash -c "cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release && cmake --build build && ctest --test-dir build --output-on-failure"
    ```
  - **Cache & Branch Preservation**: Always stay and run verification on the current working branch in-place. Avoid creating and checking out unnecessary temporary branches that invalidate CMake/Ninja object caches (`build/`), ensuring fast incremental builds (~1s).
  - Fallback to `build.bat` or local `g++` only if Docker daemon is not active.
- **Compiler**: MinGW GCC / GCC Linux (via Docker).
- **Encoding**: UTF-8 without BOM.
- **Code Style**:
  - Clear separation of concerns between Engine (`src/`, `include/`), Tools (`tools/`), and Tests (`tests/`).
  - Keep documentation integrity: do not delete existing comments or documentation headers unless instructed.

