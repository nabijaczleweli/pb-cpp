// The MIT License (MIT)

// Copyright (c) 2017 nabijaczleweli

// Permission is hereby granted, free of charge, to any person obtaining a copy of
// this software and associated documentation files (the "Software"), to deal in
// the Software without restriction, including without limitation the rights to
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
// the Software, and to permit persons to whom the Software is furnished to do so,
// subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


#include "pb-cpp/multibar.hpp"
#include <algorithm>
#include <iostream>
#include <random>
#include <thread>


using namespace std::literals;


static thread_local std::mt19937 rng(std::random_device{}());

static std::string rand_string();


int main() {
	pb::multibar mb;
	mb.println("Your Application Header:");
	mb.println("");

	for(auto i = 1u; i < 6; ++i) {
		const auto count = 100 * i;
		std::thread(
		    [](auto bar, auto count) {
			    std::uniform_int_distribution<unsigned int> dist(0, 100);

			    bar.tick_format("\xCC\xC9\xCB\xBB\xB9\xBC\xCA\xC8");
			    for(auto i = 0u; i < count / 20; ++i) {
				    for(auto i = 0u; i < 20; ++i) {
					    bar.message("Waiting  : ");
					    std::this_thread::sleep_for(50ms);
					    bar.tick();
				    }
				    for(auto i = 0u; i < 20; ++i, ++bar) {
					    bar.message("Connected: ");
					    std::this_thread::sleep_for(std::chrono::milliseconds(dist(rng)));
				    }
			    }
			    for(auto i = 0u; i < 20; ++i) {
				    bar.message("Cleaning : ");
				    std::this_thread::sleep_for(100ms);
				    bar.tick();
			    }
			    bar.finish(rand_string() + ": Pull complete");
		    },
		    mb.create_bar(count), count)
		    .detach();
	}


	mb.println("");
	mb.println("Text lines separate between two sections:");
	mb.println("");

	for(auto i = 1u; i < 4; ++i) {
		const auto count = 100 * i;
		std::thread(
		    [](auto bar, auto count) {
			    std::uniform_int_distribution<unsigned int> dist(0, 100);

			    for(auto i = 0u; i < count; ++i) {
				    ++bar;
				    std::this_thread::sleep_for(std::chrono::milliseconds(dist(rng)));
			    }
			    bar.finish();
		    },
		    mb.create_bar(count), count)
		    .detach();
	}

	mb.listen();

	std::cout << "\nAll bars done!\n";
}

std::string rand_string() {
	std::uniform_int_distribution<unsigned int> dist('!', '~');

	std::string ret;
	ret.resize(12);
	std::generate_n(ret.begin(), ret.size(), [&]() { return dist(rng); });
	return ret;
}
