#pragma once

#include <QHash>
#include <QObject>
#include <QTimer>
#include <functional>

class Debouncer : public QObject {
    Q_OBJECT
  public:
    static Debouncer& instance() {
        static Debouncer instance;
        return instance;
    }

    // 防抖执行函数
    void debounce(const QString& key, std::function<void()> func, int delay = 500) {
        // 创建或获取定时器
        QTimer* timer = nullptr;
        if (!m_activeTimers.contains(key)) {
            timer = new QTimer(this);
            timer->setSingleShot(true);
            m_activeTimers[key] = timer;
            connect(timer, &QTimer::timeout, this, &Debouncer::onTimeout);
        } else {
            timer = m_activeTimers[key];
        }

        // 如果定时器正在运行，停止它
        if (timer->isActive()) {
            timer->stop();
        }

        // 更新回调函数
        m_callbacks[key] = func;
        m_currentKey     = key;

        // 重新设置时间并启动定时器
        timer->setInterval(delay);
        timer->start();
    }

    // 取消指定key的防抖
    void cancel(const QString& key) {
        if (m_activeTimers.contains(key)) {
            m_activeTimers[key]->stop();
            m_callbacks.remove(key);
        }
    }

    // 取消所有防抖
    void cancelAll() {
        for (auto timer : m_activeTimers) {
            timer->stop();
        }
        m_callbacks.clear();
    }

  private slots:
    void onTimeout() {
        // 获取发送信号的定时器
        QTimer* timer = qobject_cast<QTimer*>(sender());
        if (!timer)
            return;

        // 查找对应的key
        QString key = m_currentKey;
        if (m_callbacks.contains(key)) {
            m_callbacks[key]();
            m_callbacks.remove(key);
        }
    }

  private:
    Debouncer() = default;
    ~Debouncer() {
        cancelAll();
        qDeleteAll(m_activeTimers);
    }

    QHash<QString, QTimer*>               m_activeTimers;
    QHash<QString, std::function<void()>> m_callbacks;
    QString                               m_currentKey; // 用于存储当前正在处理的key
};
#define DEBOUNCER Debouncer::instance()