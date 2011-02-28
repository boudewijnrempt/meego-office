#!/bin/bash
find . -name "*.qml" -exec sed -i 's/import QtQuick 1.0/import Qt 4.7/g' {} \;

