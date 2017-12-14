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
#include <algorithm>
#include <cstring>
#include <iostream>
#include <utility>


pb::progressbar::progressbar(std::size_t t) : progressbar(t, std::cout) {}

pb::progressbar::progressbar(std::size_t t, std::ostream & o)
      : start_time(std::chrono::steady_clock::now()),         //
        last_refresh_time(std::chrono::steady_clock::now()),  //
        tick_chars(defaults::tick_format),                    //
        cur_tick_idx(0),                                      //
        current(0),                                           //
        output(std::addressof(o)),                            //
        unit(unit_t::none),                                   //
        total(t),                                             //
        is_finish(false),                                     //
        is_multibar(false),                                   //
        show_bar(true),                                       //
        show_speed(true),                                     //
        show_percent(true),                                   //
        show_counter(true),                                   //
        show_time_left(true),                                 //
        show_tick(false),                                     //
        show_message(true) {
	format(defaults::bar_format);
}

pb::progressbar::progressbar(const progressbar & other)
      : start_time(other.start_time),                      //
        last_refresh_time(other.last_refresh_time),        //
        bar_max_refresh_rate(other.bar_max_refresh_rate),  //
        bar(other.bar),                                    //
        tick_chars(other.tick_chars),                      //
        cur_tick_idx(other.cur_tick_idx),                  //
        current(other.current),                            //
        bar_width(other.bar_width),                        //
        start_message(other.start_message),                //
        output(other.output),                              //
        unit(other.unit),                                  //
        total(other.total),                                //
        is_finish(other.is_finish),                        //
        is_multibar(other.is_multibar),                    //
        show_bar(other.show_bar),                          //
        show_speed(other.show_speed),                      //
        show_percent(other.show_percent),                  //
        show_counter(other.show_counter),                  //
        show_time_left(other.show_time_left),              //
        show_tick(other.show_tick),                        //
        show_message(other.show_message) {}

pb::progressbar::progressbar(progressbar && other)
      : start_time(std::move(other.start_time)),                      //
        last_refresh_time(std::move(other.last_refresh_time)),        //
        bar_max_refresh_rate(std::move(other.bar_max_refresh_rate)),  //
        bar(std::move(other.bar)),                                    //
        tick_chars(std::move(other.tick_chars)),                      //
        cur_tick_idx(std::move(other.cur_tick_idx)),                  //
        current(std::move(other.current)),                            //
        bar_width(std::move(other.bar_width)),                        //
        start_message(std::move(other.start_message)),                //
        output(std::move(other.output)),                              //
        unit(std::move(other.unit)),                                  //
        total(std::move(other.total)),                                //
        is_finish(std::move(other.is_finish)),                        //
        is_multibar(std::move(other.is_multibar)),                    //
        show_bar(std::move(other.show_bar)),                          //
        show_speed(std::move(other.show_speed)),                      //
        show_percent(std::move(other.show_percent)),                  //
        show_counter(std::move(other.show_counter)),                  //
        show_time_left(std::move(other.show_time_left)),              //
        show_tick(std::move(other.show_tick)),                        //
        show_message(std::move(other.show_message)) {}


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
	show_tick  = true;
}

void pb::progressbar::tick_format(std::string fmt) {
	tick_chars = std::move(fmt);
	show_tick  = true;
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


pb::progressbar & pb::progressbar::operator=(const progressbar & other) {
	start_time           = other.start_time;
	last_refresh_time    = other.last_refresh_time;
	bar_max_refresh_rate = other.bar_max_refresh_rate;
	bar                  = other.bar;
	tick_chars           = other.tick_chars;
	cur_tick_idx         = other.cur_tick_idx;
	current              = other.current;
	bar_width            = other.bar_width;
	start_message        = other.start_message;
	output               = other.output;
	unit                 = other.unit;
	total                = other.total;
	is_finish            = other.is_finish;
	is_multibar          = other.is_multibar;
	show_bar             = other.show_bar;
	show_speed           = other.show_speed;
	show_percent         = other.show_percent;
	show_counter         = other.show_counter;
	show_time_left       = other.show_time_left;
	show_tick            = other.show_tick;
	show_message         = other.show_message;

	return *this;
}

pb::progressbar & pb::progressbar::operator=(progressbar && other) {
	start_time           = std::move(other.start_time);
	last_refresh_time    = std::move(other.last_refresh_time);
	bar_max_refresh_rate = std::move(other.bar_max_refresh_rate);
	bar                  = std::move(other.bar);
	tick_chars           = std::move(other.tick_chars);
	cur_tick_idx         = std::move(other.cur_tick_idx);
	current              = std::move(other.current);
	bar_width            = std::move(other.bar_width);
	start_message        = std::move(other.start_message);
	output               = std::move(other.output);
	unit                 = std::move(other.unit);
	total                = std::move(other.total);
	is_finish            = std::move(other.is_finish);
	is_multibar          = std::move(other.is_multibar);
	show_bar             = std::move(other.show_bar);
	show_speed           = std::move(other.show_speed);
	show_percent         = std::move(other.show_percent);
	show_counter         = std::move(other.show_counter);
	show_time_left       = std::move(other.show_time_left);
	show_tick            = std::move(other.show_tick);
	show_message         = std::move(other.show_message);

	return *this;
}


void pb::progressbar::draw() {
	*output << "henlo\n";
}
