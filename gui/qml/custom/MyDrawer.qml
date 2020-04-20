import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12

Drawer {
    id: root
    height: parent.height
    width: Math.min(200, parent.width * 0.9)
    Material.background: "#303030"

    property int selected
    property bool isNew

    Menu {
        id: menu
        anchors.centerIn: overlay
        MenuItem {
            text: qsTr("Delete")
            onTriggered: {
                menu.close()
                logic.removeTask(selected)
            }
        }
        MenuItem {
            text: qsTr("Rename")
            onTriggered: {
                menu.close()
                namePopup.myOpen(false)
            }
        }
        MenuItem {
            text: qsTr("Cancel")
        }
    }

    Popup {
        id: namePopup
        anchors.centerIn: overlay
        bottomPadding: 0

        function myOpen(b) {
            isNew = b
            if (!isNew) {
                nameInput.text = pdt.get(selected).name
            } else {
                nameInput.text = ""
            }
            open()
            //nameInput.forceActiveFocus()
        }

        ColumnLayout {
            MyTextField {
                id: nameInput
                Layout.fillWidth: true
                showClearButton: true
                focus: true
            }
            Row {
                Button {
                    flat: true
                    text: qsTr("Cancel")
                    onClicked: namePopup.close()
                }
                Button {
                    flat: true
                    text: qsTr("Confirm")
                    Material.foreground: Material.Blue
                    onClicked: {
                        if (isNew) {
                            if (logic.verifyName(nameInput.text)) {
                                namePopup.close()
                                logic.newTask(nameInput.text)
                            }
                        } else {
                            if (nameInput.text === pdt.get(selected).name) {
                                tip.show(qsTr("Name not changed"), "red")
                            } else if (logic.verifyName(nameInput.text)) {
                                namePopup.close()
                                logic.renameTask(selected, nameInput.text)
                            }
                        }
                    }
                }
            }
        }
    }

    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        Label {
            Layout.fillWidth: true
            text: "Tasks"
            horizontalAlignment: "AlignHCenter"
            verticalAlignment: "AlignVCenter"
        }
        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            model: pdt
            delegate: SwipeDelegate {
                width: parent.width
                text: model.name
                highlighted: index === taskId
                onPressAndHold: {
                    selected = index
                    menu.open()
                }
                onClicked: {
                    root.close()
                    taskId = index
                    task = pdt.get(index).name
                    backend.file = pdt.get(index).file
                    logic.getTask()
                }
            }
            ScrollBar.vertical: ScrollBar {}
        }
        MyButton {
            Layout.preferredHeight: 40
            Layout.fillWidth: true
            bgColor: "skyblue"
            text: qsTr("New Task")
            onClicked: namePopup.myOpen(true)
        }
    }
}
