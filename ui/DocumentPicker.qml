import QtQuick 1.0
import MeeGo.Components 0.1
import org.calligra.mobile 1.0

Item {
    id: root
    property alias model : documentListView.model
    signal selected(int index, string filePath)

//     TitleBar {
//         id: titleBar
//         width: root.width
//         title: qsTr("Calligra Mobile")
// 
//         rightArea: [
//             ToolButton {
//                 id: groupBy
//                 image: "image://icon/bookmarks-organize";
//                 borderPosition: "left"
// 
//                 menu: Menu {
//                     id: menu
//                     parent: root
//                     actions: [
//                         Action { text: qsTr("Group By Name"); onTriggered: model.groupBy(DocumentListModel.GroupByName) },
//                         Action { text: qsTr("Group By Type"); onTriggered: model.groupBy(DocumentListModel.GroupByDocType) }
//                     ]
//                 }
//             },
//             ToolButton {
//                 id: aboutButton
//                 image: "image://icon/help-about";
//                 borderPosition: "left"
//                 onClicked: aboutDialog.opacity = 1;
//             }
//         ]
//     }

    property int textPixelSize: theme.fontPixelSizeLarge;

    ListView {
        id: documentListView
        clip: true

        property int textMargin: 20;

        anchors.fill: parent;
        
//         anchors.left: parent.left
//         anchors.right: parent.right
//         anchors.bottom: parent.bottom 

        model: DocumentListModel { }
        delegate: Image {
            id: delegate
            
            width: parent.width
            height: 55;
            
            //color: index % 2 ? "transparent" : "#eeeeee"
            source: index % 2 == 0 ? "image://themedimage/media/music_row_landscape" : "";

            Text {
                id: text
                text: model.fileName
                anchors.left: parent.left
                anchors.leftMargin: 10
                anchors.verticalCenter: parent.verticalCenter

		font.pixelSize: textPixelSize;
            }

            MouseArea {
                anchors.fill: parent
                onClicked: root.selected(index, model.filePath)
            }
        }

        section.property: "sectionCategory"
        section.criteria: ViewSection.FullString
        section.delegate : Rectangle {
            width: documentListView.width
            height: text.height + 5
            color: "#dfbd9f"

            Text {
                id: text
                anchors.left: parent.left
                anchors.leftMargin: 3
                anchors.verticalCenter: parent.verticalCenter
                text: section
            }
        }
    }
    
    Theme {
        id: theme;
    }
    SystemPalette {
        id: activePalette;
    }
/*
    AboutDialog {
        id: aboutDialog
        icon: "image://icon/kspread" // ##: Show Calligra icon here
        title: "Calligra Mobile"
        opacity: 0
    }*/
}
