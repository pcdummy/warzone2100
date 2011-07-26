import QtQuick 1.0
import "../../widgets" as Widgets

Item {
    id: graphicsOptionsMenu

    width: parent.width
    height: parent.height

    Component.onCompleted: {
        sideText.text = wz.tr("Graphics Options")
    }

    Widgets.ImageButton {
        defaultSource: "image://imagemap/icon back"
        hoverSource: "image://imagemap/icon back hi"
        x: 5; y: 5
        onClicked: {
            createMenu("menu/options.qml");
        }
    }

    Text {
        anchors.top: parent.top
        anchors.topMargin: 15
        anchors.left: parent.left
        anchors.leftMargin: 50

        text: wz.tr("* Takes effect on game restart")
        color: "white"
        smooth: true
        font.family: "DejaVu Sans"
        font.pointSize: 9
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

        Widgets.LargeText { text: wz.tr("Graphics Mode*") }
        Widgets.LargeText { text: wz.tr("Resolution*") }
        Widgets.LargeText { text: wz.tr("Texture Size") }
        Widgets.LargeText { text: wz.tr("Vertical Sync") }
        Widgets.LargeText { text: wz.tr("FSAA*") }
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
            options: [wz.tr("Windowed"), wz.tr("Fullscreen")];
            value: wz.config.get("fullscreen") ? 1 : 0;
            onValueChanged: wz.config.set("fullscreen", (value == 1))
        }
        Widgets.ClickSelect {
            width: parent.width;
            options: wz.getAvailableResolutions()
            value: options.indexOf(wz.getCurrentResolution())
            onValueChanged: wz.setResolution(options[value])
        }
        Widgets.ClickSelect {
            width: parent.width;
            options: [128, 256, 512, 1024, 2048]
            value: options.indexOf(wz.config.get("textureSize"))
            onValueChanged: wz.config.set("textureSize", options[value])
        }
        Widgets.ClickSelect {
            width: parent.width;
            options: [wz.tr("Off"), wz.tr("On")]
            value: wz.config.get("vsync") ? 1 : 0;
            onValueChanged: wz.config.set("vsync", (value == 1))
        }
        Widgets.ClickSelect {
            width: parent.width;
            options: [wz.tr("Off"), "2X", "4X", "8X"]
            value: wz.config.get("FSAA")
            onValueChanged: wz.config.set("FSAA", value)
        }
    }
}
