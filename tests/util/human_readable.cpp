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


#include "../../include/pb-cpp/util.hpp"
#include <catch.hpp>
#include <cmath>
#include <sstream>
#include <type_traits>


TEST_CASE("pb::util::make_human_readable()", "[util][human_readable]") {
	const auto rdbl = pb::util::make_human_readable(69);
	static_assert(std::is_same<std::remove_cv_t<decltype(rdbl)>, pb::util::human_readable>::value, "Bad pb::util::make_human_readable() return type");

	REQUIRE(rdbl.size == 69);
}

TEST_CASE("pb::util::human_readable::operator<<(0)", "[util][human_readable]") {
	std::stringstream ss;
	ss << pb::util::make_human_readable(0);
	REQUIRE(ss.str() == "0 B");
}

TEST_CASE("pb::util::human_readable::operator<<(1-1023)", "[util][human_readable]") {
	for(auto i = 1u; i < 1024; ++i) {
		std::stringstream ss;
		ss << pb::util::make_human_readable(i);
		REQUIRE(ss.str() == std::to_string(i) + " B");
	}
}

TEST_CASE("pb::util::human_readable::operator<<(KiB)", "[util][human_readable]") {
	for(auto i = 1024u; i < 1024 * 1024; ++i) {
		std::stringstream ss;
		ss << pb::util::make_human_readable(i);

		std::stringstream oss;
		oss << std::round((static_cast<double>(i) / 1024.) * 10) / 10. << " KiB";

		REQUIRE(ss.str() == oss.str());
	}
}

// Above the KiB range, the next swathes are just too big to feasibly check within a reasonable timeframe, just do spotchecks

TEST_CASE("pb::util::human_readable::operator<<(MiB)", "[util][human_readable]") {
	for(auto i : {1024ull * 1024ull, 1024ull * 1024ull * 12ull, 1024ull * 1024ull * 12ull + 612ull, 1024ull * 1024ull * 512ull}) {
		std::stringstream ss;
		ss << pb::util::make_human_readable(i);

		std::stringstream oss;
		oss << std::round((static_cast<double>(i) / 1024. / 1024.) * 10) / 10. << " MiB";

		REQUIRE(ss.str() == oss.str());
	}
}

TEST_CASE("pb::util::human_readable::operator<<(GiB)", "[util][human_readable]") {
	for(auto i :
	    {1024ull * 1024ull * 1024ull, 1024ull * 1024ull * 1024ull * 12ull, 1024ull * 1024ull * 1024ull * 12ull + 612ull, 1024ull * 1024ull * 1024ull * 512ull}) {
		std::stringstream ss;
		ss << pb::util::make_human_readable(i);

		std::stringstream oss;
		oss << std::round((static_cast<double>(i) / 1024. / 1024. / 1024.) * 10) / 10. << " GiB";

		REQUIRE(ss.str() == oss.str());
	}
}
