#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <numeric>
#include <algorithm>
#include <curl/curl.h>

// Helper function to calculate statistics
void calculate_stats(const std::vector<double>& latencies) {
    double avg_latency = std::accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size();
    double max_latency = *std::max_element(latencies.begin(), latencies.end());
    double min_latency = *std::min_element(latencies.begin(), latencies.end());

    std::cout << "Average Latency: " << avg_latency << " seconds\n";
    std::cout << "Max Latency: " << max_latency << " seconds\n";
    std::cout << "Min Latency: " << min_latency << " seconds\n";
}

class LoadTester {
public:
    LoadTester(const std::string& url, int qps, int duration)
        : url(url), qps(qps), duration(duration), errors(0), stop_flag(false) {}

    void make_request() {
        CURL* curl;
        CURLcode res;
        curl = curl_easy_init();
        if(curl) {
            auto start = std::chrono::high_resolution_clock::now();
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            res = curl_easy_perform(curl);
            auto end = std::chrono::high_resolution_clock::now();

            std::chrono::duration<double> latency = end - start;
            if(res != CURLE_OK) {
                errors++;
            } else {
                std::lock_guard<std::mutex> lock(latencies_mutex);
                latencies.push_back(latency.count());
            }
            curl_easy_cleanup(curl);
        } else {
            errors++;
        }
    }

    void worker() {
        while (!stop_flag) {
            std::unique_lock<std::mutex> lock(queue_mutex);
            queue_cv.wait(lock, [this] { return !request_queue.empty() || stop_flag; });

            if (!request_queue.empty()) {
                request_queue.pop();
                lock.unlock();
                make_request();
            }
        }
    }

    void start_test() {
        auto start = std::chrono::high_resolution_clock::now();
        std::vector<std::thread> threads;
        
        for (int i = 0; i < qps; ++i) {
            threads.emplace_back(&LoadTester::worker, this);
        }
        
        for (int i = 0; i < duration; ++i) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
            {
                std::lock_guard<std::mutex> lock(queue_mutex);
                for (int j = 0; j < qps; ++j) {
                    request_queue.push(j);
                }
            }
            queue_cv.notify_all();
        }

        stop_flag = true;
        queue_cv.notify_all();
        for (auto& thread : threads) {
            thread.join();
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> total_duration = end - start;
        report(total_duration.count());
    }

    void report(double total_duration) {
        std::lock_guard<std::mutex> lock(latencies_mutex);
        double avg_latency = std::accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size();
        double max_latency = *std::max_element(latencies.begin(), latencies.end());
        double min_latency = *std::min_element(latencies.begin(), latencies.end());
        
        std::cout << "Total Requests: " << latencies.size() + errors << "\n";
        std::cout << "Successful Requests: " << latencies.size() << "\n";
        std::cout << "Failed Requests: " << errors << "\n";
        std::cout << "Average Latency: " << avg_latency << " seconds\n";
        std::cout << "Max Latency: " << max_latency << " seconds\n";
        std::cout << "Min Latency: " << min_latency << " seconds\n";
        std::cout << "Total Duration: " << total_duration << " seconds\n";
    }

private:
    std::string url;
    int qps;
    int duration;
    std::atomic<int> errors;
    std::vector<double> latencies;
    std::mutex latencies_mutex;

    std::queue<int> request_queue;
    std::mutex queue_mutex;
    std::condition_variable queue_cv;
    std::atomic<bool> stop_flag;
};

int main(int argc, char** argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <URL> <QPS> <Duration>\n";
        return 1;
    }
    
    std::string url = argv[1];
    int qps = std::stoi(argv[2]);
    int duration = std::stoi(argv[3]);

    LoadTester tester(url, qps, duration);
    tester.start_test();
    
    return 0;
}
