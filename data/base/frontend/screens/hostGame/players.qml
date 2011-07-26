import QtQuick 1.0
import Warzone 1.0
import "../../widgets" as Widgets

Item {
    id: container
    anchors.fill: parent


    // Internal to remember which player to kick
    property int        _kickPlayer:    -1

    // Internal for the AI selector.
    property int        _aiSelectorSlot: -1

    // START: Color/Team selector
    ListModel {
        id: selectorModel
    }

    Component {
        id: selectorDelegate

        Widgets.ImageButton {
            width: 23
            defaultSource: (type == "playerColor" ? "image://imagemap/icon player" + propertyNum : "image://imagemap/TEAM" + propertyNum)
            defaultSourceWidth: 25
            defaultSourceHeight: 21
            hoverSource: (type == "playerColor" ? "image://imagemap/icon player" + propertyNum + " hi" : "image://imagemap/TEAM" + propertyNum + " HI")
            hoverSourceWidth: 25
            hoverSourceHeight: 21
            activeSource: (type == "playerColor" ? "image://imagemap/icon player" + propertyNum + " hi" : "image://imagemap/TEAM" + propertyNum + " HI")
            activeSourceWidth: 25
            activeSourceHeight: 21

            state: (off ? "off" : "")

            onClicked: {
                setPlayerProperty(type, playerIndex, propertyNum);
                selector.opacity = 0
            }
        }
    }

    Rectangle {
        id: selector

        width: 248; height: 38

        color: "#000161"
        border.color: "#0015f0"
        border.width: 1

        opacity: 0
        z: 99

        ListView {
            anchors.fill: parent
            anchors.leftMargin: 3
            anchors.rightMargin: 3
            anchors.topMargin: 9

            interactive: false
            orientation: ListView.Horizontal
            spacing: 1

            model: selectorModel
            delegate: selectorDelegate
        }

        Widgets.ImageButton {
            id: kickButton
            width: 25
            anchors.top: parent.top
            anchors.topMargin: 9
            anchors.right: parent.right
            opacity: 0

            defaultSource: "image://imagemap/icon kick"
            defaultSourceWidth: 25
            defaultSourceHeight: 21
            hoverSource: "image://imagemap/icon kick hi"
            hoverSourceWidth: 25
            hoverSourceHeight: 21
            activeSource: "image://imagemap/icon kick hi"
            activeSourceWidth: 25
            activeSourceHeight: 21

            onClicked: {
                kickButton.opacity = 0
                console.log("Kicking player " + container._kickPlayer)
                selector.opacity = 0
            }
        }
    }

    /**
     * Fills the selectorModel and sets it at the players positions.
     *
     * The Buttons call setPlayerProperty(type, playerIndex, propertyNum)
     * and hide this selector.
     *
     * @param string    type            Property type either "playerColor" or "team"
     * @param int       playerIndex     Index of the player in the playersModel
     */
    function showSelector(type, playerIndex)
    {
        selectorModel.clear()

        if (type == "playerColor")
        {
            for (var i=0; i<8;i++)
            {
                selectorModel.append({type: type,
                                      playerIndex: playerIndex,
                                      propertyNum: i,
                                      off: false})
            }

            // Non host players arent allowed to use a used flag, disable them.
            if (!wz.config.get("isHost"))
            {
                for (var i=0; i<playersModel.count;i++)
                {
                    selectorModel.setProperty(playersModel.get(i).playerColor, "off", true);
                }
            }
        }
        else // TEAM
        {
            // First add maxPlayers teams
            for (var i=0; i<hostGameScreen.maxPlayers;i++)
            {
                selectorModel.append({type: type,
                                      playerIndex: playerIndex,
                                      propertyNum: i,
                                      off: false})
            }

            // Now check if all players are in the same team and disable that one
            // if required.
            var check = (playerIndex == 0 ? playersModel.get(1).team : playersModel.get(0).team)
            for (var i=0; i<playersModel.count;i++)
            {
                if (i == playerIndex)
                {
                    continue
                }
                else if (check == playersModel.get(i).team)
                {
                    continue
                }
                else
                {
                    check = -1
                    break
                }
            }

            if (check != -1)
            {
                selectorModel.setProperty(check, "off", true);
            }

            // Configure and enable the kick button
            if (!playersModel.get(playerIndex).isAI &&
                playerIndex != wz.config.get("playerIndex") &&
                wz.config.get("isHost"))
            {
                container._kickPlayer = playerIndex
                kickButton.opacity = 1
            }
        }

        selector.x = 6
        selector.y = (38+3)*playerIndex+2 // (Height + spacer) * row number + top spacer
        selector.opacity = 1
    }

    /**
     * Set the property type for playerIndex in playersModel.
     * This exchanges colors if required.
     *
     * @param string    type             Property type either "playerColor" or "team"
     * @param int       propertyNum      Properties number (color number or team number)
     * @param int       playerIndex      Index of the player in the playersModel
     */
    function setPlayerProperty(type, playerIndex, propertyNum)
    {
        if (type == "playerColor")
        {
            for (var i=0;i<playersModel.count;i++)
            {
                if (playersModel.get(i).playerColor == propertyNum)
                {
                    playersModel.setProperty(i, type, playersModel.get(playerIndex).playerColor)
                    break
                }
            }
        }

        playersModel.setProperty(playerIndex, type, propertyNum)
    }
    // END: Color/Team selector

    // START: AI selector.
    Column {
        id: aiSelector

        anchors.fill: parent
        anchors.leftMargin: 6
        anchors.rightMargin: 6
        anchors.topMargin: 2
        anchors.bottomMargin: 2

        z: 99
        opacity: 0

        spacing: 3

        Rectangle {
            width: 248; height: 38
            color: "#000161"
            border.color: "#0015f0"
            border.width: 1

            Text {
                anchors.fill: parent
                anchors.leftMargin: 5
                verticalAlignment: Text.AlignVCenter

                text: wz.tr("Closed")

                color: "#aeaeb7"
                smooth: true
                font.family: "DejaVu Sans"
                font.pointSize: 9
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked: {
                    aiSelector.opacity = 0
                    view.opacity = 1

                    playersModel.setProperty(container._aiSelectorSlot, "isAI", true);
                    playersModel.setProperty(container._aiSelectorSlot, "isOpen", false);
                    playersModel.setProperty(container._aiSelectorSlot, "name", "Closed");
                }
            }
        }

        Rectangle {
            width: 248; height: 38
            color: "#000161"
            border.color: "#0015f0"
            border.width: 1

            opacity: (wz.config.get("gameType") == Wz.Multiplayer ? 1 : 0)

            Text {
                anchors.fill: parent
                anchors.leftMargin: 5
                verticalAlignment: Text.AlignVCenter

                text: wz.tr("Open")

                color: "#aeaeb7"
                smooth: true
                font.family: "DejaVu Sans"
                font.pointSize: 9
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked: {
                    aiSelector.opacity = 0
                    view.opacity = 1
                    playersModel.setProperty(container._aiSelectorSlot, "isAI", false);
                    playersModel.setProperty(container._aiSelectorSlot, "isOpen", true);
                    playersModel.setProperty(container._aiSelectorSlot, "name", "Open");
                }
            }
        }

        Rectangle {
            width: parent.width; height: 38
            color: "#000161"
            border.color: "#0015f0"
            border.width: 1

            Text {
                anchors.fill: parent
                anchors.leftMargin: 5
                verticalAlignment: Text.AlignVCenter

                text: "Nexus"

                color: "#aeaeb7"
                smooth: true
                font.family: "DejaVu Sans"
                font.pointSize: 9
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked: {
                    aiSelector.opacity = 0
                    view.opacity = 1
                    playersModel.setProperty(container._aiSelectorSlot, "isAI", true);
                    playersModel.setProperty(container._aiSelectorSlot, "isOpen", false);
                    playersModel.setProperty(container._aiSelectorSlot, "name", "Nexus");
                }
            }
        }

        Rectangle {
            width: parent.width; height: 38
            color: "#000161"
            border.color: "#0015f0"
            border.width: 1

            Text {
                anchors.fill: parent
                anchors.leftMargin: 5
                verticalAlignment: Text.AlignVCenter

                text: "Dydo"

                color: "#aeaeb7"
                smooth: true
                font.family: "DejaVu Sans"
                font.pointSize: 9
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked: {
                    aiSelector.opacity = 0
                    view.opacity = 1
                    playersModel.setProperty(container._aiSelectorSlot, "isAI", true);
                    playersModel.setProperty(container._aiSelectorSlot, "isOpen", false);
                    playersModel.setProperty(container._aiSelectorSlot, "name", "Dydo");
                }
            }
        }

        Rectangle {
            width: parent.width; height: 38
            color: "#000161"
            border.color: "#0015f0"
            border.width: 1

            Text {
                anchors.fill: parent
                anchors.leftMargin: 5
                verticalAlignment: Text.AlignVCenter

                text: "Semperfi"

                color: "#aeaeb7"
                smooth: true
                font.family: "DejaVu Sans"
                font.pointSize: 9
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked: {
                    aiSelector.opacity = 0
                    view.opacity = 1
                    playersModel.setProperty(container._aiSelectorSlot, "isAI", true);
                    playersModel.setProperty(container._aiSelectorSlot, "isOpen", false);
                    playersModel.setProperty(container._aiSelectorSlot, "name", "Semperfi");
                }
            }
        }
    }

    // END: AI selector.

    // START: Slot selector.
    ListModel {
        id: slotSelectorModel
    }

    Component {
        id: slotSelectorDelegate

        Rectangle {
            width: parent.width; height: 38
            color: "#000161"
            border.color: "#0015f0"
            border.width: 1

            opacity: (takeAble ? 1 : 0)

            Text {
                anchors.fill: parent
                anchors.leftMargin: 5
                verticalAlignment: Text.AlignVCenter

                text: wz.tr("Click to take player slot %1").arg(slotIndex)

                color: "#aeaeb7"
                smooth: true
                font.family: "DejaVu Sans"
                font.pointSize: 9
            }

            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onClicked: {
                    slotSelector.opacity = 0
                    playersModel.move(oldIndex, slotIndex, 1)

                    if (hostGameScreen.playerIndex == oldIndex)
                    {
                        wz.config.set("playerIndex", slotIndex)
                    }
                }
            }
        }
    }

    ListView {
        id: slotSelector

        anchors.fill: parent
        anchors.leftMargin: 6
        anchors.rightMargin: 6
        anchors.topMargin: 2
        anchors.bottomMargin: 2

        interactive: false

        z: 99
        opacity: 0

        spacing: 3

        model: slotSelectorModel
        delegate: slotSelectorDelegate
    }

    function showSlotSelector(oldIndex)
    {
        slotSelectorModel.clear()
        for (var i=0; i<playersModel.count;i++)
        {
            slotSelectorModel.append({slotIndex: i,
                                      oldIndex: oldIndex,
                                      takeAble: i != oldIndex})
        }
        slotSelector.opacity = 1
    }
    // END: Slot selector.

    Component.onCompleted: {
        rightSideText.text = wz.tr("Players")

        playersModel.clear()
        playersModel.append({isAI: false, team: 0, playerColor: 0, name: wz.config.get("playerName"), isReady: false, isHost: true, isOpen: false,
                            statsPlayed: 0, statsWins: 0, statsLosses: 0, statsTotalKills: 0, statsTotalScore: 0});

        if (!wz.config.get("gameType") == Wz.Multiplayer)
        {
            for(var i=1;i<hostGameScreen.maxPlayers;i++)
            {
                playersModel.append({isAI: true, team: i, playerColor: i, name: "Nexus", isReady: true, isHost: false, isOpen: false,
                                    statsPlayed: 0, statsWins: 0, statsLosses: 0, statsTotalKills: 0, statsTotalScore: 0});
            }
        }
        else
        {
            for(var i=1;i<hostGameScreen.maxPlayers;i++)
            {
                playersModel.append({isAI: false, team: i, playerColor: i, name: "Open", isReady: false, isHost: false, isOpen: true,
                                    statsPlayed: 0, statsWins: 0, statsLosses: 0, statsTotalKills: 0, statsTotalScore: 0});
            }
        }
    }

    Component {
        id: playersDelegate

        Rectangle {
            width: hostGameScreen._alliance ? 248 : 217
            height: 38

            color: "#000161"
            border.color: "#0015f0"
            border.width: 1

            anchors.right: parent.right

            Rectangle {
                id: teamselector
                width: 31; height: parent.height

                color: "#000161"
                border.color: "#0015f0"
                border.width: 1

                Widgets.ImageButton {
                    x: 3
                    y: 9

                    defaultSource: "image://imagemap/TEAM" + team
                    defaultSourceWidth: 25
                    defaultSourceHeight: 21
                    hoverSource: "image://imagemap/TEAM" + team + " HI"
                    hoverSourceWidth: 25
                    hoverSourceHeight: 21
                    activeSource: "image://imagemap/TEAM" + team + " HI"
                    activeSourceWidth: 25
                    activeSourceHeight: 21

                    onClicked: {
                        showSelector("team", index)
                    }
                }
            }

            Rectangle {
                id: colorflag
                width: 33; height: parent.height

                anchors.left: hostGameScreen._alliance ? teamselector.right : parent.left

                color: "#000161"
                border.color: "#0015f0"
                border.width: 1

                Widgets.ImageButton {
                    x: 5
                    y: 9

                    defaultSource: "image://imagemap/icon player" + playerColor
                    defaultSourceWidth: 25
                    defaultSourceHeight: 21
                    hoverSource: "image://imagemap/icon player" + playerColor + " hi"
                    hoverSourceWidth: 25
                    hoverSourceHeight: 21
                    activeSource: "image://imagemap/icon player" + playerColor + " hi"
                    activeSourceWidth: 25
                    activeSourceHeight: 21

                    onClicked: {
                        showSelector("playerColor", index)
                    }
                }
            }

            Item  {
                id: rank
                width: 28; height: parent.height

                anchors.left: colorflag.right

                Image {
                    id: topstar

                    anchors.top: parent.top
                    anchors.topMargin: 4
                    anchors.left: parent.left
                    anchors.leftMargin: 4

                    source: {
                        if (statsTotalKills >600)
                        {
                            "image://imagemap/star gold"
                        }
                        else if (statsTotalKills >300)
                        {
                            "image://imagemap/star silver"
                        }
                        else if (statsTotalKills >150)
                        {
                            "image://imagemap/star bronze"
                        }
                        else
                        {
                            "image://imagemap/empty"
                        }
                    }
                    sourceSize.width: 8
                    sourceSize.height: 8

                    opacity: isAI || isOpen ? 0 : 1
                }

                Image {
                    id: middlestar

                    anchors.top: topstar.bottom
                    anchors.topMargin: 2
                    anchors.left: parent.left
                    anchors.leftMargin: 4

                    source: {
                        if (statsPlayed >600)
                        {
                            "image://imagemap/star gold"
                        }
                        else if (statsPlayed >300)
                        {
                            "image://imagemap/star silver"
                        }
                        else if (statsPlayed >150)
                        {
                            "image://imagemap/star bronze"
                        }
                        else
                        {
                            "image://imagemap/empty"
                        }
                    }
                    sourceSize.width: 8
                    sourceSize.height: 8

                    opacity: isAI || isOpen ? 0 : 1
                }

                Image {
                    id: bottomstar

                    anchors.top: middlestar.bottom
                    anchors.topMargin: 2
                    anchors.left: parent.left
                    anchors.leftMargin: 4

                    source: {
                        if (statsWins >600)
                        {
                            "image://imagemap/star gold"
                        }
                        else if (statsWins >300)
                        {
                            "image://imagemap/star silver"
                        }
                        else if (statsWins >150)
                        {
                            "image://imagemap/star bronze"
                        }
                        else
                        {
                            "image://imagemap/empty"
                        }
                    }
                    sourceSize.width: 8
                    sourceSize.height: 8

                    opacity: isAI || isOpen ? 0 : 1
                }



                Image {
                    anchors.left: topstar.right
                    anchors.leftMargin: 1
                    anchors.top: parent.top
                    anchors.topMargin: 8

                    source: {
                        if (statsPlayed < 5) {
                            "image://imagemap/medal dummy"
                        }
                        else if (statsWins >= 24 && (statsWins > (8 * statsLosses)))
                        {
                            "image://imagemap/medal gold"
                        }
                        else if (statsWins >= 12 && (statsWins > (4 * statsLosses)))
                        {
                            "image://imagemap/medal silver"
                        }
                        else if (statsWins >= 6 && (statsWins > (2 * statsLosses)))
                        {
                            "image://imagemap/medal bronze"
                        }
                        else
                        {
                            "image://imagemap/empty"
                        }
                    }
                    sourceSize.width: 12
                    sourceSize.height: 50

                    opacity: isAI || isOpen ? 0 : 1
                }
            }

            Text {
                id: text
                text: name

                anchors.left: rank.right
                anchors.right: checkbox.left
                anchors.top: parent.top
                anchors.topMargin: 12

                color: "#aeaeb7"
                smooth: true
                font.family: "DejaVu Sans"
                font.pointSize: 9

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.LeftButton | Qt.RightButton
                    onClicked: {
                        if (!isAI && !isOpen)
                        {
                            showSlotSelector(index);
                        }
                        else
                        {
                            view.opacity = 0
                            container._aiSelectorSlot = index
                            aiSelector.opacity = 1
                        }
                    }
                }
            }

            Rectangle {
                id: checkbox
                width: 43; height: parent.height

                anchors.right: parent.right

                color: "#000161"
                border.color: "#0015f0"
                border.width: 1

                Image {
                    x: 10
                    y: 14
                    source: (isAI ? "image://imagemap/icon ai player" : (isReady ? "image://imagemap/icon checkbox on" : "image://imagemap/icon checkbox off"))
                    sourceSize.width: 22
                    sourceSize.height: 22

                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton

                        enabled: !isAI

                        onClicked: playersModel.setProperty(index, "isReady", !isReady)
                    }
                }
            }
        }
    }

    ListView {
        id: view

        anchors.fill: parent
        anchors.leftMargin: 6
        anchors.rightMargin: 6
        anchors.topMargin: 2
        anchors.bottomMargin: 2

        spacing: 3
        interactive: false

        model: playersModel
        delegate: playersDelegate
    }
}