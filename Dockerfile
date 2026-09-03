# Dockerfile cho C++ RPG Engine Project
# Sử dụng Ubuntu 22.04 LTS làm base image
FROM ubuntu:22.04 AS builder

# Tránh prompt tương tác trong quá trình apt-get install
ENV DEBIAN_FRONTEND=noninteractive

# Cài đặt các công cụ biên dịch C++17 và CMake
RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    cmake \
    ninja-build \
    && rm -rf /var/lib/apt-get/lists/*

# Thiết lập thư mục làm việc trong container
WORKDIR /app

# Copy toàn bộ nguồn vào container
COPY . .

# Cấu hình và biên dịch dự án bằng CMake
RUN cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build --config Release

# -------------------------------------------------------------
# Stage 2: Runtime Image nhẹ nhàng để chạy Game / Test
# -------------------------------------------------------------
FROM ubuntu:22.04 AS runner

ENV DEBIAN_FRONTEND=noninteractive

# Cài đặt thư viện runtime cần thiết (nếu có) và libstdc++
RUN apt-get update && apt-get install -y \
    libstdc++6 \
    && rm -rf /var/lib/apt-get/lists/*

WORKDIR /app

# Tạo thư mục saves cho game
RUN mkdir -p /app/saves

# Copy các file đã build và tài nguyên từ builder stage
COPY --from=builder /app/build /app/build
COPY --from=builder /app/PROJECT_SPEC.md /app/

# Mặc định chạy game RPG Engine
CMD ["./build/rpg_engine"]
