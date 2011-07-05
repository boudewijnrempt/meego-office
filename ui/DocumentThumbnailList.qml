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
    }
        
    Component {
        id: delegate

        Item {
            id: thumbTile
            height: ListView.view.height
            width: {
                if(settings.currentType == DocumentListModel.TextDocumentType) {
                    return height * 0.67;
                } else if(settings.currentType == DocumentListModel.PDFDocumentType) {
                    return height * 0.67;
                } else if(settings.currentType == DocumentListModel.PresentationType) {
                    return height * 1.2;
                } else if(settings.currentType == DocumentListModel.SpreadsheetType) {
                    return height * 1.0;
                } else {
                    return height * 1.0;
                }
            }

            Rectangle {
                id: imageFrame

                y: parent.y + 3
                anchors.horizontalCenter: parent.horizontalCenter
                width: parent.width - 6
                height: parent.height - 36

                border.color: ListView.isCurrentItem ? "green" : "black"
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
                text: root.model.hasOwnPageNumbering() ? model.pageName : ""
                height: 15
                font.pointSize: 10
                font.weight: Font.Light
                anchors.top: imageFrame.bottom
                anchors.horizontalCenter: parent.horizontalCenter
            }
            Text {
                id: txtPageNumber
                text: model.pageNumber
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
