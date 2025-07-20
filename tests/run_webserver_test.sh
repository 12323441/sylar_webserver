#!/bin/bash

# 协程Web服务器测试脚本
# 用于验证协程库在Web服务器场景下的可行性

set -e

echo "🚀 协程Web服务器可行性验证测试"
echo "=================================="

# 检查是否在正确的目录
if [ ! -f "../CMakeLists.txt" ]; then
    echo "❌ 请在项目根目录运行此脚本"
    exit 1
fi

# 创建构建目录
BUILD_DIR="../build"
if [ ! -d "$BUILD_DIR" ]; then
    mkdir -p "$BUILD_DIR"
fi

cd "$BUILD_DIR"

echo "📦 编译协程Web服务器..."
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc) test_coroutine_webserver test_webserver_client

echo "✅ 编译完成!"

# 检查端口是否被占用
PORT=8080
if lsof -Pi :$PORT -sTCP:LISTEN -t >/dev/null ; then
    echo "⚠️  端口 $PORT 已被占用，请先停止其他服务"
    exit 1
fi

echo "🌐 启动协程Web服务器..."
echo "📡 服务器将在 http://localhost:$PORT 上运行"
echo "⚡ 按 Ctrl+C 停止服务器"
echo ""

# 在后台启动服务器
./bin/test_coroutine_webserver &
SERVER_PID=$!

# 等待服务器启动
sleep 3

# 检查服务器是否成功启动
if ! curl -s http://localhost:$PORT/api/hello > /dev/null; then
    echo "❌ 服务器启动失败"
    kill $SERVER_PID 2>/dev/null || true
    exit 1
fi

echo "✅ 服务器启动成功!"
echo ""

# 运行性能测试
echo "🧪 开始性能测试..."
./bin/test_webserver_client 127.0.0.1 $PORT

echo ""
echo "🎉 测试完成!"

# 停止服务器
echo "🛑 停止服务器..."
kill $SERVER_PID 2>/dev/null || true

echo "✅ 协程Web服务器可行性验证完成!" 