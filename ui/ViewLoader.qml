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
        loader.item.progress.connect(onProgress);
        loader.item.completed.connect(onCompleted);
        titleBar.title = file
    }

    function loadDocument() {
        loader.item.loadDocument();
    }

    function onCompleted() {
        state = "loaded";
        loadingScreenProgressBar.progress = -1;
    }

    function onProgress(progress) {
        loadingScreenProgressBar.progress = progress;
    }

    SearchBar {
        id: searchBar
        width: root.width
        y: -searchBar.height
        Behavior on y { NumberAnimation { duration: 200 } }
        onSearch: console.log('search and highlight ' + str)
        onSearchNext: console.log('scroll to next result')
        onFinished: hide()

        function show() { y = 0 }
        function hide() { y = -searchBar.height }
    }

    TitleBar {
        id: titleBar
        width: root.width
        anchors.top: searchBar.bottom

        leftArea: ToolButton {
            id: backButton
            image: "image://icon/draw-arrow-back";
            borderPosition: "right"
            onClicked: { searchBar.hide(); root.viewingFinished(); }
        }

        pullDownGestureTarget: searchBar
        onPullDownGestureFinished: {
            if (searchBar.y+searchBar.height >= searchBar.height/2)
                searchBar.show()
            else
                searchBar.hide()
        }
    }

    Loader {
        id: loader
        anchors.top: titleBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: actionBar.bottom

        opacity: 0;
    }

    Item {
        id: loadingScreen;
        anchors.top: titleBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: actionBar.bottom

        Rectangle {
            anchors.fill: parent;
            color: "#000000";
            opacity: 0.5;
        }

        ProgressBar {
            id: loadingScreenProgressBar;
            anchors.centerIn: parent;
            width: 200;
        }
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

    states: [
        State {
            name: "loaded";
            PropertyChanges { target: loader; opacity: 1; }
            PropertyChanges { target: loadingScreen; opacity: 0; }
        }
    ]

    transitions: [
        Transition {
            to: "loaded"
            NumberAnimation { properties: "opacity"; duration: 500 }
        }
    ]
}

