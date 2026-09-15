winget install MSYS2.MSYS2 --silent --accept-package-agreements; & "C:\msys64\msys2_shell.cmd" -defterm -no-start -ucrt64 -c "pacman -S --noconfirm mingw-w64-ucrt-x86_64-clang"

$oldPath = [System.Environment]::GetEnvironmentVariable("Path", "User")
$newPath = "$oldPath;C:\msys64\ucrt64\bin"
[System.Environment]::SetEnvironmentVariable("Path", $newPath, "User")
$env:Path += ";C:\msys64\ucrt64\bin"
