import QtQuick 1.0

Item {
    id: root
    width: message.width + 10
    height: message.height + 10

    Behavior on opacity { NumberAnimation { duration: 250 } }

    function show() {
        root.opacity = 1
        autoHideTimer.start()
    }

    Rectangle {
        anchors.fill: parent;
        color: "black";
        opacity: 0.5;
    }

    Timer {
        id: autoHideTimer
        interval: 2000
        repeat: false
        running: false
        onTriggered: root.opacity = 0
    }

    Text {
        id: message
        color: "white"
        font.bold: true
        font.pointSize: 12
        anchors.centerIn: parent
        text: qsTr("Text copied to clipboard")
    }
}

