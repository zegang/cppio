# Storage and File Systems for Artificial Intelligence

As AI models move from gigabyte-scale to terabyte-scale parameters, the "I/O wall" has become a genuine bottleneck. Standard file systems often buckle under the pressure of millions of small files (like images for computer vision) or the massive sequential throughput required for checkpointing.

Below are reviews on requirments/designs/challanges of storage and file systems for AI and emerging techs like Data Versioning and Hardware-Software Co-design which are currently trending in research.

## Introduction
The explosion of Deep Learning (DL) has shifted the storage paradigm. Unlike traditional High-Performance Computing (HPC), AI workloads are characterized by:
- Massive Datasets: Petabyte-scale collections of unstructured data.
- Random Access Patterns: Shuffling data during training to improve model generalization.
- High Concurrency: Thousands of GPU workers requesting data simultaneously.

## Requirements for AI Storage

To sustain peak GPU utilization, an AI-optimized storage system must meet these criteria:

|Requirement|Description|
|-----------|-----------|
|High Throughput|Must feed data fast enough to prevent GPU starvation.|
|Low Latency|Critical for metadata operations (opening/closing millions of small files).|
|Scalability|Linear performance growth as storage capacity increases.|
|POSIX Compliance|Compatibility with standard deep learning frameworks (PyTorch, TensorFlow).|
|Data Locality|Minimizing data movement by bringing computation closer to storage.|

## Architecture and Design Patterns
Modern AI storage is moving away from traditional NAS (Network Attached Storage) toward more distributed and tiered architectures.

### Distributed Parallel File Systems (PFS)
Systems like Lustre and GPFS (Spectrum Scale) provide high-speed concurrent access to shared data. They separate metadata from data to allow parallel paths for I/O.

### Data Tiering and Caching
Because NVMe drives are expensive, many designs use a "Hot/Cold" strategy:
- Hot Tier: Local NVMe or burst buffers for active training sets.
- Cold Tier: Object storage (S3/Azure Blob) for long-term archival.

### Data Pre-fetching and Shuffling
Software-defined storage layers now include "smart" pre-fetching. By predicting which mini-batch the GPU will need next, the system can hide I/O latency by loading data into RAM before it is requested.

## Key Challenges
Despite advancements, several "pain points" remain in the AI lifecycle:
- The Small File Problem: Most file systems are optimized for large, sequential reads. Loading millions of 10KB images causes metadata bottlenecks.
- Checkpointing Overhead: Large models (LLMs) save "checkpoints" frequently. This writes hundreds of gigabytes to disk at once, often freezing training for several minutes.
- Data Silos: Data often sits in object storage, but training requires a file interface, leading to "hydration" delays.
- Consistency vs. Performance: Maintaining strict POSIX consistency often slows down distributed training.

## Emerging Trends

### Data Versioning and Lineage
As models are retrained, tracking which version of a dataset produced which model is vital for reproducibility. Tools like DVC (Data Version Control) and LakeFS treat data like code.

### Storage for Vector Databases
With the rise of RAG (Retrieval-Augmented Generation), storing and querying high-dimensional vectors (using databases like Milvus or Pinecone) is a new storage frontier that requires specialized indexing (HNSW, IVF).

### Near-Data Processing (NDP)
Instead of moving data to the GPU for simple preprocessing (like resizing images), "SmartSSDs" perform these tasks directly on the storage controller to save PCIe bandwidth.

## Case Study: Industry Implementations
### Meta: The Tectonic Ecosystem
Meta’s storage strategy is a prime example of disaggregated storage—separating compute from storage to scale each independently.
- Tectonic (The Backbone): Meta replaced HDFS with Tectonic, an exabyte-scale distributed file system. Unlike traditional systems, Tectonic uses a layered metadata approach, storing metadata in a distributed key-value store (ZippyDB). This allows Meta to handle billions of small files without hitting the "Metadata Wall."
- Tectonic-Shift (The Flash Tier): To combat the latency of HDDs during LLM training, Meta introduced a flash-based caching tier. This tier uses novel cache policies to "absorb" the massive I/O bursts during data loading, reducing power consumption by ~29% while increasing throughput by up to 3x.
- Parallel NFS (Hammerspace): For their 24k GPU clusters (used for Llama 3), Meta co-developed a parallel NFS solution. This allows researchers to perform interactive debugging—where code changes are instantly visible across thousands of nodes—without sacrificing the throughput needed for exabyte-scale data loading.

### OpenAI: Co-designed Supercomputing
OpenAI’s approach is characterized by deep integration with Microsoft Azure and a focus on predictable scaling.
- Azure AI Supercomputer: OpenAI and Azure co-designed a purpose-built supercomputer. Their storage architecture relies heavily on Blob Storage for the "Data Lake", which is then "hydrated" into high-performance local NVMe caches or managed Lustre/GPFS instances during training runs.
- Ray Orchestration: OpenAI uses Ray to coordinate data movement and training across thousands of GPUs. Ray’s distributed object store allows for zero-copy data sharing between tasks on the same node, significantly reducing the memory overhead typically associated with moving large tensors.
- Predictable I/O: A key focus for OpenAI (as noted in the GPT-4 technical report) is ensuring that the software stack behaves predictably at scale. They prioritize "asynchronous, zero-copy data movement" to ensure that the I/O path never stalls the compute kernels.

### DeepSeek: The Fire-Flyer File System (3FS)
While Meta and OpenAI often rely on established or proprietary cloud-native storage, DeepSeek developed 3FS from the ground up to exploit the performance of modern NVMe SSDs and RDMA (Remote Direct Memory Access) networks.

#### Disaggregated Architecture
3FS employs a disaggregated storage model, decoupling compute nodes from storage nodes. This allows thousands of SSDs across a distributed network to aggregate their bandwidth, providing a "locality-oblivious" shared storage layer. In production, DeepSeek has demonstrated aggregate read throughput exceeding 6.6 TiB/s.

#### Technical Innovations
- CRAQ (Chain Replication with Apportioned Queries): To maintain strong consistency without sacrificing read performance, 3FS uses the CRAQ algorithm. While writes propagate along a chain of storage nodes, any node in the chain can serve a read request, effectively doubling or tripling the available read bandwidth compared to traditional replication.
- Stateless Metadata with FoundationDB: 3FS offloads metadata management to a transactional key-value store (typically FoundationDB). This removes the "Metadata Bottleneck" common in traditional POSIX systems, allowing for rapid file creation and lookups even when dealing with billions of small objects.
- KVCache for Inference: Beyond training, 3FS includes specialized optimizations for KVCache. By providing a high-throughput, flash-based alternative to expensive DRAM caching, 3FS enables larger context windows and higher concurrency during model inference at a fraction of the cost.

#### Performance Benchmarks
DeepSeek's 3FS is not just theoretical; it has set high industry standards:
- GraySort Performance: 3FS demonstrated the ability to sort 110 TiB of data in roughly 30 minutes.
- Dataloader Efficiency: By enabling high-speed random access, 3FS eliminates the need for complex data pre-fetching or shuffling logic in the training code, simplifying the ML engineer's workflow.

#### Specialized AI file systems
The emergence of 3FS represents a shift toward specialized AI file systems. Unlike general-purpose distributed systems (like HDFS or Ceph), 3FS is uncompromisingly designed for the "Read-Heavy, Random-Access" nature of AI training and the "High-Throughput, Massive-Capacity" needs of LLM inference.

#### Comparative Analysis of Architectures

|Feature|Meta (Tectonic/Shift)|OpenAI (Azure/Ray)|DeepSeek (3FS)|
|-------|---------------------|------------------|--------------|
|Primary Goal|Efficiency & Global Scale|Predictability & Iteration Speed|Raw Throughput & Strong Consistency|
|Metadata Strategy|Distributed Key-Value (ZippyDB)|Centrally Managed (Azure Infrastructure)|Stateless KV (FoundationDB)|
|Data Access|FUSE-based API + Parallel NFS|Ray Object Store + Blob Storage|Native RDMA + FUSE Interface|
|Caching Layer|Tectonic-Shift (Flash Tier)|GPU-Local RAM & NVMe Buffers|SSD-based KVCache for Inference
|Open Source|High (Contributes to OCP/PyTorch)|Low (Proprietary stack/Azure exclusive)|High (Fully Open Sourced 3FS)|
|Consistency|Eventual/Tunable|Managed Cloud-Consistent|Strong (via CRAQ)|
|Network Fabric|Standard Ethernet/IP|InfiniBand / Azure Fabric|RDMA (InfiniBand/RoCE)|
|Small File Ops|Layered Metadata|Object Storage Hydration|Stateless KV Metadata|
|Key Advantage|Global Namespace Scale|Rapid Iteration & Ease|Raw I/O Throughput/Efficiency|

### Discussion: The Convergence of Storage and Compute
The case studies suggest that for state-of-the-art AI, storage is no longer a "passive" repository. It has become an active part of the training loop.
- Metadata is the Bottleneck: Both companies have moved away from traditional tree-based file systems in favor of database-backed metadata.
- Tiering is Mandatory: No single storage medium (HDD, SSD, or RAM) can cost-effectively satisfy both the capacity and speed requirements of AI.
- FUSE is Evolving: While FUSE (Filesystem in Userspace) was once considered too slow, Meta’s optimizations show it can be viable if backed by a high-performance distributed backend.

## Conclusion
The future of AI storage lies in the convergence of Object Storage scale with File System performance. Solving the I/O bottleneck is no longer just a hardware problem, but a coordination challenge between the data loader, the network fabric, and the underlying storage media.