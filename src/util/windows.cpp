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


#ifdef _WIN32


#include "../../include/pb-cpp/util.hpp"

#define WIN32_LEAN_AND_MEAN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>


struct handle_data {
	HANDLE handle;
	CONSOLE_SCREEN_BUFFER_INFO csbi;
};

static nonstd::optional<handle_data> get_handle();


nonstd::optional<std::size_t> pb::util::terminal_width() {
	const auto handle = get_handle();
	if(handle)
		return {static_cast<std::size_t>(handle->csbi.srWindow.Right - handle->csbi.srWindow.Left - 1)};
	else
		return nonstd::nullopt;
}

std::ostream & pb::util::operator<<(std::ostream & out, cursor_up_mover cum) {
	auto handle = get_handle();
	if(handle) {
		handle->csbi.dwCursorPosition.Y -= cum.by;
		SetConsoleCursorPosition(handle->handle, handle->csbi.dwCursorPosition);
	}
	return out;
}


static nonstd::optional<handle_data> get_handle() {
	const auto hand = GetStdHandle(STD_OUTPUT_HANDLE);

	CONSOLE_SCREEN_BUFFER_INFO csbi{};
	if(GetConsoleScreenBufferInfo(hand, &csbi))
		return handle_data{hand, csbi};
	else
		return nonstd::nullopt;
}


#endif
