import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls
import QtQuick.Controls.Material
import QtMultimedia
import TestPlatform.TestPlatform

ApplicationWindow {
    id: window
    title: "Test Platform"
    width: 500
    height: 700
    visible: true

    Material.theme: Material.Dark
    Material.primary: "#3f3c40"
    Material.background: "#1c1b1f"
    Material.accent: Material.Purple

    TestPlatform {
        id: testpl
    }
    Component.onCompleted: {
        // requires error dialog to be initialized
        testpl.loadTestsList()
    }

    ErrorDialog {
        id: errorDialog
    }
    property var errorDialogExport: errorDialog

    SoundEffect {
        id: correctSound
        source: "/res/sounds/correct.wav"
    }
    SoundEffect {
        id: wrongSound
        source: "/res/sounds/wrong.wav"
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
                Label {
                    text: "Test Platform"
                    font.bold: true
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                }
                ToolButton {
                    id: hideButton
                    icon.source: "/res/icons/hide.svg"
                    onClicked: window.showMinimized()
                    hoverEnabled: true
                    anchors.right: closeButton.left
                    anchors.verticalCenter: parent.verticalCenter
                }
                ToolButton {
                    id: closeButton
                    icon.source: "/res/icons/close.svg"
                    onClicked: window.close()
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
        initialItem: menuView
    }

    Component {
        id: menuView

        ScrollView {
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff
            ScrollBar.vertical.policy: ScrollBar.AsNeeded

            ListView {
                id: listView
                model: testpl.tests
                delegate: ItemDelegate {
                    width: ListView.view.width
                    hoverEnabled: true
                    onClicked: {
                        testpl.tests[listView.currentIndex].questions[0].checked = false
                        stack.push(testView, { test: listView.currentIndex })
                    }
                    contentItem: Label {
                        text: modelData.name
                        rightPadding: 100 - contentWidth
                        elide: Text.ElideRight
                    }
                }
                focus: true
                clip: true
            }
        }
    }

    Component {
        id: testView

        Item {
            property var test
            property var question: 0
            property var score: 0

            function checkQuestion() {
                if(testpl.tests[test].questions[question].checkboxes.length !== 0) {
                    var cnt = questionStack.currentItem.countWrongCheckboxes()
                    if(cnt < testpl.tests[test].questions[question].scores.length) {
                        correctSound.play()
                        score += testpl.tests[test].questions[question].scores[cnt]
                    } else {
                        wrongSound.play()
                    }
                } else if(testpl.tests[test].questions[question].variants.length !== 0) {
                    if(questionStack.currentItem.getSelectedVariant() === -1) {
                        wrongSound.play()
                    } else if(testpl.tests[test].questions[question].variants[questionStack.currentItem.getSelectedVariant()].score !== 0) {
                        correctSound.play()
                        score += testpl.tests[test].questions[question].variants[questionStack.currentItem.getSelectedVariant()].score
                    } else {
                        wrongSound.play()
                    }
                } else if(testpl.tests[test].questions[question].checkerPath !== "") {
                    var add = testpl.tests[test].questions[question].answerScore(questionStack.currentItem.getTextInput())
                    if(add !== 0) {
                        correctSound.play()
                        questionStack.currentItem.textInputCorrect = true
                        score += add
                    } else {
                        wrongSound.play()
                        questionStack.currentItem.textInputCorrect = false
                    }
                }
                else {
                    if(questionStack.currentItem.getTextInput() === testpl.tests[test].questions[question].correct) {
                        correctSound.play()
                        questionStack.currentItem.textInputCorrect = true
                        score += testpl.tests[test].questions[question].score
                    } else {
                        wrongSound.play()
                        questionStack.currentItem.textInputCorrect = false
                    }
                }
                testpl.tests[test].questions[question].checked = true
                checkButton.enabled = true
                questionStack.currentItem.stopTimer()
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20

                RowLayout {
                    Layout.fillWidth: true
                    Label {
                        text: "Баллы: " + score
                    }
                    Item {
                        Layout.fillWidth: true
                    }
                    Label {
                        id: timeLabel
                        Layout.alignment: Qt.AlignRight
                    }
                }

                ProgressBar {
                    Layout.fillWidth: true
                    value: question / testpl.tests[test].questions.length
                }

                StackView {
                    id: questionStack
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    initialItem: questionView
                }

                Component {
                    id: questionView

                    Item {
                        property int viewQuestion: 0
                        property int secondsLeft: 0
                        property bool textInputCorrect: false

                        function getSelectedVariant() {
                            for(var i = 0; i < variants.count; i++) {
                                var radioButton = variants.itemAt(i);
                                if (radioButton.checked) {
                                    return i;
                                }
                            }
                            return -1;
                        }

                        function countWrongCheckboxes() {
                            var cnt = 0
                            for(var i = 0; i < testpl.tests[test].questions[viewQuestion].checkboxes.length; i++) {
                                if(checkboxes.itemAt(i).checked !== testpl.tests[test].questions[viewQuestion].checkboxes[i].need) {
                                    cnt++
                                }
                            }
                            return cnt
                        }

                        function getTextInput() {
                            return input.text
                        }

                        function updateInputValid() {
                            if(testpl.tests[test].questions[viewQuestion].textOnly) {
                                checkButton.enabled = true
                            } else if(testpl.tests[test].questions[viewQuestion].variants.length !== 0) {
                                checkButton.enabled = (getSelectedVariant() !== -1)
                            } else if(testpl.tests[test].questions[viewQuestion].checkboxes.length !== 0) {
                                checkButton.enabled = true
                            } else if(testpl.tests[test].questions[viewQuestion].checkerPath !== "") {
                                checkButton.enabled = testpl.tests[test].questions[viewQuestion].answerValid(input.text)
                            } else {
                                checkButton.enabled = (input.text.length !== 0)
                            }
                        }

                        function updateTimeText() {
                            if(testpl.tests[test].questions[viewQuestion].timeLimit !== 0) {
                                timeLabel.text = "Осталось: " + secondsLeft + " с."
                            } else {
                                timeLabel.text = ""
                            }
                        }

                        function stopTimer() {
                            timer.stop()
                        }

                        Component.onCompleted: {
                            updateInputValid()
                            if(testpl.tests[test].questions[viewQuestion].timeLimit !== 0) {
                                secondsLeft = testpl.tests[test].questions[viewQuestion].timeLimit
                                timer.start()
                            }
                            updateTimeText()
                        }

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 20

                            Image {
                                Layout.bottomMargin: 10
                                source: testpl.tests[test].questions[viewQuestion].image
                                Layout.maximumWidth: 400
                                Layout.maximumHeight: 300
                                Layout.alignment: Qt.AlignCenter
                                fillMode: Image.PreserveAspectFit
                                mipmap: true
                                onStatusChanged: {
                                    if (status === Image.Error) errorDialog.show("Ошибка загрузки изображения " + source)
                                }
                            }

                            Label {
                                Layout.bottomMargin: 10
                                Layout.alignment: Qt.AlignLeft
                                Layout.fillWidth: true
                                wrapMode: Text.WordWrap
                                text: testpl.tests[test].questions[viewQuestion].text
                                enabled: text !== ""
                            }

                            Repeater {
                                Layout.bottomMargin: 10
                                model: testpl.tests[test].questions[viewQuestion].variants
                                id: variants

                                RadioButton {
                                    Layout.fillWidth: true
                                    text: modelData.text
                                    Material.accent: {
                                        if(!testpl.tests[test].questions[viewQuestion].checked) {
                                            return Material.Purple
                                        }
                                        if(modelData.correct) {
                                            return Material.Green
                                        }
                                        if(modelData.score !== 0) {
                                            return Material.Yellow
                                        }
                                        return Material.Red
                                    }
                                    Material.foreground: {
                                        if(!testpl.tests[test].questions[viewQuestion].checked) {
                                            return "#ffffff"
                                        }
                                        if(modelData.correct) {
                                            return "#a5d6a7"
                                        }
                                        if(modelData.score !== 0) {
                                            return "#fff59d"
                                        }
                                        if(!checked) {
                                            return "#ffffff"
                                        }
                                        return "#ef9a9a";
                                    }
                                    // hack to forbid changing answer after it is checked
                                    MouseArea {
                                        anchors.fill: parent
                                        onPressed: (mouse) => { mouse.accepted = testpl.tests[test].questions[viewQuestion].checked; }
                                    }
                                    onClicked: updateInputValid()
                                    hoverEnabled: !testpl.tests[test].questions[viewQuestion].checked
                                }
                            }

                            Repeater {
                                Layout.bottomMargin: 10
                                model: testpl.tests[test].questions[viewQuestion].checkboxes
                                id: checkboxes

                                CheckBox {
                                    Layout.fillWidth: true
                                    text: modelData.text
                                    Material.accent: {
                                        if(!testpl.tests[test].questions[viewQuestion].checked) {
                                            return Material.Purple
                                        }
                                        if(modelData.need) {
                                            return Material.Green
                                        }
                                        return Material.Red
                                    }
                                    Material.foreground: {
                                        if(!testpl.tests[test].questions[viewQuestion].checked) {
                                            return "#ffffff"
                                        }
                                        if(modelData.need) {
                                            return "#a5d6a7"
                                        }
                                        return "#ef9a9a";
                                    }
                                    // hack to forbid changing answer after it is checked
                                    MouseArea {
                                        anchors.fill: parent
                                        onPressed: (mouse) => { mouse.accepted = testpl.tests[test].questions[viewQuestion].checked; }
                                    }
                                    onClicked: checkButton.enabled = true
                                    onCheckedChanged: updateInputValid()
                                    hoverEnabled: !testpl.tests[test].questions[viewQuestion].checked
                                }
                            }

                            TextField {
                                id: input
                                Layout.bottomMargin: 10
                                Layout.fillWidth: true
                                visible: testpl.tests[test].questions[viewQuestion].correct !== "" || testpl.tests[test].questions[viewQuestion].checkerPath !== ""
                                Material.foreground: {
                                    if(!testpl.tests[test].questions[viewQuestion].checked) {
                                        return "#ffffff"
                                    }
                                    if(textInputCorrect) {
                                        return "#a5d6a7"
                                    }
                                    return "#ef9a9a";
                                }
                                // hack to forbid changing answer after it is checked
                                MouseArea {
                                    anchors.fill: parent
                                    onPressed: (mouse) => { mouse.accepted = testpl.tests[test].questions[viewQuestion].checked; }
                                    cursorShape: Qt.IBeam
                                }
                                onTextChanged: updateInputValid()
                                hoverEnabled: !testpl.tests[test].questions[viewQuestion].checked
                            }

                            Timer {
                                id: timer
                                repeat: true
                                onTriggered: {
                                    secondsLeft--;
                                    if(secondsLeft === 0) {
                                        secondsLeft = 0
                                        timer.stop()
                                        if(!testpl.tests[test].questions[question].checked && !testpl.tests[test].questions[question].textOnly) {
                                            checkQuestion()
                                        }
                                    }
                                    updateTimeText()
                                }
                            }

                            Item {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                            }
                        }
                    }
                }

                Button {
                    id: checkButton
                    Layout.alignment: Qt.AlignRight
                    Layout.preferredWidth: 128
                    text: {
                        if(testpl.tests[test].questions[question].checked || testpl.tests[test].questions[question].textOnly) {
                            return (question + 1 === testpl.tests[test].questions.length ? "ЗАВЕРШИТЬ" : "ДАЛЕЕ")
                        }
                        return "ПРОВЕРИТЬ"
                    }
                    enabled: false
                    onClicked: {
                        if(!testpl.tests[test].questions[question].checked && !testpl.tests[test].questions[question].textOnly) {
                            checkQuestion()
                        } else {
                            if(question + 1 === testpl.tests[test].questions.length) {
                                stack.pop()
                            } else {
                                testpl.tests[test].questions[question + 1].checked = false
                                question += 1
                                questionStack.replace(questionView, {viewQuestion: question})
                            }
                        }
                    }
                }
            }
        }
    }
}
