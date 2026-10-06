the JIT backend packs each variable's type size into 16 bits,  
so any single global/local larger than ~64 KB aborts

raw pointers are rejected for comptime -> runtime

comptime execution with llvm and C backend are limited to interpreter, so it could be slow

i use unix paths and bash even on windows

you should always specify llvm_target_triple="x86_64-pc-windows-msvc" to mox compiler, to be able to link with msvc clang
