Mox does not have GC or RAII.  
You operate with raw pointers and memory.  
Because of this, you could reuse existing memory for different types.

Standard library relies on allocators and arenas.

## Common patterns

With allocators:

```rust
alloc := get_context_alloc();

ptr : *i64 = mem_alloc(alloc, i64);
defer mem_free(alloc, ptr);

buf_mem : *u8 = mem_alloc(alloc, 512);
defer mem_free(alloc, buf_mem);

buf : []u8 = mem_alloc_slice(alloc, u8, 512);
defer mem_free(alloc, buf);

buf_mem = mem_realloc(alloc, buf_mem, 1024);
buf = mem_realloc_slice(alloc, buf, 1024);

buf_mem_slice := slice(buf_mem, 1024);
buf_mem_slice.ptr == buf_mem;
buf_mem_slice.len == 1024;
```

With arenas:

```rust
other_arena := get_context_temp_arena();

// pick scratch arena that is not in conflict with other_arena
// and pop it back at the end of the scope
arena := #temp_arena(other_arena);

buf : []u8 = arena_push(arena, 1024);
ptr : *i64 = arena_push(arena, i64);
ptr = arena_push_copy(arena, ptr.*);
buf_copy : []u8 = arena_push_slice_copy(arena, buf);

str : []u8 = arena_push_str_copy(arena, "hello world!");
str_null_terminated : []u8 = arena_push_str_copy_c(arena, "hello world!");
```

Utils:

```rust
// pick temp arena, push 1024 and pop back at the end of the scope
buf := #temp_buffer(1024);

// if you already have other arena in scope, pass it to avoid conflicts
buf := #temp_buffer(1024, conflict_arena);

// make temporary null terminated string from passed string slice
c_str := #temp_c_string("hello world");
c_str := #temp_c_string("hello world", conflict_arena);
```

## Pointer arithmetics

Currently there is no pointer arithmetics at all (later maybe it could be enabled for scope).  
But there are byte_offset and ty_offset utils that could be used to do same thing:

```rust
ptr : *i32 = ...;

// ptr + 8 bytes
ptr2 := byte_offset(ptr, 8);

// same with pipe operator
ptr2 = ptr |> byte_offset(10);

// ptr + 2 * 4 bytes
ptr3 := ty_offset(ptr, 2);
ptr3 = ptr |> ty_offset(2);

ptr2 == ptr3;
```

## Current allocator

To get current (thread local) allocator, use `fn get_context_alloc(): *Alloc`.

Default allocator is lazyly initialized automatically.

```rust
fn foo() {
    alloc := get_context_alloc();

    ptr : *i64 = mem_alloc(alloc, i64);
    defer mem_free(alloc, ptr);
}
```

Each container in mox stores its allocator (with which it was initialized):

```rust
struct DynArr($T: __type_ptr) {
    storage: *DynArrStorage($T);
    alloc: *Alloc;
};
```

Because of this you can safely swap allocators for some underlying code:

```rust
fn foo() {
    // get temp arena
    // #temp_arena macros will add defer to current scope
    // than will reset arena to its previous state
    arena := #temp_arena();
    
    // make alloc interface from arena
    alloc := arena_as_allocator(arena);
    
    // swap allocators
    prev_alloc := set_context_alloc(alloc);
    defer set_context_alloc(prev_alloc);

    // here all underlying code will use temp arena as default allocator
    do_smth();
}

fn do_smth() {
    alloc := get_context_alloc();

    darr := mem_alloc(alloc, DynArr(i64));
    defer mem_free(darr);

    dynarr_init(darr, alloc);
    defer dynarr_free(darr);

    push(darr, 123);
}
```

## Arenas

Best way to operate with temporary memory is arenas.  

```rust
#linkc fn gets(buf: *u8): *u8;

fn read_string_from_stdin(arena: *Arena = get_context_temp_arena()): []u8 {
    const max_len := 1024i64;
    
    // here #temp_buffer picks different scratch arena, that is not passed arena
    // so there is no conflict
    // but this tmp_buf will be freed at the end of scope, so we need to copy it to output arena
    tmp_buf := #temp_buffer(max_len, arena);
    gets(tmp_buf.ptr);

    str_len := c_strlen(tmp_buf.ptr, max_len);

    dst_buf := arena_push(arena, str_len);
    mem_copy(dst_buf, tmp_buf);

    // here returned buffer is allocated on passed arena
    return dst_buf;
}
```

## Tricks

Sometimes you need to pass arena as allocator or get original allocator from converted arena:

```rust
alloc := arena_as_allocator(arena);
arena := arena_from_allocator(alloc);
```
