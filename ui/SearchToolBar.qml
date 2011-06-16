import QtQuick 1.0
import MeeGo.Components 0.1
import org.calligra.mobile 1.0

BottomToolBar {
    id: base;

    signal findNextAction();
    signal findPreviousAction();
    signal closeAction();

    property alias matchText: findMatchesText.text;
    
    content: BottomToolBarRow {
        IconButton {
            id: findPreviousButton;

            anchors.right: findMatchesText.left;
            anchors.verticalCenter: parent.verticalCenter;
            anchors.leftMargin: 5;
            anchors.rightMargin: 5;

            icon: "image://themedimage/icons/actionbar/mail-message-previous";
            hasBackground: false;

            onClicked: base.findPreviousAction();
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

            onClicked: base.findNextAction();
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

            height: findNextButton.height;
            onClicked: base.closeACtion();
        }
    }
}