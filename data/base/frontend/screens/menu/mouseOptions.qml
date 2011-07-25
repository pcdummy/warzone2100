import QtQuick 1.0
import "../../widgets" as Widgets

Item {
    id: mouseOptionsMenu

    width: parent.width
    height: parent.height

    Component.onCompleted: {
        sideText.text = wz.tr("Mouse Options")
    }

    Widgets.ImageButton {
        defaultSource: "image://imagemap/icon back"
        hoverSource: "image://imagemap/icon back hi"
        x: 5; y: 5
        onClicked: {
            createMenu("menu/options.qml");
        }
    }

    // Labels
    Column {
        id: labels
        width: 280
        anchors.top: parent.top
        anchors.topMargin: 56
        anchors.left: parent.left
        anchors.leftMargin: 30

        spacing: 15

        Widgets.LargeText { text: wz.tr("Reverse Rotation") }
        Widgets.LargeText { height: 60;  text: wz.tr("Trap Cursor") }
        Widgets.LargeText { text: wz.tr("Switch Mouse Buttons") }
        Widgets.LargeText { text: wz.tr("Rotate Screen") }
    }

    // Options
    Column {
        width: 165
        anchors.top: parent.top
        anchors.topMargin: 56
        anchors.left: labels.right

        spacing: 15
        Widgets.ClickSelect {
            width: parent.width;
            options: [wz.tr("Off"), wz.tr("On")]
            value: wz.getConfigValue("mouseflip") ? 1 : 0;
            onValueChanged: wz.setConfigValue("mouseflip", (value == 1))
        }
        Widgets.ClickSelect {
            width: parent.width; height: 60;
            options: [wz.tr("Off"), wz.tr("On")]
            value: wz.getConfigValue("trapCursor") ? 1 : 0;
            onValueChanged: wz.setConfigValue("trapCursor", (value == 1))
        }
        Widgets.ClickSelect {
            width: parent.width;
            options: [wz.tr("Off"), wz.tr("On")]
            value: wz.getConfigValue("RightClickOrders") ? 1 : 0;
            onValueChanged: wz.setConfigValue("RightClickOrders", (value == 1))
        }
        Widgets.ClickSelect {
            width: parent.width;
            options: [wz.tr("Right Mouse"), wz.tr("Middle Mouse")]
            value: wz.getConfigValue("MiddleClickRotate") ? 1 : 0;
            onValueChanged: wz.setConfigValue("MiddleClickRotate", (value == 1))
        }
    }
}