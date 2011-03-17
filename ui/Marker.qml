import QtQuick 1.0

Image {
    id: root

    signal moved(variant newX, variant newY)

    width: 32
    height: 32
    source: "resources/marker.png"

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onPositionChanged: {
            root.moved(root.x + mouse.x, root.y + mouse.y)
        }
    }
}

