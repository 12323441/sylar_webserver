# 协程库可行性验证 - 快速使用指南

## 🚀 快速验证

### 1. 编译项目
```bash
cd /path/to/sylar-from-scratch
mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc) test_coroutine_webserver test_webserver_client
```

### 2. 启动Web服务器
```bash
./bin/test_coroutine_webserver
```

### 3. 验证功能
```bash
# 在另一个终端运行演示
cd tests
./demo_webserver.sh
```

### 4. 性能测试
```bash
# 运行完整的性能测试
./bin/test_webserver_client 127.0.0.1 8080
```

## 🔍 验证要点

### 协程调度验证
- ✅ 每个HTTP请求在独立协程中处理
- ✅ 协程ID在不同请求中变化
- ✅ 线程ID分布合理

### 异步处理验证
- ✅ 异步任务正确执行
- ✅ 非阻塞I/O操作
- ✅ 协程间协作正常

### 并发处理验证
- ✅ 支持高并发连接
- ✅ 响应时间稳定
- ✅ 无资源泄漏

### 错误处理验证
- ✅ 异常情况正确处理
- ✅ 资源自动清理
- ✅ 服务稳定性

## 📊 预期结果

| 指标 | 预期值 | 验证方法 |
|------|--------|----------|
| 并发连接 | 1000+ | 性能测试 |
| 响应时间 | <10ms | 性能测试 |
| QPS | >10000 | 性能测试 |
| 内存使用 | 稳定 | 长时间运行 |
| 协程切换 | 流畅 | 日志观察 |

## 🎯 成功标准

1. **功能完整性** - 所有API端点正常工作
2. **性能表现** - 达到预期性能指标
3. **稳定性** - 长时间运行无崩溃
4. **资源管理** - 无内存泄漏
5. **并发能力** - 支持高并发场景

## 🛠️ 故障排除

### 编译问题
```bash
# 安装依赖
sudo apt-get install libssl-dev libyaml-cpp-dev

# 清理重新编译
make clean && make -j$(nproc)
```

### 运行问题
```bash
# 检查端口占用
lsof -i :8080

# 检查权限
chmod +x tests/*.sh
```

### 性能问题
```bash
# 调整协程数量
# 修改 test_coroutine_webserver.cc 中的 IOManager 参数
```

## 📈 扩展验证

### 添加自定义测试
```cpp
// 在 test_coroutine_webserver.cc 中添加新的Servlet
class CustomServlet : public Servlet {
    // 实现你的测试逻辑
};
```

### 压力测试
```bash
# 使用ab进行压力测试
ab -n 10000 -c 100 http://localhost:8080/api/hello
```

### 监控分析
```bash
# 使用top监控资源使用
top -p $(pgrep test_coroutine_webserver)

# 使用strace跟踪系统调用
strace -p $(pgrep test_coroutine_webserver)
```

## 🎉 验证成功

当所有测试通过后，说明协程库具备以下能力：

1. **技术可行性** - 能够处理复杂的Web服务器场景
2. **性能可行性** - 在高并发下保持良好性能
3. **稳定性可行性** - 长时间运行稳定可靠
4. **扩展性可行性** - 易于扩展和维护

这证明了协程库在实际应用中的可行性！ 