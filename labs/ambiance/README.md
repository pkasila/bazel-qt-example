Чтобы собрать, в системе должны быть установлены:

- CMake
- Qt6 Core
- Qt6 Quick
- Qt6 Multimedia + бекенд с поддержкой ogg (например ffmpeg)
- Qt6 SVG

Пример установки Qt:

```sh
# Windows (MSYS2)
pacman -S mingw-w64-x86_64-qt6

# Gentoo
sudo emerge -a qtbase qtdeclarative qtmultimedia qtsvg
```

Сборка и запуск:

```sh
git clone --recurse-submodules https://github.com/mechakotik/qtlearn
cd qtlearn/labs/ambiance
mkdir build && cd build
cmake ..
cmake --build .
./ambiance
```

Должно выглядеть примерно так:

<p align="center">
    <img src="https://github.com/user-attachments/assets/7d72f917-47d1-4e13-8e9c-17749b4a1466" width="40%">
    <img src="https://github.com/user-attachments/assets/501af7db-c0a6-427c-b728-13e2c3ac2733" width="40%">
</p>

Приложение позволяет ставить на фон разные звуки природы / окружающей среды, также можно добавлять свои звуки (+ -> Импорт). В настройках можно указать светлую / тёмную тему и цвет акцента из Material Design, также включить / выключить системную рамку окна и кнопки "Закрыть" и "Свернуть" на верхней панели.

Текущие звуки и настройки сохраняются в файл `config.toml` в удобочитаемом формате. Загрузка ffmpeg у меня занимала по несколько секунд, поэтому она идёт в отдельном потоке, и вместо фриза во время загрузки показывается ProgressBar. Окно можно перетаскивать за верхнюю панель, перетаскивание вверх экрана должно разворачивать окно на полный экран.
