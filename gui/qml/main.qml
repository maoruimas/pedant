import QtQuick.Window 2.12
import QtQuick 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls.Material 2.12
import Qt.labs.settings 1.1

import com.mas.mclient 1.0
import com.mas.pedant 1.0

import "pages"
import "custom"

ApplicationWindow {
    id: app
    width: 600
    height: 400
    visible: true
    title: qsTr("Padent")
    font { family: "Arial"; pixelSize: dp(20) }

    Material.theme: Material.Dark

    // global functions
    readonly property real pixelPerDp: Screen.pixelDensity * 25.4 / 160
    function dp(x) { return x * pixelPerDp }
    function lengthString(len) {
        var h = Math.floor(len / 60)
        var m = len % 60
        return (h === 0) ? m + qsTr("m") : h + qsTr("h") + m + qsTr("m")
    }
    function timeString(t) {
        var time = new Date(t*60000)
        var year = time.getFullYear()
        var month = intString(time.getMonth() + 1)
        var day = intString(time.getDate())
        var hour = intString(time.getHours())
        var minute = intString(time.getMinutes())
        return year+'/'+month+'/'+day+' '+hour+':'+minute
    }
    function intString(n) {
        return n < 10 ? '0'+n : n
    }

    // settings
    Settings {
        fileName: "settings.ini"
        property alias hostName: client.hostName
        property alias port: client.port
        property alias username: backend.username
        property alias password: backend.password
        property alias task: app.task
    }
    property string task
    property int taskId

    MyListModel {
        id: pdt
    }

    // ui
    StackView {
        id: stack
        anchors.fill: parent
        initialItem: MainPage {}
    }
    Component {
        id: historyPage
        HistoryPage {}
    }
    LandingPage {
        id: landingPage
    }

    Logic { id: logic }
    TipPopup {
        id: tip
    }
    Popup {
        id: busy
        anchors.centerIn: parent
        closePolicy: Popup.NoAutoClose
        dim: false
        modal: true
        BusyIndicator {
            running: true
        }
    }
    MClient {
        id: client
    }
    Backend {
        id: backend
        onError: tip.show(error, "red")
    }
}
