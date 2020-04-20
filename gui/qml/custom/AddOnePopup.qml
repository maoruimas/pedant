import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

Popup {
    id: root
    function openAndUpdate() {
        var time = new Date()
        label.text = backend.isRunning ?
                    qsTr("End") : qsTr("Start")
        year.value = time.getFullYear()
        month.value = time.getMonth() + 1
        day.value = time.getDate()
        hour.value = time.getHours()
        minute.value = time.getMinutes()
        open()
    }
    function daysOfMonth(y, m) {
        var d = new Date(y, m, 0)
        return d.getDate()
    }

    anchors.centerIn: parent
    modal: true
    bottomPadding: 0
    Column {
        spacing: 5
        Label {
            id: label
        }
        Row {
            spacing: 2
            DragInput {
                id: year
                min: 1970; max: 2100
                onValueChanged: day.max = daysOfMonth(year.value, month.value)
            }
            Label { anchors.verticalCenter: parent.verticalCenter; text: "-" }
            DragInput {
                id: month
                min: 1; max: 12
                onValueChanged: day.max = daysOfMonth(year.value, month.value)
            }
            Label { anchors.verticalCenter: parent.verticalCenter; text: "-" }
            DragInput {
                id: day
                min: 1; max: 31
            }
            Label { anchors.verticalCenter: parent.verticalCenter; text: " " }
            DragInput {
                id: hour
                min: 0; max: 23
            }
            Label { anchors.verticalCenter: parent.verticalCenter; text: ":" }
            DragInput {
                id: minute
                min: 0; max: 59
            }
        }
        Button {
            anchors.right: parent.right
            text: "Confirm"
            flat: true
            Material.foreground: Material.Blue
            onClicked: {
                var time = new Date(year.value, month.value-1, day.value, hour.value, minute.value)
                var mTime = Math.floor(time.getTime() / 60000)
                root.close()
                logic.addOne(mTime)
            }
        }
    }
}
