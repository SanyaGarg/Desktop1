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

## Development

### Prerequisites

- C++ compiler
- CMake
- libcurl

### Building Locally

To build the project locally, follow these steps:

1. Clone the repository:

```sh
git clone https://github.com/yourusername/http_load_test_cpp.git
cd http_load_test_cpp
```

2. Create a build directory and navigate into it:

```sh
mkdir build
cd build
```

3. Run CMake and build the project:

```sh
cmake ..
make
```

4. Run the application:

```sh
./http_load_test_cpp --url http://example.com --qps 10 --duration 30
```