// Copyright 2025 cppio authors. All rights reserved.

#include <csignal>

#include "hsd.h"
#include "storage_interface.h"
#include "log.h"

using namespace CPPIO_NAMESPACE;

HSD* hsd = nullptr;

void sigTermHandler(int arg) {
    hsd->Stop();
}

void sigQuitHandler(int arg) {
    hsd->Stop();
}

void sigAbrtHandler(int arg) {
    hsd->Stop();
}

int main(int argc, char* argv[]) {
    if (argc == 0) {
        std::cerr << "Error: No arguments provided." << std::endl;
        return 1;
    }

    std::signal(SIGTERM, sigTermHandler);
    std::signal(SIGQUIT, sigQuitHandler);
    std::signal(SIGABRT, sigAbrtHandler);

    std::string hsd_name = argv[1];
    hsd = new HSD(hsd_name);
    LOG_INFO("CppIO HSD name: {}", hsd->name());

    hsd->Start(argc, argv);

    // hsd.Shutdown();

    return 0;
}