import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12
import "../custom"

MyPage {
    id: root

    title: task
    leftBarItem: ToolButton { // drawer button
        text: "\uF0C9"
        font.family: "fontello"
        onClicked: drawer.open()
    }
    rightBarItem: Row {
        ToolButton { // refresh button
            text: "\uE860"
            font.family: "fontello"
            flat: true
            onClicked: logic.getPdt()
        }
        ToolButton { // history button
            text: "\uE844"
            font.family: "fontello"
            flat: true
            onClicked: stack.push(historyPage)
        }
    }

    MyDrawer {
        id: drawer
    }

    AddOnePopup {
        id: addOnePopup
    }

    ToolButton { // big round button
        anchors.horizontalCenter: parent.horizontalCenter
        width: Math.min(parent.width-40, parent.height-80)
        height: width
        text: backend.isRunning ? lengthString(backend.count) : qsTr("Start")
        font.pixelSize: 50
        onClicked: logic.toggle()
        onPressAndHold: addOnePopup.openAndUpdate()
    }

    Row { // today, total, average
        height: 60
        width: parent.width
        anchors.bottom: parent.bottom
        Item {
            width: (parent.width-2)/3
            height: parent.height
            Column {
                anchors.centerIn: parent
                Label {
                    text: qsTr("Today")
                    font.pixelSize: 12
                    Material.foreground: Material.Grey
                }
                Label {
                    text: lengthString(backend.today)
                }
            }
        }
        Rectangle {
            width: 1
            height: parent.height/2
            anchors.verticalCenter: parent.verticalCenter
            color: "gray"
        }
        Item {
            width: (parent.width-2)/3
            height: parent.height
            Column {
                anchors.centerIn: parent
                Label {
                    text: qsTr("Total")
                    font.pixelSize: 12
                    Material.foreground: Material.Grey
                }
                Label {
                    text: lengthString(backend.total)
                    Material.foreground: Material.Orange
                }
            }
        }
        Rectangle {
            width: 1
            height: parent.height/2
            anchors.verticalCenter: parent.verticalCenter
            color: "gray"
        }
        Item {
            width: (parent.width-2)/3
            height: parent.height
            Column {
                anchors.centerIn: parent
                Label {
                    text: qsTr("Average")
                    font.pixelSize: 12
                    Material.foreground: Material.Grey
                }
                Label {
                    text: lengthString(backend.average)
                    Material.foreground: Material.Green
                }
            }
        }
    }

    Timer {
        id: refresher
        interval: 60000
        running: backend.isRunning
        repeat: true
        onTriggered: backend.analyze()
    }

    Pane {
        visible: pdt.count === 0
        anchors.fill: parent
        Label {
            anchors.fill: parent
            text: qsTr("No task now. Open drawer to create one.")
            horizontalAlignment: "AlignHCenter"
            verticalAlignment: "AlignVCenter"
            wrapMode: "Wrap"
        }
    }
}
