import QtQuick 1.0
import org.calligra.mobile 1.0

Rectangle {
    id: root
    property alias model: documentListView.model
    property alias currentIndex: documentListView.currentIndex
    signal selected(int index)
    color: "white"

    
    ListView {
        id: documentListView
        clip: true
        orientation: ListView.Horizontal
        
        anchors.fill: parent
        
        model: DocumentThumbnailListModel { }
        delegate: delegate
        
        Component {
            id: delegate
            
            Item {
                id: thumbTile
                height: ListView.view.height
                width: height * 0.6
                
                Rectangle {
                    id: imageFrame
                    
                    anchors.top: parent.top
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width
                    height: parent.height - 30
                    
                    border.color: thumbTile.ListView.isCurrentItem ? "green" : "black"
                    border.width: 2
                    
                    Image {
                        id: imageField
                        fillMode: Image.PreserveAspectFit
                        anchors.fill: parent
                    }
                }
                
                Text {
                    id: txtOwnPageNumber
                    text: root.model.hasOwnPageNumbering() ? model.pagename : ""
                    height: 15
                    anchors.top: imageFrame.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Text {
                    id: txtPageNumber
                    text: model.pagenumber
                    height: 15
                    anchors.top: txtOwnPageNumber.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: thumbTile.ListView.isCurrentItem ? "green" : "black"
                }
                
                MouseArea {
                    anchors.fill: parent
                    onClicked: { documentListView.currentIndex = index; root.selected(index) }
                }
            }
        }
    }
}
