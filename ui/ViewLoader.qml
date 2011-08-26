import QtQuick 1.0
import MeeGo.Components 0.1
import org.calligra.mobile 1.0

Item {
    id: root
    signal viewingFinished()
    //property alias thumbnailListModel: thumbnailList.model
    property alias controller: loader.item

    function setFile(file) {
        if(settings.currentType == DocumentListModel.TextDocumentType) {
            loader.sourceComponent = wordsComponent;
        } else if(settings.currentType == DocumentListModel.SpreadsheetType) {
            loader.sourceComponent = tablesComponent;
        } else if(settings.currentType == DocumentListModel.PresentationType) {
            loader.sourceComponent = stageComponent;
        } else if(settings.currentType == DocumentListModel.PDFDocumentType) {
            loader.sourceComponent = pdfComponent;
        } else {
            loader.sourceComponent = invalidComponent;
        }
        rowHeader.canvas = loader.item;
        columnHeader.canvas = loader.item;

        loader.item.file = file
        loader.item.progress.connect(centralView.onProgress);
        loader.item.completed.connect(centralView.onCompleted);
        loader.item.horizontalScrollHandle = horizScrollHandle;
        loader.item.verticalScrollHandle = vertScrollHandle;

        searchToolBar.visibleChanged.connect(centralView.updateToolbarVisibleHeight);
        mainToolBar.visibleChanged.connect(centralView.updateToolbarVisibleHeight);
        loadingScreen.show();
        mainToolBar.show();
        return true
    }

    function loadDocument() {
        if(loader.item) {
            loader.item.loadDocument();
        }
    }

    Connections {
        target: window;
        onSearch: loader.item.find(needle);
        onSearchRetracted: {
            searchToolBar.hide();
            loader.item.findFinished();
            mainToolBar.show();
        }
    }

    Item {
        id: centralView
        anchors.fill: parent;

        function onCompleted() {
            thumbnailList.model.setCanvasController(loader.item);
            thumbnailList.currentIndex = 0
            thumbnailList.model.setDocument(loader.item.document, settings.currentUuid);
            slideNotes.text = loader.item.currentPageNotes;
            if(settings.currentType == DocumentListModel.SpreadsheetType) {
                headerSeparator.height = 30;
                headerSeparator.width = 66;
            } else {
                headerSeparator.height = 0;
                headerSeparator.width = 0;
            }
            loadingScreen.hide();
	    loader.item.selectionAnchorHandle = anchorMarker;
	    loader.item.selectionCursorHandle = cursorMarker;
        }

        function onProgress(progress) {
            loadingScreenProgressBar.percentage = progress;
        }

        function updateToolbarVisibleHeight() {
            if(searchToolBar.visible) {
                loader.item.setVisibleToolbarHeight(searchToolBar.height);
            } else if(mainToolBar.visible) {
                loader.item.setVisibleToolbarHeight(mainToolBar.height);
            } else {
                loader.item.setVisibleToolbarHeight(0);
            }
        }

        TablesHeader {
            id: rowHeader
            vertical: true
            anchors.top: loader.top
            anchors.left: parent.left
            anchors.bottom: loader.bottom
            width: headerSeparator.width
        }
        TablesHeader {
            id: columnHeader
            anchors.top: parent.top
            anchors.left: loader.left
            anchors.right: loader.right
            height: headerSeparator.height
        }
        Rectangle {
            id: headerSeparator
            color: "black"
            anchors.top: parent.top
            anchors.left: parent.left
            width: 0
            height: 0
        }

        Loader {
            id: loader
            anchors.top: columnHeader.bottom;
            anchors.left: rowHeader.right;
            width: centralView.width - rowHeader.width;
            height: parent.height - columnHeader.height; // - (mainToolBar.visible || searchToolBar.visible ? mainToolBar.height : 0);
            clip: true;

            Connections {
                target: loader.item
                onLinkActivated: window.openUrl(url)
                onTextCopiedToClipboard: textCopiedMessage.show()
                onFindMatchFound: {
                    if(!searchToolBar.visible) {
                        mainToolBar.hide();
                        searchToolBar.show();
                    }
                    searchToolBar.matchText = "Match " + match + " of " + loader.item.matchCount();
                }

                onShowVerticalScrollHandle: vertScrollHandle.opacity = 0.75;
                onHideVerticalScrollHandle: vertScrollHandle.opacity = 0.0;
                onShowHorizontalScrollHandle: horizScrollHandle.opacity = 0.75;
                onHideHorizontalScrollHandle: horizScrollHandle.opacity = 0.0;
                onDoubleTap: {
                    if(!window.fullsCreen) {
                        mainToolBar.hide();
                        window.fullScreen = true;
                        window.fullContent = true;
                    } else {
                        window.fullScreen = false;
                        window.fullContent = false;
                        mainToolBar.show();
                    }
                }
                onSelected: {
                    var item = cursorMarker.y < anchorMarker.y ? cursorMarker : anchorMarker;

                    var winPos;
                    if(item.y > loader.item.y + selectionMenu.sizeHintMaxHeight) {
                        winPos = mapToItem(window, item.x, item.y - 10);
                        selectionMenu.forceFingerMode = 3;
                    } else {
                        var otherItem = item == cursorMarker ? anchorMarker : cursorMarker;
                        winPos = mapToItem(window, otherItem.x, otherItem.y + 10);
                        selectionMenu.forceFingerMode = 2;
                    }
                    selectionMenu.setPosition(winPos.x, winPos.y);
                    selectionMenu.show();
                }
            }

            Marker {
                id: cursorMarker
                image: "image://themedimage/images/text-selection-marker-start";
                imageWidth: 19;
                imageHeight: 41;

                z: 10
                visible: false;

                onDragEnd: loader.item.onLongTapEnd();
            }

            Marker {
                id: anchorMarker
                image: "image://themedimage/images/text-selection-marker-end";
                imageWidth: 19;
                imageHeight: 41;

                z: 10
                visible: false;

                onDragEnd: loader.item.onLongTapEnd();
            }

            Rectangle {
                id: horizScrollHandle;

                y: window.fullScreen ? root.height - height : root.height - mainToolBar.height - height;

                color: "#000000";
                opacity: 0;
                Behavior on opacity { NumberAnimation { duration: 500 } }

                height: 5;
                z: 10;
            }
            Rectangle {
                id: vertScrollHandle;

                anchors.right: parent.right;

                color: "#000000";
                opacity: 0;
                Behavior on opacity { NumberAnimation { duration: 500 } }

                width: 5;
                z: 10;
            }

            ModalContextMenu {
                id: selectionMenu;
                forceFingerMode: 3;
                sizeHintMaxHeight: 100;

                content: ActionMenu {
                    model: [qsTr("Copy")];
                    payload: [ 0, 1 ];

                    onTriggered: {
                        switch(payload[index]) {
                            case 0: {
                                loader.item.copySelection();
                                selectionMenu.hide();
                                break;
                            }
                        }
                    }
                }
            }

            states: [
                State {
                    name: "slideNotesShown"
                    PropertyChanges { target: loader; width: centralView.width * 0.75 }
                    PropertyChanges { target: showSlideNotes; opacity: 50 }
                }
            ]
            Behavior on width { NumberAnimation { duration: 150; easing.type: Easing.InOutQuad } }
        }

        Item {
            id: slideNotes
            anchors.top: loader.top
            anchors.bottom: loader.bottom
            anchors.left: loader.right
            width: (root.width * 0.25) - rowHeader.width
            property alias text: noteText.text
            z: 10
            Rectangle {
                anchors.fill: parent
                anchors.margins: 5
                color: "#DDDDDD"
                radius: 10
                Text {
                    anchors.fill: parent;
                    anchors.margins: 5
                    id: noteText
                    font.pixelSize: 20;
                    text: loader.item.currentPageNotes
                }
            }

            Text {
                id: noNote
                anchors.centerIn: parent
                width: parent.width * 0.75
                verticalAlignment: Text.AlignVCenter
                font.pixelSize: 36
                font.letterSpacing: 8
                color: "#D0D0D0"
                text: "no notes\nto show"
                visible: ( slideNotes.text === "" )
            }
        }

        IconButton {
            id: showSlideNotes
            icon: "image://icon/view-pim-notes"
            //hasBackground: false
            width: 48
            height: 48
            iconFill: true
            anchors.right: slideNotes.left
            anchors.verticalCenter: loader.verticalCenter
            z: 10
            onClicked: loader.state = (loader.state === "slideNotesShown" ? "" : "slideNotesShown")
            visible: settings.currentType == DocumentListModel.PresentationType;
            Behavior on opacity { NumberAnimation { duration: 150 } }
        }

        ModalFog {
            id: loadingScreen;
            anchors.fill: parent;

            fogClickable: false;

            ProgressBar {
                id: loadingScreenProgressBar;
                anchors.centerIn: parent;
                width: parent.width / 4;
                percentage: 0;
            }
        }
    }

    ViewToolBar {
        id: mainToolBar;

        onShowThumbnailAction: {
            thumbnailMenu.setPosition(x, mapToItem( window, window.width, y).y);
            thumbnailMenu.show();
        }

        onPreviousPageAction: {
             if(settings.currentType == DocumentListModel.PresentationType) {
                loader.item.changeSlide(loader.item.slide - 1);
            } else {
                loader.item.goToPreviousPage();
            }
        }

        onNextPageAction: {
            if(settings.currentType == DocumentListModel.PresentationType) {
                loader.item.changeSlide(loader.item.slide + 1);
            } else {
                loader.item.goToNextPage();
            }
        }

        onShowZoomAction: {
            zoomMenu.setPosition(x, mapToItem( window, window.width, y).y);
            zoomMenu.show();
            zoomControlTool.setZoomLevel(loader.item.zoomLevel);
        }

        onFullScreenAction: {
	    mainToolBar.hide();
            window.fullScreen = true;
            window.fullContent = true;
        }

        prevNextVisible: settings.currentType != DocumentListModel.SpreadsheetType;
    }

    SearchToolBar {
        id: searchToolBar;
        height: 110;

        onFindPreviousAction: loader.item.findPrevious();
        onFindNextAction: loader.item.findNext();

        onCloseAction: window.showToolBarSearch = false;
    }

    ModalContextMenu {
        id: thumbnailMenu;
        forceFingerMode: 3;
        content: DocumentThumbnailList {
            id: thumbnailList;
            height: 172;
            width: window.width * 0.98
            onSelected: loader.item.setPage(index);
        }
    }

    ModalContextMenu {
        id: zoomMenu;
        forceFingerMode: 3;

        content: ZoomControl {
            id: zoomControlTool
            height: 250
            width: 180
            onZoomLevelChanged: loader.item.setZoomLevel(newZoomLevel)
        }
    }

    Component {
        id: wordsComponent
        WordsCanvas {
            id: document
            anchors.fill: parent
            function setPage(newPage) { page = newPage }
            onPageChanged: { thumbnailList.currentIndex = newPage; mainToolBar.pageText = "Page " + (newPage + 1) + " of " + document.pageCount; }
        }
    }

    Component {
        id: tablesComponent
        TablesCanvas {
            id: document
            anchors.fill: parent
            function setPage(newPage) { document.sheet = newPage }
            onSheetChanged: { thumbnailList.currentIndex = newIndex; mainToolBar.pageText = document.sheetName; }
        }
    }

    Component {
        id: stageComponent
        StageCanvas {
            id: document
            anchors.fill: parent
            function setPage(newPage) { slide = newPage }
            onSlideChanged: { thumbnailList.currentIndex = newSlide; mainToolBar.pageText = "Slide " + (newSlide + 1) + " of " + document.slideCount; }
        }
    }

    Component {
        id: pdfComponent;
        PDFCanvas {
            id: document;
            anchors.fill: parent;
            function setPage(newPage) { page = newPage }
            onPageChanged: { thumbnailList.currentIndex = newPage; mainToolBar.pageText = "Page " + (newPage + 1) + " of " + document.pageCount; }
        }
    }

    Component {
        id: invalidComponent;

        Rectangle {
            anchors.fill: parent;
            Text {
                anchors.centerIn: parent;
                font.pointSize: 18;
                text: "Unable to open file. Unknown file type.";
            }
        }
    }

    MouseArea {
        id: fullScreenClickTracker
        enabled: window.fullScreen;
        anchors.fill: parent
        onPressed: {
            mouse.accepted = false
            restoreButton.opacity = 1
            autoHideTimer.restart()
        }

        IconButton {
            id: restoreButton
            opacity: 0

            icon: "image://themedimage/icons/actionbar/view-smallscreen"
            iconDown: "image://themedimage/icons/actionbar/view-smallscreen-active"

            anchors.bottom: parent.bottom
            anchors.bottomMargin: 10;
            anchors.right: parent.right
            anchors.rightMargin: 10;

            Behavior on opacity { NumberAnimation { duration: 200 } }
            onClicked: { opacity = 0; autoHideTimer.stop(); window.fullScreen = false; window.fullContent = false; mainToolBar.show(); }

            Timer {
                id: autoHideTimer
                interval: 5000
                repeat: false
                running: false
                onTriggered: restoreButton.opacity = 0
            }
        }
    }

    Theme {
        id: theme;
    }
}

