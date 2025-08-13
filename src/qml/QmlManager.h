#ifndef QMLMANAGER_H
#define QMLMANAGER_H

#include <QObject>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickWidget>
#include <memory>

class QmlManager : public QObject {
    Q_OBJECT
  public:
    explicit QmlManager(QObject* parent = nullptr);
    ~QmlManager();

    // 加载QML文件
    QQuickWidget* createQuickWidget(const QString& qmlFile, QWidget* parent = nullptr);

    // 注册C++类型到QML
    template <typename T>
    void registerType(const char* uri, int major, int minor, const char* name) {
        qmlRegisterType<T>(uri, major, minor, name);
    }

    // 设置上下文属性
    void setContextProperty(const QString& name, QObject* value);
    void setContextProperty(const QString& name, const QVariant& value);

  private:
    std::unique_ptr<QQmlEngine> m_engine;
};

#endif // QMLMANAGER_H