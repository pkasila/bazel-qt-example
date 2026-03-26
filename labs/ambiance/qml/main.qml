import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls
import QtQuick.Controls.Material
import Ambiance.Ambiance

ApplicationWindow {
    id: window
    title: "Ambiance"
    width: 500
    height: 500
    visible: true

    Ambiance { id: ambiance }

    function getAccent() {
        switch(ambiance.accent) {
            case 0:
                return Material.Red
            case 1:
                return Material.Pink
            case 2:
                return Material.Purple
            case 3:
                return Material.DeepPurple
            case 4:
                return Material.Indigo
            case 5:
                return Material.Blue
            case 6:
                return Material.LightBlue
            case 7:
                return Material.Cyan
            case 8:
                return Material.Teal
            case 9:
                return Material.Green
            case 10:
                return Material.LightGreen
            case 11:
                return Material.Lime
            case 12:
                return Material.Yellow
            case 13:
                return Material.Amber
            case 14:
                return Material.Orange
            case 15:
                return Material.DeepOrange
            case 16:
                return Material.Brown
            case 17:
                return Material.Grey
            case 18:
                return Material.BlueGrey
        }
    }

    Material.theme: (ambiance.theme === 0 ? Material.System : (ambiance.theme === 1 ? Material.Light : Material.Dark))
    Material.primary: (Material.theme === Material.Dark ? "#3f3c40" : "#dfdce0")
    Material.accent: getAccent()

    FileDialog {
        id: fileDialog
        title: "Please choose a file"
        nameFilters: ["Звуки (*.wav *.mp3 *.ogg *.flac)", "Все файлы (*)"]
        onAccepted: {
            ambiance.addCustom(selectedFile)
            visible = false
        }
        onRejected: {
            visible = false
        }
    }

    Menu {
        id: menu
        MenuItem {
            text: "Природа"
            hoverEnabled: true
            onTriggered: natureMenu.open()
        }
        MenuItem {
            text: "Окружение"
            hoverEnabled: true
            onTriggered: envMenu.open()
        }
        MenuItem {
            text: "Шум"
            hoverEnabled: true
            onTriggered: noiseMenu.open()
        }
        MenuSeparator {}
        MenuItem {
            text: "Импорт"
            hoverEnabled: true
            onTriggered: fileDialog.open()
        }
    }

    Menu {
        id: natureMenu
        MenuItem { text: "Дождь"; onTriggered: ambiance.add("Дождь", "qrc:/res/sounds/rain.ogg"); hoverEnabled: true }
        MenuItem { text: "Шторм"; onTriggered: ambiance.add("Шторм", "qrc:/res/sounds/storm.ogg"); hoverEnabled: true }
        MenuItem { text: "Ветер"; onTriggered: ambiance.add("Ветер", "qrc:/res/sounds/wind.ogg"); hoverEnabled: true }
        MenuItem { text: "Волны"; onTriggered: ambiance.add("Волны", "qrc:/res/sounds/waves.ogg"); hoverEnabled: true }
        MenuItem { text: "Поток"; onTriggered: ambiance.add("Поток", "qrc:/res/sounds/stream.ogg"); hoverEnabled: true }
        MenuItem { text: "Птицы"; onTriggered: ambiance.add("Птицы", "qrc:/res/sounds/birds.ogg"); hoverEnabled: true }
        MenuItem { text: "Летняя ночь"; onTriggered: ambiance.add("Летняя ночь", "qrc:/res/sounds/summer-night.ogg"); hoverEnabled: true }
    }

    Menu {
        id: envMenu
        MenuItem { text: "Поезд"; onTriggered: ambiance.add("Поезд", "qrc:/res/sounds/train.ogg"); hoverEnabled: true }
        MenuItem { text: "Лодка"; onTriggered: ambiance.add("Лодка", "qrc:/res/sounds/boat.ogg"); hoverEnabled: true }
        MenuItem { text: "Город"; onTriggered: ambiance.add("Город", "qrc:/res/sounds/city.ogg"); hoverEnabled: true }
        MenuItem { text: "Кофейня"; onTriggered: ambiance.add("Кофейня", "qrc:/res/sounds/coffee-shop.ogg"); hoverEnabled: true }
        MenuItem { text: "Костёр"; onTriggered: ambiance.add("Костёр", "qrc:/res/sounds/fireplace.ogg"); hoverEnabled: true }
    }

    Menu {
        id: noiseMenu
        MenuItem { text: "Розовый шум"; onTriggered: ambiance.add("Розовый шум", "qrc:/res/sounds/pink-noise.ogg"); hoverEnabled: true }
        MenuItem { text: "Белый шум"; onTriggered: ambiance.add("Белый шум", "qrc:/res/sounds/white-noise.ogg"); hoverEnabled: true }
    }

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            DragHandler {
                acceptedDevices: PointerDevice.GenericPointer
                grabPermissions: PointerHandler.CanTakeOverFromItems | PointerHandler.CanTakeOverFromHandlersOfDifferentType | PointerHandler.ApprovesTakeOverByAnything
                onActiveChanged: if (active) window.startSystemMove()
            }

            Item {
                Layout.fillWidth: true
                ToolButton {
                    icon.source: "/res/icons/add_box.svg"
                    onClicked: menu.open()
                    visible: stack.depth === 1
                    hoverEnabled: true
                    anchors.verticalCenter: parent.verticalCenter
                }
                ToolButton {
                    icon.source: "/res/icons/back.svg"
                    onClicked: stack.pop()
                    visible: stack.depth === 2
                    hoverEnabled: true
                    anchors.verticalCenter: parent.verticalCenter
                }
                Label {
                    text: (stack.depth === 1 ? "Ambiance" : "Настройки")
                    font.bold: true
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                }
                ToolButton {
                    icon.source: "/res/icons/settings.svg"
                    onClicked: stack.push(settingsView)
                    visible: stack.depth === 1
                    hoverEnabled: true
                    anchors.right: (ambiance.controls ? hideButton.left : parent.right)
                    anchors.verticalCenter: parent.verticalCenter
                }
                ToolButton {
                    id: hideButton
                    icon.source: "/res/icons/hide.svg"
                    onClicked: window.showMinimized()
                    visible: ambiance.controls
                    hoverEnabled: true
                    anchors.right: closeButton.left
                    anchors.verticalCenter: parent.verticalCenter
                }
                ToolButton {
                    id: closeButton
                    icon.source: "/res/icons/close.svg"
                    onClicked: window.close()
                    visible: ambiance.controls
                    hoverEnabled: true
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                }
            }
        }
    }

    StackView {
        id: stack
        anchors.fill: parent
        initialItem: mainView

        pushEnter: Transition {
            PropertyAnimation {
                property: "opacity"
                from: 0
                to: 1
                duration: 100
            }
            PropertyAnimation {
                property: "x"
                from: 100
                to: 0
                duration: 100
            }
        }
        pushExit: Transition {
            PropertyAnimation {
                property: "opacity"
                from: 1
                to: 0
                duration: 100
            }
            PropertyAnimation {
                property: "x"
                from: 0
                to: -100
                duration: 100
            }
        }
        popEnter: Transition {
            PropertyAnimation {
                property: "opacity"
                from: 0
                to: 1
                duration: 100
            }
            PropertyAnimation {
                property: "x"
                from: -100
                to: 0
                duration: 100
            }
        }
        popExit: Transition {
            PropertyAnimation {
                property: "opacity"
                from: 1
                to: 0
                duration: 100
            }
            PropertyAnimation {
                property: "x"
                from: 0
                to: 100
                duration: 100
            }
        }
    }

    Component {
        id: mainView

        ScrollView {
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ScrollBar.vertical.policy: ScrollBar.AsNeeded

            ListView {
                id: listView
                model: ambiance.sounds
                delegate: ItemDelegate {
                    width: ListView.view.width
                    height: 64
                    hoverEnabled: true
                    contentItem: RowLayout {
                        Label {
                            text: modelData.name
                            rightPadding: 100 - contentWidth
                            elide: Text.ElideRight
                        }
                        Slider {
                            Layout.fillWidth: true
                            value: modelData.volume
                            onMoved: modelData.volume = value
                            visible: modelData.loaded
                            hoverEnabled: true
                        }
                        ToolButton {
                            id: deleteButton
                            icon.source: "/res/icons/delete.svg"
                            onClicked: ambiance.remove(index)
                            visible: modelData.loaded
                            hoverEnabled: true
                        }
                        ProgressBar {
                            Layout.fillWidth: true
                            indeterminate: true
                            visible: !modelData.loaded
                        }
                    }
                }
                focus: true
                clip: true
            }
        }
    }

    Component {
        id: settingsView

        ColumnLayout {
            GridLayout {
                Layout.fillWidth: true
                Layout.margins: 20
                columns: 2
                rowSpacing: 15
                columnSpacing: 30

                Label { text: "Тема" }
                ComboBox {
                    model: ["Системная", "Светлая", "Тёмная"]
                    currentIndex: ambiance.theme
                    onActivated: ambiance.theme = currentIndex
                    Layout.fillWidth: true
                    hoverEnabled: true
                }

                Label { text: "Цвет акцента" }
                ComboBox {
                    model: [
                        "Красный",
                        "Розовый",
                        "Фиолетовый",
                        "Тёмно-фиолетовый",
                        "Индиго",
                        "Синий",
                        "Светло-синий",
                        "Голубой",
                        "Бирюзовый",
                        "Зелёный",
                        "Светло-зелёный",
                        "Лаймовый",
                        "Жёлтый",
                        "Янтарный",
                        "Оранжевый",
                        "Тёмно-оранжевый",
                        "Коричневый",
                        "Серый",
                        "Сине-серый"
                    ]
                    Layout.fillWidth: true
                    currentIndex: ambiance.accent
                    onActivated: ambiance.accent = currentIndex
                    hoverEnabled: true
                }
                
                Label { text: "Рамка окна" }
                CheckBox {
                    Layout.alignment: Qt.AlignRight
                    checked: ambiance.windowBorder
                    onCheckedChanged: ambiance.windowBorder = checked
                    hoverEnabled: true
                }

                Label { text: "Элементы управления" }
                CheckBox {
                    Layout.alignment: Qt.AlignRight
                    checked: ambiance.controls
                    onCheckedChanged: ambiance.controls = checked
                    hoverEnabled: true
                }
            }
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
        }
    }
}
