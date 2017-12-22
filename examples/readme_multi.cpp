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


#include <pb-cpp/multibar.hpp>
#include <thread>
using namespace std::literals;

int main() {
	const auto count = 100u;
	pb::multibar mb;
	mb.println("Application header:");

	const auto bar_handler = [](auto bar, auto count) {
		for(auto i = 0u; i < count; ++i) {
			++bar;
			std::this_thread::sleep_for(100ms);
		}

		bar.finish();
	};

	std::thread(bar_handler, mb.create_bar(count), count).detach();
	mb.println("Add a separator between the two bars");
	std::thread(bar_handler, mb.create_bar(count * 2), count * 2).detach();

	// Start listening to all bars' changes.
	// This blocks until all bars finish.
	// To ignore that, run it in a different thread.
	mb.listen();
}
