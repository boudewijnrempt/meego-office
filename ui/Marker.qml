import QtQuick 1.0

Image {
    id: root

    property variant lastMouseX: -1
    property variant lastMouseY: -1

    signal moved(variant newX, variant newY)

    width: 32
    height: 32
    opacity: 0.5
    source: "resources/marker.png"

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onPressed: { root.lastMouseX = mouse.x; root.lastMouseY = mouse.y }
        onReleased: { root.lastMouseX = -1; root.lastMouseY = -1 }
        onPositionChanged: {
            var deltaX = mouse.x - root.lastMouseX;
            var deltaY = mouse.y - root.lastMouseY;
            root.moved(root.x + deltaX, root.y + deltaY)
        }
    }
}

