import QtQuick 1.0

Text {
    id: container

    property variant options:   []
    property variant value: 0

    text: options[value]

    color: "#a0a0ff"
    font.weight: Font.DemiBold
    font.family: "DejaVu Sans"
    font.pixelSize: 21
    smooth: true


    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        hoverEnabled: true
        onEntered: { parent.state = "hover" }
        onExited: { parent.state = "" }

        onClicked: {
            if (options.length > value+1)
            {
                value += 1
            }
            else
            {
                value = 0
            }
        }
    }

    states: [
        State {
                name: "hover"
                PropertyChanges { target: container; color: "#ffffff" }
        }
    ]
}