pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Item {
    id: root
    width: 190
    height: 130

    signal setGuidedMode(real altitude)

    property real enteredAltitude: 0
    property bool isValidInput: false

    Rectangle {
        anchors.fill: parent
        color: "#80000000"  // Yarı şeffaf siyah
        radius: 15
        border.color: "#444444"

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 8
            spacing: 8

            // Altitude giriş alanı
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 80
                color: "#40000000"  // Daha şeffaf siyah
                radius: 10
                border.color: "#444444"

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 4

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: "Enter Altitude"
                        color: "white"
                        font.pixelSize: 12
                        font.weight: Font.Medium
                    }

                    TextField {
                        id: altitudeInput
                        Layout.fillWidth: true
                        Layout.preferredHeight: 30
                        color: "white"
                        font.pixelSize: 14
                        horizontalAlignment: TextInput.AlignHCenter
                        background: Rectangle {
                            color: "#40FFFFFF"
                            radius: 6
                        }
                        validator: DoubleValidator {
                            bottom: 0
                            notation: DoubleValidator.StandardNotation
                        }
                        onTextChanged: {
                            let value = parseFloat(text)
                            root.isValidInput = !isNaN(value) && value >= 0
                            if (root.isValidInput) {
                                root.enteredAltitude = value
                            }
                        }
                    }

                    Text {
                        Layout.alignment: Qt.AlignHCenter
                        text: root.isValidInput ? "" : "Enter a valid positive number"
                        color: "#FF6B6B"
                        font.pixelSize: 10
                        visible: altitudeInput.text.length > 0 && !root.isValidInput
                    }
                }
            }

            // Onay kaydırıcısı
            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: 30
                color: "#40000000"  // Daha şeffaf siyah
                radius: 15
                border.color: "#444444"
                enabled: root.isValidInput

                Rectangle {
                    id: slideHandle
                    width: 45
                    height: parent.height
                    radius: 15
                    color: root.isValidInput ? "#8021DF56" : "#80808080"  // Geçerli input varsa yeşil, yoksa gri
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
                            if (slideHandle.x > parent.parent.width * 0.6) {
                                slideHandle.x = parent.parent.width - slideHandle.width
                                root.setGuidedMode(root.enteredAltitude)
                                slideHandle.x = 0
                                altitudeInput.text = ""
                            } else {
                                slideHandle.x = 0
                            }
                        }
                    }
                }

                Text {
                    anchors.centerIn: parent
                    anchors.rightMargin: 10
                    text: "Slide to Confirm"
                    color: "#CCFFFFFF"
                    font.pixelSize: 10
                    opacity: slideHandle.x === 0 ? 1 : 0
                    Behavior on opacity {
                        NumberAnimation { duration: 200 }
                    }
                }
            }
        }
    }
}
