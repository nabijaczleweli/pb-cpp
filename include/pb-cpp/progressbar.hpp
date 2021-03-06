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


#include "unit.hpp"
#include <chrono>
#include <cstddef>
#include <nonstd/optional.hpp>
#include <ostream>
#include <string>
#include <vector>


namespace pb {
	class progressbar {
	private:
		struct bar_format {
			char start;
			char current;
			char current_n;
			char remain;
			char end;
		};


		std::chrono::steady_clock::time_point start_time;
		std::chrono::steady_clock::time_point last_refresh_time;
		nonstd::optional<std::chrono::steady_clock::duration> bar_max_refresh_rate;

		bar_format bar;
		std::string tick_chars;  // Used as vec<char>
		std::size_t cur_tick_idx;

		std::size_t current;
		nonstd::optional<std::size_t> bar_width;
		std::string start_message;

		std::ostream * output;

		bool is_finish;

	public:
		unit_t unit;
		std::size_t total;

		/// Whether to display the actual bar.
		///
		/// Default: true.
		bool show_bar;

		/// Whether to display the speed in units per second or minute.
		///
		/// Default: true.
		bool show_speed;

		/// Whether to display the progress %.
		///
		/// Default: true.
		bool show_percent;

		/// Whether to display "current / total".
		///
		/// Default: true.
		bool show_counter;

		/// Whether to show approximate time left.
		///
		/// Default: true.
		bool show_time_left;

		/// Whether to display rolling ticker (see tick_format()).
		///
		/// Default: false.
		bool show_tick;

		/// Whether to display the custom message.
		///
		/// Default: true.
		/// Default message: empty.
		bool show_message;


		/// Default configuration to the standard output.
		explicit progressbar(std::size_t total);

		/// Default configuration to a specified output stream.
		///
		/// # Examples
		///
		/// ```
		/// const auto count = 1000u;
		/// std::fstream file("progressbar.out");
		/// pb::progressbar bar(count, file);
		///
		/// bar.set_units(unit_t::byte);
		///
		/// for(auto i = 0u; i < count; ++i) {
		///    ++bar;
		///    std::this_thread::sleep_for(100ms);
		/// }
		/// ```
		progressbar(std::size_t total, std::ostream & out);


		/// Set custom format for the overall bar, default is `pb::defaults::bar_format`, a.k.a. `"[=>-]"`.
		///
		/// The bar format is as follows:
		///
		/// | Character ID |           Meaning           | Default |
		/// |--------------|-----------------------------|---------|
		/// |      `0`     |          bar opener         |   '['   |
		/// |      `1`     |        progress done        |   '='   |
		/// |      `2`     | done -> not done transition |   '>'   |
		/// |      `3`     |      progress not done      |   '-'   |
		/// |      `4`     |          bar closer         |   ']'   |
		///
		/// Or, in other words:
		///
		/// ```plaintext
		/// 011111111111111123333333333333333333334
		/// ^^~~~~~~~~~~~~~^^^~~~~~~~~~~~~~~~~~~~^^
		/// | progress done |  progress not done  |
		/// |               |                     |
		/// opener          transition            closer
		/// ```
		///
		/// For default format:
		///
		/// ```plaintext
		/// [===============>---------------------]
		/// ```
		///
		/// # Examples
		///
		/// ```
		/// # const auto count = 1000u;
		/// pb::progressbar bar(count);
		/// bar.format("<#}_>");
		/// ```
		///
		/// Yields:
		///
		/// ```plaintext
		/// <###############}_____________________>
		/// ```
		void format(const char * fmt) noexcept;
		void format(const std::string & fmt) noexcept;

		/// Set ticker format for the bar, default is `pb::defaults::tick_format`, a.k.a. `"\|/-"`.
		///
		/// The format is not bounded to any non-zero length and will simply cycle through all characters.
		///
		/// If the provided character sequence is non-empty, sets show_tick.
		///
		/// # Examples
		///
		/// ```
		/// # const auto count = 1000u;
		/// pb::progressbar bar(count);
		/// bar.tick_format("]|[")
		/// ```
		void tick_format(const char * fmt);
		void tick_format(std::string fmt);
		template <class FwdItr>
		void tick_format(FwdItr begin, FwdItr end);

		/// (Re)set message to display in the prefix.
		///
		/// Call no-arg version, with empty string, or `nullptr` to not display message.
		///
		/// All CRs and LFs are replaced with spaces.
		///
		/// *Nota bene*: does not set show_message.
		///
		/// # Examples
		///
		/// ```
		/// pb::progressbar bar(1000);
		///
		/// for(auto i = 0u; i < 1000u; ++i) {
		/// 	switch(i) {
		/// 		case 0:
		/// 			bar.message("Doing 1st quarter");
		/// 			break;
		/// 		case 250:
		/// 			bar.message("Doing 2nd quarter");
		/// 			break;
		/// 		case 500:
		/// 			bar.message("Doing 3rd quarter");
		/// 			break;
		/// 		case 750:
		/// 			bar.message("Doing 4th quarter");
		/// 			break;
		/// 	}
		///
		/// 	++pb;
		/// }
		/// ```
		void message();
		void message(const char * msg);
		void message(const std::string & msg);

		/// Set bar width override.
		///
		/// No-arg version disables override.
		///
		/// The width priority is as follows:
		///   1. Set override
		///   2. Determination via OS-specific API
		///   3. `80`
		void width();
		void width(std::size_t w);

		/// Set max refresh rate, above which the progress bar will not redraw.
		///
		/// No-arg version unbounds.
		///
		/// # Examples
		///
		/// ```
		/// pb::progressbar bar(1000);
		/// bar.max_refresh_rate(100ms);
		/// ```
		void max_refresh_rate();
		template <class Dur>
		void max_refresh_rate(const Dur & rate);

		/// Update bar without making progress.
		///
		/// Useful for diagnosis if program is hanging or just not making any progress.
		///
		/// Calling this is not needed in conjunction with `+=` or `++`, which call it automatically.
		void tick();

		/// Add `amount` to current progress and re-draw the bar, if appropriate.
		///
		/// # Examples
		///
		/// ```
		/// pb::progressbar bar(10);
		/// bar += 5;
		/// bar.finish();
		/// ```
		progressbar & operator+=(std::size_t amount);

		/// Set current progress to `amount` and re-draw the bar, if appropriate.
		///
		/// # Examples
		///
		/// ```
		/// pb::progressbar bar(10);
		/// bar = 8;
		/// bar.finish();
		/// ```
		progressbar & operator=(std::size_t amount);

		/// Increment current progress by `1` and re-draw the bar, if appropriate.
		///
		/// # Examples
		///
		/// ```
		/// pb::progressbar bar(10);
		/// for(auto i = 0u; i < 10u; ++i)
		/// 	++bar;
		/// ```
		progressbar & operator++();

		/// Finish the bar by saturating the progress and redrawing it for the last time, or replacing it with the specified string.
		void finish();
		void finish(const char * repl);
		void finish(const std::string & repl);


		~progressbar();

		progressbar(const progressbar & other) = default;
		progressbar & operator=(const progressbar & other) = default;

		progressbar(progressbar && other);
		progressbar & operator=(progressbar && other);

	private:
		void format(const char * fmt, std::size_t len) noexcept;

		std::size_t calc_width();
		void draw();
		bool finish_draw(bool will_override);
		bool finish_with_replace(const char * repl, std::size_t length);
	};
}


#include "impl/progressbar.inc"
