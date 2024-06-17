# Use an official Ubuntu runtime as a parent image
FROM gcc:10

# Install required dependencies
RUN apt-get update && \
apt-get install -y cmake g++ curl libcurl4-openssl-dev

# Copy the current directory contents into the container at /app
COPY . /app

# Create and set the working directory
WORKDIR /app

# Run CMake to configure the project
RUN cmake . && make

# Set the entry point to the built executable
ENTRYPOINT ["./http_load_test_cpp_update"]

# Run the application
CMD ["./http_load_test_cpp_update", "--url", "http://example.com", "--qps", "10", "--duration", "30", "--method", "GET"]