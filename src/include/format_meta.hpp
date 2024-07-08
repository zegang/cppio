#ifndef CPPIO_FORMAT_META_HPP
#define CPPIO_FORMAT_META_HPP

namespace cppio {

const std::string formatConfigFile = "format.json";
const std::string formatMetaVersionV1 = "1";

// format.json currently has the format:
// {
//   "version": "1",
//   "format": "XXXXX",
//   "XXXXX": {
//
//   }
// }
// Here "XXXXX" depends on the backend, currently we have "fs" and "xl" implementations.
// formatMetaV1 should be inherited by backend format structs. Please look at format-fs.go
// and format-xl.go for details.

// Ideally we will never have a situation where we will have to change the
// fields of this struct and deal with related migration.
struct FormatMetaV1 {
    // Version of the format config.
    std::string version;
    // Format indicates the backend format type, supports two values 'xl' and 'fs'.
    std::string fromat;
    // ID is the identifier for the cppio deployment
    std::string id;
}

}

#endif // CPPIO_FORMAT_META_HPP