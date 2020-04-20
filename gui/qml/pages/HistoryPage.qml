import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12
import "../custom"

MyPage {
    rightBarItem: ToolButton {
        text: "\uE810"
        font.family: "fontello"
        onClicked: addPopup.openAndUpdate(true, 0, backend.last())
    }

    AddPopup {
        id: addPopup
    }

    ColumnLayout {
        anchors.fill: parent
        TabBar {
            id: tabBar
            Layout.fillWidth: true
            TabButton {
                text: qsTr("Statistics")
            }
            TabButton {
                text: qsTr("Records")
            }
        }
        SwipeView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            currentIndex: tabBar.currentIndex
            interactive: false
            ListView {
                clip: true
                model: backend.statisticsModel
                delegate: RowLayout {
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width - 40
                    height: model.isDay ? dp(30) : dp(20)
                    Label {
                        Layout.alignment: Qt.AlignLeft
                        text: model.when
                        font.pixelSize: model.isDay ? dp(20) : dp(15)
                        color: model.isDay ? "white" : "#90CAF9"
                    }
                    Label {
                        Layout.alignment: Qt.AlignRight
                        text: lengthString(model.length)
                        font.pixelSize: model.isDay ? dp(20) : dp(15)
                        color: model.isDay ? "white" : "#90CAF9"
                    }
                }

                ScrollBar.vertical: ScrollBar {}
            }
            ListView {
                clip: true
                model: backend.recordsModel
                delegate: SwipeDelegate {
                    id: swipeDelegate
                    clip: true
                    width: parent.width
                    height: implicitContentHeight + topPadding + bottomPadding
                    text: timeString(model.from)+' - '+(model.to ? timeString(model.to) : '')
                    contentItem: Row {
                        Label {
                            width: (parent.width-sp.width)/2
                            text: timeString(model.from)
                            horizontalAlignment: Label.AlignHCenter
                        }
                        Label {
                            id: sp
                            text: '~'
                        }
                        Label {
                            width: (parent.width-sp.width)/2
                            text: model.to ? timeString(model.to) : ''
                            horizontalAlignment: Label.AlignHCenter
                        }
                    }
                    onPressAndHold: {
                        if(model.to)
                            addPopup.openAndUpdate(false, index, model.from, model.to)
                        else
                            addPopup.openAndUpdate(false, index, model.from, undefined, false)
                    }
                    ListView.onRemove: SequentialAnimation {
                        PropertyAction {
                            target: swipeDelegate
                            property: "ListView.delayRemove"
                            value: true
                        }
                        NumberAnimation {
                            target: swipeDelegate
                            property: "height"
                            to: 0
                            easing.type: Easing.InOutQuad
                        }
                        PropertyAction {
                            target: swipeDelegate
                            property: "ListView.delayRemove"
                            value: false
                        }
                    }
                    swipe.right: Label {
                        id: deleteLabel
                        anchors.right: parent.right
                        width: dp(50)
                        height: parent.height
                        text: "\uF1F8"
                        font.family: "fontello"
                        color: "white"
                        verticalAlignment: Label.AlignVCenter
                        horizontalAlignment: Label.AlignHCenter
                        SwipeDelegate.onClicked: logic.remove(index)
                        background: Rectangle { color: deleteLabel.SwipeDelegate.pressed ? Qt.lighter("red") : "red" }
                    }
                }
                ScrollBar.vertical: ScrollBar {}
            }
        }
    }
}
