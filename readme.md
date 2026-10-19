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

[Getting started](https://mox-lang.dev/tutorials/getting-started/setup/)

To know language better, you can read [tutorials](https://mox-lang.dev/tutorials/) or [./by_example.mox](./by_example.mox)

[Compilation speed comparison](./benchmark/result.md)

## Download link

[Prerequisites](https://mox-lang.dev/tutorials/getting-started/setup/)

Zip archive already contains both versions of compiler and base modules.  

* [0.1.10 (x86_64 Windows/Linux)](https://github.com/Morglod/mox/releases/download/0.1.10/mox_0110_171026.zip)

*For highlighting you can use C or Go or Rust for now*

*Later smaller version without LLVM backend will be added (few megabytes)*

## CLI

There are compatability flags like -O0, -O3, -g, -o  
But mostly CLI arguments are verbose

Compiler emits obj file on JIT and LLVM backends, but by default runs external linker too

```bash
mox help
mox ./hello.mox
mox compile="./hello.mox" backend=llvm
mox ./hello.mox -O3 -o=./hello
mox ./hello.mox linker=false
mox ./hello.mox linker="g++ -o :MOX_LINKER_OUT:"
```

## Some feature highlights

Builtin 3d math with swizzling:

```rust
fn foo(a: [4]f32, b: [4]f32): [3]f32 {
    return (a + b).xyy * 2.0f;
}
```

Compile time code execution:

*Which works for externally linked functions too*

```rust
#linkc fn puts(str: *u8): void;

#run puts("this message will be printed at compilation time");

// this constant will be baked from env variable at compilation time, not at runtime
const SOME_COMPTIME_FLAG := #run os_get_env("PUBLIC_URL");

fn main() {
    build_version := #run os_exec_output("git rev-parse HEAD");
}
```

Code generation and ast manipulation:

```rust
fn make_config($path: []u8): __ast_ptr {
    sb : StringBuilder = 0;
    init(sb.^);
    for (line : split_iterator(fs_read_file_full($path), "\n")) {
        eq := index_of(line, "=");
        format(sb.^, "const {} : i32 = {};\n", .{ slice(line, 0, eq), slice(line, eq + 1, line.len) });
    }
    return __compiler_parse(finalize(sb.^));
}

#run #land_ast make_config("./config.txt"); // const width : i32 = 800; ...
```

Type manipulation:

```rust
fn alloc($T: __type_ptr): *$T {
    const type_size := $T.cast(*MoxType).size;
    return malloc(type_size).cast(*$T);
}

fn foo() {
    x : *i32 = alloc(i32);
}
```

## Planned

* Union macros and struct fields offsets
* Inline assembler
* llvm-jit double backend for fast compile time execution
* Better compile time debugging and function introspection
* Web target
* Hook compiler from user space
