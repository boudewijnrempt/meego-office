import QtQuick 1.0;
import MeeGo.Components 0.1
import org.calligra.mobile 1.0

Item {
    id: settings;
    
    property string currentFile;
    property string currentUuid;
    property string currentName;
    property string currentType;
    
    function reset() {
        currentFile = "";
        currentName = "";
        currentType = "";
    }
}