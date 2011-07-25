import QtQuick 1.0
import "../widgets" as Widgets

Rectangle {
    id: container
    width: parent.width

    property int   spacing      : 1
    property alias image1Source : image1.defaultSource
    property alias image2Source : image2.defaultSource
    property alias image3Source : image3.defaultSource
    property alias image1Hover  : image1.hoverSource
    property alias image2Hover  : image2.hoverSource
    property alias image3Hover  : image3.hoverSource
    property alias image1Active : image1.activeSource
    property alias image2Active : image2.activeSource
    property alias image3Active : image3.activeSource

    // Allows to disable a single item.
    property alias image1State  : image1.state
    property alias image2State  : image2.state
    property alias image3State  : image3.state


    Row {
        spacing: parent.spacing

        Widgets.ImageButton {
            id: image1
            onClicked: {
                container.state = "1";
            }
        }
        Widgets.ImageButton {
            id: image2
            onClicked: {
                container.state = "2";
            }
        }
        Widgets.ImageButton {
            id: image3
            onClicked: {
                container.state = "3";
            }
        }
    }

    states: [
            State {
                    name: "1"
                    PropertyChanges { target: image1; state: "active" }
                    PropertyChanges { target: image2; state: "" }
                    PropertyChanges { target: image3; state: "" }
            },
            State {
                    name: "2"
                    PropertyChanges { target: image1; state: "" }
                    PropertyChanges { target: image2; state: "active" }
                    PropertyChanges { target: image3; state: "" }
            },
            State {
                    name: "3"
                    PropertyChanges { target: image1; state: "" }
                    PropertyChanges { target: image2; state: "" }
                    PropertyChanges { target: image3; state: "active" }
            }
   ]
}