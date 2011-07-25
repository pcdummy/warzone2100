import QtQuick 1.0

 Item {
     id: slider; width: 400; height: 16
     // value is read/write.
     property int value
     onValueChanged: { handle.x = 2 + (value - minimum) * slider.xMax / (maximum - minimum); }
     property real maximum: 100
     property real minimum: 1
     property int xMax: slider.width - handle.width-2
     property int xMin: 2

     Rectangle {
         anchors.fill: parent
         border.color: "white"; border.width: 0; radius: 1
         gradient: Gradient {
             GradientStop { position: 0.0; color: "#66343434" }
             GradientStop { position: 1.0; color: "#66000000" }
         }
     }

     Rectangle {
         id: handle; smooth: true
         //x: 2 + (value - minimum) * slider.xMax / (maximum - minimum)

         x: slider.width / 2 - handle.width / 2;

         y: 2; width: 12; height: slider.height-4; radius: 4

         MouseArea {
             anchors.fill: parent; drag.target: parent
             drag.axis: Drag.XAxis; drag.minimumX: slider.xMin; drag.maximumX: slider.xMax
             onPositionChanged: { value = (maximum - minimum) * (handle.x-slider.xMin) / (slider.xMax - slider.xMin) + minimum; }
         }
     }
 }