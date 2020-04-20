import QtQuick 2.12

import com.mas.mclient 1.0

Item {
    function getPdt() {
        busy.open()
        client.finished.connect(_.getPdtFinished)
        client.download(_.remotePdtPath(), _.localPdtPath())
    }
    function signUp() {
        busy.open()
        client.finished.connect(_.checkExistenceFinished)
        client.existsFile(_.remotePdtPath())
    }
    function getTask() {
        busy.open()
        client.finished.connect(_.getTaskFinished)
        client.download(_.remoteTaskPath(), backend.file)
    }
    function newTask(name) {
        busy.open()
        // add to pdt
        var file = _.randomFile()
        pdt.append({"name": name, "file": file})
        // prepare
        if (!backend.encrypt(_.localPdtPath(), pdt.toJson())) return
        task = name
        taskId = pdt.count - 1
        backend.clear()
        backend.file = file
        if (!backend.createFile()) return
        // upload pdt first
        client.finished.connect(_.newTask2)
        client.upload(_.localPdtPath(), _.remotePdtPath())
    }
    function verifyName(name) {
        if (name) {
            var exists = false
            var n = pdt.count
            for (var i = 0; i < n; ++i) {
                if (name === pdt.get(i).name) {
                    exists = true
                }
            }
            if (exists) {
                tip.show(qsTr("Task exists"))
                return false
            } else {
                return true
            }
        } else {
            tip.show(qsTr("Task name should be nonempty"), "red")
            return false
        }
    }
    function renameTask(index, name) {
        busy.open()
        // prepare
        pdt.get(index).name = name
        if (!backend.encrypt(_.localPdtPath(), pdt.toJson())) return
        if (index === taskId) {
            task = name
        }
        // upload pdt
        client.finished.connect(_.renameTask2)
        client.upload(_.localPdtPath(), _.remotePdtPath())
    }
    function removeTask(index) {
        busy.open()
        // prepare
        var remoteFileToRemove = "pedant/" + backend.username + "/" + pdt.get(index).file
        if (!backend.removeFileIfExists(pdt.get(index).file)) return

        pdt.remove(index)
        if (!backend.encrypt(_.localPdtPath(), pdt.toJson())) return
        _.removeTaskOpenNew = false
        if (index === taskId) {
            if (pdt.count) {
                task = pdt.get(0).name
                taskId = 0
                backend.file = pdt.get(0).file
                _.removeTaskOpenNew = true
            } else {
                task = ""
                backend.clear()
            }
        }
        // remove task
        client.finished.connect(_.removeTask2)
        client.removeFile(remoteFileToRemove)
    }

    function toggle() {
        busy.open()
        // prepare
        if (!backend.toggle()) return
        // upload task
        client.finished.connect(_.putTaskFinished)
        client.upload(backend.file, _.remoteTaskPath())
    }
    function add(from, to) {
        busy.open()
        // prepare
        if (!backend.add(from, to)) return
        // upload task
        client.finished.connect(_.putTaskFinished)
        client.upload(backend.file, _.remoteTaskPath())
    }
    function addOne(time) {
        busy.open()
        // prepare
        if (!backend.addOne(time)) return
        // upload task
        client.finished.connect(_.putTaskFinished)
        client.upload(backend.file, _.remoteTaskPath())
    }
    function remove(row) {
        busy.open()
        // prepare
        if (!backend.remove(row)) return
        // upload task
        client.finished.connect(_.putTaskFinished)
        client.upload(backend.file, _.remoteTaskPath())
    }
    function edit(row, from, to) {
        busy.open()
        // prepare
        if (!backend.edit(row, from, to)) return
        // upload task
        client.finished.connect(_.putTaskFinished)
        client.upload(backend.file, _.remoteTaskPath())
    }

    // private data
    QtObject {
        id: _
        property bool removeTaskOpenNew

        function localPdtPath() {
            return backend.username + ".pdt"
        }
        function remotePdtPath() {
            return "pedant/" + backend.username + "/.pdt"
        }
        function remoteTaskPath() {
            return "pedant/" + backend.username + "/" + backend.file
        }

        function clientErrorString(result) {
            switch (result) {
            case MClient.Negative:
                return qsTr("Negative")
            case MClient.UnknownError:
                return qsTr("Unknown error")
            case MClient.ConnectionFailure:
                return qsTr("Connection failure")
            case MClient.TimeOut:
                return qsTr("Time out")
            case MClient.LocalError:
                return qsTr("Local file error")
            }
        }
        function searchPdt() {
            var n = pdt.count
            for (var i = 0; i < n; ++i) {
                if (pdt.get(i).name === task) {
                    taskId = i
                    backend.file = pdt.get(i).file
                    return
                }
            }
            if (n) {
                task = pdt.get(0).name
                taskId = 0
                backend.file = pdt.get(0).file
            } else {
                task = ""
                backend.file = ""
            }
        }
        function randomFile() {
            var n = pdt.count
            while (1) {
                var str = backend.randomString()
                var exist = false
                for (var i = 0; i < n; ++i) {
                    if (pdt.get(i).file === str) {
                        exist = true
                    }
                }
                if (!exist) {
                    return str
                }
            }
        }

        function getPdtFinished(result) {
            client.finished.disconnect(getPdtFinished)
            if (result === MClient.Positive) {
                // pdt
                var text = backend.decrypt(localPdtPath())
                try {
                    pdt.init(JSON.parse(text))
                } catch(e) {
                    busy.close()
                    tip.show(qsTr("Wrong Password"), "red")
                    landingPage.load()
                    return
                }
                // landingPage
                landingPage.close()
                // task
                searchPdt()
                if (backend.file) {
                    client.finished.connect(getTaskFinished)
                    client.download(remoteTaskPath(), backend.file)
                } else {
                    busy.close()
                }
            } else {
                busy.close();
                tip.show(clientErrorString(result), "red")
            }
        }
        function putPdtFinished(result) {
            busy.close()
            client.finished.disconnect(putPdtFinished)
            if (result === MClient.Positive) {
                landingPage.close()
            } else {
                tip.show(clientErrorString(result), "red")
            }
        }
        function getTaskFinished(result) {
            busy.close()
            client.finished.disconnect(getTaskFinished)
            if (result === MClient.Positive) {
                backend.readFile()
                backend.analyze()
            } else {
                tip.show(clientErrorString(result), "red")
            }
        }
        function putTaskFinished(result) {
            busy.close()
            client.finished.disconnect(putTaskFinished)
            if (result !== MClient.Positive) {
                tip.show(clientErrorString(result), "red")
            }
        }

        function newTask2(result) {
            client.finished.disconnect(newTask2)
            if (result === MClient.Positive) {
                client.finished.connect(newTask3)
                client.upload(backend.file, remoteTaskPath())
            } else {
                busy.close()
                tip.show(clientErrorString(result), "red")
            }
        }
        function newTask3(result) {
            client.finished.disconnect(newTask3)
            busy.close()
            if (result === MClient.Positive) {
                tip.show(qsTr("Task created"), "green")
            } else {
                tip.show(clientErrorString(result), "red")
            }
        }

        function renameTask2(result) {
            busy.close()
            client.finished.disconnect(renameTask2)
            if (result === MClient.Positive) {
                tip.show(qsTr("Renamed"), "green")
            } else {
                tip.show(clientErrorString(result), "red")
            }
        }

        function removeTask2(result) {
            client.finished.disconnect(removeTask2)
            if (result === MClient.Positive) {
                client.finished.connect(removeTask3)
                client.upload(localPdtPath(), remotePdtPath())
            } else {
                busy.close()
                tip.show(clientErrorString(result), "red")
            }
        }
        function removeTask3(result) {
            client.finished.disconnect(removeTask3)
            if (result === MClient.Positive) {
                if (removeTaskOpenNew) {
                    client.finished.connect(getPdtFinished)
                    client.download(_.remoteTaskPath(), backend.file)
                } else {
                    busy.close()
                    tip.show(qsTr("Removed"), "green")
                }
            } else {
                busy.close()
                tip.show(clientErrorString(result), "red")
            }
        }

        function checkExistenceFinished(result) {
            client.finished.disconnect(checkExistenceFinished)
            if (result === MClient.Negative) {
                // pdt
				task = ""
				backend.clear()
                pdt.clear()
                if (!backend.encrypt(localPdtPath(), "[]")) {
                    busy.close()
                    return
                }
                client.finished.connect(putPdtFinished)
                client.upload(localPdtPath(), remotePdtPath())
            } else if (result === MClient.Positive) {
                busy.close()
                tip.show(qsTr("User exists"), "red")
            } else {
                busy.close()
                tip.show(clientErrorString(result), "red")
            }
        }
    }
}
