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
#include <cmath>


std::ostream & pb::util::operator<<(std::ostream & out, human_readable hr) {
	static const auto ln_kib       = std::log(1024.);
	static const char * suffixes[] = {"B", "KiB", "MiB", "GiB", "TiB", "PiB", "EiB", "ZiB", "YiB"};


	if(hr.size == 0)
		out << "0 B";
	else {
		const auto exp = std::min(std::max(static_cast<std::size_t>(std::log(static_cast<double>(hr.size)) / ln_kib), static_cast<std::size_t>(0)),
		                          sizeof(suffixes) / sizeof(*suffixes));
		const auto val = static_cast<double>(hr.size) / std::pow(2., exp * 10);

		if(exp > 0)
			out << std::round(val * 10.) / 10. << ' ' << suffixes[static_cast<std::size_t>(exp)];
		else
			out << std::round(val) << ' ' << suffixes[0];
	}

	return out;
}

pb::util::human_readable pb::util::make_human_readable(std::size_t size) {
	return {size};
}
