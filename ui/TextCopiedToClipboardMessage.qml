import QtQuick 1.0

Rectangle {
    id: root
    width: message.width + 10
    height: message.height + 10
    color: "black"
    border.color: "blue"
    border.width: 1
    radius: 4

    Behavior on opacity { NumberAnimation { duration: 250 } }

    function show() {
        root.opacity = 1
        autoHideTimer.start()
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

