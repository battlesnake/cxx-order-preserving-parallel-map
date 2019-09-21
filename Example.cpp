#include <iostream>
#include <thread>

#include "Pipeline.hpp"

static void run_pipeline(std::istream& in, std::ostream& out, size_t mapper_count)
{
	constexpr size_t max_queue_size = 100;

	/* Queues */
	InQueue input_queue { max_queue_size, 1 };
	OutQueue output_queue { max_queue_size, mapper_count };

	/* Start threads */
	std::thread reader_thread([&] () { reader(in, input_queue); });
	std::thread writer_thread([&] () { writer(output_queue, out); });
	std::vector<std::thread> mapper_threads { mapper_count };
	for (auto& mapper_thread : mapper_threads) {
		mapper_thread = std::thread([&] () { mapper(input_queue, output_queue); });
	}

	/* Wait for threads to finish */
	reader_thread.join();
	writer_thread.join();
	for (auto& mapper_thread : mapper_threads) {
		mapper_thread.join();
	}
}

int main(int argc, char *argv[])
{
	(void) argc;
	(void) argv;

	size_t mapper_count = 8;

	if (getenv("WORKERS")) {
		mapper_count = std::atoi(getenv("WORKERS"));
	}

	run_pipeline(std::cin, std::cout, mapper_count);

	return 0;
}
