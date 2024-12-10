pragma ComponentBehavior: Bound
import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    id: root
    width: 45
    height: 150

    // Signal to emit throttle value changes
    signal throttleValueChanged(double value)

    Rectangle {
        id: throttleContainer
        width: 45
        height: 150
        color: "#80000000"
        radius: 30
        border.color: "#444444"
        border.width: 1
        anchors.centerIn: parent

        Behavior on color {
            ColorAnimation {
                duration: 200
            }
        }

        Text {
            text: "Throttle"
            color: "#ffffff"
            font.pixelSize: 10
            font.family: "#9A9EB1"
            anchors.top: parent.top
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.topMargin: 5
            opacity: 0.8
        }

        Rectangle {
            id: sliderTrack
            width: 6
            height: parent.height - 40
            color: "#555555"
            radius: 3
            anchors.centerIn: parent
            opacity: 0.8

            // Gradient overlay to show throttle level
            // Gradient overlay to show throttle level
            Rectangle {
                anchors.fill: parent
                anchors.topMargin: parent.height * (1 - throttleControl.value / 100)  // bottomMargin yerine topMargin kullanıyoruz
                color: getThrottleColor(throttleControl.value)
                radius: parent.radius

                Behavior on anchors.topMargin {  // bottomMargin yerine topMargin için davranış
                    NumberAnimation {
                        duration: 200
                        easing.type: Easing.OutCubic
                    }
                }

                Behavior on color {
                    ColorAnimation {
                        duration: 200
                    }
                }
            }

            Rectangle {
                id: sliderHandle
                width: 30
                height: 15
                radius: 10
                color: Qt.rgba(1, 1, 1, 0.1 + (throttleControl.value / 100) * 0.5)
                border.color: "#ffffff"
                border.width: 1
                anchors.horizontalCenter: sliderTrack.horizontalCenter
                y: (1 - (throttleControl.value - throttleControl.from) / (throttleControl.to - throttleControl.from)) * (sliderTrack.height - height)

                Behavior on color {
                    ColorAnimation {
                        duration: 200
                    }
                }
                MouseArea {
                    id: handleArea
                    anchors.fill: parent
                    cursorShape: Qt.PointingHandCursor
                    drag.target: parent
                    drag.axis: Drag.YAxis
                    drag.minimumY: 0
                    drag.maximumY: sliderTrack.height - sliderHandle.height

                    onPositionChanged: {
                        var newValue = throttleControl.to - (sliderHandle.y / (sliderTrack.height - sliderHandle.height)) * (throttleControl.to - throttleControl.from)
                        throttleControl.value = newValue
                    }

                    // Sürükleme bittiğinde sinyal yayınla
                    onReleased: {
                        root.throttleValueChanged(throttleControl.value)
                    }
                }
            }
        }

        Text {
            id: throttleValueDisplay
            text: Math.round(throttleControl.value) + "%"
            color: "#ffffff"
            font.family: "#9A9EB1"
            font.pixelSize: 10
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottomMargin: 5
            opacity: 0.9
        }
    }

    Slider {
        id: throttleControl
        from: 0
        to: 100
        stepSize: 1
        visible: false
    }

    // Function to get color based on throttle value
    function getThrottleColor(value) {
        if (value < 30) {
            return "#21DF56" // Green for low throttle
        } else if (value < 50) {
            return "#f1c21b" // Yellow for medium throttle
        }
        else if (value < 70) {
                    return "#E46B0E" // Yellow for medium throttle
                }
        else {
            return "#FD3C3C" // Red for high throttle
        }
    }
}
