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
            // the 24 is the left and right margin of the imageFrame
            // A bit magic-numberish, but it's so close anyway ;)
            width: (model.pageWidthRatio * imageFrame.height) + 24

            Rectangle {
                id: imageFrame

                anchors {
                    topMargin: 3
                    bottomMargin: 3
                    // When changing these two, remember the thumbTile.width
                    leftMargin: 12
                    rightMargin: 12
                    top: parent.top
                    left: parent.left
                    right: parent.right
                }
                height: parent.height - 36

                border.color: ListView.isCurrentItem ? "green" : "black"
                border.width: 2
                radius: 2

                Image {
                    id: imageField
                    anchors.fill: parent
                    anchors.margins: 3
                    fillMode: Image.PreserveAspectFit
                    source: model.thumbnail
                }
            }

            Text {
                id: txtOwnPageNumber
                text: root.model.hasOwnPageNumbering() ? model.pageName : ""
                height: 12
                font.pixelSize: 12
                font.weight: Font.Light
                anchors.left: imageFrame.left
                anchors.right: imageFrame.right
                anchors.bottom: txtPageNumber.top
                horizontalAlignment: Text.AlignHCenter
            }
            Text {
                id: txtPageNumber
                text: model.pageNumber
                height: 20
                font.pixelSize: 20
                font.weight: Font.Bold
                anchors.left: imageFrame.left
                anchors.right: imageFrame.right
                anchors.bottom: parent.bottom
                anchors.bottomMargin: root.model.hasOwnPageNumbering() ? 0 : 6
                horizontalAlignment: Text.AlignHCenter
                color: thumbTile.ListView.isCurrentItem ? "green" : "black"
            }

            MouseArea {
                anchors.fill: parent
                onClicked: root.selected(index)
            }
        }
    }
}
