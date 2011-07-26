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
            value: wz.config.get("difficulty")
            onValueChanged: wz.config.set("difficulty", value)
        }
        Widgets.Slider {
            width: parent.width
            maximum: 16
            value: wz.config.get("scroll") / 100
            onValueChanged: wz.config.set("scroll", value * 100)
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

            value: wz.config.get("colour")
            onValueChanged: wz.config.set("colour", value)
        }
        Widgets.LargeText {
            id: languageButton
            width: parent.width

            text: wz.getLanguage()

            MouseArea {
                anchors.fill: parent
                hoverEnabled: true
                onEntered: parent.color = "#ffffff"
                onExited: parent.color = "#a0a0ff"
                onClicked: {
                    languageButton.text = wz.setNextLanguage()
                }
            }
        }
        Widgets.ClickSelect {
            width: parent.width;
            options: [wz.tr("Fixed"), wz.tr("Rotating")]
            value: wz.config.get("rotateRadar") ? 1 : 0;
            onValueChanged: wz.config.set("rotateRadar", (value == 1))
        }
    }
}
