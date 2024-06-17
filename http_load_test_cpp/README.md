### README.md

# HTTP Load Testing and Benchmarking Library

This repository contains a general-purpose HTTP load-testing and benchmarking tool written in C++. The tool allows you to test the performance of an HTTP endpoint by generating a specified number of requests per second (QPS) over a given duration. It reports various metrics such as latencies and error rates.

## Features

- Configurable HTTP request method (GET, POST, PUT, etc.)
- Custom headers and payload support
- Reports average, maximum, minimum, 90th percentile, and 99th percentile latencies
- Reports total, successful, and failed requests
- Dockerized for easy deployment and usage

## Requirements

- Docker

## Build and Run

### Building the Docker Image

To build the Docker image, navigate to the project directory and run the following command:

```sh
docker build -t http_load_test_cpp .
```

### Running the Docker Container

To run the Docker container with the load-testing tool, use the following command:

```sh
docker run --rm http_load_test_cpp --url <URL> --qps <QPS> --duration <DURATION> [--method <HTTP_METHOD>] [--headers <HTTP_HEADERS>] [--payload <HTTP_PAYLOAD>]
```

### Example Usage

Here is an example command to run the load test against `http://example.com` with a QPS of 10 for a duration of 30 seconds:

```sh
docker run --rm http_load_test_cpp --url http://example.com --qps 10 --duration 30
```

If you want to specify a custom HTTP method, headers, and payload, you can use the following command:

```sh
docker run --rm http_load_test_cpp --url http://example.com --qps 10 --duration 30 --method POST --headers "Content-Type: application/json" --payload '{"key": "value"}'
```

## Command-Line Options

- `--url`: The URL to send requests to (required)
- `--qps`: The number of queries per second to generate (required)
- `--duration`: The duration of the test in seconds (required)
- `--method`: The HTTP method to use (optional, default: GET)
- `--headers`: Custom headers to include in the request (optional)
- `--payload`: Payload for POST or PUT requests (optional)

## Metrics Reported

- **Total Requests**: The total number of requests sent.
- **Successful Requests**: The number of successful requests.
- **Failed Requests**: The number of failed requests.
- **Average Latency**: The average latency of successful requests.
- **Max Latency**: The maximum latency of successful requests.
- **Min Latency**: The minimum latency of successful requests.
- **90th Percentile Latency**: The 90th percentile latency of successful requests.
- **99th Percentile Latency**: The 99th percentile latency of successful requests.
- **Total Duration**: The total duration of the test.

## Improved QPS Handling for HTTP Load Testing

### Issue
The current implementation pushes requests into the queue in bursts at the start of each second, leading to uneven load distribution.

### Solution
To evenly distribute requests across each second, we use a timer-based approach to pace request generation.

### Implementation Details
1. **Calculate Request Interval**: Determine the interval (`request_interval`) between each request based on the desired QPS.
   
2. **Use Timer for Pacing**: Instead of pushing all requests into the queue at once, use a timer to add requests at regular intervals over the second.

3. **Adjust Worker Threads**: Ensure worker threads handle requests as they become available, maintaining a steady flow throughout the test duration.
