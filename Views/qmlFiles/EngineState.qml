pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    width: 150
    height: 131

    signal engineStateChanged(bool isEngineOn)

    property bool isEngineOn: false
    property bool isValidInput: false
    function setMode(engineState) {
        isEngineOn=engineState
        engineStateChanged(engineState)
     }
    Rectangle {
        anchors.fill: parent
        color: "#80000000"
        radius: 15
        border.color: "#444444"

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 8
            spacing: 8

            // Engine State Selection
            RowLayout {
                Layout.fillWidth: true
                spacing: 5

                // Off Button
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 35
                    color: !root.isEngineOn ? "#FD3C3C" : "#40FD3C3C"
                    radius: 8

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (root.isEngineOn) {
                                root.isValidInput = true
                                root.isEngineOn = false
                            }
                        }
                    }

                    Text {
                        anchors.centerIn: parent
                        text: "Engine Off"
                        color: "white"
                        font.family:"#9A9EB1"
                        font.pixelSize: 10
                        font.weight: Font.Bold
                    }
                }

                // On Button
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 35
                    color: root.isEngineOn ? "#21DF56" : "#4021DF56"
                    radius: 8

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (!root.isEngineOn) {
                                root.isValidInput = true
                                root.isEngineOn = true
                            }
                        }
                    }

                    Text {
                        anchors.centerIn: parent
                        text: "Engine On"
                        color: "white"
                        font.family:"#9A9EB1"
                        font.pixelSize: 10
                        font.weight: Font.Bold
                    }
                }
            }

            // Confirmation Slider
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 30
                color: "#40000000"
                radius: 15
                border.color: "#444444"
                enabled: root.isValidInput

                Rectangle {
                    id: guidedSlideHandle
                    width: 45
                    height: parent.height
                    radius: 15
                    color: root.isValidInput ? "#8021DF56" : "#80808080"
                    x: 0

                    Behavior on x {
                        SmoothedAnimation {
                            duration: 300
                            easing.type: Easing.OutQuad
                        }
                    }

                    MouseArea {
                        anchors.fill: parent
                        enabled: root.isValidInput
                        drag.target: parent
                        drag.axis: Drag.XAxis
                        drag.minimumX: 0
                        drag.maximumX: parent.parent.width - parent.width

                        onReleased: {
                            if (guidedSlideHandle.x > parent.parent.width * 0.6) {
                                guidedSlideHandle.x = parent.parent.width - guidedSlideHandle.width

                                // Confirm and emit signal
                                root.engineStateChanged(root.isEngineOn)

                                // Reset slider
                                guidedSlideHandle.x = 0
                                root.isValidInput = false
                            } else {
                                guidedSlideHandle.x = 0
                            }
                        }
                    }
                }

                Text {
                    anchors.centerIn: parent
                    text: "Slide to Confirm"
                    color: "#CCFFFFFF"
                    font.pixelSize: 10
                    font.family:"#9A9EB1"
                    opacity: guidedSlideHandle.x === 0 ? 1 : 0
                    Behavior on opacity {
                        NumberAnimation { duration: 200 }
                    }
                }
            }
        }
    }
}
