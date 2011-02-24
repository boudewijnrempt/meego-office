import QtQuick 1.0
import org.calligra.mobile 1.0

Rectangle {
    id: base;
    width: 1024;
    height: 600;

    property alias filter: fileSystemModel.filter;

    signal fileSelected();

    Rectangle {
        id: title;
        
        anchors.top: parent.top;
        anchors.left: parent.left;
        anchors.right: parent.right;

        height: parent.height * 0.1;

        color: "#eeeeee";

        Button {
            anchors.left: parent.left;
            width: parent.width * 0.2;
            height: parent.height;

            image: "image://icon/go-up";
            imageWidth: 32;
            imageHeight: 32;
            text: "Up";

            onTriggered: fileSystemModel.rootPath = fileSystemModel.parentFolder;
        }

        Text {
            anchors.centerIn: parent;
            text: "Open File";
        }

        Button {
            anchors.right: aboutButton.left;
            width: parent.width * 0.2;
            height: parent.height;

            image: "image://icon/go-previous-view";
            imageWidth: 32;
            imageHeight: 32;
            text: "Back";

            onTriggered: root.pop();
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

    ListView {
        id: listView;
        clip: true;

        anchors.top: title.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.bottom: parent.bottom;

        model: fileSystemModel;
        delegate: buttonDelegate;
    }

    FileSystemModel {
        id: fileSystemModel;
    }

    Component {
        id: buttonDelegate
        Button {
            id: delegateButton;

            property string path: filePath;
            property string type: fileType;

            width: listView.width;
            height: 50;

            image: "image://icon/" + fileIcon;
            imageWidth: 24;
            imageHeight: 24;
            text: fileName;

            onTriggered: {
                if(delegateButton.type == "inode/directory") {
                    fileSystemModel.rootPath = delegateButton.path;
                } else {
                    root.openFile(delegateButton.path);
                }
            }
        }
    }
}
