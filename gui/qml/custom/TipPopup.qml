import QtQuick 2.12
import QtQuick.Controls 2.12

Popup {
    id: root
    property bool isQuit: false
    function show(msg, color) {
        tipText.text = msg
        tipBg.color = color
        open()
        tipTimer.start()
    }

    anchors.centerIn: parent
    padding: dp(10)
    contentWidth: tipText.width
    contentHeight: tipText.height
    closePolicy: Popup.NoAutoClose
    background: Rectangle {
        id: tipBg
        radius: dp(5)
        opacity: 0.8
    }
    Label {
        id: tipText
        anchors.centerIn: parent
        color: "white"
        font.bold: true
        font.pixelSize: dp(20)
    }
    Timer {
        id: tipTimer
        interval: 1000
        onTriggered: {
            isQuit = false
            close()
        }
    }
}
