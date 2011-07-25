import QtQuick 1.0

Item {
    id: container

    signal clicked

    property alias defaultSource       : image.source
    property alias defaultSourceWidth  : image.sourceSize.width
    property alias defaultSourceHeight : image.sourceSize.height
    property alias hoverSource         : hover.source
    property alias hoverSourceWidth    : hover.sourceSize.width
    property alias hoverSourceHeight   : hover.sourceSize.height
    property alias activeSource        : active.source
    property alias activeSourceWidth   : active.sourceSize.width
    property alias activeSourceHeight  : active.sourceSize.height

    width: image.width
    height: image.height

    Image {
        id: image
        opacity: 1

        Image {
            id: hover
            opacity: 0

            Image {
                id: active
                opacity: 0
            }
        }
    }

    MouseArea {
        id: ma
        anchors.fill: image
        hoverEnabled: true
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onEntered: {
            if (parent.state != "active" && parent.state != "activeOff") {
                parent.state = "hover"
            }
        }
        onExited: {
            if (parent.state != "active" && parent.state != "activeOff") {
                parent.state = ""
            }

        }
        onClicked: {
            container.clicked();
        }
    }

    states: [
            State {
                    name: "hover"
                    PropertyChanges { target: hover; opacity: 1 }
            },
            State {
                    name: "active"
                    PropertyChanges { target: hover; opacity: 1 }
                    PropertyChanges { target: active; opacity: 1 }
            },
            State {
                    name: "off"
                    PropertyChanges { target: image; opacity: 0.6 }
                    PropertyChanges { target: ma; enabled: false }
            },
            State {
                    name: "activeOff"
                    PropertyChanges { target: image; opacity: 0 }
                    PropertyChanges { target: active; opacity: 0.6 }
                    PropertyChanges { target: ma; enabled: false }
            }
    ]
}
