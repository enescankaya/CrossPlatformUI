pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    width: 170
    height: 130
    property bool isConnected: false
    property bool isValidInput: ipInput.text !== "" && portInput.text !== ""
    signal connectionSignal(bool connectionState, string ip, int port)
    function onExternalConnectionSignal(connectionState) {
             root.isConnected = connectionState
             //root.connectionSignal(root.isConnected, ipInput.text, parseInt(portInput.text))
             //sliderHandle.x = root.isConnected ? connectionSlider.width - sliderHandle.width : 0
         //sliderHandle.x = 0
     }

    Rectangle {
        id: mainContainer
        width: parent.width
        height: parent.height
        color: "#80000000"
        radius: 20
        border.color: "#444444"

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 6

            Text {
                text: "TCP Setup"
                color: "#ffffff"
                font.pixelSize: 12
                font.family:"#9A9EB1"
                opacity: 0.8
                Layout.alignment: Qt.AlignHCenter
            }

            // Dynamically calculated width based on parent width
            property real inputWidth: (parent.width - 20) * 0.9  // Use 90% of parent width for inputs

            TextField {
                id: ipInput
                placeholderText: "IP Address"
                text: "127.0.0.1"
                focus: true
                color: "#ffffff"
                font.family:"#9A9EB1"
                font.pixelSize: 11
                readOnly: false // Kullanıcının yazabilmesini sağlamak
                background: Rectangle {
                    color: "#40ffffff"
                    radius: 5
                }
                Layout.preferredWidth: parent.inputWidth  // Set width to calculated value
                Layout.alignment: Qt.AlignHCenter
                selectByMouse: true
                RegularExpressionValidator {
                    regularExpression: /^(25[0-5]|2[0-4][0-9]|[0-1]?[0-9][0-9]?)\. (25[0-5]|2[0-4][0-9]|[0-1]?[0-9][0-9]?)\. (25[0-5]|2[0-4][0-9]|[0-1]?[0-9][0-9]?)\. (25[0-5]|2[0-4][0-9]|[0-1]?[0-9][0-9]?)$/
                }
                Component.onCompleted: {
                    if (text === "") {
                        text = "127.0.0.1"
                    }
                }
            }

            TextField {
                id: portInput
                placeholderText: "Port"
                focus: true
                text: "5762"
                color: "#ffffff"
                font.family:"#9A9EB1"
                font.pixelSize: 11
                readOnly: false // Kullanıcının yazabilmesini sağlamak
                background: Rectangle {
                    color: "#40ffffff"
                    radius: 5
                }
                Layout.preferredWidth: parent.inputWidth  // Set width to calculated value
                Layout.alignment: Qt.AlignHCenter
                selectByMouse: true
                validator: IntValidator {
                    bottom: 0
                    top: 65535
                }
                Component.onCompleted: {
                    if (text === "") {
                        text = "14550"
                    }
                }
            }

            Item {
                Layout.preferredWidth: parent.inputWidth  // Set width to calculated value
                Layout.preferredHeight: 30
                Layout.alignment: Qt.AlignHCenter

                Rectangle {
                    id: connectionSlider
                    width: parent.width  // Set width to calculated value
                    height: 30
                    color: "#40000000"
                    radius: 15
                    border.color: "#444444"
                    enabled: root.isValidInput

                    Rectangle {
                        id: sliderHandle
                        width: 52
                        height: parent.height
                        radius: 15
                        color: root.isValidInput ? (root.isConnected ? "#80FD3C3C" : "#8021DF56") : "#80808080"
                        x: 0

                        Behavior on x {
                            SmoothedAnimation {
                                duration: 300
                                easing.type: Easing.OutQuad
                            }
                        }

                        Text {
                            anchors.centerIn: parent
                            text: root.isConnected ? "Disconnect" : "Connect"
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
                                if (sliderHandle.x > parent.parent.width * 0.6) {
                                    if (!root.isConnected) {
                                        root.isConnected = true
                                        root.connectionSignal(true, ipInput.text, parseInt(portInput.text))
                                    } else {
                                        root.isConnected = false
                                        root.connectionSignal(false, ipInput.text, parseInt(portInput.text))
                                    }
                                }
                                sliderHandle.x = 0
                            }
                        }
                    }

                    Text {
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.rightMargin: 4  // Add 3-pixel margin to the right
                        text: root.isConnected ? "Slide to Disconnect" : "Slide to Connect"
                        color: "#CCFFFFFF"
                        font.pixelSize: 10
                        font.family:"#9A9EB1"
                        opacity: sliderHandle.x === 0 ? 1 : 0
                        visible: root.isValidInput

                        Behavior on opacity {
                            NumberAnimation { duration: 200 }
                        }
                    }

                    Text {
                        anchors.centerIn: parent
                        text: "Enter IP and Port"
                        color: "#CCFFFFFF"
                        font.family:"#9A9EB1"
                        font.pixelSize: 10
                        visible: !root.isValidInput
                    }
                }
            }
        }
    }
}
