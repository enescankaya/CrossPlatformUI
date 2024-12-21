pragma ComponentBehavior: Bound
import QtQuick 2.15

Item {
    id: batteryStatus
    width: 55
    height: 25
    property int batteryLevel: 30
    property bool charging: false
    property bool deepCharge: false
    property bool blinkState: false  // Ana component'te tanımlandı
    function setValue(value) {
        // Değerin 0 ile 100 arasında kalmasını sağlamak
        batteryStatus.batteryLevel = Math.min(Math.max(value, 0), 100);
        batteryStatus.charging = batteryStatus.batteryLevel > 50;
        batteryStatus.deepCharge = batteryStatus.batteryLevel < 10;
        batteryCanvas.requestPaint();
    }
    Canvas {
        id: batteryCanvas
        anchors.centerIn: parent
        width: 48
        height: 20

        onPaint: {
            var ctx = getContext("2d");
            ctx.reset();

            // Ana batarya gövdesi
            ctx.beginPath();
            ctx.roundedRect(3, 0, width - 6, height, 5, 5);
            ctx.strokeStyle = (batteryStatus.batteryLevel < 5 && batteryStatus.blinkState) ? "#FF4444" : "#FFFFFF";
            ctx.lineWidth = 2;
            ctx.stroke();

            // Batarya çıkıntısı
            ctx.beginPath();
            ctx.moveTo(width - 3, height/3);
            ctx.lineTo(width + 3, height/3);
            ctx.lineTo(width + 3, height*2/3);
            ctx.lineTo(width - 3, height*2/3);
            ctx.closePath();
            ctx.fillStyle = (batteryStatus.batteryLevel < 5 && batteryStatus.blinkState) ? "#FF4444" : "#FFFFFF";
            ctx.fill();

            // Batarya doluluk seviyesi
            var fillWidth = (width - 8) * (batteryStatus.batteryLevel / 100);
            ctx.fillStyle = batteryStatus.deepCharge ? "#FD3C3C" :
                          batteryStatus.charging ? "#21DF56" : "#E46B0E";
            ctx.beginPath();
            ctx.roundedRect(5, 3, fillWidth, height - 6, 3, 3);
            ctx.fill();
        }


        Text {
            anchors.centerIn: parent
            text: batteryStatus.batteryLevel + "%"
            color: "white"
            font.family:"#9A9EB1"
            font.pixelSize: 9
            font.bold: true
        }

        MouseArea {
            anchors.fill: parent

            onWheel: function(event) {
                handleWheelEvent(event);
            }

            function handleWheelEvent(wheel) {
                if (wheel.angleDelta.y > 0) {
                    batteryStatus.batteryLevel = Math.min(batteryStatus.batteryLevel + 1, 100);
                } else {
                    batteryStatus.batteryLevel = Math.max(batteryStatus.batteryLevel - 1, 0);
                }

                batteryStatus.charging = batteryStatus.batteryLevel > 50;
                batteryStatus.deepCharge = batteryStatus.batteryLevel < 10;

                batteryCanvas.requestPaint();
            }
        }


    }

    Timer {
        interval: 500
        running: batteryStatus.batteryLevel < 5
        repeat: true
        onTriggered: {
            batteryStatus.blinkState = !batteryStatus.blinkState;  // batteryStatus id'si ile nitelendirildi
            batteryCanvas.requestPaint();
        }
    }
}
