pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Layouts 1.15

Item {
    id: fuelStatus
    width: 85
    height: 20

    // Basic properties
    property real fuelLevel: 0.34
    property bool lowFuel: fuelLevel <= 0.2
    property bool criticalFuel: fuelLevel <= 0.05
    property bool blinkState: false
    property real rollAngle: 0

    // Enhanced fluid physics properties
    property real fluidOffset: 0
    property real fluidVelocity: 0
    property real dampingFactor: 0.95
    property real springConstant: 0.15
    property real maxTiltAngle: 45

    // Advanced wave simulation
    property var wavePoints: Array(12).fill(0)
    property var waveVelocities: Array(12).fill(0)
    property real waveSpringConstant: 0.025
    property real waveDamping: 0.985
    property real waveSpread: 0.18

    // Ripple effect properties
    property var ripples: []
    property int maxRipples: 3

    Timer {
        id: physicsTimer
        interval: 16
        running: true
        repeat: true
        onTriggered: {
            parent.updateFluidPhysics()
            parent.updateWavePhysics()
            parent.updateRipples()
            dropCanvas.requestPaint()
        }
    }

    // Ripple creation timer
    Timer {
        interval: 2000
        running: true
        repeat: true
        onTriggered: {
            if (fuelStatus.ripples.length < fuelStatus.maxRipples) {
                createRipple()
            }
        }
    }

    function updateFluidPhysics() {
        var targetOffset = (fuelStatus.rollAngle / fuelStatus.maxTiltAngle) * 0.3
        var offsetDiff = targetOffset - fuelStatus.fluidOffset
        fuelStatus.fluidVelocity += offsetDiff * fuelStatus.springConstant
        fuelStatus.fluidVelocity *= fuelStatus.dampingFactor
        fuelStatus.fluidOffset += fuelStatus.fluidVelocity
    }

    function updateWavePhysics() {
        var newWavePoints = [...fuelStatus.wavePoints]
        var newVelocities = [...fuelStatus.waveVelocities]

        for (var i = 0; i < fuelStatus.wavePoints.length; i++) {
            var springForce = -fuelStatus.wavePoints[i] * fuelStatus.waveSpringConstant
            var spreadForce = 0

            if (i > 0) {
                spreadForce += (fuelStatus.wavePoints[i-1] - fuelStatus.wavePoints[i]) * fuelStatus.waveSpread
            }
            if (i < fuelStatus.wavePoints.length - 1) {
                spreadForce += (fuelStatus.wavePoints[i+1] - fuelStatus.wavePoints[i]) * fuelStatus.waveSpread
            }

            newVelocities[i] = fuelStatus.waveVelocities[i] + springForce + spreadForce
            newVelocities[i] *= fuelStatus.waveDamping
            newWavePoints[i] = fuelStatus.wavePoints[i] + newVelocities[i]

            // Add ripple effects
            for (var j = 0; j < fuelStatus.ripples.length; j++) {
                var ripple = fuelStatus.ripples[j]
                var distance = Math.abs(i / fuelStatus.wavePoints.length - ripple.position)
                var rippleEffect = Math.sin(ripple.age * 8) * Math.exp(-distance * 8) * ripple.amplitude
                newWavePoints[i] += rippleEffect
            }
        }

        fuelStatus.wavePoints = newWavePoints
        fuelStatus.waveVelocities = newVelocities
    }

    function updateRipples() {
        for (var i = fuelStatus.ripples.length - 1; i >= 0; i--) {
            fuelStatus.ripples[i].age += 0.016
            fuelStatus.ripples[i].amplitude *= 0.97

            if (fuelStatus.ripples[i].amplitude < 0.01) {
                fuelStatus.ripples.splice(i, 1)
            }
        }
    }

    function createRipple() {
        fuelStatus.ripples.push({
            position: Math.random(),
            amplitude: 0.15,
            age: 0
        })
    }

    function setValue(value) {
        var oldLevel = fuelStatus.fuelLevel
        var clampedValue = Math.min(Math.max(value, 0), 100)
        fuelStatus.fuelLevel = clampedValue / 100

        var disturbance = (fuelStatus.fuelLevel - oldLevel) * 0.8
        for (var i = 0; i < fuelStatus.wavePoints.length; i++) {
            fuelStatus.waveVelocities[i] += disturbance * (Math.random() * 2 - 1)
        }

        if (Math.abs(disturbance) > 0.05) {
            createRipple()
        }
    }

    function setRoll(angle) {
        var oldAngle = fuelStatus.rollAngle
        fuelStatus.rollAngle = Math.min(Math.max(angle, -fuelStatus.maxTiltAngle), fuelStatus.maxTiltAngle)

        var disturbance = (angle - oldAngle) * 0.015
        for (var i = 0; i < fuelStatus.wavePoints.length; i++) {
            fuelStatus.waveVelocities[i] += disturbance * (Math.random() * 2 - 1)
        }

        if (Math.abs(disturbance) > 0.1) {
            createRipple()
        }
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
                    var ctx = getContext("2d")
                    ctx.reset()

                    // Enhanced droplet shape
                    var dropWidth = width * 0.75
                    var dropHeight = height * 0.85
                    var centerX = width / 2
                    var centerY = height / 2

                    // Draw drop shadow
                    ctx.shadowBlur = 4
                    ctx.shadowColor = "rgba(0, 0, 0, 0.3)"
                    ctx.shadowOffsetX = 1
                    ctx.shadowOffsetY = 1

                    // Draw main droplet shape
                    ctx.beginPath()
                    ctx.moveTo(centerX, (height - dropHeight) / 2)
                    ctx.bezierCurveTo(
                        centerX - dropWidth/5, (height - dropHeight)/2 + dropHeight * 0.1,
                        centerX - dropWidth/2, (height - dropHeight)/2 + dropHeight * 0.65,
                        centerX, height - (height - dropHeight)/2
                    )
                    ctx.bezierCurveTo(
                        centerX + dropWidth/2, (height - dropHeight)/2 + dropHeight * 0.65,
                        centerX + dropWidth/5, (height - dropHeight)/2 + dropHeight * 0.1,
                        centerX, (height - dropHeight)/2
                    )

                    ctx.strokeStyle = fuelStatus.criticalFuel ?
                        (fuelStatus.blinkState ? "#FF4444" : "#FFFFFF") : "#FFFFFF"
                    ctx.lineWidth = 2
                    ctx.stroke()
                    ctx.clip()

                    ctx.shadowBlur = 0
                    ctx.shadowOffsetX = 0
                    ctx.shadowOffsetY = 0

                    var fillHeight = dropHeight * fuelStatus.fuelLevel
                    var baseY = height - (height - dropHeight)/2 - fillHeight

                    ctx.beginPath()
                    var segments = 40
                    for (var i = 0; i <= segments; i++) {
                        var x = centerX - dropWidth/2 + (dropWidth * i / segments)
                        var normalizedX = i / segments

                        var waveHeight = 0
                        var waveIndex = Math.floor(normalizedX * (fuelStatus.wavePoints.length - 1))
                        var waveProgress = normalizedX * (fuelStatus.wavePoints.length - 1) - waveIndex

                        if (waveIndex < fuelStatus.wavePoints.length - 1) {
                            waveHeight = fuelStatus.wavePoints[waveIndex] * (1 - waveProgress) +
                                       fuelStatus.wavePoints[waveIndex + 1] * waveProgress
                        } else {
                            waveHeight = fuelStatus.wavePoints[fuelStatus.wavePoints.length - 1]
                        }

                        for (var j = 0; j < fuelStatus.ripples.length; j++) {
                            var ripple = fuelStatus.ripples[j]
                            var distance = Math.abs(normalizedX - ripple.position)
                            waveHeight += Math.sin(ripple.age * 8) * Math.exp(-distance * 8) * ripple.amplitude
                        }

                        var tiltHeight = fuelStatus.fluidOffset * dropWidth * (normalizedX - 0.5)
                        var surfaceHeight = baseY + tiltHeight + waveHeight * dropHeight * 0.06

                        if (i === 0) {
                            ctx.moveTo(x, surfaceHeight)
                        } else {
                            ctx.lineTo(x, surfaceHeight)
                        }
                    }

                    ctx.lineTo(centerX + dropWidth/2, height)
                    ctx.lineTo(centerX - dropWidth/2, height)
                    ctx.closePath()

                    var color = fuelStatus.getColor(fuelStatus.fuelLevel)
                    var gradient = ctx.createLinearGradient(0, baseY - dropHeight * 0.1, 0, height)

                    gradient.addColorStop(0, Qt.rgba(
                        Math.min(parseInt(color.substr(1,2), 16)/255 + 0.2, 1),
                        Math.min(parseInt(color.substr(3,2), 16)/255 + 0.2, 1),
                        Math.min(parseInt(color.substr(5,2), 16)/255 + 0.2, 1),
                        0.9
                    ))

                    gradient.addColorStop(0.4, color)
                    gradient.addColorStop(1, Qt.darker(color, 1.2))

                    ctx.fillStyle = gradient
                    ctx.fill()

                    ctx.beginPath()
                    ctx.moveTo(centerX - dropWidth * 0.3, baseY)
                    ctx.quadraticCurveTo(
                        centerX, baseY + dropHeight * 0.2,
                        centerX + dropWidth * 0.3, baseY
                    )
                    ctx.strokeStyle = "rgba(255, 255, 255, 0.3)"
                    ctx.lineWidth = 1
                    ctx.stroke()
                }
            }
        }

        Item {
            Layout.alignment: Qt.AlignVCenter
            Layout.preferredWidth: 25
            Layout.preferredHeight: 20

            Text {
                id: fuelText
                anchors.fill: parent
                horizontalAlignment: Text.AlignLeft
                text: "Fuel\n" + Math.round(fuelStatus.fuelLevel * 100) + "%"
                font.family: "#9A9EB1"
                font.bold: true
                color: fuelStatus.getTextColor(fuelStatus.fuelLevel)
                font.pixelSize: 10
                lineHeight: 1.2
            }

            // Loader {
            //     anchors.fill: parent
            //     sourceComponent: fuelStatus.criticalFuel ? glowEffect : null
            // }

            // Component {
            //     id: glowEffect
            //     Glow {
            //         source: fuelText
            //         color: "#FF4444"
            //         radius: 4
            //         samples: 8
            //         spread: 0.4
            //     }
            // }
        }
    }

    Timer {
        interval: 500
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

    MouseArea {
        anchors.fill: parent
        onWheel: function(event) {
            handleWheelEvent(event)
        }

        function handleWheelEvent(wheel) {
            if (wheel.angleDelta.y > 0) {
                fuelStatus.setValue(Math.min((fuelStatus.fuelLevel + 0.01) * 100, 100))
            } else {
                fuelStatus.setValue(Math.max((fuelStatus.fuelLevel - 0.01) * 100, 0))
            }
        }
    }
}
