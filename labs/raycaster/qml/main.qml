import QtQuick
import QtQuick.Layouts
import QtQuick.Dialogs
import QtQuick.Controls
import QtQuick.Controls.Material
import Raycaster.Raycaster

ApplicationWindow {
    id: window
    title: "Quick Raycaster"
    width: 1200
    height: 700
    visible: true

    Material.theme: Material.Dark
    Material.primary: "#3f3c40"
    Material.background: "#1c1b1f"
    Material.accent: Material.Grey

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
                    id: dragButton
                    hoverEnabled: true
                    icon.source: "/res/icons/drag.svg"
                    opacity: (raycaster.mode === 0 ? 1 : 0.5)
                    onClicked: raycaster.mode = 0
                    anchors.left: parent.left
                    anchors.verticalCenter: parent.verticalCenter
                }
                ToolButton {
                    id: polylineButton
                    hoverEnabled: true
                    icon.source: "/res/icons/polyline.svg"
                    opacity: (raycaster.mode === 1 ? 1 : 0.5)
                    onClicked: raycaster.mode = 1
                    anchors.left: dragButton.right
                    anchors.verticalCenter: parent.verticalCenter
                }
                ToolButton {
                    id: lightButton
                    hoverEnabled: true
                    icon.source: "/res/icons/light.svg"
                    opacity: (raycaster.mode === 2 ? 1 : 0.5)
                    onClicked: raycaster.mode = 2
                    anchors.left: polylineButton.right
                    anchors.verticalCenter: parent.verticalCenter
                }
                ToolButton {
                    id: clearButton
                    icon.source: "/res/icons/delete.svg"
                    hoverEnabled: true
                    onClicked: raycaster.clear()
                    anchors.left: lightButton.right
                    anchors.verticalCenter: parent.verticalCenter
                }
                Label {
                    text: "Quick Raycaster"
                    font.bold: true
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                    anchors.horizontalCenter: parent.horizontalCenter
                    anchors.verticalCenter: parent.verticalCenter
                }
                Label {
                    text: raycaster.fpsText
                    font.bold: true
                    horizontalAlignment: Qt.AlignHCenter
                    verticalAlignment: Qt.AlignVCenter
                    anchors.right: hideButton.left
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.rightMargin: 20
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

    Raycaster {
        id: raycaster
        anchors.fill: parent
        MouseArea {
            anchors.fill: parent
            enabled: raycaster.mode == 0
            hoverEnabled: true
            onPositionChanged: {
                if(pressed) {
                    raycaster.shift({x: mouseX, y: mouseY})
                } else {
                    raycaster.resetShift()
                }
            }
            onWheel: (wheel)=> raycaster.rescale(-wheel.angleDelta.y / 1440, {x: mouseX, y: mouseY})
        }
        MouseArea {
            anchors.fill: parent
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            enabled: raycaster.mode == 1
            hoverEnabled: true
            onClicked: (mouse)=> {
                if(mouse.button === Qt.LeftButton) {
                    raycaster.newVertex({x: mouseX, y: mouseY})
                } else {
                    raycaster.finishPolygon()
                }
            }
            onPositionChanged: raycaster.setLastVertex({x: mouseX, y: mouseY})
        }
        MouseArea {
            anchors.fill: parent
            enabled: raycaster.mode === 2
            hoverEnabled: true
            onClicked: raycaster.addStaticLight()
            onPositionChanged: raycaster.lightPosition = {x: mouseX, y: mouseY}
        }
    }
}
