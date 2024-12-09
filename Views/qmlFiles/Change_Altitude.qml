pragma ComponentBehavior: Bound


import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    width: 100
    height: 70
    color: "#80000000"
    radius: 8

    signal altitudeChanged(int newAltitude)
    property real currentAltitude: 0
    property real previousAltitude: 0

    property color accentColor: "#ffffff"
    property color backgroundColor: "#40ffffff"
    property int animationDuration: 150

    function formatAltitude(value, locale) {
        return locale.toString(value, 'f', 0);
    }

    function validateAndUpdateAltitude(newValue) {
        if (isNaN(newValue) || newValue < customInput.minValue) {
            return customInput.minValue;
        }
        if (newValue > customInput.maxValue) {
            return customInput.maxValue;
        }
        return newValue;
    }

    function setAltitude(newAltitude) {
        var validatedAltitude = validateAndUpdateAltitude(newAltitude);
        currentAltitude = validatedAltitude;
        altitudeTextInput.text = formatAltitude(validatedAltitude, Qt.locale());
        altitudeChanged(validatedAltitude);
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 6
        spacing: 6

        Text {
            Layout.alignment: Qt.AlignHCenter
            text: "Altitude"
            color: root.accentColor
            font.pixelSize: 12
            font.weight: Font.Medium
        }

        Rectangle {
            id: customInput
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: parent.width - 8
            Layout.preferredHeight: 32
            color: root.backgroundColor
            radius: 6

            property real minValue: 0
            property real maxValue: 3000
            property real stepSize: 1
            property real accelerationMultiplier: 5

            Timer {
                id: incrementTimer
                interval: 100
                repeat: true
                onTriggered: {
                    var step = customInput.stepSize * customInput.accelerationMultiplier;
                    var newValue = root.currentAltitude + step;
                    root.currentAltitude = newValue;
                    altitudeTextInput.text = root.formatAltitude(root.currentAltitude, Qt.locale());
                }
            }

            Timer {
                id: decrementTimer
                interval: 100
                repeat: true
                onTriggered: {
                    var step = customInput.stepSize * customInput.accelerationMultiplier;
                    var newValue = root.currentAltitude - step;
                    root.currentAltitude = newValue;
                    altitudeTextInput.text = root.formatAltitude(root.currentAltitude, Qt.locale());
                }
            }

            Rectangle {
                id: decrementButton
                width: 28
                height: parent.height
                color: "transparent"
                radius: 6

                Rectangle {
                    anchors.fill: parent
                    color: decrementArea.pressed ? root.accentColor : "transparent"
                    opacity: 0.1
                    radius: 6
                }

                Text {
                    text: "−"
                    color: root.accentColor
                    anchors.centerIn: parent
                    font.pixelSize: 16
                }

                MouseArea {
                    id: decrementArea
                    anchors.fill: parent
                    onPressed: {
                        var newValue = root.currentAltitude - customInput.stepSize;
                        var validatedAltitude = root.validateAndUpdateAltitude(newValue);
                        root.currentAltitude = validatedAltitude;
                        altitudeTextInput.text = root.formatAltitude(root.currentAltitude, Qt.locale());
                        decrementTimer.start();
                    }
                    onReleased: decrementTimer.stop();
                }
            }

            TextInput {
                id: altitudeTextInput
                anchors.centerIn: parent
                width: parent.width - decrementButton.width - incrementButton.width
                text: root.formatAltitude(root.currentAltitude, Qt.locale())
                font.pixelSize: 13
                color: root.accentColor
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                selectByMouse: true
                validator: DoubleValidator {
                    bottom: customInput.minValue
                    top: customInput.maxValue
                }

                onTextEdited: {
                    var inputValue = parseFloat(text);
                    if (!isNaN(inputValue)) {
                        var validatedAltitude = root.validateAndUpdateAltitude(inputValue);
                        root.currentAltitude = validatedAltitude;
                    } else {
                        altitudeTextInput.text = root.formatAltitude(root.currentAltitude, Qt.locale());
                    }
                }
            }

            Rectangle {
                id: incrementButton
                width: 28
                height: parent.height
                anchors.right: parent.right
                color: "transparent"
                radius: 6

                Rectangle {
                    anchors.fill: parent
                    color: incrementArea.pressed ? root.accentColor : "transparent"
                    opacity: 0.1
                    radius: 6
                }

                Text {
                    text: "+"
                    color: root.accentColor
                    anchors.centerIn: parent
                    font.pixelSize: 16
                }

                MouseArea {
                    id: incrementArea
                    anchors.fill: parent
                    onPressed: {
                        var newValue = root.currentAltitude + customInput.stepSize;
                        var validatedAltitude = root.validateAndUpdateAltitude(newValue);
                        root.currentAltitude = validatedAltitude;
                        altitudeTextInput.text = root.formatAltitude(root.currentAltitude, Qt.locale());
                        incrementTimer.start();
                    }
                    onReleased: incrementTimer.stop();
                }
            }
        }

        Button {
            id: applyButton
            Layout.alignment: Qt.AlignHCenter
            Layout.preferredWidth: parent.width - 8
            Layout.preferredHeight: 26
            enabled: root.currentAltitude !== root.previousAltitude

            opacity: enabled ? 1.0 : 0.5

            Behavior on opacity {
                NumberAnimation { duration: root.animationDuration }
            }

            contentItem: Text {
                text: "Apply"
                color: root.accentColor
                font.pixelSize: 12
                font.weight: Font.Medium
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }

            background: Rectangle {
                color: {
                    if (!applyButton.enabled) {
                        return Qt.rgba(0.25, 0.25, 0.25, 0.25);
                    }
                    return applyButton.pressed ? Qt.darker(root.backgroundColor, 1.1) : root.backgroundColor;
                }
                radius: 6

                Rectangle {
                    width: parent.width
                    height: 2
                    color: applyButton.enabled ? root.accentColor : "#40ffffff"
                    anchors.bottom: parent.bottom
                    radius: 1

                    Behavior on color {
                        ColorAnimation { duration: root.animationDuration }
                    }
                }
            }

            onClicked: {
                root.previousAltitude = root.currentAltitude;
                root.setAltitude(root.currentAltitude);
            }
        }
    }
}
