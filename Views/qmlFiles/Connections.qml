
import QtQuick 2.15

Rectangle {
    id: root
    width: 40
    height: 40
    color: "transparent"
    property real signalValue: 50

    function setSignalValue(value: int) {
        if (value >= 0 && value <= 100) {
            signalValue = value;
        }
    }

    // Background ring
    Rectangle {
        id: backgroundRing
        width: 30
        height: 30
        radius: width / 2
        anchors.centerIn: parent
        color: "transparent"
        border.width: 1.5
        border.color: "#20000000"
    }

    // Main indicator
    Rectangle {
        id: indicator
        width: 30
        height: 30
        radius: width / 2
        anchors.centerIn: parent

        // Shadow effect using native properties
        Rectangle {
            id: shadow
            anchors.fill: parent
            radius: parent.radius
            color: "#40000000"
            anchors.centerIn: parent
            anchors.horizontalCenterOffset: 1
            anchors.verticalCenterOffset: 1
            z: -1
        }

        // Gradient color based on signal value
        property color startColor: Qt.rgba(
            root.lerp(0x21/255, 0xFD/255, 1 - root.signalValue/100),
            root.lerp(0xDF/255, 0x3C/255, 1 - root.signalValue/100),
            root.lerp(0x56/255, 0x3C/255, 1 - root.signalValue/100),
            1.0
        )

        gradient: Gradient {
            GradientStop { position: 0.0; color: indicator.startColor }
            GradientStop { position: 1.0; color: Qt.darker(indicator.startColor, 1.3) }
        }

        // Centered MavLink symbol image
        Image {
            anchors.centerIn: parent
            source: "qrc:/Resources/mavlinkIcon.png"
            width: parent.width * 0.6
            height: width
            fillMode: Image.PreserveAspectFit
        }

        // Modern pulse animation
        SequentialAnimation {
            id: pulseAnimation
            running: root.signalValue < 30
            loops: Animation.Infinite

            ParallelAnimation {
                NumberAnimation {
                    target: indicator
                    property: "scale"
                    from: 1.0
                    to: 0.85
                    duration: 800
                    easing.type: Easing.OutCubic
                }
                NumberAnimation {
                    target: indicator
                    property: "opacity"
                    from: 1.0
                    to: 0.7
                    duration: 800
                }
            }

            ParallelAnimation {
                NumberAnimation {
                    target: indicator
                    property: "scale"
                    from: 0.85
                    to: 1.0
                    duration: 800
                    easing.type: Easing.InCubic
                }
                NumberAnimation {
                    target: indicator
                    property: "opacity"
                    from: 0.7
                    to: 1.0
                    duration: 800
                }
            }
        }
    }

    function lerp(start, end, t) {
        return start + (end - start) * t
    }
}
