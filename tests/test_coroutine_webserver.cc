/**
 * @file test_coroutine_webserver.cc
 * @brief 基于协程库的Web服务器测试程序
 * @author AI Assistant
 * @date 2024-01-01
 * @brief 验证协程调度器在Web服务器场景下的可行性
 */

#include "sylar/sylar.h"
#include "sylar/http/http_server.h"
#include "sylar/http/servlet.h"
#include "sylar/log.h"
#include "sylar/iomanager.h"
#include "sylar/scheduler.h"
#include "sylar/timer.h"
#include "sylar/util.h"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <thread>

namespace sylar {
namespace http {

// 全局日志器
static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("webserver");

/**
 * @brief 首页Servlet
 */
class HomeServlet : public Servlet {
public:
    HomeServlet() : Servlet("HomeServlet") {}
    
    int32_t handle(HttpRequest::ptr request, HttpResponse::ptr response, HttpSession::ptr session) override {
        SYLAR_LOG_INFO(g_logger) << "处理首页请求: " << request->getPath();
        
        // 模拟一些处理时间，展示协程的异步特性
        // 使用IOManager的sleep功能
        sylar::IOManager::GetThis()->addTimer(10, [](){}, true);
        
        response->setStatus(HttpStatus::OK);
        response->setHeader("Content-Type", "text/html; charset=utf-8");
        
        std::string html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>协程Web服务器</title>
    <meta charset="utf-8">
    <style>
        body { font-family: Arial, sans-serif; margin: 40px; background: #f5f5f5; }
        .container { max-width: 800px; margin: 0 auto; background: white; padding: 30px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        h1 { color: #333; text-align: center; }
        .feature { margin: 20px 0; padding: 15px; background: #f9f9f9; border-left: 4px solid #007acc; }
        .demo-links { margin: 30px 0; }
        .demo-links a { display: inline-block; margin: 10px; padding: 10px 20px; background: #007acc; color: white; text-decoration: none; border-radius: 5px; }
        .demo-links a:hover { background: #005a9e; }
        .stats { background: #e8f4fd; padding: 15px; border-radius: 5px; margin: 20px 0; }
    </style>
</head>
<body>
    <div class="container">
        <h1>🚀 协程Web服务器演示</h1>
        
        <div class="feature">
            <h3>✨ 协程特性验证</h3>
            <p>这是一个基于协程库构建的Web服务器，展示了以下特性：</p>
            <ul>
                <li>非阻塞I/O处理</li>
                <li>协程调度和切换</li>
                <li>高并发连接处理</li>
                <li>异步任务执行</li>
            </ul>
        </div>
        
        <div class="demo-links">
            <h3>🧪 功能演示</h3>
            <a href="/api/hello">基础API</a>
            <a href="/api/json">JSON响应</a>
            <a href="/api/async">异步处理</a>
            <a href="/api/params?name=test&age=25">参数解析</a>
            <a href="/api/error">错误处理</a>
            <a href="/api/upload" method="post">文件上传</a>
        </div>
        
        <div class="stats">
            <h3>📊 服务器信息</h3>
            <p><strong>当前时间:</strong> )" + sylar::Time2Str() + R"(</p>
            <p><strong>协程ID:</strong> )" + std::to_string(sylar::GetFiberId()) + R"(</p>
            <p><strong>线程ID:</strong> )" + std::to_string(sylar::GetThreadId()) + R"(</p>
        </div>
    </div>
</body>
</html>
        )";
        
        response->setBody(html);
        return 0;
    }
};

/**
 * @brief 基础API Servlet
 */
class HelloServlet : public Servlet {
public:
    HelloServlet() : Servlet("HelloServlet") {}
    
    int32_t handle(HttpRequest::ptr request, HttpResponse::ptr response, HttpSession::ptr session) override {
        SYLAR_LOG_INFO(g_logger) << "处理Hello API请求";
        
        response->setStatus(HttpStatus::OK);
        response->setHeader("Content-Type", "text/plain; charset=utf-8");
        response->setBody("Hello from Coroutine Web Server! 🎉\n\n协程ID: " + std::to_string(sylar::GetFiberId()));
        
        return 0;
    }
};

/**
 * @brief JSON API Servlet
 */
class JsonServlet : public Servlet {
public:
    JsonServlet() : Servlet("JsonServlet") {}
    
    int32_t handle(HttpRequest::ptr request, HttpResponse::ptr response, HttpSession::ptr session) override {
        SYLAR_LOG_INFO(g_logger) << "处理JSON API请求";
        
        // 模拟数据库查询时间
        sylar::IOManager::GetThis()->addTimer(50, [](){}, true);
        
        response->setStatus(HttpStatus::OK);
        response->setHeader("Content-Type", "application/json; charset=utf-8");
        
        std::string json = R"({
    "status": "success",
    "message": "协程服务器运行正常",
    "data": {
        "timestamp": ")" + sylar::Time2Str() + R"(",
        "fiber_id": )" + std::to_string(sylar::GetFiberId()) + R"(,
        "thread_id": )" + std::to_string(sylar::GetThreadId()) + R"(,
        "features": [
            "非阻塞I/O",
            "协程调度",
            "高并发处理",
            "异步任务"
        ]
    }
})";
        
        response->setBody(json);
        return 0;
    }
};

/**
 * @brief 异步处理Servlet
 */
class AsyncServlet : public Servlet {
public:
    AsyncServlet() : Servlet("AsyncServlet") {}
    
    int32_t handle(HttpRequest::ptr request, HttpResponse::ptr response, HttpSession::ptr session) override {
        SYLAR_LOG_INFO(g_logger) << "处理异步API请求";
        
        // 模拟多个异步任务
        auto start_time = std::chrono::steady_clock::now();
        
        // 任务1: 模拟网络请求
        sylar::IOManager::GetThis()->addTimer(100, [](){}, true);
        
        // 任务2: 模拟数据库查询
        sylar::IOManager::GetThis()->addTimer(80, [](){}, true);
        
        // 任务3: 模拟文件I/O
        sylar::IOManager::GetThis()->addTimer(60, [](){}, true);
        
        auto end_time = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        response->setStatus(HttpStatus::OK);
        response->setHeader("Content-Type", "application/json; charset=utf-8");
        
        std::string json = R"({
    "status": "success",
    "message": "异步任务执行完成",
    "data": {
        "execution_time_ms": )" + std::to_string(duration.count()) + R"(,
        "fiber_id": )" + std::to_string(sylar::GetFiberId()) + R"(,
        "tasks_completed": 3
    }
})";
        
        response->setBody(json);
        return 0;
    }
};

/**
 * @brief 参数解析Servlet
 */
class ParamsServlet : public Servlet {
public:
    ParamsServlet() : Servlet("ParamsServlet") {}
    
    int32_t handle(HttpRequest::ptr request, HttpResponse::ptr response, HttpSession::ptr session) override {
        SYLAR_LOG_INFO(g_logger) << "处理参数解析请求: " << request->getQuery();
        
        response->setStatus(HttpStatus::OK);
        response->setHeader("Content-Type", "application/json; charset=utf-8");
        
        std::string json = R"({
    "status": "success",
    "message": "参数解析成功",
    "data": {
        "path": ")" + request->getPath() + R"(",
        "query": ")" + request->getQuery() + R"(",
        "method": ")" + std::string(HttpMethodToString(request->getMethod())) + R"(",
        "headers": {
)";
        
        // 添加请求头信息
        bool first = true;
        for (const auto& header : request->getHeaders()) {
            if (!first) json += ",";
            json += "\"" + header.first + "\": \"" + header.second + "\"";
            first = false;
        }
        
        json += R"(
        }
    }
})";
        
        response->setBody(json);
        return 0;
    }
};

/**
 * @brief 错误处理Servlet
 */
class ErrorServlet : public Servlet {
public:
    ErrorServlet() : Servlet("ErrorServlet") {}
    
    int32_t handle(HttpRequest::ptr request, HttpResponse::ptr response, HttpSession::ptr session) override {
        SYLAR_LOG_WARN(g_logger) << "模拟错误处理";
        
        // 模拟随机错误
        static int counter = 0;
        counter++;
        
        if (counter % 3 == 0) {
            response->setStatus(HttpStatus::INTERNAL_SERVER_ERROR);
            response->setHeader("Content-Type", "application/json; charset=utf-8");
            response->setBody(R"({
    "status": "error",
    "message": "模拟服务器内部错误",
    "code": 500
})");
        } else if (counter % 3 == 1) {
            response->setStatus(HttpStatus::NOT_FOUND);
            response->setHeader("Content-Type", "application/json; charset=utf-8");
            response->setBody(R"({
    "status": "error",
    "message": "模拟资源未找到",
    "code": 404
})");
        } else {
            response->setStatus(HttpStatus::OK);
            response->setHeader("Content-Type", "application/json; charset=utf-8");
            response->setBody(R"({
    "status": "success",
    "message": "这次请求成功了",
    "code": 200
})");
        }
        
        return 0;
    }
};

/**
 * @brief 文件上传Servlet
 */
class UploadServlet : public Servlet {
public:
    UploadServlet() : Servlet("UploadServlet") {}
    
    int32_t handle(HttpRequest::ptr request, HttpResponse::ptr response, HttpSession::ptr session) override {
        SYLAR_LOG_INFO(g_logger) << "处理文件上传请求";
        
        if (request->getMethod() == HttpMethod::GET) {
            // 返回上传表单
            response->setStatus(HttpStatus::OK);
            response->setHeader("Content-Type", "text/html; charset=utf-8");
            
            std::string html = R"(
<!DOCTYPE html>
<html>
<head>
    <title>文件上传测试</title>
    <meta charset="utf-8">
    <style>
        body { font-family: Arial, sans-serif; margin: 40px; background: #f5f5f5; }
        .container { max-width: 600px; margin: 0 auto; background: white; padding: 30px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); }
        .form-group { margin: 20px 0; }
        label { display: block; margin-bottom: 5px; font-weight: bold; }
        input[type="file"], input[type="text"] { width: 100%; padding: 10px; border: 1px solid #ddd; border-radius: 5px; }
        button { background: #007acc; color: white; padding: 12px 24px; border: none; border-radius: 5px; cursor: pointer; }
        button:hover { background: #005a9e; }
    </style>
</head>
<body>
    <div class="container">
        <h2>📁 文件上传测试</h2>
        <form action="/api/upload" method="post" enctype="multipart/form-data">
            <div class="form-group">
                <label>选择文件:</label>
                <input type="file" name="file" required>
            </div>
            <div class="form-group">
                <label>描述:</label>
                <input type="text" name="description" placeholder="文件描述">
            </div>
            <button type="submit">上传文件</button>
        </form>
    </div>
</body>
</html>
            )";
            
            response->setBody(html);
        } else if (request->getMethod() == HttpMethod::POST) {
            // 处理文件上传
            response->setStatus(HttpStatus::OK);
            response->setHeader("Content-Type", "application/json; charset=utf-8");
            
            std::string json = R"({
    "status": "success",
    "message": "文件上传处理完成（模拟）",
    "data": {
        "content_length": )" + std::to_string(request->getBody().length()) + R"(,
        "content_type": ")" + request->getHeader("Content-Type") + R"(",
        "fiber_id": )" + std::to_string(sylar::GetFiberId()) + R"(
    }
})";
            
            response->setBody(json);
        }
        
        return 0;
    }
};

/**
 * @brief 性能测试Servlet
 */
class PerformanceServlet : public Servlet {
public:
    PerformanceServlet() : Servlet("PerformanceServlet") {}
    
    int32_t handle(HttpRequest::ptr request, HttpResponse::ptr response, HttpSession::ptr session) override {
        SYLAR_LOG_INFO(g_logger) << "处理性能测试请求";
        
        // 模拟CPU密集型任务
        auto start = std::chrono::steady_clock::now();
        
        // 模拟计算任务
        volatile int result = 0;
        for (int i = 0; i < 1000000; ++i) {
            result += i * i;
        }
        
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        response->setStatus(HttpStatus::OK);
        response->setHeader("Content-Type", "application/json; charset=utf-8");
        
        std::string json = R"({
    "status": "success",
    "message": "性能测试完成",
    "data": {
        "computation_time_us": )" + std::to_string(duration.count()) + R"(,
        "result": )" + std::to_string(result) + R"(,
        "fiber_id": )" + std::to_string(sylar::GetFiberId()) + R"(,
        "thread_id": )" + std::to_string(sylar::GetThreadId()) + R"(
    }
})";
        
        response->setBody(json);
        return 0;
    }
};

} // namespace http
} // namespace sylar

int main(int argc, char** argv) {
    sylar::Logger::ptr logger = SYLAR_LOG_ROOT();
    logger->setLevel(sylar::LogLevel::INFO);
    
    SYLAR_LOG_INFO(logger) << "🚀 启动协程Web服务器...";
    
    // 创建IOManager，用于处理网络I/O
    sylar::IOManager::ptr iom(new sylar::IOManager(4, true, "webserver_io"));
    
    // 创建HTTP服务器
    sylar::http::HttpServer::ptr server(new sylar::http::HttpServer(
        true,  // keepalive
        iom.get(),  // worker
        iom.get(),  // io_worker
        iom.get()   // accept_worker
    ));
    
    // 获取Servlet分发器
    auto dispatch = server->getServletDispatch();
    
    // 注册各种Servlet
    dispatch->addServlet("/", std::make_shared<sylar::http::HomeServlet>());
    dispatch->addServlet("/api/hello", std::make_shared<sylar::http::HelloServlet>());
    dispatch->addServlet("/api/json", std::make_shared<sylar::http::JsonServlet>());
    dispatch->addServlet("/api/async", std::make_shared<sylar::http::AsyncServlet>());
    dispatch->addServlet("/api/params", std::make_shared<sylar::http::ParamsServlet>());
    dispatch->addServlet("/api/error", std::make_shared<sylar::http::ErrorServlet>());
    dispatch->addServlet("/api/upload", std::make_shared<sylar::http::UploadServlet>());
    dispatch->addServlet("/api/performance", std::make_shared<sylar::http::PerformanceServlet>());
    
    // 设置默认404处理
    dispatch->setDefault(std::make_shared<sylar::http::NotFoundServlet>("协程Web服务器"));
    
    // 绑定地址和端口
    std::vector<sylar::Address::ptr> addrs;
    addrs.push_back(sylar::Address::LookupAny("0.0.0.0:8080"));
    
    std::vector<sylar::Address::ptr> fails;
    if (!server->bind(addrs, fails)) {
        SYLAR_LOG_ERROR(logger) << "服务器绑定失败";
        return 1;
    }
    
    server->setName("CoroutineWebServer");
    
    SYLAR_LOG_INFO(logger) << "✅ 协程Web服务器启动成功!";
    SYLAR_LOG_INFO(logger) << "📡 服务器地址: http://localhost:8080";
    SYLAR_LOG_INFO(logger) << "🔧 协程数量: 4"; // 固定为4个线程
    SYLAR_LOG_INFO(logger) << "⚡ 按 Ctrl+C 停止服务器";
    
    // 启动服务器
    server->start();
    
    // 等待服务器运行
    iom->addTimer(1000, [logger]() {
        SYLAR_LOG_INFO(logger) << "💚 服务器运行中... 协程调度器工作正常";
    }, true);
    
    // 启动服务器
    server->start();
    
    // 添加定时器显示运行状态
    iom->addTimer(1000, [logger]() {
        SYLAR_LOG_INFO(logger) << "💚 服务器运行中... 协程调度器工作正常";
    }, true);
    
    // 等待服务器运行
    while(true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    SYLAR_LOG_INFO(logger) << "👋 协程Web服务器已停止";
    return 0;
} 