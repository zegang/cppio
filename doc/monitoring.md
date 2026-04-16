# CppIO Monitoring with Prometheus and Grafana

This repository now exposes Prometheus-compatible metrics for CppIO I/O operations and includes a Docker Compose deployment example for Prometheus and Grafana.

## What is included

- A new `/metrics` HTTP endpoint listening on port `9399` by default.
- Prometheus counters for read/write operations, total bytes, and total processing time.
- A simple Grafana dashboard definition for CppIO I/O metrics.
- A `deploy/prometheus.yml` scrape configuration.
- Docker Compose entries for `prometheus` and `grafana`.

## Metrics exposed

- `cpio_io_read_operations_total`
- `cpio_io_write_operations_total`
- `cpio_io_read_bytes_total`
- `cpio_io_write_bytes_total`
- `cpio_io_read_seconds_total`
- `cpio_io_write_seconds_total`

## Run locally with Docker Compose

From the `deploy` directory:

```sh
docker compose up -d
```

Then browse:

- Prometheus: `http://localhost:9090`
- Grafana: `http://localhost:3000`
- Metrics endpoint: `http://localhost:9399/metrics`

Grafana default login:

- user: `admin`
- password: `admin`

## Grafana dashboard

Import `deploy/grafana-dashboard-cppio.json` into Grafana to visualize read/write counts and throughput.

## Customization

- Change the metrics port with `CPPIO_METRICS_PORT`.
- Prometheus will scrape the HSD service at `cppio-hsd:9399`.
