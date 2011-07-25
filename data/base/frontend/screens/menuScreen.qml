import QtQuick 1.0
import "../widgets" as Widgets

Item {
    id: menuScreen
    width: 480
    height: 460 // topBox + spacer + mainBox

    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    property variant _subComponent
    property variant _subMenu

    Component.onCompleted: createMenu(window.loadMenu)

    /**
     * Destroys the old Menu and loads the new one.
     *
     * @param string    file            Load the submenu from this file,
     *                                  relative paths must be from THIS file.
     */
    function createMenu(file)
    {
        if (menuScreen._subMenu) {
            menuScreen._subMenu.destroy();
        }

        try {
            menuScreen._subComponent = Qt.createComponent(file);
        }
        catch(e) { wz.log("Failed to load menu: " + file); return; }

        if (menuScreen._subComponent.status == Component.Loading)
        {
            menuScreen._subComponent.statusChanged.connect(_createSubMenu);
        }
        else
        {
            _createSubMenu();
        }
    }

    /**
     * Helper for createMenu, creates the subMenu after the component is loaded.
     */
    function _createSubMenu()
    {
        if (menuScreen._subComponent == null ||
            menuScreen._subComponent.status == Component.Error)
        {
            wz.log(menuScreen._subComponent.errorString());
            return;
        }

        menuScreen._subMenu = menuScreen._subComponent.createObject(menuHolder)
    }

    // Top blue box
    Widgets.BlueBox {
        width: parent.width
        height: 150
    }

    // Top box
    Item {
        id: topBox

        width: parent.width
        height: 150

        // Logo
        Image {
            source: "image://imagemap/FE LOGO:height"
            sourceSize.width: 10
            sourceSize.height: 115
            opacity: 1
            x: (parent.width - width) / 2
            y: (parent.height - height) / 2
        }
    }

    // Sidetext
    Widgets.SideText {
        id: sideText
        text: "Main menu"
        width: 300
        y: 437
        x: -10
    }

    // Main blue box
    Widgets.BlueBox {
        id: mainBox
        width: parent.width
        height: 300
        anchors.top:  topBox.bottom
        anchors.topMargin: 10
    }

    Item {
        id: menuHolder
        width: parent.width
        height: 300
        anchors.top:  topBox.bottom
        anchors.topMargin: 10
    }
}
