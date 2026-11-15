## 0.3.0

## Compiler

`#thread_local` global variables now supported:

```rust
global : i32 = 0; #thread_local
```

`iterator_end` now added along with iterator_next and iterator_current, which is called on iteration end (useful in case of break or return)

`#ast` now works in non comptime context, producing ast value (same as `#run #ast` but without unneeded `#run`)

``#land_ast` added inside macros which resolves everything in caller scope

Fixed inf and nan constants in C backend

Circular references for types in corner cases fixed (now works)

Fixed expressions evaluation in poly function param types:

```rust
fn foo($N: i64, x: [$N * 100]$T);
                       ^ $N*100    now works correctly and evaluates only after "is fixed array? is $T satisfied?" checks
```

For this `TypePolyExpr` type added which holds this expression on type level

Fixed macro expansion used as default arg, now it works as it was passed to argument from caller

Fixed macro expansion with control flow in interp mode used in arguments

JIT backend stack usage changed, less runtime instructions and signals fixed

Default expressions inside polymorphic funcs which refers to poly bindings (like $T as a type) works correctly now

LLVM mox mangling fixed and its now zero copy so its much faster

8 bit integer division fixed in JIT backend

Compiler emits warning when user tries to take a pointer to misaligned field

Void expression results fixed in different kind (overload pick, variable declaration, passing to arg)

Proper decl name of types in compiler messages

`#if` condition sees only declarations above it. A name declared below the `#if` in the same block is now an error there, instead of silently using an imported or outer declaration with the same name. A local const above the `#if` is no longer hidden by an imported one either

A module imported inside `#if` that imports the importer back now sees all of the importer's top-level declarations outside `#if` branches, including those below the `#if`

A pointer taken out of compile-time-only data (`text.ptr` of a const declared inside `#run`, or a pointer field of a const made by `#run`) used in runtime code is now a compile error. Before, the program silently got an address in compiler memory. Copy the slice into a runtime variable first (`s := text;` then `s.ptr`)

`__compiler_link_lib(path, true)` on Linux and macOS now loads a library from the current directory by a relative name; before, dlopen only searched the system library paths for a name without "/". The load error lists every file name it tried

The compiler creates the output directories (of `-o` and of `out`) before compiling. Before, `-o=./build/app` failed when `build/` did not exist: comptime code copying into the output dir (`build_copy_to_output`, the vendor library copies) and the linker both failed

A string literal passed to a `[]$T` parameter at comptime now works (`#run starts_with(os_get_env("MODE"), "release")`, `split_iterator(text, "\n")` in a function called through `#run`). Before, it failed with `string literal cannot be used as type "[]$T"`

A panic or failed assert in comptime code is now a compile error at the panic's line (with its message and a note at the `#run` that ran the code), followed by "Compilation failed". Before, the panic's exit ended the compiler process with exit code 3 and no compiler diagnostic

## Modules

Core/std modules moved to modules/std

`Alloc` now is just pointer to function, and *Alloc is passed as self, so it takes less space and could be placed inside Arena itself

Alignmnent fixes in containers

Arena allocator reallocs now works so it could be an allocator for DynArr or other containers (last push reallocs or new push made)

`std/alloc_bump.mox` added - bump allocator with chunks

Allocator interface now accepts `get_arena` mode which returns backing arena allocator, use `get_alloc_backing_arena` helper for it  
It solves bunch of bugs with arena conflicts behind allocators

`std/threads.mox` added to work with threads

`game/animate.mox` added to work with curves and animation functions

`game/atlas.mox` added to pack rectangles by groups (used for packing textures to atlases)

`game/immediate_state.mox` added as immediate state library, for UI

`context.mox` moved to `#thread_local` instead of OS calls

Fixes for corner cases of formatting and panic

`trim_left` fixed

StringBuilder refactored  
`#temp_string_builder` added to easily init StringBuilder with arena conflict solving

BufferBuilder reset fixed

ecs refactored, arch_destroy inside iteration are now queeud until loop end

utf16 module added

win apis now use wide versions for paths and converts to utf16 on call

win shell commands are now escaped properly

os_exec_* apis now dont inherit stdin/stdout by default (but there is an option)

os_exec_ex now uses async pipes on Windows so its much faster (30x speedup) and without limits now

os_exec_* timeouts now handle subprocess group

exit codes of os_exec handled properly

pty now uses async pipes on windows for write

changed console mode restores original on process exit properly

os directory iterator now dont follow symlink dirs on both platforms

term input now properly handles unicode input like emoji

clipboard api fixed on windows

os posix properly handles EINTR

utf8 decoding fixed

build.mox fixes for unix tools

some math functions used from c lib and this fixes some corner cases

math fixes for corner cases

*Arena conflicts replaced with *Alloc so it now no overloads needed and it works for all allocators

process_tree module added: os_process_children (with start times to detect reused pids), os_process_start_time, os_kill_tree (kills a process with all its descendants; Windows and Linux)

pty_close now kills the whole shell process tree, not only the shell

panic now exits through exit(3): it prints "panic happened", runs os_atexit handlers (console restore and others) and thread exit callbacks, then prints the message last. `is_panicking` global added; a panic inside a panic prints both messages and aborts at once

os_exit added

os_stderr_write added; panic output now goes to stderr instead of stdout

err_print and err_println added (like print and println, but to stderr)

panic and assert now print where they happened (`panic at: file:line`, only the line without -g) for every overload, not only the formatted one

A failed assert says `assertion failed` (and its message), instead of `no panic callstack yet here`

fs_path_abs and os_path_abs added: the absolute form of a path, with "." and ".." resolved (text only, the file does not need to exist)

build_copy_to_output overwrites the file in the output dir

## 0.2.0

A lot of things were fixed, tested and stabilized, so minor version is up.

## Compiler

non nullable pointers now could also be initialized and compared with `0`.

bool type also could be initialized with `0` now (same as false).

## Modules

`ty_offset` renamed to `type_offset`

## 0.1.10

## Compiler

`fn __compiler_alignof_type(TYPE): u32`

Comptime `#for` support:

```rust
#for (i : it_range($N)) {
    arr[i] = i;
}

// which is same as:

#run {
    for (i : it_range($N)) {
        #emit {
            arr[i] = i;
        }
    }
}
```

Fix constraints pattern matching for poly functions and with baked values.

Fix runtime continue/break inside #emit, inside comptime for loops, which should affect only runtime outer loop.

Fix enum auto increment and type inferring for variants without specified value.

## Modules

`fn alignof_type(TYPE): u32`

Proper alignment in alloc/arena

`vendor/box3d.mox`  
`vendor/microui.mox`

## 0.1.9

Most major thing is that function pointers and slices now could be trasferred from comptime to runtime:

```rust
struct Command {
    name: []u8;
    run: *fn (x: i32): i32;
};

commands: [16]Command = 0; #no_comptime_reset
commands_len: i32 = 0;     #no_comptime_reset

fn register(name: []u8, run: *fn(x: i32): i32) {
    commands[commands_len].name = name;
    commands[commands_len].run = run;
    commands_len = commands_len + 1;
}

fn double(x: i32): i32 { return x * 2; }
fn square(x: i32): i32 { return x * x; }

#run {
    register("double", double);
    register("square", square);
}

#linkc fn main(): i32 {
    return commands[1].run(commands_len);   // square(2) == 4
}
```

See [examples/5_comptime_command_list.mox](./examples/5_comptime_command_list.mox) for more advanced example

### Compiler

Added untyped literals passed to generic type error

Fixed types declaration resolution though named module import

Interp with failed poly instantiation now prints same diagnostic as compilation path

Added `__compiler_register_hook(hook_id, #callback, custom_data_ptr)` builtin:

`MOX_COMPILER_HOOK_COMPILATION_ENDED` hook expands macro `#callback` as if it was written like `#run #callback(custom_data_ptr)` at the end of the module  
but it runs at the end of whole compilation. Useful to emit additional code or patch #no_comptime_reset globals

Slices of any type and function pointers now survive comptime -> runtime:
in #no_comptime_reset globals (including structs and arrays of structs with slices / function pointers, nested slices),  
in global initializers (`log_fn: *fn(): i32 = default_log;`)  
and in #run results / constants.  

Raw pointers are still rejected, see known_limitations.md

Global variable of slice type are now writable (before they were lowered as readonly)

A function can be used as a global initializer value even when it is declared below the global

Fixed address of a #linkc function in JIT output pointing to an internal trampoline instead of the external symbol

Fixed address of a function taken before its definition missing a relocation in JIT object output

Fixed interpreter not able to call function pointers produced by native comptime code or pointing to #linkc functions

Fixed comptime function handles of different poly instances (and functions nested in poly bodies) being merged into one

Fixed field access on an element of a self-referential struct's slice (`tree.kids[0].name`)

Fixed comparing a nullable function pointer with 0 in runtime code (`if (cb != 0)`)

Named constants (including struct constants) stay comptime values in runtime code: they can be passed straight to `$` params (no more `#run NAME`)

When a name has both `$` and runtime overloads, a comptime argument now picks the `$` overload and a runtime argument picks the runtime one.  
(before, an exact runtime match won first, so `f(CONST)` went to the runtime overload while `f(5)` went to `$`)

Typed struct literal arguments `S.{ ... }` pick the `$` overload when all fields are comptime, and the runtime overload otherwise

Anonymous `.{ ... }` literal can be passed to a concrete struct param of a generic function (generic struct params like `Box($T)` fails)

Field access on a comptime struct value in runtime code (`$s.y`, `CONST.field`) now folds to a comptime value

Fixed a runtime value passed to a `$` param silently reusing an already baked instance, now it is an "Expected comptime value" error

Fixed JIT panic when a comptime argument could not be converted to runtime

Fixed generic function called through a named import (`lib.make_box(x)`) resolving its signature types in the caller's module ("Type with this name was not found")

Fixed loops and switch inside a generic function

Fixed parsing identifiers with op signs

Functions that returns non void without 'return' is control flow error now (before it was not checked in some cases)

Comma separators and semicolons could be mixed in struct literals or arrays. Last separator is optional.

### Modules

A lot of math utils

`arena_push_slice`

`fn c_string_into(arena: *Arena, str: []u8): *u8`

`fn bitcast($T_Dst: __type_ptr, x: $T_Src): $T_Dst`

`fn utf8_decode_iterate(text: []u8): Utf8DecodeIterator`

`c_interp.mox` renamed to `c_interop.mox`

`arena_push_slice` added to alocate new slice on arena

## 0.1.8

### Compiler

Fixed bug with interpreter on nested function declarations

Fixed distinct types for poly function signatures are now deduplicated

Fixed interpreter global allocations to prevent stack overflow

CLI's `out=` argument if not specified, defaults to file with right extention (.c for c backend, .ll/.s/.bc for different llvm formats, and .obj)

Fixed multiple issues in C backend, asteroids now compiles right to C

### Modules

Fixed thread_local is_init flag added. (Before for init status, key!=0 was checked, which was worng).

## 0.1.7

asteroids simple game example added

### Compiler

Fixed bug in interpreter with generic parameters

Fixed dwarf ranges in jit backend

Fixed bug with float mod op in jit backend

Passing function to argument (or assigning it to variable) automatically takes pointer to it (before you should write .^ everytime)

Fixed bug when function declared in another function body was wrongly compiled

### Modules

raylib vectors uses arrays now (which has vector math)

raylib keys and colors added

`*i8` for c strings replaced to mox's `*u8` in vendors

`mod`, `abs` overloads in math.mox

`mod_euclid` added

`vmin`, `vmax`, `vclamp` renamed to just min, max, clamp and now correctly works as vector overloads for scalars

`is_point_in_rect`, `wrap_in_rect`, `angle_to_direction`, `angle_degree_to_direction` added to math.mox

`mem_copy` now automatically handles memory overlap case (memmove vs memcpy)

`remove_if` and `remove_at` added to dynarr

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
