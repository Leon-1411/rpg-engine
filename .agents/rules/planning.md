# Planning & Execution Rules

## 1. Tự Động Thực Thi Các Tác Vụ Đơn Giản (Autonomous Execution for Simple Tasks)
- Đối với các tác vụ đơn giản, điều hướng, kiểm tra, tra cứu, chạy lệnh git cơ bản (checkout, pull, status), sửa lỗi cú pháp nhỏ, chạy build/test hoặc các thao tác không làm thay đổi lớn cấu trúc dự án:
  - Agent **tự động cấp lệnh và thực thi ngay lập tức**.
  - Không dừng lại hỏi xác nhận những bước nhỏ không cần thiết để tránh làm gián đoạn và mất thời gian của người dùng.

## 2. Quy Định Lập Kế Hoạch (Implementation Plan)
- Đối với các yêu cầu phát triển tính năng mới, thay đổi cốt truyện, thiết kế hệ thống gameplay/kinh tế, hoặc refactor kiến trúc lớn:
  - Agent **bắt buộc** phải nghiên cứu kỹ và tạo/cập nhật implementation_plan.md trước khi sửa mã nguồn.
  - Kế hoạch phải nêu rõ: mục tiêu, các file thay đổi/tạo mới, và kế hoạch kiểm thử tự động.

## 3. Bắt Buộc Người Dùng Phê Duyệt Kế Hoạch (User Approval Required for Plans)
- Sau khi tạo hoặc cập nhật implementation_plan.md, Agent **phải dừng lại và chờ người dùng duyệt** (qua nút "Proceed" hoặc tin nhắn xác nhận).
- Tuyệt đối không tự ý sửa đổi mã nguồn của tính năng lớn trước khi kế hoạch được người dùng chấp thuận.

## 4. Tự Động Thực Thi Sau Khi Kế Hoạch Được Duyệt
- Sau khi người dùng đã phê duyệt implementation_plan.md, Agent có toàn quyền tự động thực thi liên tục tất cả các bước (chỉnh sửa file, biên dịch, chạy test, kiểm tra lỗi) đến khi hoàn tất mà không cần hỏi lại ở từng bước nhỏ.
