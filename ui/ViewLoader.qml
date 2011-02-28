import QtQuick 1.0
import org.calligra.mobile 1.0

Item {
    id: root
    signal viewingFinished()
    property alias sourceComponent: loader.sourceComponent

    function openFile(file) {
        var ext = file.substr(-3)
        console.log(file)
        console.log(ext)
        if (ext == "odt" || ext == "doc") {
            loader.sourceComponent = wordComponent
        } else if (ext == "odp" || ext == "ppt") {
            loader.sourceComponent = stageComponent
        } else if (ext == "ods" || ext == "xls") {
            loader.sourceComponent = tablesComponent
        } else {
            console.log('Unsupported extension ' + ext)
            return
        }
        loader.item.file = file
        titleBar.title = file
    }

    TitleBar {
        id: titleBar
        width: root.width

        leftArea: Button {
            id: backButton
            height: imageHeight
            width: imageWidth

            labelPosition: 0

            onTriggered: root.viewingFinished()

            image: "image://icon/draw-arrow-back";
            imageWidth: 32
            imageHeight: 32
        }
    }

    Loader {
        id: loader
        anchors.top: titleBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: actionBar.bottom 
    }

    ActionBar {
        id: actionBar
        width: parent.width
        anchors.bottom: parent.bottom

        Button {
            image: "image://icon/zoom-in"
            text: qsTr("Zoom In")
            height: imageHeight
            width: imageWidth

            color: "#eeeeee"

            imageWidth: 32
            imageHeight: 32

            onTriggered: loader.item.zoomIn()
        }
        Button {
            image: "image://icon/zoom-out"
            text: "Zoom Out";
            width: imageWidth
            height: imageHeight

            color: "#eeeeee"

            imageWidth: 32
            imageHeight: 32

            onTriggered: loader.item.zoomOut()
        }
        Button {
            id: zoomOriginal;
            image: "image://icon/zoom-original"
            text: qsTr("Original Size")
            width: imageWidth
            height: imageHeight

            color: "#eeeeee";

            imageWidth: 32;
            imageHeight: 32;

            opacity: (loader.item && loader.item.resetZoom) ? 1 : 0
            onTriggered: loader.item.resetZoom()
        }
        Button {
            id: goPrevious
            image: "image://icon/go-previous"
            text: qsTr("Previous")
            height: imageWidth
            width: imageHeight
 
            color: "#eeeeee"
 
            imageWidth: 32
            imageHeight: 32
            opacity: (loader.item && loader.item.previousSheet) ? 1 : 0
            //enabled: document.hasPreviousSheet
            onTriggered: loader.item.previousSheet()
        }
        Button {
            id: goNext;
            image: "image://icon/go-next"
            text: qsTr("Next")
            height: imageWidth
            width: imageHeight
 
            color: "#eeeeee";
 
            imageWidth: 32;
            imageHeight: 32;
 
            labelPosition: 2;
            opacity: (loader.item && loader.item.previousSheet) ? 1 : 0
            //enabled: document.hasNextSheet;
            onTriggered: loader.item.nextSheet()
        }
    }

    Component {
        id: wordComponent
        WordsCanvas {
            id: document
            anchors.fill: parent
        }
    }

    Component {
        id: tablesComponent
        TablesCanvas {
            id: document
            anchors.fill: parent
        }
    }

    Component {
        id: stageComponent
        StageCanvas {
            id: document
            anchors.fill: parent
        }
    }
}

