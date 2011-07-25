import QtQuick 1.0
import "../../widgets" as Widgets

Item {
    id: mainMenu

    width: parent.width
    height: parent.height

    Component.onCompleted: {
        sideText.text = wz.tr("Main Menu")
    }

    Widgets.MenuTextButton { text: wz.tr("Single Player"); width: parent.width; bwidth: 248; y: 40; onClicked: {createMenu("menu/singlePlayer.qml")} }
    Widgets.MenuTextButton { text: wz.tr("Multi Player"); width: parent.width; bwidth: 248; y: 80; }
    Widgets.MenuTextButton { text: wz.tr("Tutorial"); width: parent.width; bwidth: 248; y: 120; onClicked: {createMenu("menu/tutorials.qml")} }
    Widgets.MenuTextButton { text: wz.tr("Options"); width: parent.width; bwidth: 248; y: 160; onClicked: {createMenu("menu/options.qml")} }
    Widgets.MenuTextButton { text: wz.tr("View Intro"); width: parent.width; bwidth: 248; y: 200; }
    Widgets.MenuTextButton { text: wz.tr("Quit Game"); width: parent.width; bwidth: 248; y: 240; onClicked: {createScreen("quit.qml")} }
}
