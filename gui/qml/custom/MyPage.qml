import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Page {
    signal backButtonClicked
    property alias leftBarItem: leftBarContainer.contentItem
    property alias rightBarItem: rightBarContainer.contentItem
    function goBack() {
        if(stack.depth > 1)
            stack.pop()
        else if(stack.depth === 1){
            if(tip.isQuit)
                Qt.quit()
            else{
                tip.isQuit = true
                tip.show(qsTr("Click again to quit"), "green")
            }
        }
        backButtonClicked()
    }
    header: ToolBar {
        background: Rectangle {color: "transparent"}
        RowLayout {
            anchors.fill: parent
            Container {
                id: leftBarContainer
                contentItem: ToolButton {
                    text: "\uE857"
                    font.family: "fontello"
                    onClicked: goBack()
                }
            }
            Label {
                text: title
                font { pixelSize: dp(22); bold: true }
                Layout.fillWidth: true
                elide: Text.ElideRight
            }
            Container { id: rightBarContainer }
        }
    }
    Keys.onBackPressed: goBack()
}
