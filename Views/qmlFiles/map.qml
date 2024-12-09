pragma ComponentBehavior: Bound
import QtQuick 2.9
import QtLocation 6.8
import QtPositioning 5.6
import QtQuick.Window 2.0

Rectangle {
    id: window
    width: 640
    height: 480
    property MapQuickItem markerItem: null
    property MapQuickItem rightClickMarkerItem: null  // Separate marker for right-click
    property double oldLat: 39.9334
    property double oldLng: 32.8597
    property double heading: 0.0
    property Component comMarker: mapMarker
    property Component rightClickMarker: rightClickMapMarker
    signal rightClickSignal(double lat, double lng)
    signal removerightClickSignal()
    Plugin {
        id: mapPlugin
        name: "osm"
        PluginParameter {
            name: "osm.mapping.custom.host";
            value: "https://tile.openstreetmap.org/"
        }
    }

    Map {
        id: mapView
        anchors.fill: parent
        plugin: mapPlugin
        center: QtPositioning.coordinate(window.oldLat, window.oldLng)
        zoomLevel: 15
        activeMapType: supportedMapTypes[6]
    }
    function zoomOut() {
            if (mapView.zoomLevel > 2) {
                mapView.zoomLevel -= 1;
            }
        }

        function zoomIn() {
            if (mapView.zoomLevel < 20) {
                mapView.zoomLevel += 1;
            }
        }

        function setDefaultLocation() {
            mapView.center = QtPositioning.coordinate(39.9334,  32.8597);
            addMarker(39.9334, 32.8597, 0)
        }
    MouseArea {
        acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton
        id: mapPanArea
        anchors.fill: parent
        drag.target: null
        property real startLat: mapView.center.latitude
        property real startLng: mapView.center.longitude
        property real startX: 0
        property real startY: 0

        onPressed: function(mouse) {
            startLat = mapView.center.latitude
            startLng = mapView.center.longitude
            startX = mouse.x
            startY = mouse.y
        }

        onReleased: function(mouse) {
            window.oldLat = mapView.center.latitude
            window.oldLng = mapView.center.longitude
        }

            onPositionChanged: function(mouse) {
                if (mouse.buttons & Qt.LeftButton) {
                    const zoomFactor = 1 / Math.pow(2, mapView.zoomLevel - 15); // Zoom seviyesine göre daha kontrollü hassasiyet
                    const deltaX = (startX - mouse.x) * zoomFactor * 0.0001; // Yavaşlatılmış kaydırma
                    const deltaY = (mouse.y - startY) * zoomFactor * 0.0001; // Yavaşlatılmış kaydırma
                    mapView.center = QtPositioning.coordinate(startLat + deltaY, startLng + deltaX);
                }
            }



        onWheel: function(wheel) {
            if (wheel.angleDelta.y > 0 && mapView.zoomLevel < 20) {
                mapView.zoomLevel += 0.5
            } else if (wheel.angleDelta.y < 0 && mapView.zoomLevel > 2) {
                mapView.zoomLevel -= 0.5
            }
        }

        onClicked: function(mouse) {
            if (mouse.button === Qt.RightButton) {
                           const clickedCoord = mapView.toCoordinate(Qt.point(mouse.x, mouse.y))
                           addRightClickMarker(clickedCoord.latitude, clickedCoord.longitude)
                            rightClickSignal(clickedCoord.latitude, clickedCoord.longitude)
                       }
                       if (mouse.button === Qt.MiddleButton) {
                           removeRightClickMarker()
                           removerightClickSignal()
                       }
        }
    }
    function removeRightClickMarker() {
         if (rightClickMarkerItem) {
             mapView.removeMapItem(rightClickMarkerItem)
             rightClickMarkerItem = null  // Marker'ı kaldırınca null yapıyoruz
         }
     }
    Timer {
        id: centerChangeTimer
        interval: 3000
        repeat: true
        running: true

        onTriggered: {
            setCenter(window.oldLat, window.oldLng)
        }
    }

    function setCenter(lat, lng) {
        mapView.center = QtPositioning.coordinate(lat, lng)
        window.oldLat = lat
        window.oldLng = lng
    }

    function addMarker(lat, lng, heading) {
        window.heading = heading
        window.oldLat = lat
        window.oldLng = lng
        if (markerItem) {
            markerItem.coordinate = QtPositioning.coordinate(lat, lng)
            markerItem.rotation = heading
        } else {
            markerItem = comMarker.createObject(mapView, {
                coordinate: QtPositioning.coordinate(lat, lng)
            })
            mapView.addMapItem(markerItem)
        }
    }
    function addRightClickMarker(lat, lng) {
        if (rightClickMarkerItem) {
            // Eğer marker zaten varsa, sadece konumunu değiştir
            rightClickMarkerItem.coordinate = QtPositioning.coordinate(lat, lng)
        } else {
            // Eğer marker yoksa, yeni bir tane oluştur
            rightClickMarkerItem = rightClickMarker.createObject(mapView, {
                coordinate: QtPositioning.coordinate(lat, lng)
            })
            mapView.addMapItem(rightClickMarkerItem)
            rightClickMarkerItem.sourceItem.source = "https://cdn3.iconfinder.com/data/icons/transport-29/100/18-512.png"
        }
    }

    Component {
        id: mapMarker
        MapQuickItem {
            id: markerImg
            anchorPoint.x: image.width / 2
            anchorPoint.y: image.height / 2
            coordinate: QtPositioning.coordinate(window.oldLat, window.oldLng)
            sourceItem: Image {
                id: image
                source: "https://cdn.pixabay.com/photo/2014/04/03/00/40/airplane-309073_1280.png"
                width: 32
                height: 32
            }
        }
    }

    Component {
        id: rightClickMapMarker
        MapQuickItem {
            id: rightClickMarkerImg
            anchorPoint.x: image.width / 2
            anchorPoint.y: image.height / 2
            sourceItem: Image {
                id: image
                source: "https://cdn3.iconfinder.com/data/icons/transport-29/100/18-512.png"
                width: 32
                height: 32
            }
        }
    }
}
