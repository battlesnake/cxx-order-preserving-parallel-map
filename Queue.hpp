#pragma once
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include <utility>

/*
 * Blocking thread-safe queue with size limit
 *
 * Requires underlying QueueType to support:
 *  * emplace
 *  * front
 *  * pop
 *  * size
 *  * empty
 */
template <typename ItemType, template <typename> typename QueueType>
struct Queue
{
	using item_type = ItemType;

	template <typename T>
	using queue_type = QueueType<T>;

	/* COnstruct item into queue */
	template <typename... Arg>
	void emplace(Arg&&... arg)
	{
		std::unique_lock lock(mx);
		if (writer_count == 0) {
			throw std::runtime_error("Attempt to write into completed queue");
		}
		while (queue.size() >= max_size) {
			can_push.wait(lock);
		}
		if (queue.empty()) {
			can_pop.notify_all();
		}
		queue.emplace(std::forward<Arg>(arg)...);
	}

	/* Push item into queue */
	void push(const item_type& item)
	{
		emplace(item);
	}

	/* Push item into queue */
	void push(item_type&& item)
	{
		emplace(std::move(item));
	}

	/*
	 * Read item from queue and return true, returns false if queue empty
	 * and writer_done() has been called
	 */
	bool pop(item_type& item)
	{
		std::unique_lock lock(mx);
		while (queue.empty() && writer_count > 0) {
			can_pop.wait(lock);
		}
		if (queue.empty() && writer_count == 0) {
			return false;
		} else {
			if (queue.size() == max_size) {
				can_push.notify_all();
			}
			item = std::move(queue.front());
			queue.pop();
			return true;
		}
	}

	/* Producer calls this to indicate that it has finished queueing data */
	void writer_done()
	{
		std::lock_guard lock(mx);
		if (--writer_count == 0) {
			can_pop.notify_all();
		}
	}

	Queue(size_t max_size, size_t writer_count) :
		max_size(max_size),
		writer_count(writer_count)
	{
	}

private:
	mutable std::mutex mx;
	std::condition_variable can_pop;
	std::condition_variable can_push;
	queue_type<item_type> queue;
	size_t max_size;
	size_t writer_count;
};
