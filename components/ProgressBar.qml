import QtQuick 1.0

Rectangle {
    id: base;
    width: 100;
    height: 20;

    border.width: 1;
    border.color: "#e9e9e9";
    radius: 2;
    color: "#dddddd";

    property int progress: -1;
    onProgressChanged: {
        if(progress != -1) {
            slider.x = 0;
            slider.width = base.width * (progress / 100);
        }
    }

    Rectangle {
        id: slider;

        anchors.verticalCenter: parent.verticalCenter;

        SequentialAnimation {
            id: undefinedAnimation;

            PropertyAnimation {
                target: slider;
                properties: "x";
                from: 0;
                to: base.width - slider.width;
                duration: 1000;
            }

            PropertyAnimation {
                target: slider;
                properties: "x";
                to: 0;
                from: base.width - slider.width;
                duration: 1000;
            }
            
            loops: Animation.Infinite;
        }
        
        width: parent.width / 10;
        Behavior on width { PropertyAnimation { duration: 250; } }
        
        height: parent.height - 2;

        border.width: 1;
        border.color: "#e9e9e9";
        radius: 2;
        color: "#ccd8ff";
    }

    states: [
        State {
            name: "undefined";
            when: progress == -1;
            PropertyChanges { target: undefinedAnimation; running: true; }
        }
    ]
}