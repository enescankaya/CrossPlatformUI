import QtQuick 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    width: 150
    height: 131

    signal securityStateChanged(bool isArmed)

    property bool isArmed: false
    property bool isValidInput: false
    function setMode(armState) {
        isArmed=armState
        securityStateChanged(armState)
     }
    function changeArmStateUI(armState) {
        isArmed=armState
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

            // Security State Selection
            RowLayout {
                Layout.fillWidth: true
                spacing: 5

                // Disarm Button
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 35
                    color: !root.isArmed ? "#FD3C3C" : "#40FD3C3C"
                    radius: 8

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (root.isArmed) {
                                root.isValidInput = true
                                root.isArmed = false
                            }
                        }
                    }

                    Text {
                        anchors.centerIn: parent
                        text: "Disarm"
                        color: "white"
                        font.family: "#9A9EB1"
                        font.pixelSize: 10
                        font.weight: Font.Bold
                    }
                }

                // Arm Button
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 35
                    color: root.isArmed ? "#21DF56" : "#4021DF56"
                    radius: 8

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (!root.isArmed) {
                                root.isValidInput = true
                                root.isArmed = true
                            }
                        }
                    }

                    Text {
                        anchors.centerIn: parent
                        text: "Arm"
                        color: "white"
                        font.family: "#9A9EB1"
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
                                root.securityStateChanged(root.isArmed)

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
                    font.family: "#9A9EB1"
                    opacity: guidedSlideHandle.x === 0 ? 1 : 0
                    Behavior on opacity {
                        NumberAnimation { duration: 200 }
                    }
                }
            }
        }
    }
}
