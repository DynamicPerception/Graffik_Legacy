import QtQuick 1.1

Rectangle {
    id: joystick
    signal valueChanged(int value)
    function emergencyStop() {
        handler.x = handler.startPosX
        handler.y = handler.startPosY
    }

    width: 240
    height: 337

    gradient: Gradient {
        GradientStop { position: 0.0; color: "#D6D6D6" }
        //GradientStop { position: 1.0; color: "#D6D6D6" }
        GradientStop { position: 1.0; color: "#EDEDED" }
    }

    Image {
        id: joystickBackground
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.top: parent.top
        anchors.topMargin: 30
        source: "qrc:///JogControlManager/qml/joystickBackground.png"

        Image {
            anchors.top: handler.top
            anchors.right: handler.right
            anchors.margins: -13
            source: "qrc:///JogControlManager/qml/handlerShadow.png"
        }

        Image {
            id: handler
            property int startPosX: parent.width / 2 - width / 2
            property int startPosY: parent.height / 2 - height / 2

            x: startPosX
            y: startPosY
            source: "qrc:///JogControlManager/qml/joystickHandler.png"

            onXChanged: x < 86 ? joystick.valueChanged(parseInt(((86 - x) / 86) * -210)) :
                                 joystick.valueChanged(parseInt(((x - 86) / 86) * 210))

            ParallelAnimation {
                id: backToStartAnimation
                NumberAnimation {
                    target: handler
                    properties: "x"
                    duration: 100
                    to: handler.startPosX
                    easing.type: Easing.InQuad
                }
                NumberAnimation {
                    target: handler
                    properties: "y"
                    duration: 100
                    to: handler.startPosY
                    easing.type: Easing.InQuad
                }
            }
        }

        MouseArea {
            anchors.fill: parent
            property real halfDimeter: width / 2 - handler.width / 2

            function calculateAlpha() {
                var tmpX = handler.x - handler.startPosX
                var tmpY = -(handler.y - handler.startPosY)
                var atan2 = 1.57079632679 - Math.atan2(tmpX, tmpY)
                return atan2 < 0 ? atan2 + 6.28318530718 : atan2
            }

            //onAlphaChanged: console.log(alpha)

            hoverEnabled: false
            property int prevX: 0
            property int prevY: 0
            onPressed: {
                prevX = mouseX
                prevY = mouseY
            }

            onMouseXChanged: {
                var diff = mouseX - prevX
                var alpha = calculateAlpha()

                var c2 = halfDimeter * halfDimeter
                var newX = handler.x + diff
                var origX = handler.startPosX
                var newY = handler.y
                var origY = handler.startPosY

                var t = (newX - origX) * (newX - origX) + (newY - origY) * (newY - origY)
                var del = (2 * origX) * (2 * origX) - 4 * (origX * origX + (newY - origY) * (newY - origY) - c2)

                var x1 = ((2 * origX) - Math.sqrt(del)) / 2
                var x2 = ((2 * origX) + Math.sqrt(del)) / 2

                if(handler.x + diff > x2)
                    handler.x = x2
                else if(handler.x + diff < x1)
                    handler.x = x1
                else handler.x += diff
                prevX = mouseX
            }
            onMouseYChanged: {
                var diff = mouseY - prevY
                var alpha = calculateAlpha()

                var c2 = halfDimeter * halfDimeter
                var newX = handler.x
                var origX = handler.startPosX
                var newY = handler.y + diff
                var origY = handler.startPosY

                var t = (newX - origX) * (newX - origX) + (newY - origY) * (newY - origY)
                var del = (2 * origY) * (2 * origY) - 4 * (origY * origY + (newX - origX) * (newX - origX) - c2)

                var y1 = ((2 * origY) - Math.sqrt(del)) / 2
                var y2 = ((2 * origY) + Math.sqrt(del)) / 2

                if(handler.y + diff > y2)
                    handler.y = y2
                else if(handler.y + diff < y1)
                    handler.y = y1
                else handler.y += diff
                prevY = mouseY
            }

            onReleased: backToStartAnimation.start()
        }
    }

    Image {
        id: beWay
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        source: "qrc:///JogControlManager/qml/beWay.png"

        Image {
            id: beHandler
            property int startPosX: parent.width / 2 - width / 2
            anchors.verticalCenter: parent.verticalCenter
            x: startPosX
            source: "qrc:///JogControlManager/qml/beHandler.png"

            NumberAnimation {
                id: backToStartAnimationBE
                target: beHandler
                duration: 100
                easing.type: Easing.InQuad
                properties: "x"
                to: beHandler.startPosX
            }

            MouseArea {
                anchors.fill: parent
                drag.target: beHandler
                drag.axis: Drag.XAxis
                drag.maximumX: beWay.width - beHandler.width
                drag.minimumX: 0
                onReleased: backToStartAnimationBE.start()
            }
        }
    }
}
