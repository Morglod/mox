## 0.1.3

Better build tools, better temporary memory handling in std  
Raylib dll now ships inside, so you can just import vendor/raylib.mox and use it.

### Compiler

All paths that comes from compiler (like source location) are with / slashes even on windows.

`fn __compiler_output_dir(): []u8` returns path to output directory at comptime (linker output or obj/asm/ir artifact).

### Modules

`MOX_OS` constant now could be used to distinguish between target OS.

Temporary memory handling in std like `os_get_env` fixed

raylib vendor module now ships with release dlls which are linked and copied automatically by new comptime build tools

`build.mox` module with comptime build helpers

`build_resolve_path(path, loc): []u8` resolves path from specified source location (uses caller location by default).  
Userful so file path is resolved relative to caller's module path

`build_copy_to_output(src_path)` copies file specified by src_path to output directory

It is used in raylib link module:
```rust
#run {
    __compiler_link_lib(build_resolve_path("./libraylibdll.a"), false);
    build_copy_to_output(build_resolve_path("./raylib.dll"));
}
```

## 0.1.2

### Compiler

1. `i128` / `u128` fully supported now.

Because untyped literals are i128, for full u128 bit constant, it should be explicitly typed.

2. Hard error when specifiying default values for comptime (generic) arguments in functions

3. Function pointers now could survive comptime -> runtime, so we can use them as generic params

```rust
struct Container($F: *fn(): i32) {
    x: i32;
};

fn foo(c: *Container($F)): i32 {
    return $F();
}

fn goo(): i32 {
    return 10;
}

fn boo() {
    x: Container(goo.^) = 0;
    foo(x.^);
}
```

4. Conditional compilation with #if (condition) { ... } else { ... }  
Good replacement for "#run { #emit }" pattern  
It is more performant (because avoids code generation and parsing) and is not deferred as #run

```rust
#if (MOX_PLATFORM == .x86_64_win) {
    import "./win.mox";
} else {
    import "./not_win.mox";
}
```

5. Indexing generic bound array fixed

```rust
struct Generic($ARR: [$N]u64) {}
fn index(a: *Generic($ARR), i: i64): u64 {
    return $ARR[0]; // now works
}
```

6. Inferring poly arguments of function pointer now works

### Modules

1. Rapidhash implementation

2. Comptime error when not enough values passed to format

3. Slice split_iterator which iterates over non-splitter sub slices

```rust
for (it : split_iterator(",a,b,c", ",")) {
    // it here is: "", "a", "b", "c"
}
```

4. parse_int, parse_uint, parse_float utils

5. Std module that just re-exports all core modules

6. Hash maps implementation

---

## 0.1.0

initial release
