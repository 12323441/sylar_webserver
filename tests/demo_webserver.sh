#!/bin/bash

# 协程Web服务器功能演示脚本

set -e

echo "🎭 协程Web服务器功能演示"
echo "=========================="

# 检查服务器是否运行
check_server() {
    if curl -s http://localhost:8080/api/hello > /dev/null 2>&1; then
        return 0
    else
        return 1
    fi
}

# 等待服务器启动
wait_for_server() {
    echo "⏳ 等待服务器启动..."
    for i in {1..30}; do
        if check_server; then
            echo "✅ 服务器已启动!"
            return 0
        fi
        sleep 1
    done
    echo "❌ 服务器启动超时"
    return 1
}

# 演示函数
demo_basic_api() {
    echo ""
    echo "🔗 基础API测试"
    echo "---------------"
    
    echo "📝 Hello API:"
    curl -s http://localhost:8080/api/hello | head -3
    
    echo ""
    echo "📄 JSON API:"
    curl -s http://localhost:8080/api/json | jq '.data.fiber_id, .data.thread_id' 2>/dev/null || curl -s http://localhost:8080/api/json | grep -o '"fiber_id":[0-9]*'
}

demo_async_processing() {
    echo ""
    echo "⚡ 异步处理测试"
    echo "---------------"
    
    echo "🔄 异步任务执行:"
    start_time=$(date +%s%3N)
    curl -s http://localhost:8080/api/async > /dev/null
    end_time=$(date +%s%3N)
    duration=$((end_time - start_time))
    echo "   客户端等待时间: ${duration}ms"
    
    echo "📊 异步任务详情:"
    curl -s http://localhost:8080/api/async | jq '.data.execution_time_ms, .data.tasks_completed' 2>/dev/null || curl -s http://localhost:8080/api/async | grep -o '"execution_time_ms":[0-9]*'
}

demo_parameter_parsing() {
    echo ""
    echo "🔍 参数解析测试"
    echo "---------------"
    
    echo "📋 URL参数解析:"
    curl -s "http://localhost:8080/api/params?name=test&age=25&city=beijing" | jq '.data.query' 2>/dev/null || curl -s "http://localhost:8080/api/params?name=test&age=25&city=beijing" | grep -o '"query":"[^"]*"'
    
    echo ""
    echo "📋 请求头解析:"
    curl -s -H "X-Custom-Header: test-value" -H "User-Agent: DemoClient/1.0" http://localhost:8080/api/params | jq '.data.headers."X-Custom-Header"' 2>/dev/null || curl -s -H "X-Custom-Header: test-value" http://localhost:8080/api/params | grep -o '"X-Custom-Header":"[^"]*"'
}

demo_error_handling() {
    echo ""
    echo "⚠️  错误处理测试"
    echo "---------------"
    
    echo "🔄 多次请求测试错误处理:"
    for i in {1..5}; do
        echo "   请求 $i:"
        status=$(curl -s -w "%{http_code}" http://localhost:8080/api/error -o /dev/null)
        echo "   HTTP状态码: $status"
    done
}

demo_performance_test() {
    echo ""
    echo "🚀 性能测试"
    echo "-----------"
    
    echo "⚡ CPU密集型任务:"
    start_time=$(date +%s%3N)
    curl -s http://localhost:8080/api/performance > /dev/null
    end_time=$(date +%s%3N)
    duration=$((end_time - start_time))
    echo "   客户端等待时间: ${duration}ms"
    
    echo "📊 服务器执行详情:"
    curl -s http://localhost:8080/api/performance | jq '.data.computation_time_us, .data.result' 2>/dev/null || curl -s http://localhost:8080/api/performance | grep -o '"computation_time_us":[0-9]*'
}

demo_concurrent_requests() {
    echo ""
    echo "🌊 并发请求测试"
    echo "---------------"
    
    echo "🔄 10个并发请求:"
    start_time=$(date +%s%3N)
    
    # 使用后台进程发送并发请求
    for i in {1..10}; do
        curl -s http://localhost:8080/api/hello > /dev/null &
    done
    
    # 等待所有请求完成
    wait
    
    end_time=$(date +%s%3N)
    duration=$((end_time - start_time))
    echo "   总耗时: ${duration}ms"
    echo "   平均每个请求: $((duration / 10))ms"
}

demo_coroutine_info() {
    echo ""
    echo "🧵 协程信息展示"
    echo "---------------"
    
    echo "📊 协程和线程ID信息:"
    for i in {1..5}; do
        echo "   请求 $i:"
        curl -s http://localhost:8080/api/json | jq '.data.fiber_id, .data.thread_id' 2>/dev/null || curl -s http://localhost:8080/api/json | grep -o '"fiber_id":[0-9]*\|"thread_id":[0-9]*'
        sleep 0.1
    done
}

# 主演示流程
main() {
    # 检查服务器状态
    if ! check_server; then
        echo "❌ 服务器未运行，请先启动服务器:"
        echo "   ./build/bin/test_coroutine_webserver"
        exit 1
    fi
    
    echo "✅ 服务器运行正常，开始演示..."
    
    # 执行各种演示
    demo_basic_api
    demo_async_processing
    demo_parameter_parsing
    demo_error_handling
    demo_performance_test
    demo_concurrent_requests
    demo_coroutine_info
    
    echo ""
    echo "🎉 演示完成!"
    echo ""
    echo "💡 提示:"
    echo "   - 访问 http://localhost:8080 查看完整功能"
    echo "   - 运行 ./test_webserver_client 进行性能测试"
    echo "   - 查看服务器日志了解协程调度详情"
}

# 运行演示
main 