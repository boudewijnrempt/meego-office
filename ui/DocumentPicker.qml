import QtQuick 1.0
import MeeGo.Components 0.1
import org.calligra.mobile 1.0

Item {
    id: root
    property alias model : documentListModel;
    property alias filter: documentListModel.filter;
    property alias showHeader: header.visible;
    property bool showType: false;
    
    signal selected(int index, string filePath)

    property int textPixelSize: theme.fontPixelSizeLarge;
    property variant columnWidth: [ 0.1, 0.5, 0.4 ];
    property int columnSpacing: 20;

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
    Image {
        id: header;
        anchors.left: parent.left;
        anchors.right: parent.right;
        source: "image://themedimage/images/media/subtitle_landscape_bar";

        visible: false;

        Text {
            id: filenameHeader;
            anchors.left: parent.left;
            anchors.leftMargin: parent.width * columnWidth[0] + columnSpacing;
            anchors.verticalCenter: parent.verticalCenter;

            width: parent.width * columnWidth[1];
            
            text: "Filename";
            font.pixelSize: textPixelSize;
            font.bold: true;
            color: theme.fontColorHighlightBlue;
        }

        Text {
            visible: showType;
            
            anchors.left: filenameHeader.right;
            anchors.leftMargin: columnSpacing;
            anchors.verticalCenter: parent.verticalCenter;
            width: parent.width * columnWidth[2];

            text: "Type";
            font.pixelSize: textPixelSize;
            color: theme.fontColorHighlightBlue;
        }
    }
    
    ListView {
        id: documentListView
        clip: true

        property int textMargin: 20;

        anchors.top: header.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;
        
        model: DocumentListModel { id: documentListModel; }
        delegate: deletegateComponent;
    }

//         section.property: "sectionCategory"
//         section.criteria: ViewSection.FullString
//         section.delegate : Rectangle {
//             width: documentListView.width
//             height: text.height + 5
//             color: "#dfbd9f"
// 
//             Text {
//                 id: text
//                 anchors.left: parent.left
//                 anchors.leftMargin: 3
//                 anchors.verticalCenter: parent.verticalCenter
//                 text: section
//             }
//         }

    Component {
        id: deletegateComponent;
        
        Image {
            id: delegate

            width: parent.width
            height: 55;
            source: index % 2 == 1 ? "image://themedimage/images/browser/bg_list_pink" : "image://themedimage/images/browser/bg_list_white";

            Item {
                id: icon;
                anchors.verticalCenter: parent.verticalCenter;
                width: parent.width * columnWidth[0];
                
                Image {
                    anchors.centerIn: parent;
                    width: 22;
                    height: 22;
                    source: "image://icon/text-x-plain";
                }
            }

            Text {
                id: title

                anchors.left: icon.right;
                anchors.leftMargin: columnSpacing;
                anchors.verticalCenter: parent.verticalCenter;

                width: parent.width * columnWidth[1];
                
                text: model.fileName
                font.pixelSize: textPixelSize;
                font.bold: true;
            }

            Text {
                id: type;

                visible: showType;

                anchors.left: title.right;
                anchors.leftMargin: columnSpacing;
                anchors.verticalCenter: parent.verticalCenter;

                width: parent.width * columnWidth[2];
                
                text: model.docType;
                font.pixelSize: textPixelSize;
            }

            MouseArea {
                anchors.fill: parent
                onClicked: root.selected(index, model.filePath)
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
