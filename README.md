# darray
A dynamic array indexing int to int/pointer. Inspired by Judy arrays, but as simple as hash tables.

It has been a long time since I did this work, but always meant to publish it. I might not work in its current state. Now the publishing has been done, I plan to clean it up a bit and document the idea.

It is kind of a radix tree with keys stored at the edges and a 16 (4 bit) path fan out at each node. The nodes are compacted so only the used paths are are stored. For each node only one cache line is touched.

It supports these operations:
- darray_get (get a pointer to value for reading or writing)
- darray_remove
- darray_min
- darray_max
- darray_prev
- darray_next
