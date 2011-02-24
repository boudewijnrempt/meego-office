import QtQuick 1.0
import org.calligra.mobile 1.0

Rectangle {
    width: 1024;
    height: 600;

    property alias file: document.file;

    WordsCanvas {
        id: document;
        //anchors.fill: parent;

        height: parent.height * 0.8;

        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top:  topBar.bottom;

    }

    Rectangle {
        id: topBar;

        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: parent.top;
        height: parent.height * 0.1;

        color: "#eeeeee";

        Text {
            anchors.centerIn: parent;
            text: document.file;
        }

        Button {
            anchors.right: parent.right;

            image: "image://icon/go-previous-view";
            text: "Back";
            height: parent.height;
            width: parent.width / 5;
            imageWidth: 32;
            imageHeight: 32;

            onTriggered: root.pop();
        }
    }

    Row {
        id: toolbar;
        spacing: 5;

        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top:  document.bottom;
        anchors.bottom: parent.bottom;

        Button {
            image: "image://icon/zoom-in";
            text: "Zoom In";
            height: parent.height;
            width: parent.width / parent.children.length;

            color: "#eeeeee";

            imageWidth: 32;
            imageHeight: 32;

            onTriggered: document.zoomIn();
        }
        Button {
            image: "image://icon/zoom-out";
            text: "Zoom Out";
            height: parent.height;
            width: parent.width / parent.children.length;

            color: "#eeeeee";

            imageWidth: 32;
            imageHeight: 32;

            onTriggered: document.zoomOut();
        }
    }
}
