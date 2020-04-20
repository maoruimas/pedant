import QtQuick 2.12
import QtQuick.Controls 2.12

Rectangle {
    id: root
    property int min:0
    property int max: 10
    property int value: 0
    property int initY
    implicitHeight: txt.height + 10
    implicitWidth: txt.width + 10
    color: "gray"
    radius: 5
    onMinChanged: value = Math.max(value, min)
    onMaxChanged: value = Math.min(value, max)
    Label {
        id: txt
        x: 5
        y: 5
        text: value
        Behavior on y {
            NumberAnimation { duration: 100 }
        }
    }
    MouseArea {
        anchors.fill: parent
        property int tmp
        onPressed: {
            initY = mouse.y
            tmp = value
        }
        onPositionChanged: {
            var delta = Math.floor((initY - mouse.y)/10)
            if (delta === 0)
                txt.y = 5
            else if (delta > 0)
                txt.y = 2
            else
                txt.y = 8
            value = Math.max(min, Math.min(max, tmp + delta))
        }
        onReleased: txt.y = 5
    }
}
