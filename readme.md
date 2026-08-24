# mox programming language

* Statically typed, data oriented, low level, no GC, no OOP, etc
* Fast compilation (0.5-1 mil LOC/sec)
* Comptime execution and code generation
* Interpreter based type checking, ast and types are first class
* Hygyenic macros
* Generics and function overloading
* 3 backends supported: fast x86_64, llvm, C
* DWARF debug symbols
* Cyclic imports and forward references *mostly work*

x86_64 targetted, but llvm and c backend could emit for any platform

windows / linux tested

Planned:

* Better CLI
* Better std lib
* Syntax/semantic stabilization
* Compiler optimizations
* Inline assembler
* Compiler hooks
* Generic modules

## Some feature highlights

Builtin 3d math with swizzling:

```rust
a: [4]f32 = 0;
b: [4]f32 = 0;
c: [3]f32 = (a + b).xyw * 2.0f;
```

Macros:

```rust
fn #scoped_temp_buffer($buf_size: i64): []u8 {
    buf: [$buf_size]u8 = #memory_garbage;
    buf_slice := buf.^ |> slice();
    return buf_slice;
}

fn foo() {
    temp_buf := #scoped_temp_buffer(512);
    // temp_buf is alive only in current scope
}
```

Comptime with codegen example:

```rust
fn go_like_import($path: []u8) {
    cached_path := path_to_cache($path);
    if (!cache_exists(cached_path)) {
        download_dep($path, cached_path);
    }
    ast := __compiler_parse(#format_temp("import \"{}\";", .{ cached_path; }));
    return ast;
}

// becomes import "/cache/path/module.mox";
#run #land_ast go_like_import("github.com/module/path");
```

Interp based type checking:

```rust
fn foo() {
    #run {
        type := i32;
        if (...) {
            type = f32;
        }
        x: type = 0;
    };
}

fn _promote_type_to_pointer($T: __type_ptr): __type_ptr {
    if ($T.cast(*MoxType).kind == .Ptr) {
        return $T;
    }
    return #type *$T;
}

fn something(storage: *Storage($T)): #run _promote_type_to_pointer($T) {
    if (#run $T.cast(*MoxType).kind != .Ptr) {
        return nullptr($T);
    } else {
        return zeroed($T);
    }
}
```

## CLI

```bash
mox help
mox ./entry_module.mox
mox compile="./entry_module.mox" backend=llvm
```
