Hooks allows to instrument compiler in some places

## MOX_COMPILER_HOOK_COMPILATION_ENDED

This hook is called on passed macro, after all modules are compiled, but before code was emitted.  
It runs passed macro in module's top scope using interpreter which allows to emit additional code or change global variables.

```rust
import "std.mox";

struct TextureResource {
    path: []u8;
    texture_size: vec2i;
    texture_origin: vec2i;
}

resources: [?]TextureResource = .{
    .{
        path = "img1";
        texture_size = .{ 128; 128; };
        texture_origin = 0;
    };
    .{
        path = "img2";
        texture_size = .{ 128; 128; };
        texture_origin = 0;
    };
}; #no_comptime_reset

fn #pack_textures(_: *u8): void {
    {
        // walk over resources
        // pack it to single texture atlas
        // compress this texture atlas
        // remap coordinates
        // update paths
    }
}

// here we register #pack_textures so it will be called after compilation finished
#run __compiler_register_hook(MOX_COMPILER_HOOK_COMPILATION_ENDED, #pack_textures, nullptr(u8));
```