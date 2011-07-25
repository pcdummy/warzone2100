import QtQuick 1.0

Rectangle {
    id: container

    signal clicked

    // Map name
    property alias      text     : text.text

    // Number of players
    property alias      players  : players.model


    color: "#000161"
    border.color: "#0015f0"
    border.width: 1
    width: 105; height: 32

    Text {
        id: text
        color: "white"
        smooth: true
        font.family: "DejaVu Sans"
        font.pointSize: 9
        clip: true

        anchors.top: parent.top
        anchors.topMargin: 2
        anchors.left: parent.left
        anchors.leftMargin: 6
        width: parent.width - 12
    }

    Row {
        anchors.top: text.bottom
        anchors.topMargin: 5
        anchors.left: parent.left
        anchors.leftMargin: 6
        spacing: -2
        Repeater {
            id: players
            model: 4

            Image {
                width: 10; height: 10
                source: "image://imagemap/icon player"
                sourceSize.height: 10; sourceSize.width: 10
            }

        }
    }

    MouseArea {
        id: ma
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        onClicked: {
            parent.clicked();
        }
    }
}