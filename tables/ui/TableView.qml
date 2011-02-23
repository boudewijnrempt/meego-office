import Qt 4.7
import org.calligra.mobile 1.0

Rectangle {
    id: base;
    width: 1024;
    height: 600;

    property alias file: document.file;

    TablesCanvas {
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

        y: 0;
        height: parent.height * 0.1;

        color: "#eeeeee";

        Text {
            anchors.centerIn: parent;
            text: document.file;
        }

        Button {
            anchors.right: aboutButton.left;
            
            image: "image://icon/go-previous-view";
            text: "Back";
            height: parent.height;
            width: parent.width / 5;
            imageWidth: 32;
            imageHeight: 32;

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

    Rectangle {
        id: toolBar;

        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top:  document.bottom;

        height: parent.height * 0.1;

        Row {
            anchors.fill: parent;

            Button {
                id: goPrevious;
                image: "image://icon/go-previous";
                text: "Previous";

                height: parent.height;
                width: parent.width / parent.children.length;

                color: "#eeeeee";

                imageWidth: 32;
                imageHeight: 32;
                //enabled: document.hasPreviousSheet;

                onTriggered: document.previousSheet();
            }
            Button {
                id: goNext;
                image: "image://icon/go-next";
                text: "Next";

                height: parent.height;
                width: parent.width / parent.children.length;

                color: "#eeeeee";

                imageWidth: 32;
                imageHeight: 32;

                labelPosition: 2;
                //enabled: document.hasNextSheet;

                onTriggered: document.nextSheet();
            }
            Button {
                id: zoomIn;
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
                id: zoomOut;
                image: "image://icon/zoom-out";
                text: "Zoom Out";

                height: parent.height;
                width: parent.width / parent.children.length;

                color: "#eeeeee";

                imageWidth: 32;
                imageHeight: 32;

                onTriggered: document.zoomOut();
            }
            Button {
                id: zoomOriginal;
                image: "image://icon/zoom-original";
                text: "Original Size";

                height: parent.height;
                width: parent.width / parent.children.length;

                color: "#eeeeee";

                imageWidth: 32;
                imageHeight: 32;

                onTriggered: document.resetZoom();
            }
            Button {
                id: viewFullscreen;
                image: "image://icon/view-fullscreen";
                text: "View Fullscreen";

                height: parent.height;
                width:  parent.width / parent.children.length;

                color: "#eeeeee";

                imageWidth: 32;
                imageHeight: 32;

                onTriggered: {
                    window.toggleFullScreen();
                    base.state = "fullscreen";
                }
            }
        }
    }

    MouseArea {
        id: mouseArea;

        anchors.fill: parent;

        onPressed: {
            mouse.accepted = false;
            if(base.state == "fullscreen") {
                exitFullscreen.opacity = 1;
            }
        }
    }

    Button {
        id: exitFullscreen;
        image: "image://icon/view-restore";

        anchors.right: parent.right;
        anchors.bottom: toolBar.top;

        width: 0.1 * parent.width;
        height: 0.1 * parent.height;

        labelPosition: 0;

        opacity: 0;
        Behavior on opacity {
            NumberAnimation { duration: 500; }
        }

        onOpacityChanged: {
            if(exitFullscreen.opacity > 0) {
                exitFullscreenHideTimer.restart();
            }
        }

        onTriggered: {
            window.toggleFullScreen();
            base.state = "";
        }

        Timer {
            id: exitFullscreenHideTimer;
            interval: 5000;
            running: false;
            repeat: false;

            onTriggered: {
                exitFullscreen.opacity = 0;
            }
        }
    }

    states: [
        State {
            name: "fullscreen";
            PropertyChanges { target: topBar; y: -topBar.height; }
            PropertyChanges { target: document; height: base.height + 2; }
            PropertyChanges { target: exitFullscreen; opacity: 1; }
        }
    ]

    transitions: [
        Transition {
            ParallelAnimation {
                NumberAnimation { target: topBar; properties: "y"; duration: 500; }
                NumberAnimation { target: document; properties: "height"; duration: 500; }
            }
        }
    ]
}
