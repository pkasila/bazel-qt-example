# Лабораторная работа №1

## Задание 1: Прокрастинация

### Виджеты
| Виджет | Тип |
|--------|-----|
| count | QSpinBox |
| view | QListWidget |
| question-view | QGroupBox |
| number | QLabel |
| name | QLabel |
| name-edit | QLineEdit |
| status | QComboBox |
| next-question | QPushButton |
| previous-question | QPushButton |
| total-progress | QProgressBar |
| green-progress | QProgressBar |

### Как использовать
1. `bazel run //labs/basics:ticket_reviewer`
2. Установите количество билетов
3. Кликните на билет для просмотра
4. Двойной клик меняет статус
5. Кнопки навигации выбирают билеты

### Цвета статусов
- Серый - не начат
- Желтый - повторить
- Зеленый - выучен

---

## Задание 2: Color Mixer

### Виджеты
- QSlider (4) - R, G, B, Alpha
- QLabel - цвет и подписи
- QLineEdit - название
- QPushButton (3) - копировать, вставить, случайно
- QLCDNumber (4) - значения каналов
- QProgressBar - непрозрачность

### Сигналы и слоты
| Виджет | Сигнал | Слот |
|--------|--------|------|
| QSlider | valueChanged(int) | on*SliderChanged() |
| QPushButton | clicked() | onCopy/Paste/RandomColor() |
| QLineEdit | textChanged(QString) | onColorNameChanged() |

### Как использовать
1. `bazel run //labs/basics:color_mixer`
2. Ползунки настраивают цвет
3. Кнопки работают с буфером обмена

### Layout
- QVBoxLayout - основная
- QGridLayout - слайдеры
- QHBoxLayout - кнопки
