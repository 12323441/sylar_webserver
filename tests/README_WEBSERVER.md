# 协程Web服务器可行性验证

本项目基于sylar协程库实现了一个完整的Web服务器，用于验证协程调度器在实际应用场景中的可行性。

## 🎯 验证目标

通过构建一个功能完整的Web服务器，验证以下协程库特性：

1. **协程调度器** - 多线程协程调度能力
2. **非阻塞I/O** - 基于epoll的事件驱动模型
3. **高并发处理** - 大量并发连接的处理能力
4. **异步任务** - 协程间的异步协作
5. **资源管理** - 内存和文件描述符的管理

## 📁 文件结构

```
tests/
├── test_coroutine_webserver.cc    # 主Web服务器实现
├── test_webserver_client.cc       # 性能测试客户端
├── run_webserver_test.sh          # 自动化测试脚本
└── README_WEBSERVER.md            # 本文档
```

## 🚀 快速开始

### 1. 编译项目

```bash
# 在项目根目录
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc) test_coroutine_webserver test_webserver_client
```

### 2. 运行自动化测试

```bash
# 在tests目录下
./run_webserver_test.sh
```

### 3. 手动测试

```bash
# 启动服务器
./build/bin/test_coroutine_webserver

# 在另一个终端运行性能测试
./build/bin/test_webserver_client 127.0.0.1 8080
```

## 🌐 Web服务器功能

### 基础功能

- **首页** (`/`) - 展示服务器信息和功能链接
- **Hello API** (`/api/hello`) - 基础响应测试
- **JSON API** (`/api/json`) - JSON格式响应
- **异步处理** (`/api/async`) - 模拟异步任务处理
- **参数解析** (`/api/params`) - URL参数和请求头解析
- **错误处理** (`/api/error`) - 模拟各种HTTP错误
- **文件上传** (`/api/upload`) - 文件上传表单和处理
- **性能测试** (`/api/performance`) - CPU密集型任务测试

### 技术特性

1. **协程调度**
   - 每个HTTP请求在独立协程中处理
   - 支持协程间的异步切换
   - 非阻塞I/O操作

2. **高并发支持**
   - 基于epoll的事件驱动
   - 多线程协程调度
   - 连接池管理

3. **错误处理**
   - 优雅的错误响应
   - 异常安全
   - 资源自动清理

## 📊 性能验证

### 测试指标

- **并发连接数** - 同时处理的连接数量
- **响应时间** - 请求处理延迟
- **吞吐量** - QPS (每秒查询数)
- **资源使用** - CPU和内存占用
- **稳定性** - 长时间运行稳定性

### 测试场景

1. **基础性能测试**
   ```bash
   # 测试不同并发级别
   ./test_webserver_client 127.0.0.1 8080
   ```

2. **端点性能对比**
   - `/api/hello` - 轻量级响应
   - `/api/json` - 中等复杂度
   - `/api/async` - 异步任务
   - `/api/performance` - CPU密集型

3. **压力测试**
   - 1000个并发请求
   - 长时间运行测试
   - 内存泄漏检测

## 🔍 验证要点

### 1. 协程调度验证

**观察点：**
- 协程ID在不同请求中的变化
- 线程ID的分布情况
- 协程切换的流畅性

**验证方法：**
```bash
# 查看日志中的协程和线程信息
curl http://localhost:8080/api/json
```

### 2. 异步处理验证

**观察点：**
- 异步任务的执行时间
- 协程间的协作
- 非阻塞I/O的效果

**验证方法：**
```bash
# 测试异步处理端点
curl http://localhost:8080/api/async
```

### 3. 并发处理验证

**观察点：**
- 高并发下的响应时间
- 连接处理的稳定性
- 资源使用情况

**验证方法：**
```bash
# 运行并发测试
./test_webserver_client 127.0.0.1 8080
```

### 4. 错误处理验证

**观察点：**
- 异常情况的处理
- 资源清理的完整性
- 服务的稳定性

**验证方法：**
```bash
# 测试错误处理
curl http://localhost:8080/api/error
```

## 📈 预期结果

### 性能指标

- **并发连接**：支持1000+并发连接
- **响应时间**：平均响应时间 < 10ms
- **吞吐量**：QPS > 10000
- **内存使用**：稳定，无明显泄漏
- **CPU使用**：高效利用多核

### 功能指标

- **协程调度**：流畅的协程切换
- **异步处理**：正确的异步任务执行
- **错误处理**：优雅的错误响应
- **资源管理**：无资源泄漏

## 🛠️ 故障排除

### 常见问题

1. **编译错误**
   ```bash
   # 确保依赖库已安装
   sudo apt-get install libssl-dev
   ```

2. **端口占用**
   ```bash
   # 检查端口占用
   lsof -i :8080
   # 杀死占用进程
   kill -9 <PID>
   ```

3. **权限问题**
   ```bash
   # 确保脚本有执行权限
   chmod +x run_webserver_test.sh
   ```

### 调试技巧

1. **查看详细日志**
   ```bash
   # 设置日志级别
   export SYLAR_LOG_LEVEL=DEBUG
   ```

2. **性能分析**
   ```bash
   # 使用perf进行性能分析
   perf record -g ./test_coroutine_webserver
   perf report
   ```

3. **内存检查**
   ```bash
   # 使用valgrind检查内存泄漏
   valgrind --leak-check=full ./test_coroutine_webserver
   ```

## 📚 扩展开发

### 添加新的Servlet

```cpp
class MyServlet : public sylar::http::Servlet {
public:
    MyServlet() : Servlet("MyServlet") {}
    
    int32_t handle(HttpRequest::ptr request, 
                   HttpResponse::ptr response, 
                   HttpSession::ptr session) override {
        // 实现你的逻辑
        response->setStatus(HttpStatus::OK);
        response->setBody("Hello from MyServlet!");
        return 0;
    }
};

// 注册Servlet
dispatch->addServlet("/api/my", std::make_shared<MyServlet>());
```

### 自定义中间件

```cpp
// 在handleClient方法中添加中间件逻辑
void handleClient(Socket::ptr client) {
    // 前置处理
    auto start_time = std::chrono::steady_clock::now();
    
    // 原有处理逻辑
    // ...
    
    // 后置处理
    auto end_time = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    SYLAR_LOG_INFO(g_logger) << "请求处理时间: " << duration.count() << "ms";
}
```

## 🎉 总结

通过这个Web服务器项目，我们可以全面验证协程库的可行性：

1. **技术可行性** - 协程调度器能够有效处理Web服务器场景
2. **性能可行性** - 在高并发场景下保持良好的性能
3. **稳定性可行性** - 长时间运行稳定，无资源泄漏
4. **扩展性可行性** - 易于扩展和维护

这个验证项目为协程库在实际应用中的使用提供了有力的证明。 