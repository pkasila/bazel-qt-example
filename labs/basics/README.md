Чтобы собрать, в системе должны быть установлены:

- CMake
- Qt6 Core
- Qt6 Quick

Пример установки Qt:

```sh
# Windows (MSYS2)
pacman -S mingw-w64-x86_64-qt6

# Gentoo
sudo emerge -a qtbase qtdeclarative
```

Сборка и запуск:

```sh
git clone --recurse-submodules https://github.com/mechakotik/qtlearn
cd qtlearn/labs/basics
mkdir build && cd build
cmake ..
cmake --build .
./reviser
```

Должно выглядеть примерно так:

<p align="center">
    <img src="https://github.com/user-attachments/assets/05658d39-c2b3-470d-b220-57df58b01141" width="50%">
</p>
