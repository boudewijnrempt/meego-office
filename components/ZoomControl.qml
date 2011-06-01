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
        
        Item {
            anchors.left: parent.left
            anchors.leftMargin: 30
            anchors.top: parent.verticalCenter
            
            VerticalSlider {
                id: zoomVal
                min: 50
                max: 200
                textOverlayVisible: true
                
                anchors.centerIn: parent
                
                onSliderChanged: root.zoomLevelChanged(value)
            }
        }

        IconButton {
            id: showMax
            icon: "image://icon/page-simple"
            width: 64
            height: 64
            iconFill: true
            
            anchors.right: parent.right
            anchors.top: parent.top
            
            onClicked: { zoomVal.value = 200;  root.zoomLevelChanged(200) }
        }
        IconButton {
            id: showNormal
            icon: "image://icon/page-simple"
            width: 48
            height: 48
            iconFill: true
            
            anchors.right: parent.right
            anchors.rightMargin: 8
            anchors.top : parent.verticalCenter
            anchors.topMargin: -8
            
            onClicked: { zoomVal.value = 100;  root.zoomLevelChanged(100) }
        }
        IconButton {
            id: showMin
            icon: "image://icon/page-simple"
            width: 32
            height: 32
            iconFill: true
            
            anchors.right: parent.right
            anchors.rightMargin: 16
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 8
            
            onClicked: { zoomVal.value = 50;  root.zoomLevelChanged(50) }
        }
    }
}