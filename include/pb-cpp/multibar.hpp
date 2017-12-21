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
		std::list<pb::detail::multibar_notifying_pipe> bar_pipes;
		std::shared_ptr<pb::util::mpsc<pb::detail::multibar_write_message>> chan;

	public:
		/// Empty multibar to the standard output.
		explicit multibar();

		/// Empty multibar to the specified stream.
		explicit multibar(std::ostream & out);


		/// println used to add text lines between the bars.
		/// for example: you could add a header to your application,
		/// or text separators between bars.
		///
		/// # Examples
		///
		/// ```no_run
		/// use pbr::MultiBar;
		///
		/// let mut mb = MultiBar::new();
		/// mb.println("Application header:");
		///
		/// let mut p1 = MultiBar::create_bar(count);
		/// // ...
		///
		/// mb.println("Text line between bar1 and bar2");
		///
		/// let mut p2 = MultiBar::create_bar(count);
		/// // ...
		///
		/// mb.println("Text line between bar2 and bar3");
		///
		/// // ...
		/// // ...
		/// mb.listen();
		/// ```
		void println(const char * s);
		void println(std::string s);

		/// create_bar creates new `ProgressBar` with `Pipe` as the writer.
		///
		/// The ordering of the method calls is important. it means that in
		/// the first call, you get a progress bar in level 1, in the 2nd call,
		/// you get a progress bar in level 2, and so on.
		///
		/// ProgressBar that finish its work, must call `finish()` (or `finish_print`)
		/// to notify the `MultiBar` about it.
		///
		/// # Examples
		///
		/// ```no_run
		/// use pbr::MultiBar;
		///
		/// let mut mb = MultiBar::new();
		///
		/// // progress bar in level 1
		/// let mut p1 = MultiBar::create_bar(count1);
		/// // ...
		///
		/// // progress bar in level 2
		/// let mut p2 = MultiBar::create_bar(count2);
		/// // ...
		///
		/// // progress bar in level 3
		/// let mut p3 = MultiBar::create_bar(count3);
		///
		/// // ...
		/// mb.listen();
		/// ```
		progressbar create_bar(std::size_t total);


		/// listen start listen to all bars changes.
		///
		/// `ProgressBar` that finish its work, must call `finish()` (or `finish_print`)
		/// to notify the `MultiBar` about it.
		///
		/// This is a blocking operation and blocks until all bars will
		/// finish.
		/// To ignore blocking, you can run it in a different thread.
		///
		/// # Examples
		///
		/// ```no_run
		/// use pbr::MultiBar;
		///
		/// let mut mb = MultiBar::new();
		///
		/// // ...
		/// // create some bars here
		/// // ...
		///
		/// thread::spawn(move || {
		///     mb.listen();
		///     println!("all bars done!");
		/// });
		///
		/// // ...
		/// ```
		void listen();


		multibar(const multibar & other) = delete;
		multibar & operator=(const multibar & other) = delete;

		multibar(multibar && other) = default;
		multibar & operator=(multibar && other) = default;
	};
}
