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


//! Okay so here's the deal: this was much simpler in Rust.
//!
//! First, in the Rust version `ProgressBar` is templated over `T: Write`, which means that you can just slam any old custom thing and it'll work.
//!
//! Second, Rust has a much simpler I/O model, consisting of two functions instead of fifty.
//!
//! But that means that the Rust version colludes all writes, which inhibits performace.
//!
//! So, in this wonderful piece of code (tm) we override `streambuf::sync()`,
//! which, if cppreference isn't lying to me, is the function called on `ostream::flush()`.
//!
//! Therein, we steal the data accumulated so far via `stringbuf` and send it to the channel provided by `multibar`.
//!
//! This means, that this whole shebang is transparent and we've only restricted performance
//! to requiring a flush after every *full write* and an empty flush on `finish()`.
//!
//! Or, in other words, it was simpler in Rust, but it's better in C++.


pb::detail::multibar_notifying_pipe::multibar_notifying_streambuf::multibar_notifying_streambuf(
    std::size_t l, const std::shared_ptr<pb::util::mpsc<pb::detail::multibar_write_message>> & c)
      : std::stringbuf(std::ios::out), level(l), chan(c) {}

int pb::detail::multibar_notifying_pipe::multibar_notifying_streambuf::sync() {
	const auto data = str();
	str("");
	chan->emplace(multibar_write_message{
	    data.empty(),  // finish function flushes without data
	    level,
	    std::move(data),
	});
	return 0;
}

pb::detail::multibar_notifying_pipe::multibar_notifying_pipe(std::size_t l, const std::shared_ptr<pb::util::mpsc<pb::detail::multibar_write_message>> & c)
      : buf(l, c) {
	init(&buf);
}
