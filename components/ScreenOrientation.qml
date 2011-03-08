import QtQuick 1.0
import QtMobility.sensors 1.1

Item {
    id: root
    property Item target
    property variant orientation: OrientationReading.TopUp // ## Figure out how to get initial reading

    OrientationSensor {
        active: true
        onReadingChanged: root.orientation = reading.orientation
    }

    states:  [
        State {
            name: "Landscape"
            when: root.orientation == OrientationReading.TopUp
            PropertyChanges {
                target: root.target
                rotation: 0
                x: 0
                y: 0
                width: window.width
                height: window.height
              }
        },
        State {
            name: "LandscapeInverted"
            when: root.orientation == OrientationReading.TopDown
            PropertyChanges {
              target: root.target
              rotation: 180
              x: 0
              y: 0
              width: window.width
              height: window.height
            }
        },
        State {
            name: "Portrait"
            when: root.orientation == OrientationReading.LeftUp
            PropertyChanges {
                target: root.target
                rotation: 270
                x: (window.width - window.height) / 2
                y: -(window.width - window.height) / 2
                width: window.height
                height: window.width
            }
        },
        State {
            name: "PortraitInverted"
            when: root.orientation == OrientationReading.RightUp
            PropertyChanges {
                target: root.target
                rotation: 90
                x: (window.width - window.height) / 2
                y: -(window.width - window.height) / 2
                width: window.height
                height: window.width
            }
        }
    ]
 
    transitions: Transition {
        ParallelAnimation {
            RotationAnimation {
                properties: "rotation"
                duration: 250
                direction: RotationAnimation.Shortest
                easing.type: Easing.InOutQuint
              }
              NumberAnimation {
                  target: root.target
                  properties: "x,y,width,height"
                  duration: 250
                  easing.type: Easing.InOutQuint
              }
        }
    }
}

