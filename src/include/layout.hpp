#ifndef CPPIO_LAYOUT_HPP
#define CPPIO_LAYOUT_HPP

namespace cppio {

struct PoolDisksLayout {
    std::string                                 cmdLine;
    std::vector< std::vector< std::string > >   layout;

public:
    bool isSingleDriveLayout() {
        return layout.size() == 1 && layout[0].size() == 1;
    }

    bool isEmptyLayout() {
        return layout.size() == 0 || layout[0].size() == 0 || layout[0][0].empty();
    }
};

// Define DisksLayout struct
struct DisksLayout {
    bool legacy;
    std::vector<PoolDisksLayout> pools;
};

// Define serverCtxt struct
struct ServerCtxt {
    bool json;
    bool quiet;
    bool anonymous;
    bool strictS3Compat;
    std::string addr, consoleAddr, configDir, certsDir;
    bool configDirSet, certsDirSet;
    std::string interface;
    std::string rootUser, rootPwd;
    std::vector<std::string> ftp, sftp;
    std::chrono::seconds userTimeout;
    std::chrono::seconds connReadDeadline;
    std::chrono::seconds connWriteDeadline;
    std::chrono::seconds connClientReadDeadline;
    std::chrono::seconds connClientWriteDeadline;
    std::chrono::seconds shutdownTimeout, idleTimeout, readHeaderTimeout;
    int maxIdleConnsPerHost;
    // Nested struct
    DisksLayout layout;

    // Constructor for initialization
    ServerCtxt() : json(false), quiet(false), anonymous(false), strictS3Compat(false),
                   configDirSet(false), certsDirSet(false), maxIdleConnsPerHost(0) {}
};

}

#endif // CPPIO_LAYOUT_HPP