# Use an official C++ runtime as a parent image
FROM gcc:10

# Set the working directory
WORKDIR /usr/src/app

# Copy the current directory contents into the container at /usr/src/app
COPY . .

# Install CMake and Curl
RUN apt-get update && apt-get install -y cmake libcurl4-openssl-dev

# Create a build directory
RUN mkdir -p build

# Build the project
WORKDIR /usr/src/app/build
RUN cmake ..
RUN cmake --build .

# Run the application
CMD ["./http_load_test_cpp", "--url", "http://example.com", "--qps", "10", "--duration", "30", "--method", "GET"]
