#!/usr/bin/env bash
set -x

node ./generate.js

echo "with linker"

time mox ./200k.mox backend=jit
time mox ./200k.mox backend=llvm
time clang ./200k.c -O0
time RUST_MIN_STACK=2147483648 rustc -C debuginfo=0 -C opt-level=0 ./200k.rs
time odin build ./200k.odin -file -o:none

echo "emit obj"

time mox ./200k.mox linker=false backend=jit
time mox ./200k.mox linker=false backend=llvm
time clang -c ./200k.c -O0
time RUST_MIN_STACK=2147483648 rustc -C debuginfo=0 -C opt-level=0 ./200k.rs --emit=obj
time odin build ./200k.odin -file -opt:0 -build-mode:obj