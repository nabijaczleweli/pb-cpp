# Terminal progress bar for C++ [![TravisCI Build Status](https://travis-ci.org/nabijaczleweli/pb-cpp.svg?branch=master)](https://travis-ci.org/nabijaczleweli/pb-cpp) [![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/7c1ghw38fowefi3s/branch/master?svg=true)](https://ci.appveyor.com/project/nabijaczleweli/pb-cpp/branch/master) [![Licence](https://img.shields.io/badge/license-MIT-blue.svg?style=flat)](LICENSE)
Console progress bar for C++ inspired from [pb](https://github.com/a8m/pb).

Or, you know, get an average frame from a video.

## Examples

TODO: adapt to C++

1. simple example

```rust
extern crate pbr;

use pbr::ProgressBar;
use std::thread;

fn main() {
    let count = 1000;
    let mut pb = ProgressBar::new(count);
    pb.format("╢▌▌░╟");
    for _ in 0..count {
        pb.inc();
        thread::sleep_ms(200);
    }
    pb.finish_print("done");
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

3. Broadcast writing(simple file copying)

```rust
#![feature(io)]
extern crate pbr;

use std::io::copy;
use std::io::prelude::*;
use std::fs::File;
use pbr::{ProgressBar, Units};

fn main() {
    let mut file = File::open("/usr/share/dict/words").unwrap();
    let n_bytes = file.metadata().unwrap().len() as usize;
    let mut pb = ProgressBar::new(n_bytes);
    pb.set_units(Units::Bytes);
    let mut handle = File::create("copy-words").unwrap().broadcast(&mut pb);
    copy(&mut file, &mut handle).unwrap();
    pb.finish_print("done");
}
```
