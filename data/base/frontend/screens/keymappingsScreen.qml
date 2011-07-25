import QtQuick 1.0
import "../widgets" as Widgets

Item {
    id: keymappingsScreen
    width: 580
    height: 450

    // Position in center
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2

    // Bluebox
    Widgets.BlueBox { anchors.fill: parent }

    // Left sideText
    Widgets.SideText {
        id: leftSideText
        text: wz.tr("Key Mapping")
        width: keymappingsScreen.height
        y: keymappingsScreen.height - 31 // 31 = text Height
        x: 2
    }

    Widgets.ImageButton {
        id: backButton
        defaultSource: "image://imagemap/icon back"
        hoverSource: "image://imagemap/icon back hi"
        x: 6; y: 5
        onClicked: {
            keymappingsScreen.destroy();
            window.loadMenu = window.backMenu;
            createScreen(window.backScreen);
        }
    }

    ListView {
        id: view

        anchors.fill: parent
        anchors.topMargin: 20
        anchors.leftMargin: 50
        anchors.rightMargin: 46
        anchors.bottomMargin: 20

        flickableDirection: Flickable.VerticalFlick
        boundsBehavior: Flickable.StopAtBounds
        clip: true

        spacing: 2

        Component {
            id: delegate

            Rectangle {
                id: wrapper

                width: view.width
                height: 17

                color: important ? "#800000" : "#000161"
                border.color: "#0015f0"
                border.width: 1

                Text {
                    anchors.left: parent.left
                    anchors.leftMargin: 2
                    text: wz.tr(optionText)
                    color: "white"
                    smooth: true
                    font.family: "DejaVu Sans"
                    font.pointSize: 9
                }
            }
        }

        delegate: delegate

        model: ListModel {
            ListElement { optionText: "Assign Group 0"; important: false }
            ListElement { optionText: "Assign Group 1"; important: false }
            ListElement { optionText: "Assign Group 2"; important: false }
            ListElement { optionText: "Assign Group 3"; important: false }
            ListElement { optionText: "Assign Group 4"; important: false }
            ListElement { optionText: "Assign Group 5"; important: false }
            ListElement { optionText: "Assign Group 6"; important: false }
            ListElement { optionText: "Assign Group 7"; important: false }
            ListElement { optionText: "Assign Group 8"; important: false }
            ListElement { optionText: "Assign Group 9"; important: false }
            ListElement { optionText: "Build"; important: true }
            ListElement { optionText: "Center View on HQ"; important: false }
            ListElement { optionText: "Commanders"; important: true }
            ListElement { optionText: "Console On/Off"; important: false }
            ListElement { optionText: "Decrease Game Speed"; important: false }
            ListElement { optionText: "Design"; important: true }
            ListElement { optionText: "Display In-Game Options"; important: true }
            ListElement { optionText: "Dor or Die!"; important: false }
            ListElement { optionText: "Drop a beacon"; important: false }
            ListElement { optionText: "Fire at will"; important: false }
            ListElement { optionText: "Got to Transport"; important: false }
            ListElement { optionText: "Guard Position"; important: false }
            ListElement { optionText: "Hold Fire"; important: false }
            ListElement { optionText: "Hold Position"; important: false }
            ListElement { optionText: "Increase Game Speed"; important: false }
            ListElement { optionText: "Intelligence Display"; important: true }
            ListElement { optionText: "Long Range"; important: false }
            ListElement { optionText: "Manufacture"; important: false }
            ListElement { optionText: "Multiplayer Options / Alliance dialog"; important: false }
            ListElement { optionText: "Optimum Range"; important: false }
            ListElement { optionText: "Orders Menu"; important: false }
            ListElement { optionText: "Patrol"; important: false }
            ListElement { optionText: "Pitch Back"; important: false }
            ListElement { optionText: "Pitch Forward"; important: false }
            ListElement { optionText: "Pursue"; important: false }
            ListElement { optionText: "Research"; important: true }
            ListElement { optionText: "Reset Game Speed"; important: false }
            ListElement { optionText: "Reset Pitch"; important: false }
            ListElement { optionText: "Retreat at Heavy Damage"; important: false }
            ListElement { optionText: "Retreat at Medium Damage"; important: false }
            ListElement { optionText: "Return Fire"; important: false }
            ListElement { optionText: "Return For Repair"; important: false }
            ListElement { optionText: "Return For Recycling"; important: false }
            ListElement { optionText: "Return to HQ"; important: false }
            ListElement { optionText: "Rotate Left"; important: false }
            ListElement { optionText: "Rotate Right"; important: false }
            ListElement { optionText: "Select Commander 0"; important: false }
            ListElement { optionText: "Select Commander 1"; important: false }
            ListElement { optionText: "Select Commander 2"; important: false }
            ListElement { optionText: "Select Commander 3"; important: false }
            ListElement { optionText: "Select Commander 4"; important: false }
            ListElement { optionText: "Select Commander 5"; important: false }
            ListElement { optionText: "Select Commander 6"; important: false }
            ListElement { optionText: "Select Commander 7"; important: false }
            ListElement { optionText: "Select Commander 8"; important: false }
            ListElement { optionText: "Select Commander 9"; important: false }
            ListElement { optionText: "Select EVERY unit"; important: false }
            ListElement { optionText: "Select Group 0"; important: false }
            ListElement { optionText: "Select Group 1"; important: false }
            ListElement { optionText: "Select Group 2"; important: false }
            ListElement { optionText: "Select Group 3"; important: false }
            ListElement { optionText: "Select Group 4"; important: false }
            ListElement { optionText: "Select Group 5"; important: false }
            ListElement { optionText: "Select Group 6"; important: false }
            ListElement { optionText: "Select Group 7"; important: false }
            ListElement { optionText: "Select Group 8"; important: false }
            ListElement { optionText: "Select Group 9"; important: false }
            ListElement { optionText: "Select all Combat Units"; important: false }
            ListElement { optionText: "Select all Half-tracks"; important: false }
            ListElement { optionText: "Select Heavily Damaged Units"; important: false }
            ListElement { optionText: "Select all Hovers"; important: false }
            ListElement { optionText: "Select all Similar Units"; important: false }
            ListElement { optionText: "Select all Tracks"; important: false }
            ListElement { optionText: "Select all Units on Screen"; important: false }
            ListElement { optionText: "Select all VTOLs"; important: false }
            ListElement { optionText: "Select all Wheels"; important: false }
            ListElement { optionText: "Select next Cyborg Factory"; important: false }
            ListElement { optionText: "Select next Factory"; important: false }
            ListElement { optionText: "Select next Power Generator"; important: false }
            ListElement { optionText: "Select next Research Facility"; important: false }
            ListElement { optionText: "Send Text Message"; important: false }
            ListElement { optionText: "Sensor display Off"; important: true }
            ListElement { optionText: "Sensor display On"; important: true }
            ListElement { optionText: "Short Range"; important: false }
            ListElement { optionText: "Snap View to North"; important: false }
            ListElement { optionText: "Take Screen Shot"; important: true }
            ListElement { optionText: "Toggle Console Display"; important: false }
            ListElement { optionText: "Toggle Damage Bars On/Off"; important: false }
            ListElement { optionText: "Toggle Formation Speed Limiting"; important: false }
            ListElement { optionText: "Toggle Overlays"; important: false }
            ListElement { optionText: "Toggle Radar"; important: false }
            ListElement { optionText: "Toggle Sensor display"; important: false }
            ListElement { optionText: "Toggle Tracking Camera"; important: false }
            ListElement { optionText: "Toggle all-enemy radar view"; important: false }
            ListElement { optionText: "Toggle radar terrain"; important: false }
            ListElement { optionText: "Toggle shadows"; important: false }
            ListElement { optionText: "Trap cursor"; important: false }
            ListElement { optionText: "View East"; important: false }
            ListElement { optionText: "View Location of Previous Message"; important: false }
            ListElement { optionText: "View North"; important: false }
            ListElement { optionText: "View South"; important: false }
            ListElement { optionText: "View Unassigned Units"; important: false }
            ListElement { optionText: "View West"; important: false }
            ListElement { optionText: "View next Commander"; important: false }
            ListElement { optionText: "View next Oil Derrick"; important: false }
            ListElement { optionText: "View next Repair Unit"; important: false }
            ListElement { optionText: "View next Sensor Unit"; important: false }
            ListElement { optionText: "View next Truck"; important: false }
            ListElement { optionText: "Zoom In"; important: false }
            ListElement { optionText: "Zoom Out"; important: false }
            ListElement { optionText: "Zoom Radar In"; important: false }
            ListElement { optionText: "Zoom Radar Out"; important: false }
        }
    }

    Widgets.ScrollBar {
        id: verticalScrollBar
        width: 10; height: view.height
        anchors.left: view.right
        anchors.top: view.top
        orientation: Qt.Vertical
        position: view.visibleArea.yPosition
        pageSize: view.visibleArea.heightRatio
        showAlways: false
    }
}