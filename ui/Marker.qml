import QtQuick 1.0

Item {
    id: root

    property alias image: image.source;
    property alias imageWidth: image.width;
    property alias imageHeight: image.height;

    signal dragEnd()

    Image {
        id: image;
        width: 32
        height: 32
        x: -(width / 2);
        y: -(height / 2);
    }

    width: image.width * 2;
    height: image.height * 2;

    MouseArea {
        id: mouseArea
        width: image.width * 2;
        height: image.height * 2;
        x: -(root.width / 2);
        y: -(root.height / 2);
        drag.target: root;

        onReleased: root.dragEnd();
    }
}

