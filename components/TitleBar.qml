import QtQuick 1.0

Rectangle {
    id: root
    property alias title: title.text
    property alias leftArea: leftRow.children
    property alias rightArea: rightRow.children

    property Item pullDownGestureTarget
    signal pullDownGestureFinished()

    height: title.height + 20
    gradient: Gradient {
        GradientStop { position: 0.0; color: "#e9e9e9" }
        GradientStop { position: 0.7; color: "#dddddd" }
        GradientStop { position: 1.0; color: "#e9e8e8" }
    }

    Rectangle { id: topBorder; color: "#f9f9f9"; width: root.width; height: 1 }

    MouseArea {
        anchors.fill: parent
        drag.target: root.pullDownGestureTarget
        drag.axis: Drag.YAxis
        drag.minimumY: root.pullDownGestureTarget ? -root.pullDownGestureTarget.height : 0
        drag.maximumY: 0
        onReleased: root.pullDownGestureFinished()
        z: 1
    }

    Text {
        id: title
        anchors.centerIn: parent
        font.pixelSize: 14
        z: 0
    }

    Row {
        id: leftRow
        anchors.left: root.left
        anchors.leftMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        spacing: 5
        z: 2
    }

    Row {
        id: rightRow
        anchors.right: root.right
        anchors.rightMargin: 10
        anchors.verticalCenter: parent.verticalCenter
        spacing: 5
        z: 2
    }
}

