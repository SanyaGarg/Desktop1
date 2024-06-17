#ifndef LOAD_TESTER_H
#define LOAD_TESTER_H

#include <string>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

class LoadTester {
public:
    LoadTester(const std::string& url, int qps, int duration, const std::string& method, const std::string& headers, const std::string& payload);

    void make_request();
    void worker();
    void start_test();
    void stop_test();
    void report(double total_duration);

    // Getter methods for testing purposes
    std::string get_url() const;
    int get_qps() const;
    int get_duration() const;
    std::string get_method() const;
    int get_request_queue_size() const;
    int get_errors() const;
    int get_latencies_size() const;

    // Method to simulate failure for testing purposes
    void simulate_failure();

private:
    std::string url;
    int qps;
    int duration;
    std::string method;
    std::string headers;
    std::string payload;
    std::atomic<int> errors;
    std::vector<double> latencies;
    std::mutex latencies_mutex;

    std::queue<int> request_queue;
    std::mutex queue_mutex;
    std::condition_variable queue_cv;
    std::atomic<bool> stop_flag;

    bool simulate_failure_flag;
};

void print_usage();

#endif // LOAD_TESTER_H
