
# Distributed and Parallel Storage & File Systems: A Review

## Introduction

This page reviews contemporary distributed and parallel storage and file systems, with emphasis on solutions designed for AI workloads.

## Distributed File Systems

### HDFS (Hadoop Distributed File System)
- **Purpose**: Large-scale data processing
- **Architecture**: Master-slave model with NameNode and DataNodes
- **Characteristics**: Write-once, high throughput, block replication
- **Limitations**: High latency, not suitable for real-time processing

### GlusterFS
- **Purpose**: Scale-out NAS
- **Architecture**: Distributed architecture without centralized metadata
- **Characteristics**: POSIX-compliant, geo-replication
- **Use cases**: Object storage, media streaming

### Ceph
- **Purpose**: Unified storage platform
- **Architecture**: Object-based with CRUSH algorithm
- **Characteristics**: Self-healing, scalable, POSIX interface
- **Components**: Object storage, block storage, file system

## Parallel File Systems

### Lustre
- **Purpose**: High-performance computing (HPC)
- **Architecture Diagram**: ![Lustre Components](https://wiki.lustre.org/images/5/5a/Lustre_components.png?20211116133553) ([Reference](https://wiki.lustre.org/Understanding_Lustre_Internals))
- **Lustre Software Stack**:
![Basic view of Lustre software stack](https://wiki.lustre.org/images/f/f2/Lustre_sw_stack.png?20211116195443)
- **Filesystem**: The collection of MGS, MDS, and OSS nodes are sometimes referred to as the "frontend". The individual OSTs and MDTs must be formatted with a local file system in order for Lustre to store data and metadata on those block devices. Currently, only ldiskfs (a modified version of ext4) and ZFS are supported for this purpose. The choice of ldiskfs or ZFS if often referred to as the "backend file system". Lustre provides an abstraction layer for these backend file systems to allow for the possibility of including other types of backend file systems in the future. ([Reference](https://wiki.lustre.org/Understanding_Lustre_Internals))
- **FS Layouts**: Lustre stores file data by splitting the file contents into chunks and then storing those chunks across the storage targets. By spreading the file across multiple targets, the file size can exceed the capacity of any one storage target. It also allows clients to access parts of the file from multiple Lustre servers simultaneously, effectively scaling up the bandwidth of the file system. Users have the ability to control many aspects of the file’s layout by means of the lfs setstripe command, and they can query the layout for an existing file using the lfs getstripe command. File layouts fall into one of two categories:
    - Normal / RAID0 - File data is striped across multiple OSTs in a round-robin manner.
    - Composite - Complex layouts that involve several components with potentially different striping patterns.
![Normal RAID0 file striping in Lustre](https://wiki.lustre.org/images/1/16/File_striping.png?20211116195246)
- **Characteristics**: Extreme scalability, stripe data across OSTs
- **Use cases**: Scientific computing, large simulations
- **Refernces**:
    - [Wiki - Understanding Lustre Internals](https://wiki.lustre.org/Understanding_Lustre_Internals)
    - [Video - LUG 2022: [Tutorial] Advanced Lustre File Layouts](https://www.youtube.com/watch?v=_0y2x4KSHhs)

### GPFS (IBM Spectrum Scale)
- **Purpose**: Enterprise-scale parallel file system
- **Characteristics**: Strong consistency, token-based caching
- **Performance**: Optimized for metadata-heavy workloads

### MinIO
- **Purpose**: S3-compatible object storage
- **Architecture**: Distributed object storage with erasure coding
- **Characteristics**: High performance, cloud-native, Kubernetes-ready
- **Use cases**: ML model storage, training data lakes

## Object Storage Systems

### Amazon S3 / S3-Compatible
- **Purpose**: Cloud object storage
- **Characteristics**: Eventually consistent, geo-distributed, highly available
- **Use cases**: Data lakes, model artifacts, backup storage

### Azure Blob Storage
- **Purpose**: Unstructured data storage at scale
- **Characteristics**: Hot/cool/archive tiers, immutable snapshots
- **Benefits**: Cost-effective for large datasets, good for ML training data

### Google Cloud Storage
- **Purpose**: Unified object storage
- **Characteristics**: Strong consistency, fine-grained access control
- **Advantages**: Integrations with BigQuery and Vertex AI for ML workflows


## AI-Optimized File Systems

### WekaIO
- **Focus**: AI and machine learning workloads
- **Features**: Inline deduplication, compression, GPU-acceleration
- **Benefits**: Reduced latency, improved throughput for ML training

### Alluxio
- **Purpose**: Data orchestration for AI
- **Characteristics**: In-memory distributed file system
- **Advantages**: Cache locality optimization for iterative ML jobs

### Delta Lake / Apache Iceberg
- **Focus**: Data lake optimization
- **Features**: ACID transactions, time-travel queries, schema evolution
- **Applications**: Data versioning for reproducible ML pipelines

## Comparison Matrix

| System | Type | Consistency | Scalability | AI Native |
|--------|------|-------------|-------------|-----------|
| HDFS | Distributed | Weak | High | Partial |
| Lustre | Parallel | Strong | Very High | No |
| Ceph | Distributed | Strong | High | No |
| Alluxio | Distributed | Strong | High | Yes |
| WekaIO | Distributed | Strong | Very High | Yes |

## Conclusion

Traditional parallel file systems excel in HPC but lack AI-specific optimizations. Modern AI-focused systems like Alluxio and WekaIO address performance bottlenecks in machine learning pipelines through intelligent caching and data locality strategies.
