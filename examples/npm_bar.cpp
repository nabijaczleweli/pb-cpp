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


#include "pb-cpp/progressbar.hpp"
#include <thread>


using namespace std::literals;


int main() {
	const auto count = 30u;
	pb::progressbar bar(count * 10);
	bar.tick_format("\\|/-");
	bar.format("|#--|");
	bar.show_tick      = true;
	bar.show_speed     = false;
	bar.show_percent   = false;
	bar.show_counter   = false;
	bar.show_time_left = false;
	++bar;

	for(auto i = 0u; i < count; ++i) {
		for(auto j = 0u; j < 10u; ++j) {
			bar.message("normalize -> thing ");
			std::this_thread::sleep_for(80ms);
			bar.tick();
		}
		for(auto j = 0u; j < 10u; ++j) {
			bar.message("fuzz -> tree       ");
			std::this_thread::sleep_for(80ms);
			++bar;
		}
	}

	//pb.finish_println("done!");
}
