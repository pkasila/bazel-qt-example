### Фулл копирование(не стоит) ###

```bash
git clone https://github.com/wrlnduy/QtLabs
cd QtLabs
git checkout raycaster_
cd labs/raycaster
bazel run raycaster --copt=-O3 --copt=-ffast-math
```

#### Запуск рейкастера ####

```bash
bazel run raycaster --copt=-O3 --copt=-ffast-math
```

### Shortcuts ###

#### Ctrl + R - убрать все полигоны

#### Ctrl + Z - убрать последний добавленный/ющийся полигон (Mode: Polygon)

#### Ctrl + Z - убрать последний добавленный источник света (Mode: Static Lights)

#### Ctrl + D - закончить последний строящийся полигон

#### Alt + R - убрать все статические источники света

#### Tab - следующий режим

## Не баг, а фича

1. Static light можно обрамить полигоном
2. Полигон можно обрамить полигоном

<details>
<summary>P.s</summary>
Норм история коммитов в ветке raycaster

```bash
git clone https://github.com/wrlnduy/QtLabs
cd QtLabs
git checkout raycaster
cd labs/raycaster
bazel run raycaster --copt=-O3 --copt=-ffast-math
```
</details>