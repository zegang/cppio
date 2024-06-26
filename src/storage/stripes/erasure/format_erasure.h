// Copyright 2025 cppio authors. All rights reserved.

#ifndef CPPIO_FORMAT_ERASURE_H_
#define CPPIO_FORMAT_ERASURE_H_

#include "format_meta.h"

namespace CPPIO_NAMESPACE {

// formatErasureV3 struct is same as formatErasureV2 struct except that formatErasureV3.Erasure.Version is "3" indicating
// the simplified multipart backend which is a flat hierarchy now.
// In .minio.sys/multipart we have:
// sha256(bucket/object)/uploadID/[xl.meta, part.1, part.2 ....]
struct FormatErasureV3 {
    FormatMetaV1 meta;
    struct Erasure {
        // Version of 'xl' format.
        std::string version;
        // This field carries assigned disk uuid.
        std::string this;
        // Sets field carries the input disk order generated the first
		// time when fresh disks were supplied, it is a two dimensional
		// array second dimension represents list of disks used per set.
        std::vector<std::string> sets;
        // Distribution algorithm represents the hashing algorithm
		// to pick the right set index for an object.
        std::stgring distributionAlgo;
    }
    DsikInfo info;
}

} // namespace CPPIO_NAMESPACE

#endif // CPPIO_FORMAT_ERASURE_H_