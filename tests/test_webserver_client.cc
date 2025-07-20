/**
 * @file test_webserver_client.cc
 * @brief HTTP客户端测试程序
 * @author AI Assistant
 * @date 2024-01-01
 * @brief 用于测试协程Web服务器的并发性能
 */

#include "sylar/sylar.h"
#include "sylar/http/http_connection.h"
#include "sylar/log.h"
#include "sylar/iomanager.h"
#include "sylar/address.h"
#include <iostream>
#include <vector>
#include <chrono>
#include <atomic>
#include <thread>

static sylar::Logger::ptr g_logger = SYLAR_LOG_NAME("client");

namespace sylar {
namespace http {

class HttpClient {
public:
    HttpClient(const std::string& host, int port) 
        : m_host(host), m_port(port) {}
    
    bool request(const std::string& path, std::string& response) {
        try {
            sylar::Address::ptr addr = sylar::Address::LookupAnyIPAddress(m_host + ":" + std::to_string(m_port));
            if (!addr) {
                SYLAR_LOG_ERROR(g_logger) << "无法解析地址: " << m_host << ":" << m_port;
                return false;
            }
            
            sylar::Socket::ptr sock = sylar::Socket::CreateTCP(addr);
            if (!sock) {
                SYLAR_LOG_ERROR(g_logger) << "创建Socket失败";
                return false;
            }
            
            if (!sock->connect(addr)) {
                SYLAR_LOG_ERROR(g_logger) << "连接失败: " << *addr;
                return false;
            }
            
            sylar::http::HttpConnection::ptr conn(new sylar::http::HttpConnection(sock));
            
            sylar::http::HttpRequest::ptr req(new sylar::http::HttpRequest);
            req->setPath(path);
            req->setHeader("Host", m_host + ":" + std::to_string(m_port));
            req->setHeader("User-Agent", "CoroutineTestClient/1.0");
            
            auto rsp = conn->recvResponse();
            if (!rsp) {
                SYLAR_LOG_ERROR(g_logger) << "接收响应失败";
                return false;
            }
            
            response = rsp->getBody();
            return true;
        } catch (const std::exception& e) {
            SYLAR_LOG_ERROR(g_logger) << "请求异常: " << e.what();
            return false;
        }
    }
    
private:
    std::string m_host;
    int m_port;
};

} // namespace http
} // namespace sylar

// 全局统计变量
std::atomic<int> g_success_count{0};
std::atomic<int> g_failed_count{0};
std::atomic<int64_t> g_total_time{0};

void test_single_request(const std::string& host, int port, const std::string& path, int request_id) {
    sylar::http::HttpClient client(host, port);
    std::string response;
    
    auto start = std::chrono::steady_clock::now();
    bool success = client.request(path, response);
    auto end = std::chrono::steady_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    if (success) {
        g_success_count++;
        SYLAR_LOG_DEBUG(g_logger) << "请求 " << request_id << " 成功, 耗时: " << duration.count() << "us";
    } else {
        g_failed_count++;
        SYLAR_LOG_ERROR(g_logger) << "请求 " << request_id << " 失败";
    }
    
    g_total_time += duration.count();
}

void test_concurrent_requests(const std::string& host, int port, const std::string& path, int concurrency, int total_requests) {
    SYLAR_LOG_INFO(g_logger) << "开始并发测试: " << concurrency << " 并发, " << total_requests << " 总请求数";
    
    auto start = std::chrono::steady_clock::now();
    
    std::vector<std::thread> threads;
    int requests_per_thread = total_requests / concurrency;
    int remaining_requests = total_requests % concurrency;
    
    for (int i = 0; i < concurrency; ++i) {
        int thread_requests = requests_per_thread + (i < remaining_requests ? 1 : 0);
        threads.emplace_back([host, port, path, thread_requests, i, requests_per_thread]() {
            for (int j = 0; j < thread_requests; ++j) {
                int request_id = i * requests_per_thread + j;
                test_single_request(host, port, path, request_id);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    auto end = std::chrono::steady_clock::now();
    auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    SYLAR_LOG_INFO(g_logger) << "并发测试完成!";
    SYLAR_LOG_INFO(g_logger) << "总耗时: " << total_duration.count() << "ms";
    SYLAR_LOG_INFO(g_logger) << "成功请求: " << g_success_count.load() << "/" << total_requests;
    SYLAR_LOG_INFO(g_logger) << "失败请求: " << g_failed_count.load();
    SYLAR_LOG_INFO(g_logger) << "平均响应时间: " << (g_total_time.load() / g_success_count.load()) << "us";
    SYLAR_LOG_INFO(g_logger) << "QPS: " << (g_success_count.load() * 1000.0 / total_duration.count());
}

void test_different_endpoints(const std::string& host, int port) {
    SYLAR_LOG_INFO(g_logger) << "测试不同端点的性能...";
    
    std::vector<std::string> endpoints = {
        "/api/hello",
        "/api/json", 
        "/api/async",
        "/api/performance"
    };
    
    for (const auto& endpoint : endpoints) {
        SYLAR_LOG_INFO(g_logger) << "测试端点: " << endpoint;
        
        // 重置统计
        g_success_count = 0;
        g_failed_count = 0;
        g_total_time = 0;
        
        test_concurrent_requests(host, port, endpoint, 10, 100);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main(int argc, char** argv) {
    sylar::Logger::ptr logger = SYLAR_LOG_ROOT();
    logger->setLevel(sylar::LogLevel::INFO);
    
    std::string host = "127.0.0.1";
    int port = 8080;
    
    if (argc >= 3) {
        host = argv[1];
        port = std::stoi(argv[2]);
    }
    
    SYLAR_LOG_INFO(logger) << "🧪 开始协程Web服务器性能测试";
    SYLAR_LOG_INFO(logger) << "📡 目标服务器: " << host << ":" << port;
    
    // 测试基本连接
    SYLAR_LOG_INFO(logger) << "🔗 测试基本连接...";
    sylar::http::HttpClient client(host, port);
    std::string response;
    
    if (client.request("/api/hello", response)) {
        SYLAR_LOG_INFO(logger) << "✅ 基本连接测试成功";
        SYLAR_LOG_DEBUG(logger) << "响应: " << response.substr(0, 100) << "...";
    } else {
        SYLAR_LOG_ERROR(logger) << "❌ 基本连接测试失败，请确保服务器正在运行";
        return 1;
    }
    
    // 测试不同并发级别
    std::vector<int> concurrency_levels = {1, 5, 10, 20, 50};
    
    for (int concurrency : concurrency_levels) {
        SYLAR_LOG_INFO(logger) << "\n" << std::string(50, '=');
        SYLAR_LOG_INFO(logger) << "测试并发级别: " << concurrency;
        SYLAR_LOG_INFO(logger) << std::string(50, '=');
        
        // 重置统计
        g_success_count = 0;
        g_failed_count = 0;
        g_total_time = 0;
        
        test_concurrent_requests(host, port, "/api/json", concurrency, 200);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
    
    // 测试不同端点
    SYLAR_LOG_INFO(logger) << "\n" << std::string(50, '=');
    SYLAR_LOG_INFO(logger) << "测试不同端点性能";
    SYLAR_LOG_INFO(logger) << std::string(50, '=');
    
    test_different_endpoints(host, port);
    
    // 压力测试
    SYLAR_LOG_INFO(logger) << "\n" << std::string(50, '=');
    SYLAR_LOG_INFO(logger) << "压力测试: 1000个请求";
    SYLAR_LOG_INFO(logger) << std::string(50, '=');
    
    g_success_count = 0;
    g_failed_count = 0;
    g_total_time = 0;
    
    test_concurrent_requests(host, port, "/api/hello", 50, 1000);
    
    SYLAR_LOG_INFO(logger) << "\n🎉 性能测试完成!";
    SYLAR_LOG_INFO(logger) << "📊 测试结果总结:";
    SYLAR_LOG_INFO(logger) << "   - 总成功请求: " << g_success_count.load();
    SYLAR_LOG_INFO(logger) << "   - 总失败请求: " << g_failed_count.load();
    SYLAR_LOG_INFO(logger) << "   - 成功率: " << (g_success_count.load() * 100.0 / (g_success_count.load() + g_failed_count.load())) << "%";
    
    return 0;
} 