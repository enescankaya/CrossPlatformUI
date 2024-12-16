pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Item {
    id: root
    width: 190
    height: 130
    // Properties
    property var modes: ["Manual", "Circle", "Auto", "Guided", "Taxi", "RTL"]
    property int currentModeIndex: 0
    property bool isGuidedModeActive: false
    property real guidedAltitude: 500 // Default altitude
    property bool isValidInput: true

    // Signals
    signal modeChanged(string modeName, int modeIndex)
    signal setGuidedMode(int altitude)
    function setMode(modeIndex) {
             currentModeIndex = modeIndex
             modeChanged(modes[modeIndex], modeIndex)
     }
    function changeModeUI(modeIndex) {
             currentModeIndex = modeIndex
     }
    Rectangle {
        anchors.fill: parent
        color: "#80000000"  // Semi-transparent black
        radius: 15
        border.color: "#444444"

        StackLayout {
            id: mainLayout
            anchors.fill: parent
            anchors.margins: 8
            currentIndex: root.isGuidedModeActive ? 1 : 0

            // First Page: Mode Selection
            Item {
                ColumnLayout {
                    anchors.fill: parent
                    spacing: 8

                    // Mode Selection Grid
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 80
                        color: "#40000000"
                        radius: 10
                        border.color: "#444444"

                        GridLayout {
                            anchors.fill: parent
                            anchors.margins: 4
                            columns: 3
                            rowSpacing: 4
                            columnSpacing: 4

                            Repeater {
                                model: root.modes
                                delegate: Rectangle {
                                    required property int index
                                    required property var modelData
                                    Layout.fillWidth: true
                                    Layout.fillHeight: true
                                    color: index === root.currentModeIndex ? "#803498DB" : "#40FFFFFF"
                                    radius: 6
                                    border.color: "#444444"

                                    Text {
                                        anchors.centerIn: parent
                                        text: parent.modelData
                                        color: "white"
                                        font.family:"#9A9EB1"
                                        font.pixelSize: 10
                                        font.weight: Font.Medium
                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            root.currentModeIndex = parent.index
                                            if (root.modes[parent.index] === "Guided") {
                                                root.isGuidedModeActive = true
                                            }
                                        }
                                    }
                                }
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

                        Rectangle {
                            id: mainSlideHandle
                            width: 45
                            height: parent.height
                            radius: 15
                            color: "#8021DF56"
                            x: 0

                            Behavior on x {
                                SmoothedAnimation {
                                    duration: 300
                                    easing.type: Easing.OutQuad
                                }
                            }

                            Text {
                                anchors.centerIn: parent
                                text: "Confirm"
                                color: "white"
                                font.family:"#9A9EB1"
                                font.pixelSize: 10
                                font.weight: Font.Bold
                            }

                            MouseArea {
                                anchors.fill: parent
                                drag.target: parent
                                drag.axis: Drag.XAxis
                                drag.minimumX: 0
                                drag.maximumX: parent.parent.width - parent.width
                                onReleased: {
                                    if (mainSlideHandle.x > parent.parent.width * 0.6) {
                                        mainSlideHandle.x = parent.parent.width - mainSlideHandle.width
                                        root.modeChanged(root.modes[root.currentModeIndex], root.currentModeIndex)
                                        mainSlideHandle.x = 0
                                    } else {
                                        mainSlideHandle.x = 0
                                    }
                                }
                            }
                        }

                        Text {
                            anchors.centerIn: parent
                            text: "Slide to Confirm"
                            color: "#CCFFFFFF"
                            font.family:"#9A9EB1"
                            font.pixelSize: 10
                            opacity: mainSlideHandle.x === 0 ? 1 : 0
                            Behavior on opacity {
                                NumberAnimation { duration: 200 }
                            }
                        }
                    }
                }
            }

            // Second Page: Guided Mode Altitude Input
            Item {
                ColumnLayout {
                    anchors.fill: parent
                    spacing: 8

                    // Descriptive Text
                    Text {
                        Layout.fillWidth: true
                        text: "Enter Guided Mode Altitude"
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignTop
                        font.pixelSize: 12
                        font.family:"#9A9EB1"
                        font.weight: Font.Medium
                    }

                    // Altitude Input Area
                    Rectangle {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 50
                        color: "#40000000"
                        radius: 10
                        border.color: "#444444"

                        ColumnLayout {
                            anchors.fill: parent
                            anchors.margins: 4
                            spacing: 4

                            // Altitude Input Section
                            RowLayout {
                                Layout.fillWidth: true

                                // Back Button
                                Rectangle {
                                    Layout.preferredWidth: 40
                                    Layout.preferredHeight: 30
                                    color: "#40000000"
                                    radius: 10
                                    border.color: "#444444"

                                    Text {
                                        anchors.centerIn: parent
                                        text: "←"
                                        color: "white"
                                        font.family:"#9A9EB1"
                                        font.pixelSize: 20
                                        font.weight: Font.Bold
                                    }

                                    MouseArea {
                                        anchors.fill: parent
                                        onClicked: {
                                            root.isGuidedModeActive = false
                                        }
                                    }
                                }

                                // Altitude Input
                                TextField {
                                    id: altitudeInput
                                    Layout.fillWidth: true
                                    Layout.preferredHeight: 30
                                    color: "white"
                                    font.pixelSize: 14
                                    font.family:"#9A9EB1"
                                    focus: true  // Klavyenin açılması için odaklanma
                                    horizontalAlignment: TextInput.AlignHCenter
                                    background: Rectangle {
                                        color: "#40FFFFFF"
                                        radius: 6
                                    }
                                    text: root.guidedAltitude.toString()
                                    validator: DoubleValidator {
                                        bottom: 0
                                        notation: DoubleValidator.StandardNotation
                                    }
                                    onTextChanged: {
                                        let value = parseFloat(text)
                                        root.guidedAltitude = !isNaN(value) && value >= 0 ? value : 500
                                    }
                                }

                            }

                            // Error Message
                            Text {
                                Layout.alignment: Qt.AlignHCenter
                                text: "Invalid altitude (0-10000m)"
                                color: "#FF6B6B"
                                font.family:"#9A9EB1"
                                font.pixelSize: 10
                                visible: !root.isValidInput && altitudeInput.text.length > 0
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

                            Text {
                                anchors.centerIn: parent
                                text: "Confirm"
                                color: "white"
                                font.family:"#9A9EB1"
                                font.pixelSize: 10
                                font.weight: Font.Bold
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
                                        root.modeChanged("Guided", root.modes.indexOf("Guided"))
                                        guidedSlideHandle.x = parent.parent.width - guidedSlideHandle.width
                                        root.setGuidedMode(root.guidedAltitude)
                                        root.isGuidedModeActive = false
                                        guidedSlideHandle.x = 0
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
    }
}
