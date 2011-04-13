import QtQuick 1.0

Item {
    id: root

    signal moved(variant newX, variant newY)

    property alias image: image.source;
    property alias imageWidth: image.width;
    property alias imageHeight: image.height;

    Image {
        id: image;
        anchors.centerIn: parent;
        width: 32
        height: 32
    }

    width: image.width * 2;
    height: image.width * 2;

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onPositionChanged: {
            root.moved(root.x + mouse.x, root.y + mouse.y - root.height / 2)
        }
    }
}

