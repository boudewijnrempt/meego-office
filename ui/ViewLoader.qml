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

        leftArea: ToolButton {
            id: backButton
            image: "image://icon/draw-arrow-back";
            onClicked: root.viewingFinished()
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

        ToolButton {
            image: "image://icon/zoom-in"
            onClicked: loader.item.zoomIn()
        }
        ToolButton {
            image: "image://icon/zoom-out"
            onClicked: loader.item.zoomOut()
        }
        ToolButton {
            image: "image://icon/zoom-original"
            opacity: (loader.item && loader.item.resetZoom) ? 1 : 0
            onClicked: loader.item.resetZoom()
        }
        ToolButton {
            image: "image://icon/go-previous"
            opacity: (loader.item && loader.item.previousSheet) ? 1 : 0
            onClicked: loader.item.previousSheet()
        }
        ToolButton {
            image: "image://icon/go-next"
            opacity: (loader.item && loader.item.previousSheet) ? 1 : 0
            onClicked: loader.item.nextSheet()
        }
        ToolButton {
            image: "image://icon/view-fullscreen"
            onClicked: window.toggleFullScreen()
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

