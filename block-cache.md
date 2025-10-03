# Project 3 Part 2 - Block Cache

Let me clarify the roles of each data structure. You don't choose *between* a HashMap and a fixed-size array of structs. **You use both together,** because they solve two different problems.

### The Two Problems a Cache Must Solve

1.  **Fast Lookup by Key:** "I have block number 0x12345. Is it in the cache, and if so, where is the data?"
2.  **Fast Eviction Management:** "My cache is full. Which of the 1000 blocks I'm currently holding is the *least recently used* one?"

A single data structure is rarely good at both. Here's how the combination works:

---

### 1. The Fixed-Size Array of Structs: The "Physical" Cache

It's the **backing store** or the **memory pool**.

*   **What it is:** A pre-allocated array (e.g., `cache_entry_t cache[FIXED_SIZE];`). Each entry in the array is a struct containing:
    *   The actual block data (the `char data[BLOCK_SIZE]`).
    *   Metadata: `block_number`, `dirty_bit`, `pin_count`, `last_used_timestamp`.
    *   **Pointers for the LRU list** (more on this below).

*   **Its Purpose:**
    *   It physically limits the number of cached blocks to `FIXED_SIZE`. This is the "limited number of entries."
    *   It holds all the data and the core metadata for the blocks that are *currently resident* in memory.

**This array directly represents your limited cache capacity.**

---

### 2. The HashMap: The "Logical" Index

The HashMap is **not** where the data is stored. It is an **index into the fixed-size array.**

*   **What it is:** A HashMap where:
    *   **Key:** `block_number` (e.g., 0x12345).
    *   **Value:** A **pointer** (or an array index) to the corresponding `cache_entry_t` within the *fixed-size array*.

*   **Its Purpose:**
    *   To answer the "Fast Lookup" question in **O(1) time**. Without this, you would have to do a linear scan of your entire fixed-size array to see if a block is cached, which would be far too slow for a performant filesystem.

### Why the HashMap Doesn't Defy the "Limited Entries" Rule

The HashMap's size is **directly tied to the size of the fixed array.**

*   When you load a new block into the cache, you place it in a free slot in your **fixed array** and then **insert a mapping** (`block_number` -> `&array[slot]`) into the HashMap.
*   When you evict a block from the cache, you remove it from your **fixed array** and **delete the corresponding mapping** from the HashMap.

**Therefore, the number of entries in the HashMap is always exactly equal to the number of *active, resident* blocks in your fixed-size array.** It cannot grow beyond the array's size. It's a mirror of what's physically in the cache, optimized for fast key-based access.

---

### A Concrete Example of the Workflow

Let's say your cache is a fixed array of 3 entries.

1.  **Initial State:** Array is empty. HashMap is empty.
2.  **Read Block 100:**
    *   **Cache Miss.** You find a free slot in the array, say `array[0]`.
    *   You read the data for block 100 from disk into `array[0].data`.
    *   You set `array[0].block_number = 100`.
    *   You **insert into the HashMap:** `{ 100 : &array[0] }`.
3.  **Read Block 200:**
    *   **Cache Miss.** Place in `array[1]`.
    *   **Insert into HashMap:** `{ 200 : &array[1] }`.
4.  **Read Block 100 again:**
    *   **Cache Hit!** You consult the HashMap with key `100`. It immediately returns `&array[0]` in O(1) time. You do *not* need to scan `array[0]`, `array[1]`, etc.
5.  **Read Block 300 (Cache is now full):**
    *   **Cache Miss.** You need to evict. Your LRU policy (using timestamps or a list) identifies `array[1]` (holding block 200) as the victim.
    *   **Before eviction:** You **remove from the HashMap** the entry for the victim's block number: `200`.
    *   Now you can overwrite `array[1]` with the data for block 300.
    *   You **insert into the HashMap:** `{ 300 : &array[1] }`.

As you can see, the HashMap's size fluctuates between 0 and 3, perfectly mirroring the state of the fixed-size array.

### A Refinement: Linking the Array for LRU

You can use timestamps to calculate LRU. That works, but scanning all entries to find the smallest timestamp on every eviction is O(N), which is inefficient.

The standard, more efficient approach is to use a **Doubly-Linked List** to manage the LRU order. This is where the "pointers for the LRU list" in your `cache_entry` struct come in.

*   You maintain a global "LRU List". The most recently used block is at the "head," the least recently used is at the "tail."
*   When a block is accessed, you move it to the head of the list. This is O(1).
*   When you need to evict, you look at the tail of the list. This is O(1).

**In this design, your data structures are:**
*   **A Fixed-Size Array:** The physical memory.
*   **A HashMap:** For O(1) lookup by `block_number`.
*   **A Doubly-Linked List:** For O(1) eviction decisions.

The `cache_entry` structs in the fixed array are the nodes of this linked list. The HashMap points to these same nodes.

### Summary

**The cache must be a fixed-size pool.** The HashMap is not the storage; it's an index *into* that fixed-size pool that makes it fast to use. It's a critical component that enables the high-performance lookups necessary for a B+Tree, without violating the fundamental constraint of a limited cache size.
