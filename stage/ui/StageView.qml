import Qt 4.7
import org.calligra.mobile 1.0

Rectangle {
    width: 1024;
    height: 600;

    property alias file: document.file;

    StageCanvas {
        id: document;
        //anchors.fill: parent;

        height: parent.height * 0.9;

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
}