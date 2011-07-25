import QtQuick 1.0

Rectangle {
    id: container

    signal clicked

    property alias text: buttonText.text
    property alias bwidth: buttonText.width
    property alias bheight: buttonText.height

    property color buttonColor: "#a0a0ff"
    property color hoverColor: "#ffffff"

    Text {
        id: buttonText
        color: buttonColor
        font.weight: Font.DemiBold
        font.family: "DejaVu Sans"
        font.pixelSize: 21
        smooth: true

        // Text into center
        horizontalAlignment: Text.AlignHCenter

        // Box into center
        x: (parent.width - width) / 2

        MouseArea {
            anchors.fill: parent
            hoverEnabled: true
            acceptedButtons: Qt.LeftButton | Qt.RightButton
            onEntered: parent.color = hoverColor
            onExited: parent.color = buttonColor
            onClicked: container.clicked()
        }
    }
}
