import QtQuick 1.0
import MeeGo.Components 0.1
import org.calligra.mobile 1.0

Item {
    id: root
    property alias model : documentListModel;
    property alias filter: documentListModel.filter;
    property alias showHeader: header.visible;
    property bool showType: false;
    
    signal selected(int index, string filePath, string fileType, string uuid)

    property int textPixelSize: theme.fontPixelSizeLarge;
    property variant columnWidth:       [ 0.05, 0.3, 0.25, 0.15, 0.15, 0.1 ];
    property variant narrowColumnWidth: [ 0.05, 0.5, 0,    0.3,  0.3,  0 ];
    property int columnSpacing: 20;
    property int narrowSize: 700;
    property int adjustedParentWidth: parent.width - (columnSpacing * columnWidth.length)

    function getColumnWidth(column) {
        var percentage = columnWidth[column];
        if( adjustedParentWidth < narrowSize ) {
            percentage = narrowColumnWidth[column];
        }
        return adjustedParentWidth * percentage;
    }
    
    Image {
        id: header;
        anchors.left: parent.left;
        anchors.right: parent.right;
        source: "image://themedimage/images/media/subtitle_landscape_bar";

        visible: false;

        Text {
            id: filenameHeader;
            anchors.left: parent.left;
            anchors.leftMargin: getColumnWidth(0) + columnSpacing;
            anchors.verticalCenter: parent.verticalCenter;

            width: getColumnWidth(1);
            
            text: "Filename";
            font.pixelSize: textPixelSize;
            font.bold: true;
            color: theme.fontColorHighlightBlue;
        }

        Text {
            id: authorHeader
            visible: width > 5 ? true : false;
            
            anchors.left: filenameHeader.right;
            anchors.leftMargin: columnSpacing;
            anchors.verticalCenter: parent.verticalCenter;
            width: getColumnWidth(2);

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
            width: getColumnWidth(3);

            text: "Type";
            font.pixelSize: textPixelSize;
            color: theme.fontColorHighlightBlue;
        }

        Text {
            id: modifiedHeader
            
            anchors.left: typeHeader.right;
            anchors.leftMargin: columnSpacing;
            anchors.verticalCenter: parent.verticalCenter;
            width: getColumnWidth(4);

            text: "Modified";
            font.pixelSize: textPixelSize;
            color: theme.fontColorHighlightBlue;
        }

        Text {
            id: sizeHeader
            visible: width > 5 ? true : false;
            
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
        
        Rectangle {
            id: delegate

            width: parent.width
            height: 55;
            color: index % 2 == 1 ? "#faf7f7" : "#ffffff";

            Item {
                id: icon;
                anchors.verticalCenter: parent.verticalCenter;
                width: getColumnWidth(0);
                
                Image {
                    anchors.centerIn: parent;
                    width: 32;
                    height: 32;
                    source: "image://icon/by-path/" + filePath;
                }
            }

            Text {
                id: title
                anchors.left: icon.right;
                anchors.leftMargin: columnSpacing;
                anchors.verticalCenter: parent.verticalCenter;
                width: getColumnWidth(1);

                text: fileName;
                elide: Text.ElideRight;
                font.pixelSize: textPixelSize;
                font.bold: true;
            }
            
            Text {
                id: author
                visible: width > 5 ? true : false;
                anchors.left: title.right;
                anchors.leftMargin: columnSpacing;
                anchors.verticalCenter: parent.verticalCenter;
                width: getColumnWidth(2);
                
                text: authorName;
                font.pixelSize: textPixelSize;
                font.bold: false
            }

            Text {
                id: type;

                visible: showType;

                anchors.left: author.right;
                anchors.leftMargin: columnSpacing;
                anchors.verticalCenter: parent.verticalCenter;

                width: getColumnWidth(3);
                
                text: documentTypeToString(docType);
                font.pixelSize: textPixelSize;
                font.bold: false
            }
            
            Text {
                id: mtime
                
                anchors.left: type.right
                anchors.leftMargin: columnSpacing
                anchors.verticalCenter: parent.verticalCenter
                width: getColumnWidth(4)
                text: modifiedTime;
                font.pixelSize: textPixelSize;
                font.bold: false
            }

            Text {
                id: size
                visible: width > 5 ? true : false;
                
                anchors.left: mtime.right
                anchors.leftMargin: columnSpacing
                anchors.verticalCenter: parent.verticalCenter
                width: getColumnWidth(5)
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
                onClicked: root.selected(index, filePath, docType, model.uuid)
            }
        }
    }
    
    Theme {
        id: theme;
    }
    SystemPalette {
        id: activePalette;
    }

    Connections {
        target: window;

        onSearch: documentListModel.search = needle;
    }

    function documentTypeToString(type) {
        switch(type) {
	    case 1: return qsTr('Text Document');
	    case 2: return qsTr('Presentation');
	    case 3: return qsTr('Spreadsheet');
            case 4: return qsTr('PDF Document');
	    default: return qsTr('Unknown');
        }
    }
}
