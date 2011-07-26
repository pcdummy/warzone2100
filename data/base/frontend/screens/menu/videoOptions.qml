import QtQuick 1.0
import "../../widgets" as Widgets

Item {
    id: videoOptionsMenu

    width: parent.width
    height: parent.height

    Component.onCompleted: {
        sideText.text = wz.tr("Video Options")
    }

    Widgets.ImageButton {
        defaultSource: "image://imagemap/icon back"
        hoverSource: "image://imagemap/icon back hi"
        x: 5; y: 5
        onClicked: {
            createMenu("menu/options.qml");
        }
    }

    // Labels
    Column {
        id: labels
        width: 280
        anchors.top: parent.top
        anchors.topMargin: 56
        anchors.left: parent.left
        anchors.leftMargin: 30

        spacing: 15

        Widgets.LargeText { text: wz.tr("Video Playback") }
        Widgets.LargeText { text: wz.tr("Scanlines") }
        Widgets.LargeText { text: wz.tr("Screen Shake") }
        Widgets.LargeText { text: wz.tr("Fog") }
        Widgets.LargeText { text: wz.tr("Subtitles") }
        Widgets.LargeText { text: wz.tr("Shadows") }
    }

    // Options
    Column {
        width: 165
        anchors.top: parent.top
        anchors.topMargin: 56
        anchors.left: labels.right

        spacing: 15
        Widgets.ClickSelect {
            width: parent.width;
            options: [wz.tr("Fullscreen"), "1X", "2X"]
            value: {
                var tmp = wz.config.get("FMVmode");
                (tmp == 0 ? 0 : options.indexOf(tmp+"X"))
            }
            onValueChanged: wz.config.set("fsaa", (value == 0 ? 0 : options[value].substr(0, options[value].length-1)))
        }
        Widgets.ClickSelect {
            width: parent.width;
            options: [wz.tr("Off"), wz.tr("On")]
            value: wz.config.get("scanlines") ? 1 : 0;
            onValueChanged: wz.config.set("scanlines", (value == 1))
        }
        Widgets.ClickSelect {
            width: parent.width;
            options: [wz.tr("Off"), wz.tr("On")]
            value: wz.config.get("shake") ? 1 : 0;
            onValueChanged: wz.config.set("shake", (value == 1))
        }
        Widgets.ClickSelect {
            width: parent.width;
            options: [wz.tr("Mist"), wz.tr("Fog Of War")]
            value: wz.config.get("visfog") ? 1 : 0;
            onValueChanged: wz.config.set("visfog", (value == 1))
        }
        Widgets.ClickSelect {
            width: parent.width;
            options: [wz.tr("Off"), wz.tr("On")]
            value: wz.config.get("subtitles") ? 1 : 0;
            onValueChanged: wz.config.set("subtitles", (value == 1))
        }
        Widgets.ClickSelect {
            width: parent.width;
            options: [wz.tr("Off"), wz.tr("On")]
            value: wz.config.get("shadows") ? 1 : 0;
            onValueChanged: wz.config.set("shadows", (value == 1))
        }
    }
}
