# CppIO Monitoring with Prometheus and Grafana

This repository exposes Prometheus-compatible metrics for CppIO I/O operations and includes automated Docker Compose deployment with Prometheus and Grafana.

## What is included

- A new `/metrics` HTTP endpoint listening on port `9399` by default.
- Prometheus counters for read/write operations, total bytes, and total processing time.
- Automatic Grafana dashboard provisioning (no manual import required).
- Grafana datasource auto-provisioning for Prometheus.
- A `deploy/prometheus.yml` scrape configuration.
- Docker Compose services for `prometheus` and `grafana`.

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

## Automatic Provisioning

The CppIO I/O Metrics Dashboard is automatically provisioned when Grafana starts. No manual import is required.

Dashboard location:
- `deploy/grafana/provisioning/dashboards/cppio-io-metrics.json`

Datasource configuration:
- `deploy/grafana/provisioning/datasources/prometheus-datasource.yaml`

Dashboard provider config:
- `deploy/grafana/provisioning/dashboards/dashboard-provider.yaml`

The dashboard displays:
1. **Total Read/Write Operations** - cumulative operation count over time
2. **I/O Throughput** - bytes per second (5-minute average)
3. **Total Processing Time** - cumulative time spent in I/O operations
4. **Total Bytes Transferred** - cumulative bytes read/written

## Customization

- Change the metrics port with `CPPIO_METRICS_PORT`.
- Prometheus will scrape the HSD service at `cppio-hsd:9399`.
- Modify dashboard in `deploy/grafana/provisioning/dashboards/cppio-io-metrics.json` to customize visualizations.
- Modify datasource in `deploy/grafana/provisioning/datasources/prometheus-datasource.yaml` to change Prometheus connection details.
