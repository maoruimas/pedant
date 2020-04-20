import QtQuick 2.0

Item {
    property real startAngle: -Math.PI/2
    property real period: 60000
    property int frameNumber: 60 // frameNumber is recommended to be lower than period/50
    property int currentFrame: 1 // 1 - frameNumber
    property real adjustTime: 1000
    property bool anticlockwise: true
    property color strokeStyle: "#00B050"
    property real lineWidth: 10
    property real diameter: 100
    function start() {
        if(currentFrame >= 1 && currentFrame <= frameNumber && frameNumber > 1){
            currentFrame += Math.round(adjustTime/timer.interval) // take 1s to start
            currentFrame = currentFrame % frameNumber + 1
            var targetAngle = (2*currentFrame-1)/frameNumber*Math.PI+startAngle
            var currentAngle = canvas.endAngle
            startTimer.deltaAngle = (targetAngle-currentAngle)*startTimer.interval/adjustTime
            startTimer.start()
        }
    }
    function pause() { timer.stop() }
    function stop() {
        var targetAngle = anticlockwise ? startAngle+0.001 : startAngle+2*Math.PI-0.001
        var currentAngle = canvas.endAngle
        stopTimer.deltaAngle = (targetAngle-currentAngle)*stopTimer.interval/adjustTime
        timer.stop()
        stopTimer.start()
    }
    width: diameter; height: diameter
    Canvas {
        id: canvas
        width: diameter; height: diameter
        property real endAngle: startAngle + 0.001
        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
            ctx.strokeStyle = strokeStyle
            ctx.lineWidth = lineWidth
            ctx.lineCap = "round"
            ctx.beginPath()
            ctx.arc(width/2, height/2, width/2-lineWidth/2, startAngle, endAngle, anticlockwise)
            ctx.stroke()
        }
    }
    Timer {
        id: timer
        interval: Math.round(period/frameNumber)
        repeat: true
        onTriggered: {
            currentFrame++
            if(currentFrame > frameNumber){
                currentFrame = 1
                anticlockwise = !anticlockwise
            }
            canvas.endAngle = (2*currentFrame-1)/frameNumber*Math.PI+startAngle
            canvas.requestPaint()
        }
    }
    Timer {
        id: startTimer
        interval: 50
        repeat: true
        property int cnt: 0
        property real deltaAngle
        onTriggered: {
            cnt += interval
            if(cnt >= adjustTime){
                cnt = 0
                stop()
                timer.start()
            }
            canvas.endAngle += deltaAngle
            canvas.requestPaint()
        }
    }
    Timer {
        id: stopTimer
        interval: 50
        repeat: true
        property int cnt: 0
        property real deltaAngle
        onTriggered: {
            cnt += interval
            if(cnt >= adjustTime){
                cnt = 0
                stop()
            }
            canvas.endAngle += deltaAngle
            canvas.requestPaint()
        }
    }
}
