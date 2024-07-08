#ifndef GOINCPP_IO_WRITER_HPP
#define GOINCPP_IO_WRITER_HPP

#include <iostream>
#include <fstream>
#include <vector>

namespace goincpp {

namespace io {

class Writer {
private:
    std::ostream& os;

public:
    explicit Writer(std::ostream& stream) : os(stream) {}

    template<typename T>
    void Write(const T& data) {
        os.write(reinterpret_cast<const char*>(&data), sizeof(data));
    }

    void Write(const std::string& str) {
        os << str;
    }

    void Write(const char* data) {
        os << data;
    }

    void Write(const std::vector<char>& buffer) {
        os.write(buffer.data(), buffer.size());
    }

    // Add more overloads as needed for different types of data

    // Example of a method that writes bytes
    void WriteBytes(const char* buffer, size_t size) {
        os.write(buffer, size);
    }

    // Example of a method that returns the underlying std::ostream
    std::ostream& GetStream() {
        return os;
    }
};

}
}

#endif // GOINCPP_IO_WRITER_HPP