#!/usr/bin/env bash
set -x

node ./generate_generics.js

echo "with linker"

time mox ./generics.mox backend=jit
time mox ./generics.mox backend=llvm
time clang++ ./generics.cpp -O0
time RUST_MIN_STACK=2147483648 rustc -C debuginfo=0 -C opt-level=0 ./generics.rs
time odin build ./generics.odin -file -o:none

echo "emit obj"

time mox ./generics.mox linker=false backend=jit
time mox ./generics.mox linker=false backend=llvm
time clang++ -c ./generics.cpp -O0
time RUST_MIN_STACK=2147483648 rustc -C debuginfo=0 -C opt-level=0 ./generics.rs --emit=obj
time odin build ./generics.odin -file -opt:0 -build-mode:obj
