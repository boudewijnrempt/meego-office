import QtQuick 1.0

Rectangle {
    id: base;
    border.width: 1;
    radius: 5;
    width: calculateWidth();
    height: calculateHeight();
    clip: true;

    //Label Positions, like KDE action buttons.
    //0: No text
    //1: Text beside icon (icon to the left)
    //2: Text beside icon (icon to the right)
    //3: Text underneath icon
    //4: Only text
    property int labelPosition: 1;
    property int padding: 5;
    property int spacing: 5;
    property bool enabled: true;

    property alias text: label.text;
    property alias image: icon.source;
    property alias imageWidth: icon.width;
    property alias imageHeight: icon.height;
    
    signal triggered();

    MouseArea {
        id: mouseArea;
        anchors.fill: parent;
        onReleased: triggered();
    }

    Image {
        id: icon;
        anchors.leftMargin: parent.spacing;
        anchors.rightMargin: parent.spacing;
        anchors.topMargin: parent.spacing;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.verticalCenter: parent.verticalCenter;

        states: [
            State { name: "noText"; when: base.labelPosition == 0; },
            State { name: "textBesideLeft"; when: base.labelPosition == 1; AnchorChanges { target: icon; anchors.left: parent.left; anchors.horizontalCenter: undefined; } },
            State { name: "textBesideRight"; when: base.labelPosition == 2; AnchorChanges { target: icon; anchors.right: parent.right; anchors.horizontalCenter: undefined; } },
            State { name: "textUnderneath"; when: base.labelPosition == 3; AnchorChanges { target: icon; anchors.top: parent.top; anchors.verticalCenter: undefined; } },
            State { name: "textOnly"; when: base.labelPosition == 4; PropertyChanges { target: icon; visible: false; } }
        ]
    }

    Text {
        id: label;
        anchors.leftMargin: parent.spacing;
        anchors.rightMargin: parent.spacing;
        anchors.topMargin: parent.spacing;
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.verticalCenter: parent.verticalCenter;

        text: "!!Label!!";

        states: [
            State { name: "noText"; when: base.labelPosition == 0; PropertyChanges { target: label; visible: false; } },
            State { name: "textBesideLeft"; when: base.labelPosition == 1; AnchorChanges { target: label; anchors.left: icon.right; anchors.horizontalCenter: undefined; anchors.verticalCenter: icon.verticalCenter; } },
            State { name: "textBesideRight"; when: base.labelPosition == 2; AnchorChanges { target: label; anchors.right: icon.left; anchors.horizontalCenter: undefined; anchors.verticalCenter: icon.verticalCenter; } },
            State { name: "textUnderneath"; when: base.labelPosition == 3; AnchorChanges { target: label; anchors.top: icon.bottom; anchors.verticalCenter: undefined; anchors.horizontalCenter: icon.horizontalCenter; } },
            State { name: "textOnly"; when: base.labelPosition == 4; }
        ]
    }

    states: [
        State {
            name: "pushed";
            when: mouseArea.pressed;
            PropertyChanges { target: label; color: "#ff0000"; }
            PropertyChanges { target: base; border.color: "#ff0000"; }
        },
        State {
            name: "disabled";
            when: !base.enabled;
            PropertyChanges { target: label; color: "#aaaaaa"; }
            PropertyChanges { target: base; border.color: "#aaaaaa"; }
            PropertyChanges { target: mouseArea; enabled: false; }
        }
    ]

    transitions: [
        Transition {
            ColorAnimation { properties: "color,border.color"; duration: 100; }
        }
    ]

    function calculateWidth() {
        switch(this.labelPosition) {
        case 0:
            return icon.width + this.padding * 2;
        case 1:
            return icon.width + this.spacing + label.width + this.padding * 2;
        case 2:
            return Math.max(icon.width, label.width) + this.padding * 2;
        case 3:
            return label.width + this.padding * 2;
        }
    }

    function calculateHeight() {
        switch(this.labelPosition) {
        case 0:
            return icon.height + this.padding * 2;
        case 1:
            return Math.max(icon.height, label.width) + this.padding * 2;
        case 2:
            return icon.width + this.spacing + label.width + this.padding * 2;
        case 3:
            return label.height + this.padding * 2;
        }
    }
}
