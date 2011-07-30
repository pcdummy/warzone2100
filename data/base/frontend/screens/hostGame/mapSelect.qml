import QtQuick 1.0
import Warzone 1.0
import "../../widgets" as Widgets

Item {
    id: container
    anchors.fill: parent

    function setPlayers(activeWidget, players, updateList)
    {
        // First deactivate all Textbuttons
        _all.state = "";
        _2p.state = "";
        _3p.state = "";
        _4p.state = "";
        _5p.state = "";
        _6p.state = "";
        _7p.state = "";
        _8p.state = "";
        _9p.state = "";
        _10p.state = "";

        // Then (re)activate the active one.
        activeWidget.state = "active";

        hostGameScreen.maxPlayers = players;

        if (updateList != false) {
            updateMapList();
        }
    }

    function setTechLevel(activeWidget, level, updateList)
    {
        // same as above
        _t1.state = "";
        _t2.state = "";
        _t3.state = "";

        activeWidget.state = "active";

        hostGameScreen.techlevel = level;

        if (updateList != false) {
            updateMapList();
        }
    }

    function updateMapList()
    {
        mapModel.clear();
		wz.log(hostGameScreen.techlevel);
        var maps = wz.getMapList(hostGameScreen.techlevel);

        for (var mapname in maps)
        {
            var mapdata = maps[mapname];
            if (hostGameScreen.maxPlayers == 0 || mapdata == hostGameScreen.maxPlayers)
            {
                mapModel.append({"name": mapname, "players2": mapdata});
            }
        }
    }

    Component.onCompleted: {
        rightSideText.text = wz.tr("Select Map")

		if (hostGameScreen.techlevel == Wz.Techlevel_1)
		{
			setTechLevel(_t1, Wz.Techlevel_1, false);
		}
		else if (hostGameScreen.techlevel == Wz.Techlevel_2)
		{
			setTechLevel(_t1, Wz.Techlevel_2, false);
		}
		else if (hostGameScreen.techlevel == Wz.Techlevel_3)
		{
			setTechLevel(_t1, Wz.Techlevel_3, false);
		}
		else
		{
			wz.log("Unknown techlevel " + hostGameScreen.techlevel)
			setTechLevel(_t1, Wz.Techlevel_1, false);
		}

        switch(hostGameScreen.maxPlayers) {
            case 0:
                setPlayers(_all, 0, false);
            break;
            case 2:
                setPlayers(_2p, 2, false);
            break;
            case 3:
                setPlayers(_3p, 3, false);
            break;
            case 4:
                setPlayers(_4p, 4, false);
            break;
            case 5:
                setPlayers(_5p, 5, false);
            break;
            case 6:
                setPlayers(_6p, 6, false);
            break;
            case 7:
                setPlayers(_7p, 7, false);
            break;
            case 8:
                setPlayers(_8p, 8, false);
            break;
            case 9:
                setPlayers(_9p, 9, false);
            break;
            case 10:
                setPlayers(_10p, 10, false);
            break;
        };

        updateMapList();
    }

    Column {
        id: playerFilters
        anchors.top: parent.top
        anchors.topMargin: 28
        anchors.left: parent.left
        anchors.leftMargin: 2

        spacing: 3

        Widgets.SmallTextButton { id: _all; text: " *"; onClicked: setPlayers(_all, 0); }
        Widgets.SmallTextButton { id: _2p; text: "2P"; onClicked: setPlayers(_2p, 2); }
        Widgets.SmallTextButton { id: _3p; text: "3P"; onClicked: setPlayers(_3p, 3); }
        Widgets.SmallTextButton { id: _4p; text: "4P"; onClicked: setPlayers(_4p, 4); }
        Widgets.SmallTextButton { id: _5p; text: "5P"; onClicked: setPlayers(_5p, 5); }
        Widgets.SmallTextButton { id: _6p; text: "6P"; onClicked: setPlayers(_6p, 6); }
        Widgets.SmallTextButton { id: _7p; text: "7P"; onClicked: setPlayers(_7p, 7); }
        Widgets.SmallTextButton { id: _8p; text: "8P"; onClicked: setPlayers(_8p, 8); }
        Widgets.SmallTextButton { id: _9p; text: "9P"; onClicked: setPlayers(_9p, 9); }
        Widgets.SmallTextButton { id: _10p; text: "10"; onClicked: setPlayers(_10p, 10); }
    }

    Column {
        id: techFilters
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 2
        anchors.left: parent.left
        anchors.leftMargin: 2

        spacing: 3

        Widgets.SmallTextButton { id: _t1; text: "T1"; onClicked: setTechLevel(_t1, Wz.Techlevel_1); }
        Widgets.SmallTextButton { id: _t2; text: "T2"; onClicked: setTechLevel(_t2, Wz.Techlevel_2); }
        Widgets.SmallTextButton { id: _t3; text: "T3"; onClicked: setTechLevel(_t3, Wz.Techlevel_3); }
    }

    Component {
        id: mapDelegate
        Widgets.MapButton {
            text: name
            players: players2
            onClicked: {
                if (wz.setMap(hostGameScreen.techlevel, name) == 0)
                {
                    wz.log("Failed to load map: " + name + ", techlevel: " + hostGameScreen.techlevel);
                }
                else
                {
                    hostGameScreen.maxPlayers = players2
                    hostGameScreen.map = name
                }

                mapButton.state = "";
                container.destroy();
                rightSideText.text = "";                
            }
        }
    }

    ListModel {
        id: mapModel
    }

    GridView {
        id: view

        flickableDirection: Flickable.VerticalFlick
        boundsBehavior: Flickable.StopAtBounds
        clip: true

        anchors.top: parent.top
        anchors.topMargin: 15
        anchors.left: playerFilters.right
        anchors.leftMargin: 5
        anchors.right: parent.right
        anchors.rightMargin: 2
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 15

        width: container.width - playerFilters.width - 6
        height: container.height - 30
        cellWidth: 107 + 2; cellHeight: 32 + 2.5

        model: mapModel
        delegate: mapDelegate
    }

    Widgets.ScrollBar {
        id: verticalScrollBar
        width: 10; height: view.height
        anchors.right: parent.right
        anchors.top: view.top
        orientation: Qt.Vertical
        position: view.visibleArea.yPosition
        pageSize: view.visibleArea.heightRatio
        showAlways: false
    }
}