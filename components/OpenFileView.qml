import QtQuick 1.0
import org.calligra.mobile 1.0

Rectangle {
    id: base;
    width: 1024;
    height: 600;

    property string file: "";
    property alias recentFiles: recentFiles;
    
    signal openFile();
    signal openFileDialog();

    ListView {
        id: listView;
        
        anchors.top: title.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;

        height: parent.height * 0.8;
        
        model: recentFiles;
        delegate: buttonDelegate;
    }

    RecentFilesModel {
        id: recentFiles;
    }

    Rectangle {
        id: title;

        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.right: parent.right;

        color: "#eeeeee";

        height: parent.height * 0.1;

        Text {
            anchors.centerIn: parent;
            text: "Recent Documents";
        }

        Button {
            id: aboutButton;
            anchors.right: parent.right;

            height: parent.height;
            width: aboutButton.height;

            labelPosition: 0;

            onTriggered: aboutDialog.opacity = 1;

            image: "image://icon/help-about";
            imageWidth: 32;
            imageHeight: 32;
        }
    }

    Button {
        anchors.top: listView.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;

        color: "#eeeeee";

        onTriggered: openFileDialog();

        text: "Open Other File...";
        image: "image://icon/document-open";
        imageWidth: 32;
        imageHeight: 32;
    }

    Component {
        id: buttonDelegate
        Button {
            id: delegateButton;
            
            width: listView.width;
            height: 50;
            text: fileName;
            
            onTriggered: {
                base.file = delegateButton.text;
                openFile();
            }
        }
    }

    AboutDialog {
        id: aboutDialog
        icon: "image://icon/kspread" // ##: Show Calligra icon here
        title: "Calligra Mobile"
        opacity: 0
        z: 1
    }
}
