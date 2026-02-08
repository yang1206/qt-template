#include "QJsonSettings.h"
#include <QDebug>
#include "json/json_utils.h"

QJsonSettings::QJsonSettings(const QString& organization,
                             const QString& application,
                             const QString& fileName,
                             bool           autoLoad)
    : m_organization(organization), m_application(application), m_fileName(fileName) {
    if (autoLoad) {
        load();
    }
}

QJsonSettings::~QJsonSettings() {
    if (m_dirty) {
        sync();
    }
}

void QJsonSettings::setValue(const QString& key, const QVariant& value) {
    QString    fullKey   = makeKey(key);
    QJsonValue jsonValue = QJsonValue::fromVariant(value);

    QStringList parts   = fullKey.split('/');
    QString     lastKey = parts.takeLast();

    // 使用递归方式更新嵌套的JSON对象
    m_data  = updateJsonObject(m_data, parts, lastKey, jsonValue);
    m_dirty = true;
}

QVariant QJsonSettings::value(const QString& key, const QVariant& defaultValue) const {
    QString     fullKey = makeKey(key);
    QStringList parts   = fullKey.split('/');
    QString     lastKey = parts.takeLast();

    // 使用递归方式获取嵌套的值
    QJsonValue value = getNestedValue(m_data, parts, lastKey);
    return value.isUndefined() || value.isNull() ? defaultValue : value.toVariant();
}

void QJsonSettings::beginGroup(const QString& prefix) {
    if (!m_group.isEmpty()) {
        m_group += '/';
    }
    m_group += prefix;
}

void QJsonSettings::endGroup() {
    int lastSep = m_group.lastIndexOf('/');
    if (lastSep < 0) {
        m_group.clear();
    } else {
        m_group = m_group.left(lastSep);
    }
}

QString QJsonSettings::group() const { return m_group; }

void QJsonSettings::sync() {
    if (m_dirty) {
        save();
        m_dirty = false;
    }
}

void QJsonSettings::load() {
    QJsonObject obj;
    if (JsonUtils::loadFromFile(getFilePath(), obj)) {
        m_data = obj;
    }
}

void QJsonSettings::save() { JsonUtils::saveToFile(getFilePath(), m_data); }

QString QJsonSettings::getFilePath() const { return JsonUtils::getConfigFilePath(m_fileName); }

QString QJsonSettings::makeKey(const QString& key) const {
    if (m_group.isEmpty()) {
        return key;
    }
    return m_group + '/' + key;
}

QJsonObject QJsonSettings::updateJsonObject(const QJsonObject& obj,
                                            const QStringList& path,
                                            const QString&     key,
                                            const QJsonValue&  value) {
    QJsonObject result = obj;

    if (path.isEmpty()) {
        // 到达目标层级，直接设置值
        result[key] = value;
        return result;
    }

    // 获取当前层级的key
    QString     currentKey    = path.first();
    QStringList remainingPath = path.mid(1);

    // 如果当前key不存在或不是对象，创建新对象
    QJsonValue currentValue = result[currentKey];
    if (!currentValue.isObject()) {
        currentValue = QJsonObject();
    }

    // 递归更新下一层
    result[currentKey] = updateJsonObject(currentValue.toObject(), remainingPath, key, value);

    return result;
}

QJsonValue QJsonSettings::getNestedValue(const QJsonObject& obj, const QStringList& path, const QString& key) const {
    if (path.isEmpty()) {
        // 到达目标层级，返回值
        return obj[key];
    }

    // 获取当前层级的key
    QString currentKey = path.first();
    if (!obj.contains(currentKey)) {
        return QJsonValue();
    }

    QJsonValue current = obj[currentKey];
    if (!current.isObject()) {
        return QJsonValue();
    }

    // 递归获取下一层的值
    return getNestedValue(current.toObject(), path.mid(1), key);
}

bool QJsonSettings::contains(const QString& key) const {
    QString     fullKey = makeKey(key);
    QStringList parts   = fullKey.split('/');
    QString     lastKey = parts.takeLast();

    QJsonValue value = getNestedValue(m_data, parts, lastKey);
    return !value.isUndefined();
}

void QJsonSettings::remove(const QString& key) {
    QString     fullKey = makeKey(key);
    QStringList parts   = fullKey.split('/');
    QString     lastKey = parts.takeLast();

    // 使用空值更新来实现删除
    m_data  = updateJsonObject(m_data, parts, lastKey, QJsonValue());
    m_dirty = true;
}

QStringList QJsonSettings::allKeys() const {
    // TODO: 实现递归获取所有键
    QStringList keys;
    collectKeys(m_data, QString(), keys);
    return keys;
}

void QJsonSettings::collectKeys(const QJsonObject& obj, const QString& prefix, QStringList& keys) const {
    for (auto it = obj.begin(); it != obj.end(); ++it) {
        QString currentKey = prefix.isEmpty() ? it.key() : prefix + "/" + it.key();

        if (it.value().isObject()) {
            // 递归收集子对象的键
            collectKeys(it.value().toObject(), currentKey, keys);
        } else {
            keys.append(currentKey);
        }
    }
}

void QJsonSettings::clear() {
    m_data  = QJsonObject();
    m_dirty = true;
}

// 数组操作的实现
void QJsonSettings::beginArray(const QString& prefix) {
    beginGroup(prefix);
    // 确保该键存在且是数组
    if (!contains(QString::number(0))) {
        setValue(QString::number(0), QJsonArray());
    }
}

void QJsonSettings::endArray() { endGroup(); }

void QJsonSettings::setArrayIndex(int i) {
    if (i >= 0) {
        if (!m_group.isEmpty()) {
            m_group += '/';
        }
        m_group += QString::number(i);
    }
}

int QJsonSettings::arraySize() const {
    QJsonValue value = getNestedValue(m_data, m_group.split('/'), QString());
    if (value.isArray()) {
        return value.toArray().size();
    }
    return 0;
}