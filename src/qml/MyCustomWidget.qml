import QtQuick
import QtQuick.Controls.FluentWinUI3

ScrollView {
    anchors.fill: parent
    contentHeight: column.height
    // 确保内容高度足够大以触发滚动
    contentWidth: parent.width

    // 使用 Column 布局容器包含所有组件
    Column {
        id: column

        padding: 20
        spacing: 20
        width: parent.width

        Text {
            color: "#1b5e20"
            font.pixelSize: 24
            text: "QML 组件演示"
        }
        Rectangle {
            color: "#e0e0e0"
            height: 1
            width: parent.width
        }
        Text {
            text: "这是 Text 组件，用于显示文本内容。可以设置字体大小、颜色等属性。"
            width: parent.width
            wrapMode: Text.Wrap
        }
        Button {
            text: "标准按钮"

            onClicked: {
                console.log("按钮被点击！");
                statusText.text = "最后点击: 按钮 (" + new Date().toLocaleTimeString() + ")";
            }
        }
        TextField {
            placeholderText: "TextField - 输入文本"
            width: parent.width
        }
        Slider {
            value: 0.5
            width: parent.width

            onMoved: {
                sliderValue.text = "当前值: " + (value * 100).toFixed(0) + "%";
            }
        }
        Text {
            id: sliderValue

            horizontalAlignment: Text.AlignHCenter
            text: "当前值: 50%"
            width: parent.width
        }
        CheckBox {
            text: "CheckBox - 勾选框"
        }
        RadioButton {
            text: "RadioButton - 单选按钮"
        }
        ComboBox {
            currentIndex: 0
            model: ["选项 1", "选项 2", "选项 3", "选项 4"]
            width: parent.width
        }
        ProgressBar {
            value: 0.75
            width: parent.width
        }
        TextArea {
            height: 100
            placeholderText: "TextArea - 多行文本输入区域"
            width: parent.width
        }
        Text {
            id: statusText

            color: "#0288d1"
            text: "状态信息将显示在这里"
            width: parent.width
            wrapMode: Text.Wrap
        }
    }
}
