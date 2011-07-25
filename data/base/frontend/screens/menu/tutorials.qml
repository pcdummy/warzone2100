import QtQuick 1.0
import "../../widgets" as Widgets

Item {
    id: tutorialMenu

    width: parent.width
    height: parent.height

    Component.onCompleted: {
        sideText.text = wz.tr("Tutorials")
    }

    Widgets.ImageButton {
        defaultSource: "image://imagemap/icon back"
        hoverSource: "image://imagemap/icon back hi"
        x: 5; y: 5
        onClicked: {
            createMenu("menu/main.qml");
        }
    }

    Widgets.MenuTextButton { text: wz.tr("Tutorial"); width: parent.width; bwidth: 248; y: 40; }
    Widgets.MenuTextButton { text: wz.tr("Fast Play"); width: parent.width; bwidth: 248; y: 80; }
}
