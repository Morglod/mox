*This is release repository. Development is going on closed source for now.*  

# mox

Mox is statically typed, data oriented, low level programming language, for software and games.

* Compile time code execution and code generation
* Interpreter based type checking, ast and types are first class
* Fast compilation (20-30x faster than clang, 0.5-1 mil LOC/sec vs 28k LOC/sec)
* Hygyenic macros
* Generics, polymorphism and function overloading
* 3 backends supported: fast x86_64 JIT, LLVM, C
* Cyclic module imports
* DWARF debug symbols

JIT backend is x86_64 targetted, but llvm and C backend could emit for any platform  

Windows / Linux x86_64 tested

To know language better, you can read [./by_example.mox](./by_example.mox)

[Compilation speed comparison](./benchmark/result.md)

## Download link

Zip archive already contains both versions of compiler and base modules.  
Compile hello.mox from root directory.

* [0.1.0 pre release (x86_64 win/linux)](https://github.com/Morglod/mox/releases/download/0.1.0/mox_010_250826_2.zip)

*Later smaller version without LLVM backend will be added (few megabytes)*

*For highlighting you can use C or Go or Rust for now*

## CLI

There are compatability flags like -O0, -O3, -g, -o  
But mostly CLI arguments are verbose

Compiler emits obj file on JIT and LLVM backends, but by default runs external linker too

```bash
mox help
mox ./hello.mox
mox compile="./hello.mox" backend=llvm
mox ./hello.mox -O3 -o ./hello
```

## Some feature highlights

Builtin 3d math with swizzling:

```rust
a: [4]f32 = 0;
b: [4]f32 = 0;
c: [3]f32 = (a + b).xyw * 2.0f;
```

Compile time execution with code generation example:

```rust
fn go_like_import($path: []u8) {
    cached_path := path_to_cache($path);
    if (!cache_exists(cached_path)) {
        download_dep($path, cached_path);
    }
    ast := __compiler_parse(#format_temp("import \"{}\";", .{ cached_path; }));
    return ast;
}

// becomes import "cache/path/module.mox";
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

## Planned

* Union macros and struct fields offsets
* Inline assembler
* llvm-jit double backend for fast compile time execution
* Better compile time debugging and function introspection
* Web target
* Hook compiler from user space
