#include <QApplication>
#include <QLayout>
#include <QPushButton>
#include <fmt/core.h>
#include <spdlog/spdlog.h>

int main(int argc, char *argv[]) {
    spdlog::info("Welcome to spdlog!");
    spdlog::warn("This is a warning!");

    // 格式化输出
    spdlog::info("Hello, {}!", "world");
    QApplication a(argc, argv);
    QPushButton button("Hello world!", nullptr);
    button.resize(200, 100);
    button.show();
    fmt::print("Hello, World!\n");
    return QApplication::exec();
}
