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


#include <pb-cpp/progressbar.hpp>
#include <fstream>

int main() {
	std::ifstream in_file("/usr/share/dict/words", std::ios::binary);
	const auto file_size = std::ifstream("/usr/share/dict/words", std::ios::ate | std::ios::binary).tellg();

	std::ofstream out_file("copy-words", std::ios::binary);

	pb::progressbar bar(file_size);
	bar.unit = pb::unit_t::byte;

	char buf[4096];
	do {
		in_file.read(buf, sizeof buf);
		out_file.write(buf, in_file.gcount());
		bar += in_file.gcount();
	} while(in_file.gcount() == sizeof buf);
	bar.finish();
}
