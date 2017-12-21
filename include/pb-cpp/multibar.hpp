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


#include "progressbar.hpp"
#include "util.hpp"
#include <cstddef>
#include <list>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>


namespace pb {
	namespace detail {
		struct multibar_write_message {
			bool done;
			std::size_t level;
			std::string str;
		};

		class multibar_notifying_pipe : public std::ostream {
		private:
			class multibar_notifying_streambuf : public std::stringbuf {
			private:
				std::size_t level;
				std::shared_ptr<pb::util::mpsc<pb::detail::multibar_write_message>> chan;

			public:
				multibar_notifying_streambuf(std::size_t level, const std::shared_ptr<pb::util::mpsc<pb::detail::multibar_write_message>> & chan);

			protected:
				virtual int sync() override;
			};

			multibar_notifying_streambuf buf;

		public:
			multibar_notifying_pipe(std::size_t level, const std::shared_ptr<pb::util::mpsc<pb::detail::multibar_write_message>> & chan);
		};
	}


	class multibar {
	private:
		std::ostream * output;
		std::vector<std::string> lines;
		std::list<pb::detail::multibar_notifying_pipe> bar_pipes;  // Must be a list, because it never invalidates pointers
		std::shared_ptr<pb::util::mpsc<pb::detail::multibar_write_message>> chan;

	public:
		/// Empty multibar to the standard output.
		explicit multibar();

		/// Empty multibar to the specified stream.
		explicit multibar(std::ostream & out);


		/// Add pure text between bars.
		///
		/// For example: you could add a header to your application, or separators between bars.
		///
		/// # Examples
		///
		/// ```
		/// # const auto count = 1024u;
		/// pb::multibar mb;
		///
		/// mb.println("Application header:");
		/// auto bar_1 = mb.create_bar(count);
		/// mb.println("Text line between bar 1 and bar 2");
		/// auto bar_2 = mb.create_bar(count);
		/// mb.println("Text line after bar 2");
		///
		/// // Set up bar updaters
		///
		/// mb.listen();
		/// ```
		///
		/// Will yield:
		///
		/// ```plaintext
		/// Application header:
		/// [====The=first=progress=bar>------]
		/// Text line between bar 1 and bar 2
		/// [====The=second=progress=bar>-----]
		/// Text line after bar 2
		/// ```
		void println(const char * s);
		void println(std::string s);

		/// Create a progress bar for the next level.
		///
		/// While you can, you absolutely shall *not* prolong the life of the the returned `progressbar` (or its copies) past that of this `multibar`:
		/// special `ostream` instances used by the bars are owned by `multibar` and only live as long as it does.
		///
		/// # Examples
		///
		/// ```
		/// # const auto count1 = 1024u;
		/// # const auto count2 = 2048u;
		/// # const auto count3 = 4096u;
		/// pb::multibar mb;
		///
		/// // progress bar on level 1
		/// auto bar_1 = mb.create_bar(count1);
		///
		/// // progress bar on level 2
		/// auto bar_2 = mb.create_bar(count2);
		///
		/// // progress bar on level 3
		/// auto bar_3 = mb.create_bar(count3);
		///
		/// // Set up bar updaters
		///
		/// mb.listen();
		/// ```
		progressbar create_bar(std::size_t total);

		/// Listen to bar updates.
		///
		/// Blocks until all child progress bars are finished.
		/// If you don't care about that, just run it in a different thread.
		///
		/// # Examples
		///
		/// ```
		/// pb::multibar mb;
		///
		/// // create some bars here
		///
		/// std::thread mb_listener([](pb::multibar mb) {
		///     mb.listen();
		///     println!("all bars done!");
		/// }, std::move(mb));
		///
		/// // Do stuff, but probably don't write to stdout
		/// ```
		void listen();


		multibar(const multibar & other) = delete;
		multibar & operator=(const multibar & other) = delete;

		multibar(multibar && other) = default;
		multibar & operator=(multibar && other) = default;
	};
}
