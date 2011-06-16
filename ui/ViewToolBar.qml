import QtQuick 1.0
import MeeGo.Components 0.1
import org.calligra.mobile 1.0

BottomToolBar {
    id: base;
    
    signal showThumbnailAction(real x, real y);
    signal nextPageAction();
    signal previousPageAction();
    signal showZoomAction(real x, real y);
    signal fullScreenAction();

    property bool prevNextVisible: true;

    property alias pageText: pageDescriptionText.text;
    property alias zoomText: zoomLevelText.text;
    
    content: BottomToolBarRow {
        IconButton {
            id: showThumbnailsButton;

            anchors.left: parent.left;
            anchors.leftMargin: 5;
            anchors.verticalCenter: parent.verticalCenter;

            icon: "image://themedimage/icons/actionbar/mail-message-previous";
            hasBackground: false;

            onClicked: base.showThumbnailAction(showThumbnailsButton.x + showThumbnailsButton.width/2, showThumbnailsButton.y);
        }

        Button {
            id: pageDescriptionText;
            anchors.left: showThumbnailsButton.right;
            anchors.leftMargin: 5;
            anchors.verticalCenter: parent.verticalCenter;

            textColor: theme.fontColorHighlight;
            font.pixelSize: theme.toolbarFontPixelSize;
            hasBackground: false;

            Component.onCompleted: pageDescriptionText.clicked.connect(showThumbnailsButton.clicked);
        }

        IconButton {
            id: prevPageButton;

            anchors.left: pageDescriptionText.right;
            anchors.leftMargin: 5;
            anchors.verticalCenter: parent.verticalCenter;

            icon: "image://themedimage/icons/actionbar/media-backward";
            iconDown: "image://themedimage/icons/actionbar/media-backward-active";

            hasBackground: false;
            visible: base.prevNextVisible;

            onClicked: base.previousPageAction();
        }

        IconButton {
            id: nextPageButton;

            anchors.left: prevPageButton.right;
            anchors.leftMargin: 5;
            anchors.verticalCenter: parent.verticalCenter;

            icon: "image://themedimage/icons/actionbar/media-forward";
            iconDown: "image://themedimage/icons/actionbar/media-forward-active";

            hasBackground: false;
            visible: base.prevNextVisible;

            onClicked: base.nextPageAction();
        }

        Button {
            id: separator;

            anchors.left: nextPageButton.right;
            anchors.leftMargin: 10;
            anchors.verticalCenter: parent.verticalCenter;
            hasBackground: false;
            text: "";
        }

        IconButton {
            id: showZoomControlButton;

            anchors.left: separator.right;
            anchors.leftMargin: 10;
            anchors.verticalCenter: parent.verticalCenter;

            icon: "image://icon/page-zoom";
            hasBackground: false;

            onClicked: base.showZoomAction(showZoomControlButton.x + showZoomControlButton.width/2, showZoomControlButton.y);
        }

        Button {
            id: zoomLevelText;
            anchors.left: showZoomControlButton.right;
            anchors.leftMargin: 10;
            anchors.verticalCenter: parent.verticalCenter;

            text: loader.item.zoomLevel + "%"

            textColor: theme.fontColorHighlight;
            font.pixelSize: theme.toolbarFontPixelSize;
            hasBackground: false;

            Component.onCompleted: zoomLevelText.clicked.connect(showZoomControlButton.clicked);
        }

        IconButton {
            id: viewFullScreenButton;

            anchors.verticalCenter: parent.verticalCenter;
            anchors.right: parent.right;
            anchors.rightMargin: 10;

            icon: "image://themedimage/icons/actionbar/view-fullscreen";
            iconDown: "image://themedimage/icons/actionbar/view-fullscreen-active";
            hasBackground: false;

            onClicked: base.fullScreenAction();
        }
    }
}
