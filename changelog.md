## 0.1.6

Docs on how to work with memory

### Compiler

Removed comptime builtins that duplicated `*MoxType` reflection:
`__compiler_sizeof`, `__compiler_members_count`, `__compiler_member_type`, `__compiler_member_offset`, `__compiler_type_kind`, `__compiler_comptime_alloc` / `*_realloc` / `*_free` (use allocators), `__compiler_print`, `__compiler_print_i32`, `__interp_break`, `__compiler_assert_known_type`, `__compiler_get_type_of_value`, `__compiler_comptime_ptr_to_runtime_const_slice`, `__mox_jit_load_symbol`

`__type_equal` now takes `(__type_ptr, __type_ptr)` instead of "unknown" value type

`#is_interp` builtin added to check if current place is running inside interpreter (useful for loop and recursion, where compiler stack could exceed)

### Modules

`internal.mox`:
`sizeof_type` now just reads `MoxType.size`  
`mox_struct_fields`, `mox_struct_field_type` as a replacement for removed __compiler_* builtins.  
`__compiler_member_index_of_type` renamed to `mox_struct_field_index_of_type`.

`#is_comptime` is now fully in mox, so now exactly same code could run in comptime and runtime with this check

## 0.1.5

Added documentation and tooling for setup

Added clang install scripts for Windows

### Compiler

Args parsing fixed, now escaped \" and \\ inside string args works properly

`$LINKER_OUT` substitutions and other, rewritten to `:MOX_LINKER_OUT:` form

Now `mox help` prints all available substitutions

If substitution is misspelled, but starts with `:MOX_`, error is printed and compiler panics

## 0.1.4

### Compiler

Fixed interpreter `defer` that directly calls a function and poisons original returned value.

Fixed comptime function pointers taken in one module and used from another.

### Modules

Raylib link.mox autodiscovering on Linux

SDL3 prebuild shipped

Unix build tools updated

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
