import QtQuick 1.0
import "../../widgets" as Widgets

Item {
    id: gameOptionsMenu

    width: parent.width
    height: parent.height

    Component.onCompleted: {
        sideText.text = wz.tr("Game Options")
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

        Widgets.LargeText { text: wz.tr("Difficulty") }
        Widgets.LargeText { text: wz.tr("Scroll Speed") }
        Widgets.LargeText { text: wz.tr("Unit Colour") }
        Widgets.LargeText { text: wz.tr("Language") }
        Widgets.LargeText { text: wz.tr("Radar") }
    }

    // Options
    Column {
        width: 165
        anchors.top: parent.top
        anchors.topMargin: 56
        anchors.left: labels.right

        spacing: 15
        Widgets.ClickSelect {
            width: parent.width
            options: [wz.tr("Easy"), wz.tr("Normal"), wz.tr("Hard"), wz.tr("Tough"), wz.tr("Killer")]
            value: wz.getConfigValue("difficulty")
            onValueChanged: wz.setConfigValue("difficulty", value)
        }
        Widgets.Slider {
            width: parent.width
            maximum: 16
            value: wz.getConfigValue("scroll") / 100
            onValueChanged: wz.setConfigValue("scroll", value * 100)
        }
        Widgets.ImageSelectV2 {
            id: color
            width: parent.width; height: 25
            model: ListModel {
                ListElement {
                    value: "0"
                    defaultSource2: "image://imagemap/icon player0"
                    hoverSource2: "image://imagemap/icon player0 hi"
                    activeSource2: "image://imagemap/icon player0 hi"
                }
                ListElement {
                    value: "4"
                    defaultSource2: "image://imagemap/icon player4"
                    hoverSource2: "image://imagemap/icon player4 hi"
                    activeSource2: "image://imagemap/icon player4 hi"
                }
                ListElement {
                    value: "5"
                    defaultSource2: "image://imagemap/icon player5"
                    hoverSource2: "image://imagemap/icon player5 hi"
                    activeSource2: "image://imagemap/icon player5 hi"
                }
                ListElement {
                    value: "6"
                    defaultSource2: "image://imagemap/icon player6"
                    hoverSource2: "image://imagemap/icon player6 hi"
                    activeSource2: "image://imagemap/icon player6 hi"
                }
                ListElement {
                    value: "7"
                    defaultSource2: "image://imagemap/icon player7"
                    hoverSource2: "image://imagemap/icon player7 hi"
                    activeSource2: "image://imagemap/icon player7 hi"
                }
            }

            value: wz.getConfigValue("colour")
            onValueChanged: wz.setConfigValue("colour", value)
        }
        Widgets.ClickSelect {
            width: parent.width;
            options: [wz.tr("System"), "German", "English"]
            value: {
                var index = options.indexOf(wz.getConfigValue("language"));
                (index == -1 ? 0 : index)
            }
            onValueChanged: wz.setConfigValue("language", (value != 0 ? options[value] : ""))
        }
        Widgets.ClickSelect {
            width: parent.width;
            options: [wz.tr("Fixed"), wz.tr("Rotating")]
            value: wz.getConfigValue("rotateRadar") ? 1 : 0;
            onValueChanged: wz.setConfigValue("rotateRadar", (value == 1))
        }
    }
}
