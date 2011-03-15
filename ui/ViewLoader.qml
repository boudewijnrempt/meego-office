import QtQuick 1.0
import org.calligra.mobile 1.0

Item {
    id: root
    signal viewingFinished()
    property alias sourceComponent: loader.sourceComponent

    function setFile(file) {
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
        loader.item.progress.connect(centralView.onProgress);
        loader.item.completed.connect(centralView.onCompleted);
        titleBar.title = file
    }

    state: "normal"

    states: [
        State {
            name: "normal"
            PropertyChanges { target: searchBar; y: -searchBar.height }
            PropertyChanges { target: actionBar; y: root.height-actionBar.height }
            StateChangeScript { script: window.fullScreen = false }
        },
        State {
            name: "fullScreen"
            PropertyChanges { target: searchBar; y: -(searchBar.height+titleBar.height) }
            PropertyChanges { target: actionBar; y: root.height }
            StateChangeScript { script: window.fullScreen = true }
        }
    ]

    function loadDocument() {
        loader.item.loadDocument();
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
            onClicked: { searchBar.hide(); root.viewingFinished(); centralView.state = "" }
        }

        pullDownGestureTarget: searchBar
        onPullDownGestureFinished: {
            if (searchBar.y+searchBar.height >= searchBar.height/2)
                searchBar.show()
            else
                searchBar.hide()
        }
    }

    Item {
        id: centralView
        anchors.top: titleBar.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: actionBar.bottom

        function onCompleted() {
            centralView.state = "loaded";
            loadingScreenProgressBar.progress = -1;
        }

        function onProgress(progress) {
            loadingScreenProgressBar.progress = progress;
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

        Loader {
            id: loader
            anchors.fill: parent

            opacity: 0;

            Image {
                id: selectionMarker1
                width: 16
                height: 16
                opacity: 0.5
                source: "resources/marker.png"
                z: 10
                x: loader.item ? loader.item.cursorPos.x - width/2 : -100
                y: loader.item ? loader.item.cursorPos.y - height : -100
            }

            Image {
                id: selectionMarker2
                width: 16
                height: 16
                opacity: 0.5
                source: "resources/marker.png"
                z: 10
                x: loader.item ? loader.item.anchorPos.x - width/2 : -100
                y: loader.item ? loader.item.anchorPos.y - height : -100
            }
        }

        Item {
            id: loadingScreen;
            anchors.fill: parent

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
    }

    ActionBar {
        id: actionBar
        width: parent.width
        y: parent.height - actionBar.height

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
            image: window.fullScreen ? "image://icon/view-restore" : "image://icon/view-fullscreen"
            onClicked: root.state = window.fullScreen ? "normal" : "fullScreen"
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

    MouseArea {
        id: fullScreenClickTracker
        enabled: root.state == "fullScreen"
        anchors.fill: parent
        onPressed: {
            mouse.accepted = false
            restoreButton.opacity = 1 
            autoHideTimer.restart()
        }

        ToolButton {
            id: restoreButton
            opacity: 0
            image: "image://icon/view-restore"
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            Behavior on opacity { NumberAnimation { duration: 200 } }
            onClicked: { opacity = 0; autoHideTimer.stop(); root.state = "normal" }

            Timer {
                id: autoHideTimer
                interval: 5000
                repeat: false
                running: false
                onTriggered: restoreButton.opacity = 0
            }
        }
    }
}

