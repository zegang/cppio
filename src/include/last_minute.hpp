#ifndef CPPIO_LAST_MINUTE_HPP
#define CPPIO_LAST_MINUTE_HPP

#include <iostream>
#include <string>

namespace cppio {


// Enum for size tags
enum SizeTag {
    sizeLessThan1KiB,
    sizeLessThan1MiB,
    sizeLessThan10MiB,
    sizeLessThan100MiB,
    sizeLessThan1GiB,
    sizeGreaterThan1GiB,
    sizeLastElemMarker
};

// Function to convert size to tag
SizeTag sizeToTag(int64_t size) {
    if (size < 1024)
        return sizeLessThan1KiB;
    else if (size < 1024 * 1024)
        return sizeLessThan1MiB;
    else if (size < 10 * 1024 * 1024)
        return sizeLessThan10MiB;
    else if (size < 100 * 1024 * 1024)
        return sizeLessThan100MiB;
    else if (size < 1024 * 1024 * 1024)
        return sizeLessThan1GiB;
    else
        return sizeGreaterThan1GiB;
}

// Function to convert tag to string
std::string sizeTagToString(SizeTag tag) {
    switch (tag) {
        case sizeLessThan1KiB:
            return "LESS_THAN_1_KiB";
        case sizeLessThan1MiB:
            return "LESS_THAN_1_MiB";
        case sizeLessThan10MiB:
            return "LESS_THAN_10_MiB";
        case sizeLessThan100MiB:
            return "LESS_THAN_100_MiB";
        case sizeLessThan1GiB:
            return "LESS_THAN_1_GiB";
        case sizeGreaterThan1GiB:
            return "GREATER_THAN_1_GiB";
        default:
            return "unknown";
    }
}

// AccElem holds information for calculating an average value
struct AccElem {
	uint64_t total;
	uint64_t size;
    uint64_t n;
};

}

#endif // CPPIO_LAST_MINUTE_HPP