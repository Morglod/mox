Windows 11 x86_64  
Intel Ultra 9 185H, laptop

## Compilation speed

|                  | emit exe  | vs mox | emit obj  | vs mox |
|------------------|-----------|--------|-----------|--------|
| mox 0.1.0 jit    | 0.499 sec |   1.0x | 0.436 sec |   1.0x |
| mox 0.1.0 llvm   | 8.8 sec   |  17.6x | 8.57 sec  |  19.7x |
| rustc 1.96.1     | 33.9 sec  |  67.9x | 36.9 sec  |  84.6x |
| clang 19.1.3     | 13.3 sec  |  26.7x | 13.1 sec  |  30.0x |
| odin dev-2026-08 | 13.8 sec  |  27.7x | 19.8 sec  |  45.4x |

```bash
node ./generate.js

# emit exe

time mox ./200k.mox backend=jit
time mox ./200k.mox backend=llvm
time clang ./200k.c -O0
time RUST_MIN_STACK=2147483648 rustc -C debuginfo=0 -C opt-level=0 ./200k.rs
time odin build ./200k.odin -file -o:none

# emit obj

time mox ./200k.mox linker=false backend=jit
time mox ./200k.mox linker=false backend=llvm
time clang -c ./200k.c -O0
time RUST_MIN_STACK=2147483648 rustc -C debuginfo=0 -C opt-level=0 ./200k.rs --emit=obj
time odin build ./200k.odin -file -opt:0 -build-mode:obj
```

## Rapidhash

MB/s, higher is better

![](./rapidhash.png)

## Hash Maps

ns/op, less is better

mox HashMap vs robin_hood::unordered_flat_map
mox SwissMap vs phmap::flat_hash_map

![](./map_bench.png)
