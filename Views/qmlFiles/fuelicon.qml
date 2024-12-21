pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

Item {
    id: fuelStatus
    width: 85
    height: 20  // Reduced height to 20

    property real fuelLevel: 0.34
    property bool lowFuel: fuelLevel <= 0.2
    property bool criticalFuel: fuelLevel <= 0.05
    property bool blinkState: false
    function setValue(value) {
        // Gelen değeri 0 ile 100 arasında sınırlıyoruz
        var clampedValue = Math.min(Math.max(value, 0), 100);
        fuelLevel = clampedValue / 100; // Yüzdeyi oran değerine çeviriyoruz
        dropCanvas.requestPaint(); // Yeniden çizim isteği gönderiyoruz
    }


    RowLayout {
        anchors.fill: parent
        anchors.margins: 0
        spacing: 0

        Item {
            Layout.preferredWidth: 60
            Layout.fillHeight: true

            Canvas {
                id: dropCanvas
                anchors.fill: parent
                onPaint: {
                    var ctx = getContext("2d");
                    ctx.reset();

                    // Authentic droplet shape: narrow at top, wide at bottom
                    ctx.beginPath();
                    var dropWidth = width * 0.7;  // Width of droplet
                    var dropHeight = height * 0.8;  // Height of droplet

                    // Top of the droplet (very narrow)
                    ctx.moveTo(width/2, (height - dropHeight)/2);

                    // Left curve of droplet - narrow to wide
                    ctx.bezierCurveTo(
                        width/2 - dropWidth/6, (height - dropHeight)/2 + dropHeight * 0.1,  // Left top control point
                        width/2 - dropWidth/2, (height - dropHeight)/2 + dropHeight * 0.7,  // Left bottom control point
                        width/2, height - (height - dropHeight)/2  // Bottom point
                    );

                    // Right curve of droplet - narrow to wide
                    ctx.bezierCurveTo(
                        width/2 + dropWidth/2, (height - dropHeight)/2 + dropHeight * 0.7,  // Right bottom control point
                        width/2 + dropWidth/6, (height - dropHeight)/2 + dropHeight * 0.1,  // Right top control point
                        width/2, (height - dropHeight)/2  // Back to top
                    );

                    ctx.strokeStyle = fuelStatus.criticalFuel ?
                        (fuelStatus.blinkState ? "#FF4444" : "#FFFFFF") : "#FFFFFF";
                    ctx.lineWidth = 2;
                    ctx.stroke();
                    ctx.clip();

                    // Fill with fuel level
                    var fillHeight = dropHeight * fuelStatus.fuelLevel;
                    var fillStartY = height - (height - dropHeight)/2 - fillHeight;

                    var gradient = ctx.createLinearGradient(0, fillStartY, 0, height - (height - dropHeight)/2);
                    gradient.addColorStop(0, fuelStatus.getColor(fuelStatus.fuelLevel));
                    gradient.addColorStop(1, fuelStatus.getColor(fuelStatus.fuelLevel));
                    ctx.fillStyle = gradient;
                    ctx.fillRect(width/2 - dropWidth/2, fillStartY, dropWidth, fillHeight);
                    // Wave effect
                    var time = new Date().getTime() / 300;
                    var waveHeight = fillHeight * 0.05;
                    ctx.beginPath();
                    for (var x = width/2 - dropWidth/2; x < width/2 + dropWidth/2; x++) {
                        var y = Math.sin((x - (width/2 - dropWidth/2)) * 0.8 + time * 2) * waveHeight +
                                Math.cos((x - (width/2 - dropWidth/2)) * 0.6 + time) * waveHeight * 0.5 +
                                fillStartY;
                        if (x === width/2 - dropWidth/2) {
                            ctx.moveTo(x, y);
                        } else {
                            ctx.lineTo(x, y);
                        }
                    }
                    ctx.lineTo(width/2 + dropWidth/2, height - (height - dropHeight)/2);
                    ctx.lineTo(width/2 - dropWidth/2, height - (height - dropHeight)/2);
                    ctx.closePath();
                    ctx.fillStyle = fuelStatus.getColor(fuelStatus.fuelLevel);
                    ctx.fill();
                }
            }
        }

        Text {
            Layout.alignment: Qt.AlignVCenter
            horizontalAlignment: Qt.AlignLeft
            text: "Fuel\n" + Math.round(fuelStatus.fuelLevel * 100) + "%"
            font.family: "#9A9EB1"
            font.bold: true
            color: fuelStatus.getTextColor(fuelStatus.fuelLevel)
            font.pixelSize: 10
            lineHeight: 1.2
            leftPadding: -6
        }
    }

    MouseArea {
        anchors.fill: parent

        onWheel: function(event) {
            handleWheelEvent(event);
        }

        function handleWheelEvent(wheel) {
            if (wheel.angleDelta.y > 0) {
                fuelStatus.fuelLevel = Math.min(fuelStatus.fuelLevel + 0.01, 1.0);
            } else {
                fuelStatus.fuelLevel = Math.max(fuelStatus.fuelLevel - 0.01, 0.0);
            }
            dropCanvas.requestPaint();
        }
    }



    Timer {
        interval: 16 // Approximately 60 FPS
        running: true
        repeat: true
        onTriggered: dropCanvas.requestPaint()
    }

    Timer {
        interval: 500 // 500ms blinking speed
        running: fuelStatus.criticalFuel
        repeat: true
        onTriggered: {
            fuelStatus.blinkState = !fuelStatus.blinkState
            dropCanvas.requestPaint()
        }
    }

    function getColor(level) {
        if (level <= 0.2) return "#FD3C3C"
        if (level <= 0.4) return "#E46B0E"
        return "#21DF56"
    }

    function getTextColor(level) {
        if (level <= 0.2) return "#FD3C3C"
        if (level <= 0.4) return "#E46B0E"
        return "#FFFFFF"
    }
}
