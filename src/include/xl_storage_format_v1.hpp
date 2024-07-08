#ifndef CPPIO_XL_STORAGE_FROMAT_V1_HPP
#define CPPIO_XL_STORAGE_FROMAT_V1_HPP

#include <iostream>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <cstdint>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace pt = boost::property_tree;

namespace cppio {

// BitrotAlgorithm specifies an algorithm used for bitrot protection.
enum class BitrotAlgorithm : uint32_t {
    CRC32 = 1,
    CRC64,
    SHA256,
    // Add more algorithms here as needed
};

// ChecksumInfo - carries checksums of individual scattered parts per disk.
struct ChecksumInfo {
    int PartNumber;
    BitrotAlgorithm Algorithm;
    std::vector<uint8_t> Hash;
};

// ErasureInfo holds erasure coding and bitrot related information.
struct ErasureInfo {
    std::string algorithm;       // Algorithm name
    int dataBlocks;              // Number of data blocks
    int parityBlocks;            // Number of parity blocks
    int64_t blockSize;           // Size of each block
    int index;                   // Index of the disk
    std::vector<int> distribution; // Distribution of data and parity blocks
    std::vector<ChecksumInfo> checksums; // Checksum information

    // Method to serialize to Boost Property Tree
    pt::ptree to_ptree() const {
        pt::ptree tree;
        tree.put("algorithm", algorithm);
        tree.put("data", dataBlocks);
        tree.put("parity", parityBlocks);
        tree.put("blockSize", blockSize);
        tree.put("index", index);
        
        // Serialize distribution
        pt::ptree distributionTree;
        for (size_t i = 0; i < distribution.size(); ++i) {
            distributionTree.put("", distribution[i]);
        }
        tree.add_child("distribution", distributionTree);

        // Serialize checksums
        pt::ptree checksumsTree;
        for (const auto& checksum : checksums) {
            pt::ptree checksumTree;
            checksumTree.put("type", checksum.type);
            checksumTree.put("value", checksum.value);
            checksumsTree.push_back(std::make_pair("", checksumTree));
        }
        tree.add_child("checksum", checksumsTree);

        return tree;
    }

    // Method to deserialize from Boost Property Tree
    static ErasureInfo from_ptree(const pt::ptree& tree) {
        ErasureInfo erasureInfo;
        erasureInfo.algorithm = tree.get<std::string>("algorithm");
        erasureInfo.dataBlocks = tree.get<int>("data");
        erasureInfo.parityBlocks = tree.get<int>("parity");
        erasureInfo.blockSize = tree.get<int64_t>("blockSize");
        erasureInfo.index = tree.get<int>("index");

        // Deserialize distribution
        erasureInfo.distribution.clear();
        for (const auto& pair : tree.get_child("distribution")) {
            erasureInfo.distribution.push_back(pair.second.get_value<int>());
        }

        // Deserialize checksums
        erasureInfo.checksums.clear();
        for (const auto& pair : tree.get_child("checksum")) {
            ChecksumInfo checksum;
            checksum.type = pair.second.get<std::string>("type");
            checksum.value = pair.second.get<std::string>("value");
            erasureInfo.checksums.push_back(checksum);
        }

        return erasureInfo;
    }
};

// ObjectPartInfo Info of each part kept in the multipart metadata
// file after CompleteMultipartUpload() is called.
struct ObjectPartInfo {
    std::string etag;                               // ETag of the object part
    int number;                                     // Part number
    int64_t size;                                   // Size of the part on disk
    int64_t actualSize;                             // Original size of the part without compression or encryption bytes
    std::chrono::system_clock::time_point modTime;  // Date and time when the part was uploaded
    std::vector<uint8_t> index;                     // Index of the part (optional)
    std::unordered_map<std::string, std::string> checksums; // Content checksums

    // Method to serialize to JSON
    pt::ptree to_ptree() const {
        pt::ptree tree;
        tree.put("etag", etag);
        tree.put("number", number);
        tree.put("size", size);
        tree.put("actualSize", actualSize);
        tree.put("modTime", std::chrono::system_clock::to_time_t(modTime));
        
        // Serialize index as base64 string
        std::ostringstream oss;
        for (auto byte : index) {
            oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(byte);
        }
        tree.put("index", oss.str());

        // Serialize checksums
        pt::ptree checksumsTree;
        for (const auto& pair : checksums) {
            checksumsTree.put(pair.first, pair.second);
        }
        tree.add_child("checksums", checksumsTree);

        return tree;
    }

    // Method to deserialize from JSON
    static ObjectPartInfo from_ptree(const pt::ptree& tree) {
        ObjectPartInfo partInfo;
        partInfo.etag = tree.get<std::string>("etag");
        partInfo.number = tree.get<int>("number");
        partInfo.size = tree.get<int64_t>("size");
        partInfo.actualSize = tree.get<int64_t>("actualSize");
        partInfo.modTime = std::chrono::system_clock::from_time_t(tree.get<std::time_t>("modTime"));

        // Deserialize index from base64 string
        std::string indexHexStr = tree.get<std::string>("index");
        partInfo.index.clear();
        for (size_t i = 0; i < indexHexStr.size(); i += 2) {
            std::string byteString = indexHexStr.substr(i, 2);
            uint8_t byte = static_cast<uint8_t>(std::stoul(byteString, nullptr, 16));
            partInfo.index.push_back(byte);
        }

        // Deserialize checksums
        for (const auto& pair : tree.get_child("checksums")) {
            partInfo.checksums[pair.first] = pair.second.get_value<std::string>();
        }

        return partInfo;
    }
};

}

#endif // CPPIO_XL_STORAGE_FROMAT_V1_HPP