pragma ComponentBehavior: Bound

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    width: 170
    height: 160
    property bool isConnected: false
    property bool isValidInput: ipInput.text !== "" && portInput.text !== ""
    property bool isUDP: false
    signal connectionSignal(bool connectionState, string ip, int port)
    signal udpConnectionSignal(bool connectionState, string ip, int port)

    function onExternalConnectionSignal(connectionState) {
        root.isConnected = connectionState
    }

    // Yeni fonksiyon: Bağlantı durumunu kontrol eden ve sinyal gönderen
    function handleConnectionState(connect) {
        root.isConnected = connect
        if (root.isUDP) {
            root.udpConnectionSignal(connect, ipInput.text, parseInt(portInput.text))
        } else {
            root.connectionSignal(connect, ipInput.text, parseInt(portInput.text))
        }
    }

    Rectangle {
        id: mainContainer
        width: parent.width
        height: parent.height
        color: "#80000000"
        radius: 20
        border.color: "#444444"

        ColumnLayout {
            id: columnLayout
            anchors.fill: parent
            anchors.margins: 10
            spacing: 6

            Text {
                text: "Connection Setup"
                color: "#ffffff"
                font.pixelSize: 12
                font.family: "#9A9EB1"
                opacity: 0.8
                Layout.alignment: Qt.AlignHCenter
            }

            property real inputWidth: (columnLayout.width - 20) * 0.9

            // Protocol Selection Row
            Row {
                Layout.alignment: Qt.AlignHCenter
                spacing: 10

                Rectangle {
                    id: tcpButton
                    width: 60
                    height: 24
                    radius: 12
                    color: !root.isUDP ? "#8021DF56" : "#40ffffff"
                    border.color: !root.isUDP ? "#21DF56" : "#444444"
                    border.width: 1

                    Text {
                        anchors.centerIn: parent
                        text: "TCP"
                        color: "#ffffff"
                        font.pixelSize: 11
                        font.family: "#9A9EB1"
                        font.bold: !root.isUDP
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (root.isConnected) {
                                // Bağlantıyı kapat ve protokolü değiştir
                                root.handleConnectionState(false)
                                root.isUDP = false
                            } else {
                                root.isUDP = false
                            }
                        }
                    }
                }

                Rectangle {
                    id: udpButton
                    width: 60
                    height: 24
                    radius: 12
                    color: root.isUDP ? "#8021DF56" : "#40ffffff"
                    border.color: root.isUDP ? "#21DF56" : "#444444"
                    border.width: 1

                    Text {
                        anchors.centerIn: parent
                        text: "UDP"
                        color: "#ffffff"
                        font.pixelSize: 11
                        font.family: "#9A9EB1"
                        font.bold: root.isUDP
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            if (root.isConnected) {
                                // Bağlantıyı kapat ve protokolü değiştir
                                root.handleConnectionState(false)
                                root.isUDP = true
                            } else {
                                root.isUDP = true
                            }
                        }
                    }
                }
            }

            TextField {
                id: ipInput
                placeholderText: "IP Address"
                text: "127.0.0.1"
                focus: true
                color: "#ffffff"
                font.family: "#9A9EB1"
                font.pixelSize: 11
                readOnly: false
                background: Rectangle {
                    color: "#40ffffff"
                    radius: 5
                }
                Layout.preferredWidth: columnLayout.inputWidth
                Layout.alignment: Qt.AlignHCenter
                selectByMouse: true
                RegularExpressionValidator {
                    regularExpression: /^(25[0-5]|2[0-4][0-9]|[0-1]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[0-1]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[0-1]?[0-9][0-9]?)\.(25[0-5]|2[0-4][0-9]|[0-1]?[0-9][0-9]?)$/
                }
            }

            TextField {
                id: portInput
                placeholderText: "Port"
                focus: true
                text: "5762"
                color: "#ffffff"
                font.family: "#9A9EB1"
                font.pixelSize: 11
                readOnly: false
                background: Rectangle {
                    color: "#40ffffff"
                    radius: 5
                }
                Layout.preferredWidth: columnLayout.inputWidth
                Layout.alignment: Qt.AlignHCenter
                selectByMouse: true
                validator: IntValidator {
                    bottom: 0
                    top: 65535
                }
            }

            Item {
                Layout.preferredWidth: columnLayout.inputWidth
                Layout.preferredHeight: 30
                Layout.alignment: Qt.AlignHCenter

                Rectangle {
                    id: connectionSlider
                    width: parent.width
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
                            font.family: "#9A9EB1"
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
                                    root.handleConnectionState(!root.isConnected)
                                }
                                sliderHandle.x = 0
                            }
                        }
                    }

                    Text {
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.rightMargin: 4
                        text: root.isConnected ? "Slide to Disconnect" : "Slide to Connect"
                        color: "#CCFFFFFF"
                        font.pixelSize: 10
                        font.family: "#9A9EB1"
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
                        font.family: "#9A9EB1"
                        font.pixelSize: 10
                        visible: !root.isValidInput
                    }
                }
            }
        }
    }
}
