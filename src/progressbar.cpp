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


#include "../include/pb-cpp/progressbar.hpp"
#include "../include/pb-cpp/defaults.hpp"
#include "../include/pb-cpp/util.hpp"
#include <algorithm>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <utility>


pb::progressbar::progressbar(std::size_t t) : progressbar(t, std::cout) {}

pb::progressbar::progressbar(std::size_t t, std::ostream & o)
      : start_time(std::chrono::steady_clock::now()),         //
        last_refresh_time(std::chrono::steady_clock::now()),  //
        tick_chars(defaults::tick_format),                    //
        cur_tick_idx(0),                                      //
        current(0),                                           //
        output(std::addressof(o)),                            //
        is_finish(false),                                     //
        unit(unit_t::none),                                   //
        total(t),                                             //
        show_bar(true),                                       //
        show_speed(true),                                     //
        show_percent(true),                                   //
        show_counter(true),                                   //
        show_time_left(true),                                 //
        show_tick(false),                                     //
        show_message(true) {
	format(defaults::bar_format);
}


void pb::progressbar::format(const char * fmt) noexcept {
	format(fmt, std::strlen(fmt));
}

void pb::progressbar::format(const std::string & fmt) noexcept {
	format(fmt.c_str(), fmt.size());
}

void pb::progressbar::format(const char * fmt, std::size_t len) noexcept {
	if(len >= 5) {
		bar.start     = fmt[0];
		bar.current   = fmt[1];
		bar.current_n = fmt[2];
		bar.remain    = fmt[3];
		bar.end       = fmt[4];
	}
}


void pb::progressbar::tick_format(const char * fmt) {
	tick_chars = fmt;
	show_tick  = !tick_chars.empty();
}

void pb::progressbar::tick_format(std::string fmt) {
	tick_chars = std::move(fmt);
	show_tick  = !tick_chars.empty();
}


template <class FwdItr>
static void pb_progressbar_message_impl(std::string & start_message, FwdItr begin, FwdItr end) {
	start_message.resize(end - begin);
	std::transform(begin, end, std::begin(start_message), [](char c) {
		if(c == '\n' || c == '\r')
			c = ' ';
		return c;
	});
}

void pb::progressbar::message() {
	start_message.clear();
}

void pb::progressbar::message(const char * msg) {
	if(!msg)
		start_message.clear();
	else
		pb_progressbar_message_impl(start_message, msg, msg + std::strlen(msg));
}

void pb::progressbar::message(const std::string & msg) {
	pb_progressbar_message_impl(start_message, std::begin(msg), std::end(msg));
}


void pb::progressbar::width() {
	bar_width = nonstd::nullopt;
}

void pb::progressbar::width(std::size_t w) {
	bar_width = {w};
}


void pb::progressbar::max_refresh_rate() {
	bar_max_refresh_rate = nonstd::nullopt;
}


void pb::progressbar::tick() {
	if(current <= total) {
		if(!tick_chars.empty())
			cur_tick_idx = (cur_tick_idx + 1) % tick_chars.size();
		draw();
	}
}


pb::progressbar & pb::progressbar::operator+=(std::size_t amount) {
	current += amount;
	tick();
	return *this;
}


pb::progressbar & pb::progressbar::operator=(std::size_t amount) {
	current = amount;
	tick();
	return *this;
}


pb::progressbar & pb::progressbar::operator++() {
	return *this += 1;
}


void pb::progressbar::finish() {
	finish_draw(false);
	output->flush();  // Speszol empty flush for multibar
}

void pb::progressbar::finish(const char * repl) {
	finish_with_replace(repl, std::strlen(repl));
	output->flush();  // Speszol empty flush for multibar
}

void pb::progressbar::finish(const std::string & repl) {
	finish_with_replace(repl.c_str(), repl.size());
	output->flush();  // Speszol empty flush for multibar
}


std::size_t pb::progressbar::calc_width() {
	if(bar_width)
		return bar_width.value();
	else
		return pb::util::terminal_width().value_or(80);
}


void pb::progressbar::draw() {
	const auto now = std::chrono::steady_clock::now();
	if(bar_max_refresh_rate && now - last_refresh_time < *bar_max_refresh_rate)
		return;

	const auto time_elapsed = now - start_time;
	const auto speed        = current / static_cast<double>(std::chrono::duration_cast<std::chrono::seconds>(time_elapsed).count());
	const auto width        = calc_width();

	std::size_t prefix_length = (show_message ? start_message.size() : 0) +  //
	                            ((show_tick && !tick_chars.empty()) ? 1 + 1 : 0);
	std::size_t body_length = 0;
	std::ostringstream additional_prefix;
	std::ostringstream suffix;

	*output << '\r';

	// message box
	if(show_message)
		*output << start_message;

	// counter box
	if(show_counter) {
		switch(unit) {
			case unit_t::none:
				additional_prefix << current << " / " << total;
				break;
			case unit_t::byte:
				additional_prefix << pb::util::make_human_readable(current) << " / " << pb::util::make_human_readable(total);
				break;
		}
		additional_prefix << ' ';

		*output << additional_prefix.str();
	}

	// tick box
	if(show_tick && !tick_chars.empty())
		*output << tick_chars[cur_tick_idx] << ' ';

	// precent box
	if(show_percent) {
		const auto percentage = total == 0 ? 0 : ((100 * current) / total);
		suffix << ' ' << std::setw(2) << percentage << "% ";
	}

	// speed box
	if(show_speed && !std::isinf(speed)) {
		switch(unit) {
			case unit_t::none:
				// 2 decimal places
				suffix << std::round(speed * 100) / 100;
				break;
			case unit_t::byte:
				suffix << pb::util::make_human_readable(speed);
				break;
		}
		suffix << "/s ";
	}

	// time left box
	if(show_time_left && current > 0 && total > current) {
		const auto left = 1. / speed * static_cast<double>(total - current);
		if(left < 60)
			suffix << std::round(left) << 's';
		else
			suffix << std::round(left / 60) << 'm';
	}

	// bar box
	if(show_bar) {
		const auto p = prefix_length + additional_prefix.tellp() + suffix.tellp() + 3;
		if(p < width) {
			const auto size                    = width - p;
			const std::size_t bar_filler_count = std::ceil((current / static_cast<double>(total)) * size);
			if(size >= bar_filler_count) {
				*output << bar.start;

				const auto bar_remainder_count = size - bar_filler_count;
				*output << std::setw(bar_filler_count - 1) << std::setfill(bar.current) << "";
				if(bar_remainder_count > 0 && bar_filler_count > 0)
					*output << bar.current_n;
				else
					*output << bar.current;

				*output << std::setw(bar_remainder_count) << std::setfill(bar.remain) << "";
				*output << bar.end << ' ';

				body_length += 1 + bar_filler_count + bar_remainder_count + 1 + 1;
			}
		}
	}

	*output << suffix.str();

	// pad
	const std::size_t full_width = prefix_length + additional_prefix.tellp() + body_length + suffix.tellp();
	if(full_width < width)
		*output << std::setw(width - full_width) << std::setfill(' ') << "";

	output->flush();
	last_refresh_time = std::chrono::steady_clock::now();
}


void pb::progressbar::finish_draw(bool will_override) {
	if(is_finish)
		return;

	auto redraw = false;

	if(bar_max_refresh_rate && std::chrono::steady_clock::now() - last_refresh_time < bar_max_refresh_rate) {
		bar_max_refresh_rate = nonstd::nullopt;
		redraw               = true;
	}

	if(current < total) {
		current = total;
		redraw  = true;
	}

	if(redraw && !will_override)
		draw();

	is_finish = true;
}


void pb::progressbar::finish_with_replace(const char * repl, std::size_t length) {
	finish_draw(true);

	*output << '\r' << repl;
	*output << std::setw(calc_width() - length) << std::setfill(' ') << "";

	output->flush();
}
