# Durability
This on one of the most critical trade-offs in filesystem design: **performance vs. durability**.

Let's break down the problem and then examine proposed solutions.

### The Problem: The Durability Guarantee

When an application calls `write()` and then `fsync()`, or when it closes a file, it has a reasonable expectation that its data is **safely on stable storage** (the disk). A pure write-back cache, where writes linger in memory until eviction, breaks this promise. A power loss at the wrong moment means **data loss**, which is unacceptable for most applications.

---

### A Proposed Solution: "Write-Through" Caching

One suggestion—"ensure that all writes are taken care of immediately"—describes a **Write-Through Cache**.

*   **How it works:** Every time a block in the cache is modified (written to), the filesystem **immediately** writes that block to disk *before* acknowledging the write operation to the application.
*   **Pros:**
    *   **Strong Durability:** Data is on disk as soon as the `write()` syscall returns. Power loss does not lead to data loss (for that write).
*   **Cons:**
    *   **Devastating Performance:** This destroys the primary performance benefit of the cache for writes. Every single application write now incurs the latency of a physical disk write. A B+Tree insertion, which might involve modifying multiple nodes (leaf and internal), would become extremely slow.

Because of this massive performance penalty, a pure write-through cache is rarely used for general-purpose filesystems.

---

### The Real-World Solution: A Hybrid Approach with Ordered Writes

Modern filesystems (like ext4, XFS, NTFS) use a more sophisticated approach that balances performance and durability. Here's the conceptual breakdown:

#### 1. The Default: Write-Back Caching for Performance
*   By default, all writes go to the block cache, the `dirty` bit is set, and the syscall returns immediately. **This is fast.**
*   A background kernel thread (often called the "pdflush" daemon) periodically scans the cache for dirty blocks that are older than a certain threshold (e.g., 30 seconds) and writes them to disk. This batches writes, which is much more efficient.

#### 2. The Guarantee: Explicit Synchronization (`fsync`)
*   When an application *requires* durability (e.g., a database committing a transaction), it must explicitly ask for it using the `fsync()` system call.
*   `fsync(file_descriptor)` does **not** mean "write this one block." It means: "Ensure that **all dirty data associated with this file** is physically on disk before this call returns."
*   This is a heavy operation. The filesystem must:
    1.  Identify all dirty cache entries for that file.
    2.  Write them to disk.
    3.  Often, it must also write the file's **metadata** (the inode, and the directory entries) to disk.
    4.  Potentially wait for the disk's own write-cache to physically flush (using a command like `FLUSH CACHE`).

#### 3. The Critical Refinement: Journaling and Atomicity
For a B+Tree filesystem, a simple `fsync()` isn't always enough. Consider what happens if you need to split a node during an insert. This requires writing multiple blocks (the split node, the new node, and the parent node). If power fails *in the middle* of these writes, the filesystem's on-disk structure could be left in a corrupted, inconsistent state.

The solution is **Journaling (Write-Ahead Logging)**:

*   **Concept:** Before modifying the actual B+Tree blocks on disk, the filesystem first writes a "note" (a log record) to a special, sequential area of the disk called the **journal**. This note describes all the changes it is *about* to make.
*   **The Workflow:**
    1.  **Journal Write:** Write the "before and after" images of the blocks-to-be-modified to the journal. **Wait for this write to complete.**
    2.  **Commit Record:** Write a special "commit" record to the journal. **Wait for this to complete.** At this point, the change is *logged* and can be recovered.
    3.  **Checkpoint:** Now, the filesystem proceeds to write the actual blocks to their final locations in the B+Tree. This is the "checkpointing" process.
    4.  **Journal Cleanup:** Once the checkpoint is complete, the space in the journal can be freed.

*   **How this solves the problem:** After a crash, the filesystem recovery process reads the journal. If it finds a transaction with a commit record but without a corresponding completion marker, it knows the main filesystem might be corrupt. It then "replays" the logged changes from the journal, ensuring the B+Tree is brought back to a consistent state.

### Summary: The Effective Strategy is Layered

So, to answer this question directly: **No, we don't write everything immediately because it's too slow.** Instead, we use a combination of:

1.  **A Write-Back Block Cache:** For maximum performance during normal operation.
2.  **Explicit Application Control (`fsync`):** To let applications demand durability when needed.
3.  **A Journal:** To guarantee the **atomicity and consistency** of complex operations (like B+Tree updates) even across crashes, without requiring every single write to be synchronous.

This layered approach provides an excellent balance, giving applications the tools they need for safety while maintaining high performance for the vast majority of operations that can tolerate a small risk of data loss.
