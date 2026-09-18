# Planning & Execution Rules / Quy Tắc Lập Kế Hoạch & Phê Duyệt

## 1. When to Plan (Quy định lập Kế hoạch thực thi)
- **Mandatory Planning**: For any task involving new feature implementation, story/gameplay logic changes, economy/system updates, significant refactoring, or multi-file code modifications, the Agent **MUST** create or update the `implementation_plan.md` artifact first.
- **Tài liệu Kế hoạch**: Kế hoạch phải mô tả rõ mục tiêu thay đổi, danh sách các file cần sửa/tạo mới, và phương án kiểm thử tự động (Unit Tests / Build).
- **Exemptions (Tác vụ nhỏ/Một lần)**: For purely investigatory questions ("explain X", "where is Y"), simple single-line fixes, syntax corrections, or minor follow-up tweaks to an already-approved plan, the Agent MAY execute directly without creating a plan.

---

## 2. Mandatory Manual User Approval (Bắt buộc phê duyệt thủ công)
- **STOP and Block**: After creating or updating `implementation_plan.md`, the Agent **MUST STOP** its turn immediately and wait for explicit user approval (via the "Proceed" button or explicit confirmation message).
- **DO NOT Pre-execute**: The Agent **MUST NOT** modify any source code, create new project files, or run mutating commands prior to receiving explicit user approval for the plan.

---

## 3. Tool Execution Autonomy (Quyền tự chủ khi thực thi)
- **Full Autonomy Upon Approval**: Once the user approves the plan, the Agent has full autonomy to execute all necessary tool actions in sequence without asking for permission at every micro-step:
  - Editing/creating source code files.
  - Running compilation scripts (`build.bat`, `g++`, `cmake`).
  - Running automated unit tests and checking outputs.
- **Verification & Walkthrough**: After execution, the Agent must verify the build/tests and summarize completed changes in `walkthrough.md`.

