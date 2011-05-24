import QtQuick 1.0
import MeeGo.Components 0.1
import org.calligra.mobile 1.0

Item {
    id: root
    signal viewingFinished()
    property alias thumbnailListModel: thumbnailList.model
    property alias controller: loader.item

    function setFile(file) {
        if(settings.currentType == DocumentListModel.TextDocumentType) {
            loader.sourceComponent = wordsComponent;
        } else if(settings.currentType == DocumentListModel.SpreadsheetType) {
            loader.sourceComponent = tablesComponent;
        } else if(settings.currentType == DocumentListModel.PresentationType) {
            loader.sourceComponent = stageComponent;
        } else {
            console.log("Unable to open file. Unrecognised file type.");
        }

        loader.item.file = file
        loader.item.progress.connect(centralView.onProgress);
        loader.item.completed.connect(centralView.onCompleted);
        loader.item.horizontalScrollHandle = horizScrollHandle;
        loader.item.verticalScrollHandle = vertScrollHandle;
        window.search.connect(centralView.find);
        window.showToolBarSearchChanged.connect(centralView.searchVisibleChanged);
        loadingScreen.show();
        mainToolBar.show();
        return true
    }

    function loadDocument() {
        if(loader.item) {
            loader.item.loadDocument();
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
            loadingScreen.hide();
	    loader.item.selectionAnchorHandle = anchorMarker;
	    loader.item.selectionCursorHandle = cursorMarker;
        }

        function onProgress(progress) {
            loadingScreenProgressBar.percentage = progress;
        }

        function find(text) {
            loader.item.find(text);
        }

        function searchVisibleChanged() {
            if(!window.showToolBarSearch) {
                searchToolBar.hide();
                loader.item.findFinished();
                mainToolBar.show();
            }
        }

        Loader {
            id: loader
            anchors.top: parent.top;
            anchors.left: parent.left;
            width: centralView.width;
            clip: true;

            height: parent.height - (mainToolBar.visible || searchToolBar.visible ? mainToolBar.height : 0);

            Connections {
                target: loader.item
                onLinkActivated: window.openUrl(url)
                onTextCopiedToClipboard: textCopiedMessage.show()
                onFindMatchFound: {
                    if(!searchToolBar.visible) {
                        mainToolBar.hide();
                        searchToolBar.show();
                    }
                    findMatchesText.text = "Match " + match + " of " + loader.item.matchCount();
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
		    var winPos = mapToItem(window, origin.x, origin.y);
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

                anchors.bottom: parent.bottom;

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
                }
            ]
            Behavior on width { NumberAnimation { duration: 150; easing.type: Easing.InOutQuad } }
        }

        Item {
            id: slideNotes
            anchors.top: loader.top
            anchors.bottom: loader.bottom
            anchors.left: loader.right
            width: root.width * 0.25
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
        }
       
        IconButton {
            id: showSlideNotes
            icon: "image://themedimage/icons/actionbar/view-smallscreen"
            //hasBackground: false
            anchors.right: slideNotes.left
            anchors.verticalCenter: centralView.verticalCenter
            z: 10
            onClicked: loader.state = (loader.state === "slideNotesShown" ? "" : "slideNotesShown")
            visible: loader.item.canHavePageNotes
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

    BottomToolBar {
        id: mainToolBar;
        content: BottomToolBarRow {
            IconButton {
                id: showThumbnailsButton;

                anchors.left: parent.left;
                anchors.leftMargin: 10;
                anchors.verticalCenter: parent.verticalCenter;

                icon: "image://themedimage/icons/actionbar/mail-message-previous";
                hasBackground: false;

                onClicked: {
                    thumbnailMenu.setPosition(0, mapToItem( window, window.width, showThumbnailsButton.y).y);
                    thumbnailMenu.show();
                }
            }

            Button {
                id: pageDescriptionText;
                anchors.left: showThumbnailsButton.right;
                anchors.leftMargin: 10;
                anchors.verticalCenter: parent.verticalCenter;

                textColor: theme.fontColorHighlight;
                font.pixelSize: theme.toolbarFontPixelSize;
                hasBackground: false;

                Component.onCompleted: pageDescriptionText.clicked.connect(showThumbnailsButton.clicked);
            }

            IconButton {
                id: prevPageButton;

                anchors.left: pageDescriptionText.right;
                anchors.leftMargin: 10;
                anchors.verticalCenter: parent.verticalCenter;

                icon: "image://themedimage/icons/actionbar/media-backward";
                iconDown: "image://themedimage/icons/actionbar/media-backward-active";

                hasBackground: false;
                visible: settings.currentType != DocumentListModel.SpreadsheetType;

                onClicked: {
                    if(settings.currentType == DocumentListModel.PresentationType) {
                        loader.item.changeSlide(loader.item.slide - 1);
                    } else {
                        loader.item.goToPreviousPage();
                    }
                }
            }

            IconButton {
                id: nextPageButton;

                anchors.left: prevPageButton.right;
                anchors.leftMargin: 10;
                anchors.verticalCenter: parent.verticalCenter;

                icon: "image://themedimage/icons/actionbar/media-forward";
                iconDown: "image://themedimage/icons/actionbar/media-forward-active";

                hasBackground: false;
                visible: settings.currentType != DocumentListModel.SpreadsheetType;

                onClicked: {
                    if(settings.currentType == DocumentListModel.PresentationType) {
                        loader.item.changeSlide(loader.item.slide + 1);
                    } else {
                        loader.item.goToNextPage();
                    }
                }
            }

            IconButton {
                id: showZoomControlButton;

                anchors.right: zoomInButton.left
                anchors.rightMargin: 10;
                anchors.verticalCenter: parent.verticalCenter;

                icon: "image://themedimage/icons/actionbar/mail-message-previous";
                hasBackground: false;

                onClicked: {
                    zoomMenu.setPosition(showZoomControlButton.x + (showZoomControlButton.width / 2), mapToItem( window, window.width, showZoomControlButton.y).y);
                    zoomMenu.show();
                }
            }

            IconButton {
                id: zoomInButton;
                anchors.right: zoomOutButton.left;
                anchors.rightMargin: 10;
                icon: "image://icon/zoom-in";
                hasBackground: false;

                onClicked: {
                    loader.item.zoomIn();
                }
            }

            IconButton {
                id: zoomOutButton;
                anchors.centerIn: parent;
                icon: "image://icon/zoom-out";
                hasBackground: false;

                onClicked: {
                    loader.item.zoomOut();
                }
            }
            IconButton {
                id: zoomRestoreButton;
                anchors.left: zoomOutButton.right;
                anchors.leftMargin: 10;
                icon: "image://icon/zoom-original";
                hasBackground: false;

                onClicked: {
                    loader.item.resetZoom();
                }
            }


            IconButton {
                id: viewFullScreenButton;

                anchors.verticalCenter: parent.verticalCenter;
                anchors.right: parent.right;
                anchors.rightMargin: 10;

                icon: "image://themedimage/icons/actionbar/view-fullscreen";
                iconDown: "image://themedimage/icons/actionbar/view-fullscreen-active";
                hasBackground: false;

                onClicked: { mainToolBar.hide(); window.fullScreen = true; window.fullContent = true; }
            }
        }
    }

    BottomToolBar {
        id: searchToolBar;
        content: BottomToolBarRow {
            IconButton {
                id: findPreviousButton;

                anchors.right: findMatchesText.left;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.leftMargin: 5;
                anchors.rightMargin: 5;

                icon: "image://themedimage/icons/actionbar/mail-message-previous";
                hasBackground: false;

                onClicked: loader.item.findPrevious();
            }
            Text {
                id: findMatchesText;

                anchors.horizontalCenter: parent.horizontalCenter;
                anchors.leftMargin: 5;
                anchors.rightMargin: 5;
                anchors.verticalCenter: parent.verticalCenter;

                font.pixelSize: theme.toolbarFontPixelSize;
                color: theme.fontColorHighlight;
            }
            IconButton {
                id: findNextButton;

                anchors.left: findMatchesText.right;
                anchors.leftMargin: 5;
                anchors.rightMargin: 5;
                anchors.verticalCenter: parent.verticalCenter;

                icon: "image://themedimage/icons/actionbar/mail-message-next";
                hasBackground: false;

                onClicked: loader.item.findNext();
            }
            IconButton {
                id: findCloseButton;

                anchors.right: parent.right;
                anchors.leftMargin: 5;
                anchors.rightMargin: 5;
                anchors.verticalCenter: parent.verticalCenter;

                icon: "image://themedimage/images/contacts/icn_cross_up";
                iconDown: "image://themedimage/images/contacts/icn_cross_dn";
                hasBackground: false;
                //text: "Close";
                height: findNextButton.height;
                onClicked: window.showToolBarSearch = false;
            }
        }
    }

    ModalContextMenu {
        id: thumbnailMenu;
        forceFingerMode: 3;
        content: DocumentThumbnailList {
            id: thumbnailList;
            height: 130;
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
            onPageChanged: { thumbnailList.currentIndex = newPage; pageDescriptionText.text = "Page " + (newPage + 1) + " of " + document.pageCount; }
            onDocMoved: thumbnailList.state = ""
        }
    }

    Component {
        id: tablesComponent
        TablesCanvas {
            id: document
            anchors.fill: parent
            function setPage(newPage) { sheet = newPage }
            onSheetChanged: { thumbnailList.currentIndex = newIndex; pageDescriptionText.text = document.sheetName; }
            onDocMoved: thumbnailList.state = ""
        }
    }

    Component {
        id: stageComponent
        StageCanvas {
            id: document
            anchors.fill: parent
            function setPage(newPage) { slide = newPage }
            onSlideChanged: { thumbnailList.currentIndex = newSlide; pageDescriptionText.text = "Slide " + (newSlide + 1) + " of " + document.slideCount; }
            onDocMoved: thumbnailList.state = ""
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

