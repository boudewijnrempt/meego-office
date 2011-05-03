import QtQuick 1.0
import org.calligra.mobile 1.0

Item {
    id: root
    property alias model: documentListView.model
    property alias currentIndex: documentListView.currentIndex
    signal selected(int index)
    function pageChanged(newPage) { documentListView.currentIndex = newPage }
    
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
                function getWidthAdjustment() {
                    if(documentListView.model.docType === DocumentThumbnailListModel.WordsDocType) {
                        return 0.67;
                    }
                    else if(documentListView.model.docType === DocumentThumbnailListModel.StageDocType) {
                        return 1.2;
                    }
                    else if(documentListView.model.docType === DocumentThumbnailListModel.TablesDocType) {
                        return 1;
                    }
                }
                width: height * getWidthAdjustment()
                
                Rectangle {
                    id: imageFrame
                    
                    y: parent.y + 3
                    anchors.horizontalCenter: parent.horizontalCenter
                    width: parent.width - 6
                    height: parent.height - 36
                    
                    border.color: thumbTile.ListView.isCurrentItem ? "green" : "black"
                    border.width: 2
                    radius: 2
                    
                    Image {
                        id: imageField
                        anchors.centerIn: parent
                        width: parent.width - 6
                        height: parent.height - 6
                        fillMode: Image.PreserveAspectFit
                        //asynchronous: true
                        source: model.thumbnail
                    }
                }
                
                Text {
                    id: txtOwnPageNumber
                    text: root.model.hasOwnPageNumbering() ? model.pagename : ""
                    height: 15
                    font.pointSize: 10
                    font.weight: Font.Light
                    anchors.top: imageFrame.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                }
                Text {
                    id: txtPageNumber
                    text: model.pagenumber
                    height: 15
                    font.pointSize: 12
                    font.weight: Font.Bold
                    anchors.top: txtOwnPageNumber.bottom
                    anchors.horizontalCenter: parent.horizontalCenter
                    color: thumbTile.ListView.isCurrentItem ? "green" : "black"
                }
                
                MouseArea {
                    anchors.fill: parent
                    onClicked: root.selected(index)
                }
            }
        }
    }
}
