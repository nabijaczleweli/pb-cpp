# Terminal progress bar for C++ [![TravisCI Build Status](https://travis-ci.org/nabijaczleweli/pb-cpp.svg?branch=master)](https://travis-ci.org/nabijaczleweli/pb-cpp) [![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/7c1ghw38fowefi3s/branch/master?svg=true)](https://ci.appveyor.com/project/nabijaczleweli/pb-cpp/branch/master) [![Licence](https://img.shields.io/badge/license-MIT-blue.svg?style=flat)](LICENSE)
Console progress bar for C++ inspired by [pb](https://github.com/a8m/pb).

[![Poster for demo video](demo/pb-cpp%20demo%20small.png)](https://raw.githubusercontent.com/nabijaczleweli/pb-cpp/master/demo/pb-cpp%20demo%20small.mp4)

## Examples

1. Simple example

```cpp
#include <pb-cpp/progressbar.hpp>
#include <thread>
using namespace std::literals;

int main() {
	const auto count = 1000u;
	pb::progressbar bar(count);
	bar.format("<#} >");

	for(auto i = 0u; i < count; ++i) {
		++bar;
		std::this_thread::sleep_for(200ms);
	}

	bar.finish();
}
```

2. `pb::multibar` example, see full example [here](examples/multi.cpp):
```cpp
#include <pb-cpp/multibar.hpp>
#include <thread>
using namespace std::literals;

int main() {
	const auto count = 100u;
	pb::multibar mb;
	mb.println("Application header:");

	const auto bar_handler = [](auto bar, auto count) {
		for(auto i = 0u; i < count; ++i) {
			++bar;
			std::this_thread::sleep_for(100ms);
		}

		bar.finish();
	};

	std::thread(bar_handler, mb.create_bar(count), count).detach();
	mb.println("Add a separator between the two bars");
	std::thread(bar_handler, mb.create_bar(count * 2), count * 2).detach();

	// Start listening to all bars' changes.
	// This blocks until all bars finish.
	// To ignore that, run it in a different thread.
	mb.listen();
}
```

3. Simple file copy

```cpp
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
```
