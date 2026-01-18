# Project 3 Part 2: High-Performance Block Cache

This project implements a high-performance, fixed-size **Write-Back Block Cache** designed for a B-tree based filesystem. The core objective is to provide $\text{O}(1)$ performance for critical path operations such as block lookup, slot allocation, and eviction decisions by leveraging multiple, specialized data structures.

---

## 🏗️ Architecture and Data Structures

The cache operates as a **fixed-size memory pool** (an array of `cache_entry_t` structs) whose size is dynamically configured based on available system RAM during initialization.

The design is a classic example of balancing **Performance** (via hash maps and lists) and **Memory Boundary** (via the fixed array).

| Data Structure | Role | Optimization | Complexity |
| :--- | :--- | :--- | :--- |
| **Fixed-Size Array** (`cache->cache`) | **Physical Memory Pool** | The backing store for all block data and metadata. Enforces a strict memory limit. | $\text{O}(1)$ for direct index access |
| **Hash Map 1 (PCI)** (`PCI_HM`) | **Primary Cache Index** | Maps `block_number` to its array index/slot. Answers: "Is block X in the cache?" | $\text{O}(1)$ average for Lookup/Insert/Delete |
| **Doubly-Linked List** (`LRU_List`) | **Eviction Mechanism** | Tracks access recency. Head is MRU, Tail is LRU. Used for selecting a victim block on cache miss. | $\text{O}(1)$ for Eviction Decision (Tail) and Access Update (Move-to-Head) |
| **Singly-Linked List** (`FL_LL`) | **Free Slot Allocator** | Stores indices of currently unused slots in the Fixed Array. | $\text{O}(1)$ for Allocation and Deallocation (Push/Pop) |
| **Hash Map 2 (DL)** (`DL_HM`) | **`fsync` Optimizer** | Maps `inode_number` to a list of its associated dirty blocks. Essential for efficient, fine-grained durability. | $\text{O}(1)$ lookup for `fsync` |
| **Doubly-Linked List 2** (`GDL`) | **`sync` Optimizer** | Tracks all dirty pages in the block cache for efficient filesystem-wide `sync` operations. | $\text{O}(1)$ lookup for `sync` |

---

## 🛡️ Durability and Security Features

### Write Policy (Durability)

The cache employs a **Write-Back** policy for high performance, where modifications are buffered in memory and marked with a `dirty_bit`. Durability is guaranteed through explicit synchronization calls:

* **`cache_fsync(inum)`:** Uses the **Per-Inode Dirty List (DL)** to quickly find and write back *only* the dirty blocks belonging to a specific inode, making it an $\text{O}(\text{Dirty Blocks for file})$ operation.
* **`cache_sync()`:** Uses the **Global Dirty List (GDL)** to write back *all* dirty blocks currently in the cache.

### Security

The implementation prioritizes data sanitization upon freeing memory to prevent information leakage:

* **Secure Wiping (`arc4random_buf`):** The BSD function `arc4random_buf` is used to overwrite the memory of all dynamically allocated structures (linked list nodes, hash map nodes, and the actual page data) with cryptographically secure random bytes immediately before calling `free()`. This is critical for cache entries that may contain sensitive data before being evicted.

---

## ⚙️ Compilation

The project uses a simple `Makefile` and requires the `bsd` library for `arc4random_buf`.

To compile the project:
```bash
clang -lbsd -g -o cache_test *.c
dd if=/dev/zero of=my.img bs=1M count=2 # Creates a 2MB disk image for testing
