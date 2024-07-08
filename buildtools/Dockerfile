# Use an official GCC image from Docker Hub as a base image
FROM gcc:latest

# Set the working directory inside the container
WORKDIR /workspace

# Copy the CMakeLists.txt file (if needed) into the container
# COPY CMakeLists.txt .

# Install CMake (if not already installed in the base image)
RUN apt-get update && \
    apt-get install -y \
        cmake \
        && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Copy the entire current directory into the container
# COPY . .

# # Create a build directory and switch to it
# RUN mkdir build && cd build

# # Run CMake to configure the project
# RUN cmake ..

# # Build the project using make
# RUN make

# Example command to execute after the build (replace with your own)
CMD ["bash"]
