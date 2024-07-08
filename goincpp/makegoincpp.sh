#!/bin/bash
docker build -t goincpp .
docker run -v .:/goincpp -it --rm goincpp:latest
# cmake -S . -B build;cmake --build build