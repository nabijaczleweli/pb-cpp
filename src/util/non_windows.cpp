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


#ifndef _WIN32


#include "../../include/pb-cpp/util.hpp"
#include <sys/ioctl.h>
#include <termios.h>
#include <iostream>
#include <unistd.h>


nonstd::optional<std::size_t> pb::util::terminal_width() {
	if(!isatty(STDOUT_FILENO))
		return nonstd::nullopt;

	winsize ws{};
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
	if(ws.ws_col > 0)
		return ws.ws_col;
	else
		return nonstd::nullopt;
}

std::ostream & pb::util::operator<<(std::ostream & out, cursor_up_mover cum) {
	out << "\x1B[" << cum.by << "A";
	return out;
}

void pb::util::cursor_hide::hide() {
	if(!(actually = isatty(STDOUT_FILENO)))
		return;

	std::cout << "\x1b[?25l";
}

void pb::util::cursor_hide::restore() const {
	std::cout << "\x1b[?25h";
}


#endif
