import QtQuick 1.0
import "../widgets" as Widgets

Item {
    id: chatbox
    anchors.fill: parent

    signal newText

    property string textColor         : "black"
    property int margins              : 2

    property alias inputText          : input.text
    property alias viewPaintedHeight  : text.paintedHeight
    property alias viewText           : text.text
    property alias viewHeight         : text.height

    function addSystemMessage(line)
    {
        addLine("<span style=\"font-size: large; font-weight: medium; color:#e1e417\">System message:<br />" + line + "</span>");
    }

    function addLine(line)
    {
        text.text += line + "<br />";
        text.height = (view.height < text.paintedHeight ? text.paintedHeight : view.height)
        view.contentY = text.height - view.height
    }

    function clear()
    {
        text.text = ""
        text.height = (view.height < text.paintedHeight ? text.paintedHeight : view.height)
        view.contentY = text.height - view.height
    }

    Flickable {
        id: view

        anchors.fill: parent
        anchors.margins: parent.margins
        anchors.bottomMargin: 20

        contentWidth: text.width
        contentHeight: text.height
        flickableDirection: Flickable.VerticalFlick

        boundsBehavior: Flickable.StopAtBounds
        clip: true

        Text {
            id: text
            color: chatbox.textColor

            smooth: true
            font.family: "DejaVu Sans"
            font.pointSize: 9

            width: view.width
            height: view.height

            wrapMode: Text.WordWrap
        }
    }

    // The texts vertical scrollbar.
    Widgets.ScrollBar {
        id: verticalScrollBar
        width: 10; height: view.height
        anchors.right: chatbox.right
        orientation: Qt.Vertical
        position: view.visibleArea.yPosition
        pageSize: view.visibleArea.heightRatio
        showAlways: false
    }

    Rectangle {
        id: line

        height: 1
        anchors.top: view.bottom
        anchors.left: chatbox.left; anchors.right: chatbox.right
        anchors.leftMargin: parent.margins; anchors.rightMargin: parent.margins

        color: "white";
        opacity: 0.3
    }

    TextInput {
        id: input

        anchors.top: line.bottom
        anchors.bottom: chatbox.bottom
        anchors.left: chatbox.left; anchors.right: chatbox.right
        anchors.margins: parent.margins

        color: chatbox.textColor
        smooth: true
        font.family: "DejaVu Sans"
        font.pointSize: 9

        onAccepted: {
            chatbox.newText();
        }
    }
}