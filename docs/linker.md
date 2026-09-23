## How to link

mox compiler by default outputs .obj. To get executable, external linker should be used

By default compiler runs linker command specified with `linker=""` argument

Default linker command is `linker="clang -o :MOX_LINKER_OUT:"` which gets appended with libraries by compiler

So when you run: 

```bash
mox ./example.mox
```

Compiler sees it as:

```bash
mox ./example.mox linker="clang -o :MOX_LINKER_OUT:"
```

And runs linker command after compilation:

```bash
clang -o ./example.mox.exe ./example.mox.obj
```

You can turn off linker command and run it manually:

```bash
mox ./example.mox linker=false
```

Or pass custom linker (eg g++):

```bash
mox ./example.mox linker="g++ -o :MOX_LINKER_OUT:"
```

mox compiler assumes that linker will link C lib / runtime automatically

## Linking libs from mox

You can append linker command from mox's comptime by calling `__compiler_append_linker("string")` or `__compiler_link_lib("lib", link_at_comptime: bool)`.

### Appending linker command

`fn __compiler_append_linker(str: []u8): void`

Appends passed string to linker command.

You can pass anything there.

For example in mox:

```rust
#run __compiler_append_linker("-passed_from_comptime");
```

```bash
mox ./example.mox linker="g++ -o :MOX_LINKER_OUT:"

# compiler will run this linker command:
g++ -o ./example.mox.exe ./example.mox.obj -passed_from_comptime
```

### Linking library

`fn __compiler_link_lib(lib_path: []u8, link_at_comptime: bool): void`

When `link_at_comptime=false` appends specified librariy to linker command with -l

eg:

```rust
#run __compiler_link_lib("user32");
```

```bash
mox ./example.mox

# compiler will run this linker command:
clang -o ./example.mox.exe ./example.mox.obj -luser32
```

When `link_at_comptime=true`, compiler tries to dynamically load this library to compiler's runtime so it will be available to call from comptime.

Good example of using `__compiler_link_lib` to link at comptime is in [`examples/3_comptime.mox`](../examples/3_comptime.mox)

This behaviour is used to link raylib at comptime or as a library: [`modules/vendor/raylib/link.mox`](../modules/vendor/raylib/link.mox)

## Troubleshoots

### Windows

If you installed clang manually, you also need Windows Kit SDK and VC Tools.  
They are usually shipped with Visual Studio.

If there is some strange error with linker, try first to make linker working manually by disabling linker command for compiler (pass `linker=false` argument).  
Then compiler will just produce .obj file.

If msvc clang can not find system libraries:

- try to run linker from Visual Studio command prompt:

    - run `x64 Native Tools Command Prompt for VS20..`

    - or run `vcvarsall.bat x64` in cmd, somewhere from `C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\`

    - or use "Developer Profile" in Windows Terminal

- try to specify paths manually:

    ```bash
    C:/Libs/clang_x86_64-pc-windows-msvc/bin/clang.exe -o ./examples/2_raylib.mox.exe ./examples/2_raylib.mox.obj -luser32 ./modules/vendor/raylib/win64_mingw/libraylibdll.a "-Wl,/vctoolsdir:C:/Program Files (x86)/Microsoft Visual Studio/2017/Community/VC/Tools/MSVC/14.16.27023/" "-Wl,/winsdkdir:C:/Program Files (x86)/Windows Kits/10/"
    ```

- try to install msys2 clang, read Windows section of [docs/1_prerequisites.md](./1_prerequisites.md)

⚠️ You should always specify llvm_target_triple="x86_64-pc-windows-msvc" to mox compiler, to be able to link with msvc clang
