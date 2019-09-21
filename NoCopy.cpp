#include <iostream>
#include <thread>
#include <memory>
#include <queue>

#include "Queue.hpp"

template <typename T>
using InternalQueue = std::queue<T>;

/*
 * Compiled to ensure that data is only ever moved (not copied) through the
 * queue between the producer and the consumer
 */
__attribute__((__unused__))
static void enforce_copyfree()
{
	Queue<std::unique_ptr<int>, InternalQueue> queue(2, 1);

	queue.emplace(std::make_unique<int>(42));
	queue.push(std::make_unique<int>(3));
	queue.writer_done();

	std::unique_ptr<int> p;
	queue.pop(p);
	queue.pop(p);
}
