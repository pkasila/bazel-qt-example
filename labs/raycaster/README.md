# Raycaster

Qt-приложение для визуализации алгоритма трассировки лучей (лабораторная работа №2).

## Запуск

```bash
bazel run //labs/raycaster:basics
```
Системные зависимости
```bash
sudo apt install -y libxcb-cursor0 libxkbcommon-x11-0 libfontconfig1 libfreetype6 libgl1 libegl1 libdbus-1-3
```
Qt может не найти графические плагины в кэше Bazel. Выполните эту команду перед запуском:
```bash
export QT_QPA_PLATFORM=xcb
export QT_QPA_PLATFORM_PLUGIN_PATH=$(bazel info output_base)/external/rules_qt++fetch+qt_linux_x86_64/plugins/platforms
bazel run //labs/raycaster:basics
```