# Planning & Approval Rule

## 1. Quy Định Lập Kế Hoạch (Implementation Plan)
- Đối với các yêu cầu phát triển tính năng mới, thay đổi thiết kế cốt tryện, hệ thống kinh tế/gameplay, hoặc refactor kiến trúc lớn, Agent BẫT BUỘC phại tạo hoặc cập nhật tài liệu `implementation_plan.md` trước.
- Kếh hoạch phẨ� mô tả rõ ràng: mục tiêu thay đổi, danh sách file thay đổi/tạo mới, và kế hoạch kiểm thử tự động (Unit Tests/Build).

## 2. Bắt Buộc Phê Duyệt Thủ Công (Manual Approval Required)
- Sau khi tạo hoặc cập nhật `implementation_plan.md`, Agent BẫT BUỘC PHẦI DẠNG LẠI (kết thúc lượt tương tác) và chờ người dùng xem zét, phê duyệt (bằng nút "Proceed" hoặc tin nhắn xác nhận).
- Tuyệt đối KHÔNG ĐƯỢC tự ý bắt đầu sửa đổi mã nguồn hoặc chạy các thao tác chỉnh sủa trước khi nhận được sự đồng ý rõ ràng từ người dùng.

## 3. Cấp Quyền Hành Động Tự Động (Tool Execution Autonomy)
- Khi kế hoạch Đã ĐƯỢC người dùng phê duyệt, Agent được toàn quyền tự động thực thi các công cụ (chạy command, biển dịch g++, chỉnh sửa file mã nguồn, chạy unit tests) để hoàn thành nhiệm vụ nhanh chóng mà không cần hỏi lại ở từng buớc nhỏ.
