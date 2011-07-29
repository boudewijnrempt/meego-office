import QtQuick 1.0
import MeeGo.Components 0.1
import org.calligra.mobile 1.0

BottomToolBar {
    id: base;

    function show() {
        //Nasty hack to prevent the toolbar from stealing focus.
        visible = true;
        base.children[1].extend = true;
    }

    signal findNextAction();
    signal findPreviousAction();
    signal closeAction();

    property alias matchText: findMatchesText.text;
    
    content: BottomToolBarRow {
        IconButton {
            id: findPreviousButton;

            anchors.right: findMatchesText.left;
            anchors.leftMargin: 5;
            anchors.rightMargin: 5;
            anchors.topMargin: (64 - height) / 2;

            icon: "image://themedimage/icons/actionbar/mail-message-previous";
            hasBackground: false;

            onClicked: base.findPreviousAction();
        }
        Text {
            id: findMatchesText;

            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.leftMargin: 5;
            anchors.rightMargin: 5;
            anchors.verticalCenter: findPreviousButton.verticalCenter;

            font.pixelSize: theme.toolbarFontPixelSize;
            color: theme.fontColorHighlight;
        }
        IconButton {
            id: findNextButton;

            anchors.left: findMatchesText.right;
            anchors.leftMargin: 5;
            anchors.rightMargin: 5;
            anchors.verticalCenter: findPreviousButton.verticalCenter;

            icon: "image://themedimage/icons/actionbar/mail-message-next";
            hasBackground: false;

            onClicked: base.findNextAction();
        }
        IconButton {
            id: findCloseButton;

            anchors.right: parent.right;
            anchors.leftMargin: 5;
            anchors.rightMargin: 5;
            anchors.verticalCenter: findPreviousButton.verticalCenter;

            icon: "image://themedimage/images/contacts/icn_cross_up";
            iconDown: "image://themedimage/images/contacts/icn_cross_dn";
            hasBackground: false;

            height: findNextButton.height;
            onClicked: base.closeAction();
        }
    }
}