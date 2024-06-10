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
#include <string>
#include <signal.h>
#include <getopt.h>
#include <curl/curl.h>

class LoadTester {
public:
    LoadTester(const std::string& url, int qps, int duration, const std::string& method, const std::string& headers, const std::string& payload)
        : url(url), qps(qps), duration(duration), method(method), headers(headers), payload(payload), errors(0), stop_flag(false) {}

    void make_request() {
        CURL* curl;
        CURLcode res;
        curl = curl_easy_init();
        if(curl) {
            auto start = std::chrono::high_resolution_clock::now();
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());

            struct curl_slist* header_list = nullptr;
            if (!headers.empty()) {
                header_list = curl_slist_append(header_list, headers.c_str());
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);
            }

            if (method == "POST" || method == "PUT") {
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payload.c_str());
            }

            res = curl_easy_perform(curl);
            auto end = std::chrono::high_resolution_clock::now();

            std::chrono::duration<double> latency = end - start;
            if(res != CURLE_OK) {
                errors++;
            } else {
                std::lock_guard<std::mutex> lock(latencies_mutex);
                latencies.push_back(latency.count());
            }

            if (header_list) {
                curl_slist_free_all(header_list);
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
        if (latencies.empty()) {
            std::cout << "No successful requests.\n";
            return;
        }

        std::sort(latencies.begin(), latencies.end());
        double avg_latency = std::accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size();
        double max_latency = *std::max_element(latencies.begin(), latencies.end());
        double min_latency = *std::min_element(latencies.begin(), latencies.end());
        double p90_latency = latencies[static_cast<int>(latencies.size() * 0.9)];
        double p99_latency = latencies[static_cast<int>(latencies.size() * 0.99)];

        std::cout << "Total Requests: " << latencies.size() + errors << "\n";
        std::cout << "Successful Requests: " << latencies.size() << "\n";
        std::cout << "Failed Requests: " << errors << "\n";
        std::cout << "Average Latency: " << avg_latency << " seconds\n";
        std::cout << "Max Latency: " << max_latency << " seconds\n";
        std::cout << "Min Latency: " << min_latency << " seconds\n";
        std::cout << "90th Percentile Latency: " << p90_latency << " seconds\n";
        std::cout << "99th Percentile Latency: " << p99_latency << " seconds\n";
        std::cout << "Total Duration: " << total_duration << " seconds\n";
    }

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
};

void print_usage() {
    std::cout << "Usage: ./http_load_test_cpp --url <URL> --qps <QPS> --duration <Duration> [--method <HTTP_METHOD>] [--headers <HTTP_HEADERS>] [--payload <HTTP_PAYLOAD>]\n";
}

int main(int argc, char** argv) {
    if (argc < 7) {
        print_usage();
        return 1;
    }

    std::string url, method = "GET", headers, payload;
    int qps, duration;

    const struct option longopts[] = {
        {"url", required_argument, 0, 'u'},
        {"qps", required_argument, 0, 'q'},
        {"duration", required_argument, 0, 'd'},
        {"method", optional_argument, 0, 'm'},
        {"headers", optional_argument, 0, 'h'},
        {"payload", optional_argument, 0, 'p'},
        {0, 0, 0, 0}
    };

    int index;
    int iarg = 0;
    while ((iarg = getopt_long(argc, argv, "u:q:d:m:h:p:", longopts, &index)) != -1) {
        switch (iarg) {
            case 'u':
                url = optarg;
                break;
            case 'q':
                qps = std::stoi(optarg);
                break;
            case 'd':
                duration = std::stoi(optarg);
                break;
            case 'm':
                method = optarg;
                break;
            case 'h':
                headers = optarg;
                break;
            case 'p':
                payload = optarg;
                break;
            default:
                print_usage();
                return 1;
        }
    }

    if (url.empty() || qps <= 0 || duration <= 0) {
        print_usage();
        return 1;
    }

    LoadTester tester(url, qps, duration, method, headers, payload);
    tester.start_test();

    return 0;
}