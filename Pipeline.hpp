#pragma once
#include <queue>
#include <istream>
#include <ostream>

#include "Queue.hpp"

using Id = size_t;

using InputItem = struct { float x; float y; };
using OutputItem = struct { float r; float theta; };

using TaggedInputItem = std::pair<Id, InputItem>;
using TaggedOutputItem = std::pair<Id, OutputItem>;

template <typename T>
using InternalQueue = std::queue<T>;

using InQueue = Queue<TaggedInputItem, InternalQueue>;
using OutQueue = Queue<TaggedOutputItem, InternalQueue>;

void reader(std::istream& input, InQueue& queue);

void mapper(InQueue& in_queue, OutQueue& out_queue);

void writer(OutQueue& queue, std::ostream& output);
