pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Window

Rectangle {
    id: root
    width: 40
    height: 30
    color: "transparent"

    property int signalStrength: 50
    property bool lowSignalWarning: signalStrength < 26
    function setValue(value){
        var clampedValue = Math.min(Math.max(value, 0), 100);
        signalStrength=clampedValue;
    }
    // Apply 180 degree rotation
    transform: Rotation {
        angle: 180
        axis.x: 1
        axis.y: 0  // Rotate around Y-axis
        axis.z: 0
        origin.x: root.width / 2
        origin.y: root.height / 2
    }

    Item {
        anchors.centerIn: parent

        Row {
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 3

            Repeater {
                model: 4
                delegate: Rectangle {
                    id: signalBar
                    required property int index

                    width: 6
                    height: 10 + (index * 5)
                    radius: 2
                    color: determineColor()

                    function determineColor() {
                        const activeBarCount = Math.floor(root.signalStrength / 25)

                        if (index === 0) {
                            if (root.lowSignalWarning) {
                                return "#FD3C3C"
                            }
                        }

                        if (index < activeBarCount) {
                            switch (activeBarCount) {
                                case 1: return "#FD3C3C"
                                case 2: return "#E46B0E"
                                case 3: return "#F1C21B"
                                case 4: return "#21DF56"
                            }
                        }

                        return "#D3D3D3"
                    }

                    SequentialAnimation {
                        id: lowSignalAnimation
                        running: root.lowSignalWarning && signalBar.index === 0
                        loops: Animation.Infinite

                        ColorAnimation {
                            target: signalBar
                            property: "color"
                            from: "#FD3C3C"
                            to: "#D3D3D3"
                            duration: 500
                        }
                        ColorAnimation {
                            target: signalBar
                            property: "color"
                            from: "#D3D3D3"
                            to: "#FD3C3C"
                            duration: 500
                        }
                    }
                }
            }
        }
    }

    MouseArea {
        anchors.fill: parent
         onWheel: {
            if (wheel.angleDelta.y > 0) {
                parent.signalStrength = Math.min(100, root.signalStrength + 10)
            } else {
                parent.signalStrength = Math.max(0, root.signalStrength - 10)
            }
        }
    }
}
