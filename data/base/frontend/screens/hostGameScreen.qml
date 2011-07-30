import QtQuick 1.0
import Warzone 1.0
import "../widgets" as Widgets
import "../functions.js" as Support

Item {
    id: hostGameScreen
    width: 580
    height: 450

    // Position in center
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    property string     map
    property int        maxPlayers
    property int        techlevel

    // Contains all the players
    ListModel {
        id: playersModel
    }

    // Internal: to allow switching in the players screen.
    property bool       _alliance

    // Internal: Indicates that we are in the player screen.
    property bool       _isHosting  :       false

    property variant    _subComponent
    property variant    _subScreen

    Component.onCompleted: {
        _alliance = wz.config.get("alliance") == 1 ? true : false

        map = wz.config.get("mapName")
        techlevel = wz.config.get("techlevel")

        maxPlayers = wz.setMap(techlevel, map)

        if (maxPlayers == 0)
        {
            // TODO: Unknown map do something here.
            map = "Rush"
            techlevel = Wz.Techlevel_1
            maxPlayers = wz.setMap(techlevel, map)
        }
    }

    function createMenu(file)
    {
        if (hostGameScreen._subScreen) {
            hostGameScreen._subScreen.destroy();
        }

        try {
            hostGameScreen._subComponent = Qt.createComponent(file);
        }
        catch(e) { wz.log("Failed to create hostgame subwidget: " + name); return; }

        if (hostGameScreen._subComponent.status == Component.Loading)
        {
            hostGameScreen._subComponent.statusChanged.connect(_createSubMenu);
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
        if (hostGameScreen._subComponent == null ||
            hostGameScreen._subComponent.status == Component.Error)
        {
            wz.log(hostGameScreen._subComponent.errorString());
            return;
        }

        hostGameScreen._subScreen = hostGameScreen._subComponent.createObject(rightBox)
    }

    // Left sideText
    Widgets.SideText {
        id: leftSideText
        text: wz.tr("Options")
        width: 329
        y: leftBox.height - 31 // 31 = text Height
        x: 2
    }

    // Left blue box
    Widgets.BlueBox {
        id: leftBlueBox
        width: leftBox.width
        height: leftBox.height
    }
    Item {
        id: leftBox
        width: 280
        height: 331

        Widgets.ImageButton {
            id: backButton
            defaultSource: "image://imagemap/icon back"
            hoverSource: "image://imagemap/icon back hi"
            x: 6; y: 5
            onClicked: {
                if (!hostGameScreen._isHosting) {
                    hostGameScreen.destroy();
                    if (wz.config.get("gameType") == Wz.Multiplayer)
                    {
                        createScreen("screens/multiplayer.qml")                        
                    }
                    else
                    {
                        window.loadMenu = "menu/singlePlayer.qml"
                        createScreen("screens/menuScreen.qml")
                    }
                } else {
                    hostGameScreen._subScreen.destroy();

                    if (wz.config.get("gameType") == Wz.Multiplayer) {
                        passwordButton.state = ""
                        passwordInput.state = ""
                        hostnameInput.state = ""
                    }

                    mapButton.state = ""
                    hostGameButton.state = ""
                    hostGameButton.opacity = 1
                    hostGameScreen._isHosting = false;
                    backButton.state = ""
                    chatBox.clear()
                }
            }
        }

        Widgets.ImageButton {
            id: limitsButton
            defaultSource: "image://imagemap/icon multiopts"
            hoverSource: "image://imagemap/icon multiopts hi"
            x: 6; y: 43
            onClicked: {
                hostGameScreen.destroy();
                window.loadMenu = window.backMenu;
                createScreen(window.backScreen);
            }
        }

        Widgets.ImageButton {
            id: hostGameButton
            defaultSource: "image://imagemap/icon start game"
            hoverSource: "image://imagemap/icon start game hi"
            x: 6; y: 73
            onClicked: {
                mapButton.state = "off"
                passwordButton.state = "off"
                hostGameButton.state = "off"
                hostnameInput.state = "off"
                passwordInput.state = "off"
                hostGameButton.opacity = 0
                hostGameScreen._isHosting = true
                createMenu("hostGame/players.qml")
                chatBox.clear()
                if (wz.config.get("gameType") == Wz.Multiplayer) {
                    chatBox.addSystemMessage("You'r game is not listed. This is a dummy, haha!")
                }
            }
        }

         Column {
            spacing: 4
            width: 226
            x: 50
            y: 2
            Widgets.SingleLineEdit {
                id: playername
                text: wz.config.get("playerName")
                maximumLength: 14

                onAccepted: {
                    if (wz.config.get("playerName") != text) {
                        if (wz.config.get("gameType") == Wz.Multiplayer) {
                            chatBox.addLine(wz.config.get("playerName") + " -> " + text);
                        }

                        wz.config.set("playerName", text)

                        if (playersModel.count) {
                            playersModel.setProperty(wz.config.get("playerIndex"), "name", text)
                        }
                    }
                }

                Widgets.ImageButton {
                    width: 25
                    y: 3
                    anchors.right: parent.right
                    defaultSource: "image://imagemap/icon flag"
                    hoverSource: "image://imagemap/icon flag hi"
                    activeSource: "image://imagemap/icon flag hi"
                    defaultSourceWidth: 22
                    defaultSourceHeight: 22
                    hoverSourceWidth: 22
                    hoverSourceHeight: 22
                    activeSourceWidth: 22
                    activeSourceHeight: 22
                }
            }
            Widgets.SingleLineEdit {
                id: hostnameInput

                text: (wz.config.get("gameType") == Wz.Multiplayer ? wz.config.get("gameName") : wz.tr("One-Player Skirmish"))

                state: (wz.config.get("gameType") == Wz.Multiplayer ? "" : "off")
            }
            Widgets.SingleLineEdit {
                id: mapInput
                text: hostGameScreen.map + "-T" + hostGameScreen.techlevel
                readOnly: true

                Widgets.ImageButton {
                    id: mapButton
                    width: 25
                    y: 3
                    anchors.right: parent.right
                    defaultSource: "image://imagemap/icon select map"
                    hoverSource: "image://imagemap/icon select map hi"
                    activeSource: "image://imagemap/icon select map hi"
                    defaultSourceWidth: 22
                    defaultSourceHeight: 22
                    hoverSourceWidth: 22
                    hoverSourceHeight: 22
                    activeSourceWidth: 22
                    activeSourceHeight: 22

                    onClicked: {
                        mapButton.state = "activeOff";
                        createMenu("hostGame/mapSelect.qml");
                    }
                }
            }
            Widgets.SingleLineEdit {
                id: passwordInput
                text: wz.tr("Enter password here")

                state: (wz.config.get("gameType") == Wz.Multiplayer ? "" : "off")

                Widgets.ImageButton {
                    id: passwordButton
                    width: 25
                    y: 3
                    anchors.right: parent.right
                    defaultSource: "image://imagemap/icon lock off"
                    hoverSource: "image://imagemap/icon lock on"
                    activeSource: "image://imagemap/icon lock on"
                    defaultSourceWidth: 22
                    defaultSourceHeight: 22
                    hoverSourceWidth: 22
                    hoverSourceHeight: 22
                    activeSourceWidth: 22
                    activeSourceHeight: 22

                    state: (wz.config.get("gameType") == Wz.Multiplayer ? "" : "off")

                    onClicked: {
                        defaultSource: "image://imagemap/icon lock on"
                        hoverSource: "image://imagemap/icon lock of"
                        activeSource: "image://imagemap/icon lock off"
                        passwordButton.state = ""
                    }
                }
            }
            Widgets.SingleLineEdit {
                text: wz.tr("Scavengers"); readOnly: true
                Widgets.ImageSelect {
                    id: scavs
                    x: 105
                    y: 3
                    image1Source: "image://imagemap/button scavs on"
                    image1Hover: "image://imagemap/button scavs on hi"
                    image1Active: "image://imagemap/button active"
                    image2Source: "image://imagemap/button scavs off"
                    image2Hover: "image://imagemap/button scavs off hi"
                    image2Active: "image://imagemap/button active"

                    state: wz.config.get("scavengers") ? 2 : 1
                    onStateChanged: state == 2 ? wz.config.set("scavengers", true) : wz.config.set("scavengers", false)
                }
            }
            Widgets.SingleLineEdit {
                text: wz.tr("Fog"); readOnly: true
                Widgets.ImageSelect {
                    id: fog
                    x: 105
                    y: 3
                    image1Source: "image://imagemap/button eye"
                    image1Hover: "image://imagemap/button eye hi"
                    image1Active: "image://imagemap/button active"
                    image2Source: "image://imagemap/button fog on"
                    image2Hover: "image://imagemap/button fog on hi"
                    image2Active: "image://imagemap/button active"

                    state: wz.config.get("fog") ? 2 : 1
                    onStateChanged: state == 2 ? wz.config.set("fog", true) : wz.config.set("fog", false)
                }
            }
            Widgets.SingleLineEdit {
                text: wz.tr("Alliances"); readOnly: true
                Widgets.ImageSelect {
                    id: alliances
                    x: 105
                    y: 3
                    image1Source: "image://imagemap/button ffa"
                    image1Hover: "image://imagemap/button ffa hi"
                    image1Active: "image://imagemap/button active"
                    image2Source: "image://imagemap/button alliances"
                    image2Hover: "image://imagemap/button alliances hi"
                    image2Active: "image://imagemap/button active"
                    image3Source: "image://imagemap/button fixed teams"
                    image3Hover: "image://imagemap/button fixed teams hi"
                    image3Active: "image://imagemap/button active"

                    state: wz.config.get("alliance") + 1
                    onStateChanged: {
                        wz.config.set("alliance", state - 1)
                        state == 2 ? hostGameScreen._alliance = true : false
                    }
                }
            }
            Widgets.SingleLineEdit {
                text: wz.tr("Power"); readOnly: true
                Widgets.ImageSelect {
                    id: power
                    x: 105
                    y: 3
                    image1Source: "image://imagemap/button low power"
                    image1Hover: "image://imagemap/button low power hi"
                    image1Active: "image://imagemap/button active"
                    image2Source: "image://imagemap/button medium power"
                    image2Hover: "image://imagemap/button medium power hi"
                    image2Active: "image://imagemap/button active"
                    image3Source: "image://imagemap/button high power"
                    image3Hover: "image://imagemap/button high power hi"
                    image3Active: "image://imagemap/button active"

                    state: {
                        var cfg = wz.config.get("power");
                        switch (cfg)
                        {
                            case 400:
                                1
                            break;
                            case 700:
                                2
                            break;
                            case 1000:
                                3
                            break;
                            default:
                                3
                        }
                    }
                    onStateChanged: {
                        wz.config.set("power", (state == 1 ? 400 : (state == 2 ? 700 : 1000)))
                    }
                }
            }
            Widgets.SingleLineEdit {
                text: wz.tr("Base"); readOnly: true
                Widgets.ImageSelect {
                    id: base
                    x: 105
                    y: 3
                    image1Source: "image://imagemap/button no bases"
                    image1Hover: "image://imagemap/button no bases hi"
                    image1Active: "image://imagemap/button active"
                    image2Source: "image://imagemap/button medium bases"
                    image2Hover: "image://imagemap/button medium bases hi"
                    image2Active: "image://imagemap/button active"
                    image3Source: "image://imagemap/button full bases"
                    image3Hover: "image://imagemap/button full bases hi"
                    image3Active: "image://imagemap/button active"

                    state: wz.config.get("base") + 1
                    onStateChanged: wz.config.set("base", state - 1)
                }
            }
            Widgets.SingleLineEdit {
                text: wz.tr("Map Preview"); readOnly: true
                Widgets.ImageButton {
                    id: fogg_show
                    x: 141
                    y: 3
                    defaultSource: "image://imagemap/button eye"
                    hoverSource: "image://imagemap/button eye hi"
                    activeSource: "image://imagemap/button active"
                }
            }
        }
    }

    // Right blue Box
    Widgets.BlueBox {
        id: rightBlueBox
        width: rightBox.width
        height: rightBox.height
        anchors.left:  leftBlueBox.right
        anchors.leftMargin: 40
    }
    Item {
        id: rightBox
        width: 260
        height: 331
        anchors.left:  rightBlueBox.left
    }


    // Right sideText
    Widgets.SideText {
        id: rightSideText
        text: " "
        width: rightBox.height
        y: rightBox.height - 31 // 31 = text Height
        x: hostGameScreen.width - rightBox.width + 3
    }

    // Bottom blue Box
    Widgets.BlueBox {
        id: bottomBlueBox
        width: bottomBox.width
        height: bottomBox.height
        anchors.top:  leftBlueBox.bottom
        anchors.topMargin: 8
    }

    // Bottom sideText
    Widgets.SideText {
        id: bottomSideText
        text: wz.tr("Chat")
        width: bottomBox.height
        y: 430 // This should be calculated.
        x: 2
    }

    Item {
        id: bottomBox
        width: parent.width
        height: 120
        anchors.top:  bottomBlueBox.top

        Widgets.Chatbox {
            id: chatBox
            textColor: "white"
            onNewText: {
                chatBox.addLine(playername.text + ": " + chatBox.inputText)
                chatBox.inputText = ""
            }
        }
    }
}
