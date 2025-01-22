#!/bin/bash

############# Submodule Init ##############
git submodule update --init --recursive

############## Build Container Image ############# 
podman build -t cppio -f buildtools/Dockerfile .
podman run -v .:/workspace/cppio -it --name cppio cppio:latest