# Gemini / Antigravity Project Instructions

Refer to [AGENTS.md](file:///c:/Users/tpnam/OneDrive/E-profile/Portfolio/Case%20studies/The%20middle-semester%20project%20of%20C++/rpg-engine/AGENTS.md) for full project rules and directives.

## Summary of Core Workflow:
1. **Planning First**: Any non-trivial task (features, gameplay/story logic, refactoring) requires creating `implementation_plan.md` first.
2. **Mandatory Approval**: Stop and wait for user approval before making any code modifications.
3. **Execution Autonomy**: Once approved, run all tool actions (code edits, Docker / `build.bat`, tests) autonomously.
4. **Fast Testing via Docker**: ALWAYS prioritize running test suites with Docker (`rpg-engine:dev` + `ctest`) for fast 1-2s parallel execution. Always run in-place on the current branch to preserve Ninja object cache.
5. **Small Tasks**: Minor fixes, explanations, or follow-ups do not require a full plan.

