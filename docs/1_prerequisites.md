## Setup

Mox currently supports custom x86_64 backend and LLVM.

Compiler executable should be in same directory with `/modules`.

Mox does not have linker so it outputs .obj artifact which should be linked manually.

By default it assumes clang is available (see below how to install it), but you can use you own linker.

To link manually, disable linker with `linker=false` argument (eg `mox ./example.mox linker=false`) and link manually.

[More about linking](./linker.md)

### Linux

Everything should just work

```bash
sudo apt update
sudo apt install clang
```

If you have `version GLIBCXX_* not found` error, you need to update libstdc++:

```bash
sudo apt-get update
sudo apt-get install --only-upgrade libstdc++6
```

Some vendor prebuilt dependencies need newer `glibc`, safest way on Linux is to upgrade your system  
(or just build those dependencies manually and comment linking part in those vendor link.mox modules)

### Windows

On Windows Microsoft SDK should be installed or compatible compiler setup

- I prefer msys2 environment on Windows and use MinGW Clang.  
    
    - Using script:

        Press Win+R, type `powershell.exe -ExecutionPolicy Bypass` then Enter  
        cd to mox directory, and run `tools/install_windows_msys2.ps1`

    - Or manually:

        ```bash
        winget install MSYS2.MSYS2 --silent --accept-package-agreements; & "C:\msys64\msys2_shell.cmd" -defterm -no-start -ucrt64 -c "pacman -S --noconfirm mingw-w64-ucrt-x86_64-clang"
        ```

        Then add `C:\msys64\ucrt64\bin` to PATH and just use it.

- Second option is to install MSVC Clang (slow and microsoft'ish):

    - Using script:
    
        Press Win+R, type `powershell.exe -ExecutionPolicy Bypass` then Enter  
        cd to mox directory, and run `tools/install_windows_msvc.ps1`

    - Or manually:

        ```bash
        winget install Microsoft.VisualStudio.BuildTools --override "--passive --wait --add Microsoft.VisualStudio.Component.VC.Tools.x86.x64 --add Microsoft.VisualStudio.Component.Windows11SDK.22621 --add Microsoft.VisualStudio.Component.VC.Llvm.Clang"
        ```

        Then open `x64 Native Tools Command Prompt` from start menu and use compiler from it.

    ⚠️ After this you should always open "Developer Command Prompt"  
    or run `C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat`  
    each time you want to compile something

    ⚠️ You should always specify llvm_target_triple="x86_64-pc-windows-msvc" to mox compiler, to be able to link with msvc clang
