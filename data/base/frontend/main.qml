import QtQuick 1.0
import "widgets" as Widgets
import "functions.js" as Support

Image {
    id: window

    width: 1024; height: 768

    source: "image://imagemap/" + Support.getRandBackground()
    fillMode:  Image.Stretch

    property string backScreen
    property string backMenu
    property string loadMenu

    property variant _subComponent
    property variant _subScreen

    Component.onCompleted: {
        window.loadMenu = "menu/main.qml"
        createScreen("screens/menuScreen.qml")
    }

    function createScreen(file)
    {
        if (window._subScreen) {
            window._subScreen.destroy();
        }

        try {
            window._subComponent = Qt.createComponent(file);
        }
        catch(e) { wz.log("Failed to load screen: " + name); return; }

        if (window._subComponent.status == Component.Loading)
        {
            window._subComponent.statusChanged.connect(_createItem);
        }
        else
        {
            _createItem();
        }
    }

    function _createItem()
    {
        if (window._subComponent == null || window._subComponent.status == Component.Error) {
            wz.log(window._subComponent.errorString());
            return;
        }

        window._subScreen = window._subComponent.createObject(window)
    }
}
