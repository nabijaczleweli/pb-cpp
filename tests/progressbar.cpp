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
#include <catch.hpp>
#include <iostream>


static void check_default_config(const pb::progressbar & bar);


TEST_CASE("pb::progressbar(count)", "[progressbar]") {
	pb::progressbar bar(420);
	REQUIRE(bar.total == 420);
	REQUIRE(bar.unit == pb::unit_t::none);
	check_default_config(bar);
}

TEST_CASE("pb::progressbar(count, out)", "[progressbar]") {
	pb::progressbar bar(420, std::cerr);
	REQUIRE(bar.total == 420);
	REQUIRE(bar.unit == pb::unit_t::none);
	check_default_config(bar);
}

TEST_CASE("pb::progressbar(const progressbar &)", "[progressbar]") {
	pb::progressbar bar_src(69);
	bar_src.unit = pb::unit_t::byte;
	pb::progressbar bar_dest(bar_src);
	REQUIRE(bar_dest.unit == pb::unit_t::byte);
}

TEST_CASE("pb::progressbar(progressbar &&)", "[progressbar]") {
	pb::progressbar bar_src(69);
	bar_src.unit = pb::unit_t::byte;
	pb::progressbar bar_dest(std::move(bar_src));
	REQUIRE(bar_dest.unit == pb::unit_t::byte);
}

TEST_CASE("pb::progressbar::operator=(const progressbar &)", "[progressbar]") {
	pb::progressbar bar_dest(420);
	pb::progressbar bar_src(69);
	bar_src.unit = pb::unit_t::byte;

	bar_dest = bar_src;
	REQUIRE(bar_dest.unit == pb::unit_t::byte);
}

TEST_CASE("pb::progressbar::operator=(progressbar &&)", "[progressbar]") {
	pb::progressbar bar_dest(420);
	pb::progressbar bar_src(69);
	bar_src.unit = pb::unit_t::byte;

	bar_dest = std::move(bar_src);
	REQUIRE(bar_dest.unit == pb::unit_t::byte);
}

TEST_CASE("pb::progressbar::tick_format() -- show_tick", "[progressbar]") {
	pb::progressbar bar(420);
	bar.tick_format("tick");
	REQUIRE(bar.show_tick == true);
}


void check_default_config(const pb::progressbar & bar) {
	REQUIRE(bar.show_bar == true);
	REQUIRE(bar.show_speed == true);
	REQUIRE(bar.show_percent == true);
	REQUIRE(bar.show_counter == true);
	REQUIRE(bar.show_time_left == true);
	REQUIRE(bar.show_tick == false);
	REQUIRE(bar.show_message == true);
}
