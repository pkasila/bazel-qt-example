### Ranobe Lib Offline(но пока только 3 новеллы)

#### bazel run reader

Структура:

### Каталог
Каталог(RanobeList(QListWidget) в QScrollArea), RanobeListItem при изменении экрана меняет размер QIcon. 
DoubleClick по какому-то айтему -> переход в RanobeView.

### RanobeView

Сверху QPushButton, чтобы вернуться в Каталог, слева масштабируемая QIcon, справа QScrollArea с QLabel внутри(масштабируетcя).
Под ним QListWidget. DoubleClick по какому-то айтему -> переход в ChapterView.

### ChapterView

#### Сверху 

QPushButton: вернуться в RanobeView;

QPushButton: прошлая глава, если есть;

QComboBox: выбор главы;

QPushButton: след глава, если есть;

#### Посередине 

QScrollArea с QLabel

#### Снизу 

QPushButton: прошлая глава, если есть;

QPushButton: вызывает QDialog с QLineEdit, где можно записать комментарий к главе(сохраняется при нажатии на "Сохранить").

QPushButton: след глава, если есть / вернуться в RanobeView, если ласт глава;


Ну и RanobeView(от QWidget) посылает сигналы и имеет слоты