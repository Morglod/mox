the JIT backend packs each variable's type size into 16 bits,  
so any single global/local larger than ~64 KB aborts

pointers could not pass from comptime to runtime

comptime execution with llvm and C backend are limited to interpreter, so it could be slow

i use unix paths and bash even on windows
