#include "QmlManager.h"

QmlManager::QmlManager(QObject* parent) : QObject(parent), m_engine(std::make_unique<QQmlEngine>()) {}

QmlManager::~QmlManager() = default;

QQuickWidget* QmlManager::createQuickWidget(const QString& qmlFile, QWidget* parent) {
    auto widget = new QQuickWidget(m_engine.get(), parent);
    widget->setResizeMode(QQuickWidget::SizeRootObjectToView);
    widget->setSource(QUrl(qmlFile));
    return widget;
}

void QmlManager::setContextProperty(const QString& name, QObject* value) {
    m_engine->rootContext()->setContextProperty(name, value);
}

void QmlManager::setContextProperty(const QString& name, const QVariant& value) {
    m_engine->rootContext()->setContextProperty(name, value);
}