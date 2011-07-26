import QtQuick 1.0
import "../../widgets" as Widgets

Item {
    id: audioOptionsMenu

    width: parent.width
    height: parent.height

    Component.onCompleted: {
        sideText.text = wz.tr("Audio Options")
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

        Widgets.LargeText { text: wz.tr("Voice Volume") }
        Widgets.LargeText { text: wz.tr("FX Volume") }
        Widgets.LargeText { text: wz.tr("Music Volume") }
    }

    // Options
    Column {
        width: 165
        anchors.top: parent.top
        anchors.topMargin: 56
        anchors.left: labels.right

        spacing: 24
        Widgets.Slider {width: parent.width; value: wz.config.get("voicevol"); onValueChanged: wz.config.set("voicevol", value)}
        Widgets.Slider {width: parent.width; value: wz.config.get("fxvol"); onValueChanged: wz.config.set("fxvol", value)}
        Widgets.Slider {width: parent.width; value: wz.config.get("cdvol"); onValueChanged: wz.config.set("cdvol", value)}
    }
}
