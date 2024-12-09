pragma ComponentBehavior: Bound
import QtQuick 2.15
import QtQuick.Controls 2.15

Item {
    width: 150
    height: 170
    Rectangle {
        id: zoomSliderContainer
        width: 45
        height: 170
        color: "#80000000"
        radius: 20
        border.color: "#444444"
        visible: true  // Slider başlangıçta görünsün
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.margins: 10

        Column {
            anchors.fill: parent
            anchors.margins: 10
            spacing: 10

            Text {
                text: "Zoom"
                color: "#ffffff"
                font.family:"#9A9EB1"
                font.pixelSize: 13
                anchors.horizontalCenter: parent.horizontalCenter
                opacity: 0.8  // Daha şeffaf
            }

            Rectangle {
                id: sliderTrack
                width: 10
                height: parent.height - 60
                color: "#555555"
                radius: 5
                anchors.horizontalCenter: parent.horizontalCenter
                opacity: 0.8  // Daha şeffaf

                Rectangle {
                    id: sliderHandle
                    width: 20
                    height: 20
                    radius: 10
                    color: "#ffffff" // Şeffaf beyazımsı renk
                    opacity: 0.6  // Daha şeffaf
                    border.color: "#ffffff"
                    anchors.horizontalCenter: sliderTrack.horizontalCenter
                    y: (zoomControl.value - zoomControl.from) / (zoomControl.to - zoomControl.from) * (sliderTrack.height - height)

                    MouseArea {
                        id: handleArea
                        anchors.fill: parent
                        cursorShape: Qt.PointingHandCursor
                        drag.target: parent
                        drag.axis: Drag.YAxis
                        drag.minimumY: 0
                        drag.maximumY: sliderTrack.height - sliderHandle.height
                        onPositionChanged: {
                            zoomControl.value = zoomControl.from + (sliderHandle.y / (sliderTrack.height - sliderHandle.height)) * (zoomControl.to - zoomControl.from);
                        }
                    }
                }
            }

            Text {
                id: zoomValueDisplay
                text: Math.round(zoomControl.value) + "%"
                color: "#ffffff"
                font.pixelSize: 12
                font.family:"#9A9EB1"
                anchors.horizontalCenter: parent.horizontalCenter
                opacity: 0.9  // Daha şeffaf
            }
        }
    }

    Slider {
        id: zoomControl
        from: 100
        to: 0
        stepSize: 1
        visible: false
    }
}
