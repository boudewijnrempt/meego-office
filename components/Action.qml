import QtQuick 1.0

QtObject {
    id: root
    property string text

    signal triggered()

    function trigger() { root.triggered() }
}

