#ifndef CPPIO_ENDPOINT_HPP
#define CPPIO_ENDPOINT_HPP

#include <boost/url/url.hpp>
#include <boost/url/parse.hpp>
#include <boost/system/result.hpp>
#include <filesystem>
#include "layout.hpp"

namespace cppio {

class Node;                 // Node holds information about a node in this cluster
class Endpoint;             // Endpoint - any type of endpoint.
class PoolEndpoints;        // PoolEndpoints represent endpoints in a given pool
                            // along with its setCount and setDriveCount.
class EndpointServerPools;  // EndpointServerPools - list of list of endpoints

extern EndpointServerPools globalEndpoints;

enum EndpointType {
    PathEndpointType,
    URLEndpointType
};

class Node {

protected:
    boost::urls::url    url;
    std::vector<int>    pools;
    bool                isLocal;
    std::string         gridHost;
};

class Endpoint {

public:
    Endpoint(std::string uri) {
        // if (arg.empty()) {
        //     throw std::invalid_argument("Empty URI");
        // }
        // boost::system::result< url > ru = boost::urls::parse_uri_reference(uri);
        // if (ru) {
        //     url = *ru;
        //     isLocal = false;
        // } else {
        //     // Not an URI, try on Path
        //     path = uri;
        //     isLocal = true;
        // }
    }

public:
    // bool equal(Endpoint* ep) {
    //     if (isLocal == ep->isLocal && poolIdx == ep->poolIdx &&
    //         setIdx == ep->poolIdx && diskIdx == ep->diskIdx) {
    //             if (url.path == ep->url.path && url.host == ep->url.host) {
    //                 return true;
    //             }
    //     }
    //     return false;
    // }

    // EndpointType type() {
    //     if (url.host == "") {
    //         return PathEndpointType;
    //     }
    //     return URLEndpointType;
    // }

public:
    boost::urls::url        url;
    std::filesystem::path   path;
    bool                    isLocal     = true;
    int                     poolIdx     = -1;
    int                     setIdx      = -1;
    int                     diskIdx     = -1;
};

class PoolEndpoints {

public:
    PoolEndpoints(std::string& serverAddr, PoolDisksLayout& pdsl) {

        if (pdsl.isSingleDriveLayout()) {
            endpoints.push_back(Endpoint(pdsl.layout[0][0]));
            assert(endpoints.size() == 1);
            endpoints[0].poolIdx    = 0;
            endpoints[0].setIdx     = 0;
            endpoints[0].diskIdx    = 0;
        }

        setCount        = pdsl.layout.size();
        driversPerSet   = pdsl.layout[0].size();  
        cmdLine         = pdsl.cmdLine;
        platform        = "OS: linux | Arch: "; 
    }

public:
    bool                    legacy;
    int                     setCount;
    int                     driversPerSet;
    std::vector<Endpoint>   endpoints;
    std::string             cmdLine;
    std::string             platform;
};

class EndpointServerPools {

public:
    EndpointServerPools(std::string serverAddr, std::vector<PoolDisksLayout> poolArgs, bool legacy) {
        if (poolArgs.size() == 0) {
            throw std::invalid_argument("EndpointServerPools: No PoolDisksLayout");
        }

        for (auto& pool : poolArgs) {
            poolEndpoints.push_back(PoolEndpoints(serverAddr, pool));
        }
    }

    std::vector<Node> getNodes() {
        std::vector<Node> nodes;
        return nodes;
    }

    int getPoolIdx(std::string pool) {
        int idx = 0;
        for(; idx < poolEndpoints.size(); ++idx) {
            if (poolEndpoints[idx].cmdLine == pool) {
                return idx;
            }
        }
        return -1;
    }

    void add(PoolEndpoints peps) {
        poolEndpoints.push_back(peps);
    }

protected:
    std::vector<PoolEndpoints> poolEndpoints;
};

}

#endif