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


#pragma once


#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <queue>
#include <nonstd/optional.hpp>
#include <ostream>


namespace pb {
	namespace util {
		/// Write out a human-readable representation of a file size.
		///
		/// Stolen from [http](https://github.com/thecoshman/http/blob/112e35f88c9e08158839d1f3f80a9ac841e990da/src/util/mod.rs#L212-L235).
		struct human_readable {
			std::size_t size;
		};
		std::ostream & operator<<(std::ostream & out, human_readable hr);
		human_readable make_human_readable(std::size_t size);


		/// Get the terminal window's width or `nullopt` in case of error or stdout not being tied to terminal.
		nonstd::optional<std::size_t> terminal_width();


		template <class T>
		class mpsc {
		private:
			std::mutex condvar_back;
			std::condition_variable condvar;
			std::mutex lock;
			std::queue<T> msgs;

		public:
			mpsc() = default;
			mpsc(mpsc &&) = default;

			bool empty();

			void push(const T& elem);
			void push(T && elem);
			template <class... Args>
			void emplace(Args &&... args);

			T pop();

			void wait();
		};
	}
}


#include "impl/mpsc.inc"
