import QtQuick 1.0
import Warzone 1.0
import "../../widgets" as Widgets

Item {
    id: singlePlayerMenu

    width: parent.width
    height: parent.height

    Component.onCompleted: {
        sideText.text = wz.tr("Single Player")
    }

    Widgets.ImageButton {
        defaultSource: "image://imagemap/icon back"
        hoverSource: "image://imagemap/icon back hi"
        x: 5; y: 5
        onClicked: {
            createMenu("menu/main.qml");
        }
    }

    Widgets.MenuTextButton { text: wz.tr("New Campaign"); width: parent.width; bwidth: 248; y: 40; }
    Widgets.MenuTextButton {
        text: wz.tr("Start Skirmish Game"); width: parent.width; bwidth: 248; y: 80;
        onClicked: {
            wz.config.set("gameType", Wz.Campaign);
            wz.config.set("isHost", true);
            wz.config.set("playerIndex", 0);
            createScreen("screens/hostGameScreen.qml");
        }
    }
    Widgets.MenuTextButton { text: wz.tr("Challenges"); width: parent.width; bwidth: 248; y: 120; }
    Widgets.MenuTextButton { text: wz.tr("Load Game"); width: parent.width; bwidth: 248; y: 160; }
}
