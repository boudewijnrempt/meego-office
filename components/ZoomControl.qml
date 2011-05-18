import QtQuick 1.0
import MeeGo.Components 0.1

Item {
    id: root
    signal zoomLevelChanged(int newZoomLevel)
    
    property alias zoomLevel: zoomVal.value
    
    Item {
        id: container
        anchors.fill: parent
        anchors.margins: 10
        
        Text {
            id: maxText
            text: "200"
            anchors.left: parent.left
            anchors.top: parent.top
            font.pixelSize: 25
        }
        Text {
            id: minText
            text: "50"
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            font.pixelSize: 25
        }
        
        Item {
            anchors.left: maxText.right
            anchors.leftMargin: 10
            anchors.top: parent.verticalCenter
            
            VerticalSlider {
                id: zoomVal
                min: 50
                max: 200
                
                anchors.centerIn: parent
                
                onSliderChanged: root.zoomLevelChanged(value)
            }
        }

        IconButton {
            id: showMax
            icon: "image://icon/zoom-in"
            
            anchors.right: parent.right
            anchors.top: parent.top
            
            onClicked: { zoomVal.value = 200;  root.zoomLevelChanged(200) }
        }
        IconButton {
            id: showNormal
            icon: "image://icon/zoom-original"
            
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            
            onClicked: { zoomVal.value = 100;  root.zoomLevelChanged(100) }
        }
        IconButton {
            id: showMin
            icon: "image://icon/zoom-out"
            
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            
            onClicked: { zoomVal.value = 50;  root.zoomLevelChanged(50) }
        }
    }
}