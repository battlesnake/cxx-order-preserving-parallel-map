This demonstration shows how to use C++11 (and newer) thread classes and standard containers:

 * to scatter input data from one reader thread to multiple worker threads,

 * to gather results back from multiple worker threads into one writer thread,

 * to re-order the data at the output (using a `priority_queue`), such that results are emitted in the same order as the inputs which they were mapped from (regardless of the order which worker threads enqueued the results).

The core of this is the `Queue` class, which is just a thread-safe blocking wrapper around a queue container, allowing multiple writer threads to enqueue data for multiple reader threads.
This example class also provides a simple interface for readers to know when there is no more data to read (i.e. all writers completed, queue empty).

From this primitive, complex parallel processing pipelines can be built, with multiple instances of scatter, map, filter, and reduce stages.
