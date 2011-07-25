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
                var tmp = wz.getConfigValue("FMVmode");
                (tmp == 0 ? 0 : options.indexOf(tmp+"X"))
            }
            onValueChanged: wz.setConfigValue("fsaa", (value == 0 ? 0 : options[value].substr(0, options[value].length-1)))
        }
        Widgets.ClickSelect {
            width: parent.width;
            options: [wz.tr("Off"), wz.tr("On")]
            value: wz.getConfigValue("scanlines") ? 1 : 0;
            onValueChanged: wz.setConfigValue("scanlines", (value == 1))
        }
        Widgets.ClickSelect {
            width: parent.width;
            options: [wz.tr("Off"), wz.tr("On")]
            value: wz.getConfigValue("shake") ? 1 : 0;
            onValueChanged: wz.setConfigValue("shake", (value == 1))
        }
        Widgets.ClickSelect {
            width: parent.width;
            options: [wz.tr("Mist"), wz.tr("Fog Of War")]
            value: wz.getConfigValue("visfog") ? 1 : 0;
            onValueChanged: wz.setConfigValue("visfog", (value == 1))
        }
        Widgets.ClickSelect {
            width: parent.width;
            options: [wz.tr("Off"), wz.tr("On")]
            value: wz.getConfigValue("subtitles") ? 1 : 0;
            onValueChanged: wz.setConfigValue("subtitles", (value == 1))
        }
        Widgets.ClickSelect {
            width: parent.width;
            options: [wz.tr("Off"), wz.tr("On")]
            value: wz.getConfigValue("shadows") ? 1 : 0;
            onValueChanged: wz.setConfigValue("shadows", (value == 1))
        }
    }
}
