import QtQuick 1.0
import "../../widgets" as Widgets

Item {
    id: optionsMenu

    width: parent.width
    height: parent.height

    Component.onCompleted: {
        sideText.text = wz.tr("Options")
    }

    Widgets.ImageButton {
        defaultSource: "image://imagemap/icon back"
        hoverSource: "image://imagemap/icon back hi"
        x: 5; y: 5
        onClicked: {
            createMenu("menu/main.qml");
        }
    }

    Widgets.MenuTextButton { text: wz.tr("Game Options"); width: parent.width; bwidth: 248; y: 40; onClicked: {createMenu("menu/gameOptions.qml")} }
    Widgets.MenuTextButton { text: wz.tr("Graphics Options"); width: parent.width; bwidth: 248; y: 80; onClicked: {createMenu("menu/graphicsOptions.qml")} }
    Widgets.MenuTextButton { text: wz.tr("Video Options"); width: parent.width; bwidth: 248; y: 120; onClicked: {createMenu("menu/videoOptions.qml")} }
    Widgets.MenuTextButton { text: wz.tr("Audio Options"); width: parent.width; bwidth: 248; y: 160; onClicked: {createMenu("menu/audioOptions.qml")} }
    Widgets.MenuTextButton { text: wz.tr("Mouse Options"); width: parent.width; bwidth: 248; y: 200; onClicked: {createMenu("menu/mouseOptions.qml")} }
    Widgets.MenuTextButton {
        text: wz.tr("Key Mappings");
        width: parent.width; bwidth: 248; y: 240;
        onClicked: {
            window.backScreen = "screens/menuScreen.qml"; window.backMenu = "menu/options.qml";
            createScreen("screens/keymappingsScreen.qml");
        }
    }
}
