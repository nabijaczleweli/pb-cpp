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


#include "../include/pb-cpp/multibar.hpp"
#include <iostream>
#include <utility>


pb::multibar::multibar() : multibar(std::cout) {}

pb::multibar::multibar(std::ostream & o) : output(std::addressof(o)), chan(std::make_shared<pb::util::mpsc<pb::detail::multibar_write_message>>()) {}


void pb::multibar::println(const char * s) {
	lines.emplace_back(s);
}

void pb::multibar::println(std::string s) {
	lines.emplace_back(std::move(s));
}


pb::progressbar pb::multibar::create_bar(std::size_t total) {
	println("");
	bar_pipes.emplace_back(lines.size() - 1, chan);
	progressbar p(total, bar_pipes.back());
	// p.is_multibar = true;
	p = 0;
	return p;
}


void pb::multibar::listen() {
	// auto first = true;
	auto nbars = bar_pipes.size();
	while(nbars) {
		// receive message
		chan->wait();
		while(!chan->empty()) {
			const auto msg = chan->pop();

			if(msg.done) {
				--nbars;
				continue;
			}

			lines[msg.level] = std::move(msg.str);

			// and draw

			// TODO: implement move_cursor_up
			// let mut out = String::new();
			// if(!first) {
			//	out += &move_cursor_up(nlines);
			//} else {
			//	first = false;
			//}

			for(auto && l : lines)
				*output << '\r' << l << '\n';
		}
	}
}
