import QtQuick 1.0

Image {
    id: quit

    width: parent.width
    height: parent.height

    source: "image://imagemap/Credits"
    fillMode:  Image.Stretch

    MouseArea {
        anchors.fill: quit
        onClicked: {
            Qt.quit()
        }
    }
}