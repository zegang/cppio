# Use an official GCC image from Docker Hub as a base image
FROM docker.io/gcc:latest

# Set the working directory inside the container
WORKDIR /workspace

# Install CMake (if not already installed in the base image)
COPY buildtools/cmake-3.31.4-linux-x86_64.sh /workspace
RUN sh /workspace/cmake-3.31.4-linux-x86_64.sh --skip-license --prefix=/usr

# Example command to execute after the build (replace with your own)
CMD ["bash"]
