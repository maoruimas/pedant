import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12
import "../custom"

Page {
    width: app.width; height: app.height

    function load() {
        visible = true
        pwInput.clear()
    }
    function close() {
        visible = false
    }

    Component.onCompleted: {
        if (backend.username && backend.password) {
            logic.getPdt()
        }
    }

    Column {
        id: bodyColumnLayout
        anchors.centerIn: parent
        width: Math.min(dp(300), parent.width - dp(60))
        spacing: dp(5)
        MyTextField {
            id: unInput
            width: parent.width
            icon: "\uE81F"
            text: backend.username
            showClearButton: true
            placeholderText: qsTr("Username")
        }
        MyTextField {
            id: pwInput
            width: parent.width
            icon: "\uE80b"
            passwordMode: true
            placeholderText: qsTr("Password")
        }
        Button {
            id: signInButton
            anchors.horizontalCenter: parent.horizontalCenter
            text: qsTr("Sign in")
            onClicked: {
                backend.username = unInput.text
                backend.password = pwInput.text
                logic.getPdt()
            }
        }
        Button {
            anchors.horizontalCenter: parent.horizontalCenter
            flat: true
            text: qsTr("Sign up")
            onClicked: {
                backend.username = unInput.text
                backend.password = pwInput.text
                logic.signUp()
            }
        }
    }
    RoundButton {
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        flat: true
        text: "\uE839"
        font.family: "fontello"
        onClicked: settingsPopup.open()
    }

    Popup {
        id: settingsPopup
        anchors.centerIn: parent
        contentWidth: body.width
        leftPadding: 20; rightPadding: 20; topPadding: 20; bottomPadding: 0
        modal: true
        closePolicy: Popup.NoAutoClose
        Column {
            id: body
            width: app.width-80
            GridLayout {
                width: parent.width
                columns: 2; rows: 2
                Label { text: qsTr("IP") }
                MyTextField {
                    id: ipInput
                    Layout.fillWidth: true
                    text: client.hostName
                    showClearButton: true
                }
                Label { text: qsTr("Port") }
                MyTextField {
                    id: portInput
                    Layout.fillWidth: true
                    text: client.port
                    showClearButton: true
                    validator: IntValidator {bottom: 1; top: 65536}
                }
            }
            Row {
                anchors.right: parent.right
                spacing: 5
                RoundButton {
                    text: "\uF29C" // help button
                    font.family: "fontello"
                    flat: true
                    onClicked: {
                        settingsPopup.close()
                        ipInput.text = client.hostName
                        portInput.text = client.port
                        infoPopup.open()
                    }
                }
                Button {
                    text: qsTr("Cancel")
                    flat: true
                    Material.foreground: Material.Grey
                    onClicked: {
                        settingsPopup.close()
                        ipInput.text = client.hostName
                        portInput.text = client.port
                    }
                }
                Button {
                    text: qsTr("Confirm")
                    flat: true
                    Material.foreground: Material.Blue
                    onClicked: {
                        client.hostName = ipInput.text
                        client.port = parseInt(portInput.text)
                        settingsPopup.close()
                    }
                }
            }
        }
    }
    Popup {
        id: infoPopup
        anchors.centerIn: parent
        padding: 20
        modal: true
        Column {
            width: app.width-80
            Label {
                width: parent.width
                text: qsTr("Passworld is a password manager applying AES algorithm. You need to set up a cloud server before using it. For more details, visit")
                wrapMode: Label.WrapAnywhere
            }
            Label {
                width: parent.width
                wrapMode: Label.WrapAnywhere
                text: "<a href=\"https://gitee.com/maoruimas/passworld\">https://gitee.com/maoruimas/passworld</a>"
                onLinkActivated: Qt.openUrlExternally("https://gitee.com/maoruimas/passworld")
            }
            Label { text: qsTr("or") }
            Label {
                width: parent.width
                wrapMode: Label.WrapAnywhere
                text: "<a href=\"https://github.com/maoruimas/passworld\">https://github.com/maoruimas/passworld</a>"
                onLinkActivated: Qt.openUrlExternally("https://github.com/maoruimas/passworld")
            }
        }
    }
}
