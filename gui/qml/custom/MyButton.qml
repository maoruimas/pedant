import QtQuick 2.12
import QtQuick.Controls 2.12

Label {
    property color bgColor
    signal clicked()

    horizontalAlignment: "AlignHCenter"
    verticalAlignment: "AlignVCenter"
    background: Rectangle {
        color: mouseArea.containsPress ?
                   Qt.lighter(bgColor, 1.2) : bgColor
    }

    MouseArea {
        id: mouseArea
        anchors.fill: background
        onClicked: parent.clicked()
    }
}
