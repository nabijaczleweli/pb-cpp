# Terminal progress bar for C++ [![TravisCI Build Status](https://travis-ci.org/nabijaczleweli/pb-cpp.svg?branch=master)](https://travis-ci.org/nabijaczleweli/pb-cpp) [![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/7c1ghw38fowefi3s/branch/master?svg=true)](https://ci.appveyor.com/project/nabijaczleweli/pb-cpp/branch/master) [![Licence](https://img.shields.io/badge/license-MIT-blue.svg?style=flat)](LICENSE)
Console progress bar for C++ inspired by [pb](https://github.com/a8m/pb).

## Examples

TODO: adapt multibar to C++

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

2. MultiBar example. see full example [here](https://github.com/a8m/pb/blob/master/examples/multi.rs)
```rust
extern crate pbr;

use std::thread;
use pbr::MultiBar;
use std::time::Duration;

fn main() {
	let mut mb = MultiBar::new();
	let count = 100;
	mb.println("Application header:");

	let mut p1 = mb.create_bar(count);
	let _ = thread::spawn(move || {
		for _ in 0..count {
			p1.inc();
			thread::sleep(Duration::from_millis(100));
		}
		// notify the multibar that this bar finished.
		p1.finish();
	});

	mb.println("add a separator between the two bars");

	let mut p2 = mb.create_bar(count * 2);
	let _ = thread::spawn(move || {
		for _ in 0..count * 2 {
			p2.inc();
			thread::sleep(Duration::from_millis(100));
		}
		// notify the multibar that this bar finished.
		p2.finish();
	});

	// start listen to all bars changes.
	// this is a blocking operation, until all bars will finish.
	// to ignore blocking, you can run it in a different thread.
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
