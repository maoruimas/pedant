import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Controls.Material 2.12

Popup {
    id: root
    property bool isAdd
    property int row
    function openAndUpdate(_isAdd, _row, from, to, toEnabled) {
        isAdd = _isAdd
        row = _row
        var start = new Date(from*60000)
        var end = (to === undefined) ? new Date() : new Date(to*60000)
        eRow.enabled = (toEnabled === undefined) ? true : toEnabled
        // start row
        sYear.value = start.getFullYear()
        sMonth.value = start.getMonth() + 1
        sDate.value = start.getDate()
        sHour.value = start.getHours()
        sMinute.value = start.getMinutes()
        // end row
        eYear.value = end.getFullYear()
        eMonth.value = end.getMonth() + 1
        eDate.value = end.getDate()
        eHour.value = end.getHours()
        eMinute.value = end.getMinutes()
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
        Label { text: "Start" }
        Row {
            id: sRow
            spacing: 2
            DragInput {
                id: sYear
                min: 1970; max: 2100
                onValueChanged: sDate.max = daysOfMonth(sYear.value, sMonth.value)
            }
            Label { anchors.verticalCenter: parent.verticalCenter; text: "-" }
            DragInput {
                id: sMonth
                min: 1; max: 12
                onValueChanged: sDate.max = daysOfMonth(sYear.value, sMonth.value)
            }
            Label { anchors.verticalCenter: parent.verticalCenter; text: "-" }
            DragInput {
                id: sDate
                min: 1; max: 31
            }
            Label { anchors.verticalCenter: parent.verticalCenter; text: " " }
            DragInput {
                id: sHour
                min: 0; max: 23
            }
            Label { anchors.verticalCenter: parent.verticalCenter; text: ":" }
            DragInput {
                id: sMinute
                min: 0; max: 59
            }
        }
        Label { text: "End" }
        Row {
            id: eRow
            spacing: 2
            DragInput {
                id: eYear
                min: 1970; max: 2100
                onValueChanged: eDate.max = daysOfMonth(eYear.value, eMonth.value)
            }
            Label { anchors.verticalCenter: parent.verticalCenter; text: "-" }
            DragInput {
                id: eMonth
                min: 1; max: 12
                onValueChanged: eDate.max = daysOfMonth(eYear.value, eMonth.value)
            }
            Label { anchors.verticalCenter: parent.verticalCenter; text: "-" }
            DragInput {
                id: eDate
                min: 1; max: 31
            }
            Label { anchors.verticalCenter: parent.verticalCenter; text: " " }
            DragInput {
                id: eHour
                min: 0; max: 23
            }
            Label { anchors.verticalCenter: parent.verticalCenter; text: ":" }
            DragInput {
                id: eMinute
                min: 0; max: 59
            }
        }
        Button {
            anchors.right: parent.right
            text: "Confirm"
            flat: true
            Material.foreground: Material.Blue
            onClicked: {
                var start = new Date(sYear.value, sMonth.value-1, sDate.value, sHour.value, sMinute.value)
                var end = new Date(eYear.value, eMonth.value-1, eDate.value, eHour.value, eMinute.value)
                var startTime = Math.floor(start.getTime() / 60000)
                var endTime = Math.floor(end.getTime() / 60000)
                if (startTime >= endTime) {
                    tip.show(qsTr("Invalid"), "red")
                } else {
                    if (isAdd)
                        logic.add(startTime, endTime)
                    else
                        logic.edit(root.row, startTime, endTime)
                    root.close()
                }
            }
        }
    }
}
