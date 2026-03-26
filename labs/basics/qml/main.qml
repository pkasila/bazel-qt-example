import QtQuick
import QtQuick.Layouts
import QtQuick.Controls
import QtQuick.Controls.Material
import Reviser.Reviser

ApplicationWindow {
    title: "Reviser"
    width: 500
    height: 600
    visible: true

    Reviser { id: reviser }

    ColumnLayout {
        anchors.fill: parent
        anchors.leftMargin: 15
        anchors.topMargin: 15
        anchors.rightMargin: 15
        anchors.bottomMargin: 7
        spacing: 15

        GroupBox {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ScrollView {
                anchors.fill: parent
                ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
                ScrollBar.vertical.policy: ScrollBar.AsNeeded

                ListView {
                    id: listView
                    model: reviser.list
                    delegate: CheckDelegate {
                        id: itemDelegate
                        text: modelData.name
                        highlighted: ListView.isCurrentItem
                        hoverEnabled: true
                        width: ListView.view.width
                        tristate: true
                        checkState: modelData.state
                        nextCheckState: function() {
                            return modelData.state
                        }
                        onPressed: reviser.index = index
                        onDoubleClicked: function() {
                            if(modelData.state === 0) {
                                modelData.state = 2
                            } else if(modelData.state === 2) {
                                modelData.state = 1
                            } else {
                                modelData.state = 2
                            }
                        }
                    }

                    currentIndex: reviser.index
                    focus: true
                    clip: true
                }
            }
        }

        GroupBox {
            title: qsTr("Билет")
            Layout.fillWidth: true
            GridLayout {
                columns: 2
                rowSpacing: 15
                columnSpacing: 30
                Layout.fillWidth: true
                width: parent.width
                Label {
                    text: "Имя"
                }
                TextField {
                    Layout.fillWidth: true
                    text: reviser.curName
                    onEditingFinished: reviser.curName = text
                    hoverEnabled: true
                }
                Label {
                    text: "Статус"
                }
                ComboBox {
                    model: ["Не пройден", "Нужно повторить", "Пройден"]
                    currentIndex: reviser.curState
                    onActivated: reviser.curState = currentIndex
                    Layout.fillWidth: true
                    hoverEnabled: true
                }
            }
        }

        ProgressBar {
            Layout.fillWidth: true
            value: reviser.progress
            height: 40
        }

        RowLayout {
            Layout.fillWidth: true
            Button {
                text: "НАЗАД"
                Layout.fillWidth: true
                Layout.preferredWidth: 60
                onPressed: reviser.previous()
                enabled: reviser.canPrevious
                hoverEnabled: true
            }
            Button {
                text: "УДАЛИТЬ"
                Layout.fillWidth: true
                Layout.preferredWidth: 60
                onPressed: reviser.removeTicket()
                enabled: reviser.canRemove
                hoverEnabled: true
            }
            Button {
                text: "ДОБАВИТЬ"
                Layout.fillWidth: true
                Layout.preferredWidth: 60
                onPressed: reviser.addTicket()
                hoverEnabled: true
            }
            Button {
                text: "ВПЕРЁД"
                Layout.fillWidth: true
                Layout.preferredWidth: 60
                onPressed: reviser.next()
                enabled: reviser.canNext
                hoverEnabled: true
            }
        }
    }
}
