pragma ComponentBehavior: Bound

import QtQuick 2.0
Rectangle {
    id: root
    width: 300
    height: 150
    color: "transparent"
    property int instantHeading: 0
    function updateHeading(newHeading) {
        instantHeading = newHeading
    }
    // Compass (Centered)
    Canvas {
        id: compassCanvas
        anchors.fill: parent
        onPaint: {
            var ctx = getContext("2d")
            ctx.reset()
            var radius = Math.min(width, height) / 2
            var labels = ["0", "30", "60", "90", "120", "150", "180", "210", "240", "270", "300", "330"]
            var directions = ["N", "E", "S", "W"]
            // Set font sizes
            ctx.font = "7px Arial"
            // Compass labels
            for (var i = 0; i < labels.length; i++) {
                ctx.save()
                ctx.translate(width / 2, height / 2)
                ctx.rotate((30 * i - root.instantHeading) * Math.PI / 180)
                ctx.translate(-width / 2, -height / 2)
                ctx.textAlign = "center"
                ctx.fillStyle = "#21DF56"
                ctx.fillText(labels[i], width / 2, 15)
                ctx.restore()
            }
            // Compass directions
            for (var j = 0; j < directions.length; j++) {
                ctx.save()
                ctx.translate(width / 2, height / 2)
                ctx.rotate((90 * j - root.instantHeading) * Math.PI / 180)
                ctx.translate(-width / 2, -height / 2)
                ctx.font = "10px Arial"
                ctx.textAlign = "center"
                ctx.fillStyle = "#21DF56"
                ctx.fillText(directions[j], width / 2, 30)
                ctx.restore()
            }
            // Compass arrow
            ctx.beginPath()
            ctx.moveTo(width / 2, height / 2 - 10)
            ctx.lineTo(width / 2 - 5, height / 2)
            ctx.lineTo(width / 2 + 5, height / 2)
            ctx.closePath()
            ctx.fillStyle = "#21DF56"
            ctx.fill()

            // Heading text below arrow
            ctx.font = "8px Arial"
            ctx.textAlign = "center"
            ctx.fillStyle = "#21DF56"
            ctx.fillText(root.instantHeading + "°", width / 2+2, height / 2 + 10)
        }
    }
    Connections {
        target: root
        function onInstantHeadingChanged() {
            compassCanvas.requestPaint()
        }
    }
}
