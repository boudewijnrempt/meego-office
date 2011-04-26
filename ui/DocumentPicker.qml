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
    property variant columnWidth: [ 0.05, 0.3, 0.25, 0.15, 0.15, 0.1 ];
    property int columnSpacing: 20;
    property int adjustedParentWidth: parent.width - (columnSpacing * columnWidth.length)

    Image {
        id: header;
        anchors.left: parent.left;
        anchors.right: parent.right;
        source: "image://themedimage/images/media/subtitle_landscape_bar";

        visible: false;

        Text {
            id: filenameHeader;
            anchors.left: parent.left;
            anchors.leftMargin: adjustedParentWidth * columnWidth[0] + columnSpacing;
            anchors.verticalCenter: parent.verticalCenter;

            width: adjustedParentWidth * columnWidth[1];
            
            text: "Filename";
            font.pixelSize: textPixelSize;
            font.bold: true;
            color: theme.fontColorHighlightBlue;
        }

        Text {
            id: authorHeader
            visible: showType;
            
            anchors.left: filenameHeader.right;
            anchors.leftMargin: columnSpacing;
            anchors.verticalCenter: parent.verticalCenter;
            width: adjustedParentWidth * columnWidth[2];

            text: "Author";
            font.pixelSize: textPixelSize;
            color: theme.fontColorHighlightBlue;
        }

        Text {
            id: typeHeader
            visible: showType
            anchors.left: authorHeader.right;
            anchors.leftMargin: columnSpacing;
            anchors.verticalCenter: parent.verticalCenter;
            width: adjustedParentWidth * columnWidth[3];

            text: "Type";
            font.pixelSize: textPixelSize;
            color: theme.fontColorHighlightBlue;
        }

        Text {
            id: modifiedHeader
            
            anchors.left: typeHeader.right;
            anchors.leftMargin: columnSpacing;
            anchors.verticalCenter: parent.verticalCenter;
            width: adjustedParentWidth * columnWidth[4];

            text: "Modified";
            font.pixelSize: textPixelSize;
            color: theme.fontColorHighlightBlue;
        }

        Text {
            id: sizeHeader
            
            anchors.left: modifiedHeader.right;
            anchors.leftMargin: columnSpacing;
            anchors.verticalCenter: parent.verticalCenter;

            text: "Size";
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
                width: adjustedParentWidth * columnWidth[0];
                
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
                width: adjustedParentWidth * columnWidth[1];

                text: model.fileName
                font.pixelSize: textPixelSize;
                font.bold: true;
            }
            
            Text {
                id: author
                anchors.left: title.right;
                anchors.leftMargin: columnSpacing;
                anchors.verticalCenter: parent.verticalCenter;
                width: adjustedParentWidth * columnWidth[2];
                
                text: model.authorName;
                font.pixelSize: textPixelSize;
                font.bold: false
            }

            Text {
                id: type;

                visible: showType;

                anchors.left: author.right;
                anchors.leftMargin: columnSpacing;
                anchors.verticalCenter: parent.verticalCenter;

                width: adjustedParentWidth * columnWidth[3];
                
                text: model.docType;
                font.pixelSize: textPixelSize;
                font.bold: false
            }
            
            Text {
                id: mtime
                
                anchors.left: type.right
                anchors.leftMargin: columnSpacing
                anchors.verticalCenter: parent.verticalCenter
                width: adjustedParentWidth * columnWidth[4]
                text: model.modifiedTime;
                font.pixelSize: textPixelSize;
                font.bold: false
            }

            Text {
                id: size
                
                anchors.left: mtime.right
                anchors.leftMargin: columnSpacing
                anchors.verticalCenter: parent.verticalCenter
                width: adjustedParentWidth * columnWidth[5]
                function humanifySize(bytes) {
                    if (bytes == 0) return 'n/a';
                    var sizes = ['B', 'KiB', 'MiB', 'GiB', 'TiB', 'PiB', 'EiB', 'ZiB', 'YiB'],
                    i = parseInt(Math.floor(Math.log(bytes) / Math.log(1024)));
                    return Math.round(bytes / Math.pow(1024, i), 2) + sizes[i];
                }
                text: humanifySize(model.fileSize)
                font.pixelSize: textPixelSize
                font.bold: false
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
}
