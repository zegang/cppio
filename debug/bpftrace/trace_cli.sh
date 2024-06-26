#!/bin/bash
#
# File: trace_cli.sh
#
# Description: This script sets up the environment and
# to trace cppio "cli" program using bpftrace.
#
###############################################################################
#
# opyright 2026 cppio authors. All rights reserved.
#
###############################################################################

set -e

# 1. Mount debugfs as needed
echo "Checking debugfs mount..."
if ! mountpoint -q /sys/kernel/debug; then
    echo "Mounting debugfs..."
    sudo mount -t debugfs none /sys/kernel/debug
fi

# 2. enable bpf
echo "Enabling BPF support in the kernel..."
sudo sysctl kernel.bpf_stats_enabled=1

# 3. Dump all available events with details to a single file
echo "Dumping available events for bpftrace..."
sudo bpftrace -vl > available_events.txt 2>/dev/null || echo "No tracepoints found" > available_events.txt

# 4. Call trace_cli.bt to trace "cli" program
echo "Starting bpftrace on cli program..."
if command -v bpftrace &> /dev/null; then
    sudo bpftrace trace_cli.bt
else
    echo "Error: bpftrace not found. Please install bpftrace."
    exit 1
fi
