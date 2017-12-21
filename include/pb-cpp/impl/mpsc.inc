// The MIT License (MIT)

// Copyright (c) 2017 nabijaczleweli

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.


#include "../progressbar.hpp"
#include <type_traits>


template <class T>
bool pb::util::mpsc<T>::empty() {
	std::lock_guard<std::mutex> grd(lock);
	return msgs.empty();
}


template <class T>
void pb::util::mpsc<T>::push(const T & elem) {
	{
		std::lock_guard<std::mutex> grd(lock);
		msgs.push(elem);
	}
	condvar.notify_all();
}

template <class T>
void pb::util::mpsc<T>::push(T && elem) {
	{
		std::lock_guard<std::mutex> grd(lock);
		msgs.push(std::move(elem));
	}
	condvar.notify_all();
}

template <class T>
template <class... Args>
void pb::util::mpsc<T>::emplace(Args &&... args) {
	{
		std::lock_guard<std::mutex> grd(lock);
		msgs.emplace(std::forward<Args>(args)...);
	}
	condvar.notify_all();
}

template <class T>
T pb::util::mpsc<T>::pop() {
	std::lock_guard<std::mutex> grd(lock);
	T ret(std::move(msgs.front()));
	msgs.pop();
	return ret;
}

template <class T>
void pb::util::mpsc<T>::wait() {
	std::unique_lock<std::mutex> lck(condvar_back);
	condvar.wait(lck);
}
