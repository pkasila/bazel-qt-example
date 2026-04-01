import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls
import QtQuick.Controls.Material
import TestPlatform.TestPlatform

Dialog {
    id: errorDialog
    title: "Ошибка"
    visible: false
    anchors.centerIn: parent
    width: parent.width * 0.7

    Material.theme: Material.Dark
    Material.primary: "#3f3c40"
    Material.background: "#1c1b1f"
    Material.accent: Material.Grey

    property string errorText

    function show(error: string) {
        errorText = error
        visible = true
    }

    ColumnLayout {
        anchors.fill: parent
        Label {
            text: errorText
            wrapMode: Text.WordWrap
            Layout.fillWidth: true
        }
        Button {
            Layout.alignment: Qt.AlignRight
            text: "ЛАДНО"
            onClicked: {
                errorDialog.visible = false
            }
        }
    }
}
