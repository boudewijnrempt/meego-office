import QtQuick 1.0

Rectangle {
    id: root
    property alias title: title.text
    property alias leftArea: leftRow.children
    property alias rightArea: rightRow.children
    signal pullDownGesture()

    height: title.height + 20
    gradient: Gradient {
        GradientStop { position: 0.0; color: "#e9e9e9" }
        GradientStop { position: 0.7; color: "#dddddd" }
        GradientStop { position: 1.0; color: "#e9e8e8" }
    }

    Rectangle { id: topBorder; color: "#f9f9f9"; width: root.width; height: 1 }

    MouseArea {
        anchors.fill: parent
        onClicked: root.pullDownGesture()
    }

    Text {
        id: title
        anchors.centerIn: parent
        font.pixelSize: 14
    }

    Row {
        id: leftRow
        anchors.left: root.left
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        spacing: 5
    }

    Row {
        id: rightRow
        anchors.right: root.right
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        spacing: 5
    }
}

