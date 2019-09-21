#include <string>
#include <vector>
#include <stdexcept>
#include <cmath>
#include <cstring>
#include <thread>

#include "Pipeline.hpp"

void reader(std::istream& input, InQueue& queue)
{
	std::string line;
	size_t id = 0;
	/* Read input items in and tag them with sequential identifier */
	while (std::getline(input, line)) {
		float x;
		float y;
		if (std::sscanf(line.c_str(), "%f %f", &x, &y) != 2) {
			throw std::runtime_error("Invalid input: \"" + line + "\"");
		}
		queue.emplace(id++, InputItem{ x, y });
	}
	queue.writer_done();
}

void mapper(InQueue& in_queue, OutQueue& out_queue)
{
	/* Map items, preserving identifier */
	TaggedInputItem in;
	while (in_queue.pop(in)) {
		const auto& id = in.first;
		const auto& [x, y] = in.second;
		out_queue.emplace(id, OutputItem{ std::hypot(y, x), std::atan2(y, x) });
		/* Force delay per item for test purposes */
		std::this_thread::sleep_for(std::chrono::microseconds{10});
	}
	out_queue.writer_done();
}

/* Comparator for std::priority_queue to put lowest ID value at top */
struct CompareId
{
	bool operator () (const TaggedOutputItem& a, const TaggedOutputItem& b) const
	{
		return a.first > b.first;
	}
};

void writer(OutQueue& queue, std::ostream& output)
{
	/*
	 * Read mapped items, re-ordering into original order by using the
	 * sequential identifier assigned by the reader
	 */
	std::priority_queue<TaggedOutputItem, std::vector<TaggedOutputItem>, CompareId> reorder_buffer;
	TaggedOutputItem item;
	size_t next_id = 0;
	while (queue.pop(item)) {
		reorder_buffer.emplace(std::move(item));
		while (!reorder_buffer.empty() && reorder_buffer.top().first == next_id) {
			item = std::move(reorder_buffer.top());
			reorder_buffer.pop();
			++next_id;
			const auto& [r, theta] = item.second;
			output << r << " " << theta << std::endl;
		}
	}
}
