
import QtQuick 2.15
import QtQuick.Layouts 1.15

Item {
    id: root
    width: 180  // Increased width for better readability
    height: 20  // Increased height for more modern look
    // Properties remain the same
    property string errorTitle: ""
    property string errorMessage: ""
    property string errorColor: ""
    property int displayDuration: 3000
    property bool show: false
    signal errorDismissed()

    function showError(title, message, color, duration) {
        errorTitle = title
        errorMessage = message
        errorColor = color
        displayDuration = duration
        show = true
        dismissTimer.restart()
    }

    Timer {
        id: dismissTimer
        interval: root.displayDuration
        onTriggered: {
            exitAnimation.start()
        }
    }

    Rectangle {
        id: errorContainer
        width: parent.width
        height: parent.height
        color:"#80000000"
        border.color: {
            switch(root.errorColor) {
                case "red": return "#FF4D4D"
                case "green": return "#4DFF6B"
                default: return "white"
            }
        }
        MouseArea {
               anchors.fill: parent
               drag.target: parent
               drag.axis: Drag.XAxis
               drag.minimumX: -parent.width
               drag.maximumX: parent.width
               onReleased: {
                   if (Math.abs(errorContainer.x) > errorContainer.width / 3) {
                       exitAnimation.start()
                   } else {
                       resetAnimation.start()
                   }
               }
               onClicked: {
                   exitAnimation.start()
               }
           }
        border.width: 1
        radius: 15  // More rounded corners
        opacity: root.show ? 1 : 0
        visible: opacity > 0
        RowLayout {
            anchors.fill: parent
            anchors.margins: 1

            // Icon Placeholder (could be replaced with actual icon)
            Rectangle {
                Layout.preferredWidth: 30
                Layout.preferredHeight: 30
                Layout.leftMargin:5
                color: {
                    switch(root.errorColor) {
                        case "red": return "#FF4D4D"
                        case "green": return "#4DFF6B"
                        default: return "white"
                    }
                }
                // Image placeholder
                Image {
                    anchors.fill: parent
                    source: {
                        switch(root.errorColor) {
                            case "red": return "qrc:/Resources/warning.png"  // Kaynağı uygun şekilde değiştirin
                            case "green": return "qrc:/Resources/success.png"
                            default: return "qrc:/Resources/info.png"
                        }
                    }
                    fillMode: Image.PreserveAspectFit
                }
                radius: width/2
                Layout.alignment: Qt.AlignVCenter
            }

            // Text Container
            ColumnLayout {
                Layout.fillWidth: true
                spacing: 2

                Text {
                    text: root.errorTitle
                    color: {
                        switch(root.errorColor) {
                            case "red": return "#FF4D4D"
                            case "green": return "#4DFF6B"
                            default: return "white"
                        }
                    }
                    font.pixelSize: 11
                    font.weight: Font.Bold
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    Layout.alignment:Qt.AlignTop
                }

                Text {
                    text: root.errorMessage
                    color: "#FFFFFF"
                    font.pixelSize: 10
                    Layout.fillWidth: true
                    elide: Text.ElideRight
                    wrapMode: Text.WrapAtWordBoundary
                    Layout.alignment:Qt.AlignTop
                }
            }
        }

        // Animations remain the same as in previous version
        ParallelAnimation {
            id: entryAnimation
            running: root.show

            NumberAnimation {
                target: errorContainer
                property: "x"
                from: root.width
                to: 0
                duration: 300
                easing.type: Easing.OutQuad
            }

            NumberAnimation {
                target: errorContainer
                property: "opacity"
                from: 0
                to: 1
                duration: 300
            }
        }

        ParallelAnimation {
            id: exitAnimation

            NumberAnimation {
                target: errorContainer
                property: "x"
                to: -root.width
                duration: 300
                easing.type: Easing.InQuad
            }

            NumberAnimation {
                target: errorContainer
                property: "opacity"
                to: 0
                duration: 300
            }

            onFinished: {
                root.show = false
                errorContainer.x = 0
                root.errorDismissed()
            }
        }

        NumberAnimation {
            id: resetAnimation
            target: errorContainer
            property: "x"
            to: 0
            duration: 200
            easing.type: Easing.OutQuad
        }
    }

}
