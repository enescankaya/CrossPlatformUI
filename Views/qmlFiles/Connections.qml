pragma ComponentBehavior: Bound
import QtQuick 2.15
import QtQuick.Layouts 1.15
Item {
    id: connectionStatus
    width: 80
    height: 50
    property int timeElapsed: 0
    Timer {
        id: timer
        interval: 1000
        running: true
        repeat: true
        onTriggered: {
            connectionStatus.timeElapsed += 1
        }
    }
    RowLayout {
        anchors.centerIn: parent
        anchors.margins: 2
        spacing: 3
        // Sayaç göstergesi
        Rectangle {

            Layout.preferredWidth: 30
            Layout.preferredHeight: 20
            radius: 10
            color: "transparent"
            border.color: "transparent"
            border.width: 1
            Text {
                anchors.centerIn: parent
                text: {
                    // timeElapsed saniye cinsindeyse dakika:saniye formatına çevir
                    var minutes = Math.floor(connectionStatus.timeElapsed / 60);
                    var seconds = connectionStatus.timeElapsed % 60;
                    return minutes + ":" + (seconds < 10 ? "0" + seconds : seconds);
                }
                color:"white"
                font.family: "#9A9EB1"
                font.pixelSize: 10
                font.bold: true
            }
        }
    }
}
