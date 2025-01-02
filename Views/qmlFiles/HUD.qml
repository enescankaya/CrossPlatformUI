import QtQuick 2.0

Rectangle {
    id: root
    width: 450
    height: 340
    color: "transparent"

    // Genel özellikler

    // Dışarıdan güncellenecek özellikler
    property int instantAirspeed: 0
    property int altitude: 0 // Changed to integer
    property int pitchValue: 0
    property int rollValue: 0 // Added rollValue
    property color rollColor: Math.abs(rollValue) > 45 ? "#FD3C3C" : "#21DF56"

    // C++ tarafından tetiklenecek slot fonksiyonu
    function updateValues(newAirspeed, newAltitude, newPitch, newRoll) {
        instantAirspeed = newAirspeed
        altitude = newAltitude
        pitchValue = newPitch
        rollValue = newRoll // Assign newRoll to rollValue
    }
    // Roll Value Display (Attitude Indicator'ın üstünde)
    Rectangle {
        anchors {
            bottom: attitudeIndicator.top
            horizontalCenter: attitudeIndicator.horizontalCenter
            bottomMargin: 10
        }
        width: rollText.width + 20
        height: rollText.height + 10
        color: Math.abs(root.rollValue) > 45 ? "#40FD3C3C" : "transparent"
        radius: 5

        Text {
            id: rollText
            anchors.centerIn: parent
            color: root.rollColor
            font.pixelSize: 15
            font.bold: Math.abs(root.rollValue) > 45
            font.family: "Arial"
            text: "Roll: " + root.rollValue + "°"
        }
    }
    // İrtifa göstergesi (Sol orta)
        Item {
            id: altitudeIndicator
            anchors {
                verticalCenter: parent.verticalCenter
                left: parent.left
                leftMargin: 5
            }
            width: parent.width * 0.25
            height: parent.height * 0.5

            // İrtifa Göstergesi Etiketi
            Text {
                anchors {
                    top: parent.top
                    horizontalCenter: parent.horizontalCenter
                    topMargin: -20
                }
                color: "#21DF56"
                font.pixelSize: 15
                font.bold: false
                font.family: "Arial"
                text: "Altitude"
            }

            readonly property int verticalLength: height * 0.8
            readonly property int horizontalLength: width * 0.4
            readonly property int lineMarginLeft: width * 0.05
            readonly property int scaleLineLength: width * 0.05
            readonly property int scaleLineVerticalSpacing: altitudeIndicator.verticalLength / 10

            Canvas {
                id: altitudeCanvas
                anchors.fill: parent
                onPaint: {
                    var ctx = getContext("2d")
                    ctx.reset()
                    ctx.font = "10px Arial"
                    ctx.strokeStyle = "#21DF56"
                    ctx.fillStyle = "#21DF56"
                    ctx.lineWidth = 1
                    ctx.beginPath()
                    // Ana çizgi
                    ctx.moveTo(altitudeIndicator.lineMarginLeft, height / 2 - altitudeIndicator.verticalLength / 2)
                    ctx.lineTo(altitudeIndicator.lineMarginLeft + altitudeIndicator.horizontalLength, height / 2 - altitudeIndicator.verticalLength / 2)
                    ctx.lineTo(altitudeIndicator.lineMarginLeft + altitudeIndicator.horizontalLength, height / 2 + altitudeIndicator.verticalLength / 2)
                    ctx.lineTo(altitudeIndicator.lineMarginLeft, height / 2 + altitudeIndicator.verticalLength / 2)

                    // Yatay ölçek çizgileri
                    for (var i = 1; i < 11; i++) {
                        // 3 küçük ölçek çizgisi
                        for (var j = 1; j < 4; j++) {
                            ctx.moveTo(
                                altitudeIndicator.lineMarginLeft + altitudeIndicator.horizontalLength,
                                height / 2 - altitudeIndicator.verticalLength / 2 + i * altitudeIndicator.scaleLineVerticalSpacing - j * altitudeIndicator.scaleLineVerticalSpacing / 4
                            )
                            ctx.lineTo(
                                altitudeIndicator.lineMarginLeft + altitudeIndicator.horizontalLength - altitudeIndicator.scaleLineLength,
                                height / 2 - altitudeIndicator.verticalLength / 2 + i * altitudeIndicator.scaleLineVerticalSpacing - j * altitudeIndicator.scaleLineVerticalSpacing / 4
                            )
                        }
                        ctx.moveTo(
                            altitudeIndicator.lineMarginLeft + altitudeIndicator.horizontalLength,
                            height / 2 - altitudeIndicator.verticalLength / 2 + i * altitudeIndicator.scaleLineVerticalSpacing
                        )
                        ctx.lineTo(
                            altitudeIndicator.lineMarginLeft + altitudeIndicator.horizontalLength - altitudeIndicator.scaleLineLength * 2.5,
                            height / 2 - altitudeIndicator.verticalLength / 2 + i * altitudeIndicator.scaleLineVerticalSpacing
                        )
                        // İrtifa değerleri
                        if (i == 10) break
                        ctx.fillText(
                            root.altitude + (5 - i) * 1,
                            altitudeIndicator.lineMarginLeft + altitudeIndicator.horizontalLength - altitudeIndicator.scaleLineLength * 2.5 - 21,
                            height / 2 - altitudeIndicator.verticalLength / 2 + i * altitudeIndicator.scaleLineVerticalSpacing + 3
                        )
                    }
                    ctx.stroke()
                }

                Connections {
                    target: root
                    function onAltitudeChanged() {
                        altitudeCanvas.requestPaint()
                    }
                }
            }

            Canvas {
                id: currentAltitudeCanvas
                anchors.fill: parent
                onPaint: {
                    var ctx = getContext("2d")
                    ctx.reset()
                    ctx.font = "13px Arial"
                    ctx.strokeStyle = "#21DF56"
                    ctx.fillStyle = "#21DF56"
                    ctx.lineWidth = 1
                    ctx.beginPath()
                    // Mevcut irtifa değeri
                    ctx.fillText(
                        root.altitude,
                        altitudeIndicator.lineMarginLeft + altitudeIndicator.horizontalLength / 2 - ctx.measureText(root.altitude.toString()).width / 2,
                        height / 2 - altitudeIndicator.verticalLength / 2 - 5
                    )

                    // İrtifa gösterge oku (üçgen)
                    ctx.moveTo(altitudeIndicator.lineMarginLeft + altitudeIndicator.horizontalLength, height / 2)
                    ctx.lineTo(altitudeIndicator.lineMarginLeft + altitudeIndicator.horizontalLength + 10, height / 2 - 10)
                    ctx.lineTo(altitudeIndicator.lineMarginLeft + altitudeIndicator.horizontalLength + 10, height / 2 + 10)
                    ctx.lineTo(altitudeIndicator.lineMarginLeft + altitudeIndicator.horizontalLength, height / 2)
                    ctx.fill()
                    ctx.stroke()
                }

                Connections {
                    target: root
                    function onAltitudeChanged() {
                        currentAltitudeCanvas.requestPaint()
                    }
                }
            }
        }

        // Hız göstergesi (Sağ orta)
        Item {
            id: speedIndicator
            anchors {
                verticalCenter: parent.verticalCenter
                right: parent.right
                rightMargin: 5
            }
            width: parent.width * 0.25
            height: parent.height * 0.5

            // Hız Göstergesi Etiketi
            Text {
                anchors {
                    top: parent.top
                    horizontalCenter: parent.horizontalCenter
                    topMargin: -20
                }
                color: "#21DF56"
                font.pixelSize: 15
                font.bold: false
                font.family: "Arial"
                text: "Verticle Speed"
            }

            readonly property int verticalLength: height * 0.8
            readonly property int horizontalLength: width * 0.4
            readonly property int lineMarginRight: width * 0.05
            readonly property int scaleLineLength: width * 0.05
            readonly property int scaleLineVerticalSpacing: altitudeIndicator.verticalLength / 10

            Canvas {
                id: airSpeedCanvas
                anchors.fill: parent
                onPaint: {
                    var ctx = getContext("2d")
                    ctx.reset()
                    ctx.font = "10px Arial"
                    ctx.strokeStyle = "#21DF56"
                    ctx.fillStyle = "#21DF56"
                    ctx.lineWidth = 1
                    ctx.beginPath()
                    // Ana çizgi
                    ctx.moveTo(width - speedIndicator.lineMarginRight, height / 2 - altitudeIndicator.verticalLength / 2)
                    ctx.lineTo(width - speedIndicator.lineMarginRight - altitudeIndicator.horizontalLength, height / 2 - altitudeIndicator.verticalLength / 2)
                    ctx.lineTo(width - speedIndicator.lineMarginRight - altitudeIndicator.horizontalLength, height / 2 + altitudeIndicator.verticalLength / 2)
                    ctx.lineTo(width - speedIndicator.lineMarginRight, height / 2 + altitudeIndicator.verticalLength / 2)

                    // Yatay ölçek çizgileri
                    for (var i = 1; i < 11; i++) {
                        // 3 küçük ölçek çizgisi
                        for (var j = 1; j < 4; j++) {
                            ctx.moveTo(
                                width - speedIndicator.lineMarginRight - altitudeIndicator.horizontalLength,
                                height / 2 - altitudeIndicator.verticalLength / 2 + i * altitudeIndicator.scaleLineVerticalSpacing - j * altitudeIndicator.scaleLineVerticalSpacing / 4
                            )
                            ctx.lineTo(
                                width - speedIndicator.lineMarginRight - altitudeIndicator.horizontalLength + altitudeIndicator.scaleLineLength,
                                height / 2 - altitudeIndicator.verticalLength / 2 + i * altitudeIndicator.scaleLineVerticalSpacing - j * altitudeIndicator.scaleLineVerticalSpacing / 4
                            )
                        }
                        ctx.moveTo(
                            width - speedIndicator.lineMarginRight - altitudeIndicator.horizontalLength,
                            height / 2 - altitudeIndicator.verticalLength / 2 + i * altitudeIndicator.scaleLineVerticalSpacing
                        )
                        ctx.lineTo(
                            width - speedIndicator.lineMarginRight - altitudeIndicator.horizontalLength + altitudeIndicator.scaleLineLength * 2.5,
                            height / 2 - altitudeIndicator.verticalLength / 2 + i * altitudeIndicator.scaleLineVerticalSpacing
                        )
                        // Hız değerleri
                        if (i == 10) break
                        ctx.fillText(
                            root.instantAirspeed + (5 - i),
                            width - speedIndicator.lineMarginRight - altitudeIndicator.horizontalLength + altitudeIndicator.scaleLineLength * 2.5 + 5,
                            height / 2 - altitudeIndicator.verticalLength / 2 + i * altitudeIndicator.scaleLineVerticalSpacing + 3
                        )
                    }
                    ctx.stroke()
                }

                Connections {
                    target: root
                    function onInstantAirspeedChanged() {
                        airSpeedCanvas.requestPaint()
                    }
                }
            }

            Canvas {
                id: currentAirSpeedCanvas
                anchors.fill: parent
                onPaint: {
                    var ctx = getContext("2d")
                    ctx.reset()
                    ctx.font = "13px Arial"
                    ctx.strokeStyle = "#21DF56"
                    ctx.fillStyle = "#21DF56"
                    ctx.lineWidth = 1
                    ctx.beginPath()
                    // Mevcut hız değeri
                    ctx.fillText(
                        root.instantAirspeed,
                        width - speedIndicator.lineMarginRight - altitudeIndicator.horizontalLength / 2 - ctx.measureText(root.instantAirspeed.toString()).width / 2,
                        height / 2 - altitudeIndicator.verticalLength / 2 - 5
                    )

                    // Hız gösterge oku (üçgen)
                    ctx.moveTo(width - speedIndicator.lineMarginRight - altitudeIndicator.horizontalLength - 10, height / 2 - 10)
                    ctx.lineTo(width - speedIndicator.lineMarginRight - altitudeIndicator.horizontalLength - 10, height / 2 + 10)
                    ctx.lineTo(width - speedIndicator.lineMarginRight - altitudeIndicator.horizontalLength, height / 2)
                    ctx.lineTo(width - speedIndicator.lineMarginRight - altitudeIndicator.horizontalLength - 10, height / 2 - 10)
                    ctx.fill()
                    ctx.stroke()
                }

                Connections {
                    target: root
                    function onInstantAirspeedChanged() {
                        currentAirSpeedCanvas.requestPaint()
                    }
                }
            }
        }
    // Attitude Indicator (Ladder - Ortalanmış)
        Item {
                id: attitudeIndicator
                anchors {
                    verticalCenter: parent.verticalCenter
                    horizontalCenter: parent.horizontalCenter
                }
                width: parent.width
                height: parent.height * 0.8

                // Nişangah (Merkezde) with dynamic color
                Item {
                    id: aimCursor
                    anchors.centerIn: parent
                    width: parent.width * 0.2
                    height: parent.width * 0.2
                    readonly property int aimCursorRadius: width * 0.2

                    Canvas {
                        id: aimCursorCanvas
                        anchors.fill: parent
                        onPaint: {
                            var ctx = getContext("2d")
                            ctx.reset()
                            ctx.strokeStyle = root.rollColor
                            ctx.lineWidth = 1
                            ctx.beginPath()
                            ctx.setLineDash([5, 5])
                            ctx.arc(width / 2, height / 2, aimCursor.aimCursorRadius, 0, 2 * Math.PI)
                            ctx.stroke()
                            ctx.beginPath()
                            ctx.moveTo(width / 2, height / 2 - aimCursor.aimCursorRadius / 2)
                            ctx.lineTo(width / 2, height / 2 + aimCursor.aimCursorRadius / 2)
                            ctx.stroke()
                            ctx.beginPath()
                            ctx.moveTo(width / 2 - aimCursor.aimCursorRadius / 2, height / 2)
                            ctx.lineTo(width / 2 + aimCursor.aimCursorRadius / 2, height / 2)
                            ctx.stroke()
                        }
                    }

                    Connections {
                        target: root
                        function onRollValueChanged() {
                            aimCursorCanvas.requestPaint()
                        }
                    }
                }

        readonly property int lineVerticalSpacing: 40
        readonly property int lineHorizontalSpacing: 25
        readonly property int dashLineWidth: 18
        readonly property int dashSpacing: 8
        readonly property int lineMargin: 80
        readonly property int leftHalfEnd: width / 2 - lineHorizontalSpacing / 2
        readonly property int rightHalfStart: width / 2 + lineHorizontalSpacing / 2
        readonly property int aimCursorRadius: 35
        property int uppestLineValue: root.pitchValue +2
        property int upperLineValue: root.pitchValue  + 1
        property int middleLineValue: root.pitchValue // Middle line value
        property int lowerLineValue: root.pitchValue  - 1
        property int lowestLineValue: root.pitchValue - 2

        Canvas {
            id: attitudeCanvas
            anchors.fill: parent
            onPaint: {
                var ctx = getContext("2d")
                ctx.reset()
                ctx.save()
                ctx.translate(width / 2, height / 2)
                ctx.rotate(root.rollValue * Math.PI / 180) // Rotate according to rollValue
                ctx.translate(-width / 2, -height / 2)
                ctx.strokeStyle = "#21DF56"
                ctx.lineWidth = 2
                ctx.beginPath()
                const smallGap = 1
                const textOffset = 10
                // Middle line (longest)
                ctx.moveTo(attitudeIndicator.lineMargin, height / 2)
                ctx.lineTo(attitudeIndicator.leftHalfEnd - attitudeIndicator.aimCursorRadius, height / 2)
                ctx.moveTo(attitudeIndicator.rightHalfStart + attitudeIndicator.aimCursorRadius, height / 2)
                ctx.lineTo(width - attitudeIndicator.lineMargin, height / 2)
                // Lines get progressively shorter
                for (var i = 1; i < 3; i++) {
                    var extraMargin = i * 40
                    // Upper lines
                    var lineStartX = attitudeIndicator.lineMargin + extraMargin
                    var lineEndX = attitudeIndicator.leftHalfEnd - attitudeIndicator.aimCursorRadius - smallGap
                    ctx.moveTo(lineStartX, height / 2 - attitudeIndicator.lineVerticalSpacing * i)
                    ctx.lineTo(lineEndX, height / 2 - attitudeIndicator.lineVerticalSpacing * i)
                    var rightLineStartX = attitudeIndicator.rightHalfStart + attitudeIndicator.aimCursorRadius + smallGap
                    var rightLineEndX = width - attitudeIndicator.lineMargin - extraMargin
                    ctx.moveTo(rightLineStartX, height / 2 - attitudeIndicator.lineVerticalSpacing * i)
                    ctx.lineTo(rightLineEndX, height / 2 - attitudeIndicator.lineVerticalSpacing * i)
                    // Lower lines
                    ctx.moveTo(lineStartX, height / 2 + attitudeIndicator.lineVerticalSpacing * i)
                    ctx.lineTo(lineEndX, height / 2 + attitudeIndicator.lineVerticalSpacing * i)
                    ctx.moveTo(rightLineStartX, height / 2 + attitudeIndicator.lineVerticalSpacing * i)
                    ctx.lineTo(rightLineEndX, height / 2 + attitudeIndicator.lineVerticalSpacing * i)
                }
                ctx.font = "12px Arial"
                ctx.fillStyle = "#21DF56"
                // Left side text - positioned at the start of each line
                ctx.textAlign = "right"
                ctx.fillText(attitudeIndicator.middleLineValue, width - attitudeIndicator.lineMargin - 105 + textOffset, height / 2 + 4)
                ctx.fillText(attitudeIndicator.uppestLineValue, attitudeIndicator.lineMargin + 80 - textOffset, height / 2 - attitudeIndicator.lineVerticalSpacing * 2 + 4)
                ctx.fillText(attitudeIndicator.upperLineValue, attitudeIndicator.lineMargin + 40 - textOffset, height / 2 - attitudeIndicator.lineVerticalSpacing + 4)
                ctx.fillText(attitudeIndicator.lowerLineValue, attitudeIndicator.lineMargin + 40 - textOffset, height / 2 + attitudeIndicator.lineVerticalSpacing + 4)
                ctx.fillText(attitudeIndicator.lowestLineValue, attitudeIndicator.lineMargin + 80 - textOffset, height / 2 + attitudeIndicator.lineVerticalSpacing * 2 + 4)
                // Right side text - positioned at the end of each line
                ctx.textAlign = "left"
                ctx.fillText(attitudeIndicator.middleLineValue, attitudeIndicator.lineMargin + 105 - textOffset, height / 2 + 4)
                ctx.fillText(attitudeIndicator.uppestLineValue, width - attitudeIndicator.lineMargin - extraMargin + textOffset, height / 2 - attitudeIndicator.lineVerticalSpacing * 2 + 4)
                ctx.fillText(attitudeIndicator.upperLineValue, width - attitudeIndicator.lineMargin - 40 + textOffset, height / 2 - attitudeIndicator.lineVerticalSpacing + 4)
                ctx.fillText(attitudeIndicator.lowerLineValue, width - attitudeIndicator.lineMargin - 40 + textOffset, height / 2 + attitudeIndicator.lineVerticalSpacing + 4)
                ctx.fillText(attitudeIndicator.lowestLineValue, width - attitudeIndicator.lineMargin - extraMargin + textOffset, height / 2 + attitudeIndicator.lineVerticalSpacing * 2 + 4)
                ctx.stroke()
                ctx.restore()
            }

            Connections {
                target: root
                function onPitchValueChanged() {
                    attitudeCanvas.requestPaint()
                }
                function onRollValueChanged() {
                    attitudeCanvas.requestPaint()
                }
            }
        }
    }
}
