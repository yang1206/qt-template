#pragma once
#include <QJsonObject>
#include <QString>
#include <QVariant>

// 定义配置键的基类
class ConfigKey {
  public:
    virtual ~ConfigKey()         = default;
    virtual QString path() const = 0;
};

// 定义配置项模板
template <typename T>
class TypedConfigKey : public ConfigKey {
  public:
    using ValueType = T;
    explicit TypedConfigKey(const QString& path, const T& defaultValue = T())
        : m_path(path), m_defaultValue(defaultValue) {}

    QString  path() const override { return m_path; }
    const T& defaultValue() const { return m_defaultValue; }

  private:
    QString m_path;
    T       m_defaultValue;
};

// 宏用于定义配置键
#define DEFINE_CONFIG_KEY(KeyName, Type, Path, DefaultValue)                                                           \
    static const TypedConfigKey<Type> KeyName(Path, DefaultValue)

class QJsonSettings {
  public:
    // 构造函数，指定组织名和应用名
    explicit QJsonSettings(const QString& organization,
                           const QString& application,
                           const QString& fileName = "settings.json",
                           bool           autoLoad = true);
    ~QJsonSettings();

    // 类型安全的访问方法
    template <typename T>
    void setValue(const TypedConfigKey<T>& key, const T& value) {
        setValue(key.path(), QVariant::fromValue(value));
    }

    template <typename T>
    T value(const TypedConfigKey<T>& key) const {
        return value(key.path(), QVariant::fromValue(key.defaultValue())).value<T>();
    }

    // 基本操作接口
    void     setValue(const QString& key, const QVariant& value);
    QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;

    // 分组操作
    void    beginGroup(const QString& prefix);
    void    endGroup();
    QString group() const;

    // 数组操作
    void beginArray(const QString& prefix);
    void endArray();
    void setArrayIndex(int i);
    int  arraySize() const;

    // 其他操作
    void        clear();
    bool        contains(const QString& key) const;
    void        remove(const QString& key);
    QStringList allKeys() const;

    // 同步操作
    void sync();

  private:
    QString     m_organization;
    QString     m_application;
    QString     m_fileName;
    QString     m_group;
    QJsonObject m_data;
    bool        m_dirty{false}; // 标记是否有未保存的更改

    // 内部辅助方法
    QString getFilePath() const;
    void    load();
    void    save();
    QString makeKey(const QString& key) const;
    void    collectKeys(const QJsonObject& obj, const QString& prefix, QStringList& keys) const;

    QJsonObject
    updateJsonObject(const QJsonObject& obj, const QStringList& path, const QString& key, const QJsonValue& value);

    QJsonValue getNestedValue(const QJsonObject& obj, const QStringList& path, const QString& key) const;
};

// 用于定义配置组的宏
#define BEGIN_CONFIG_GROUP(GroupName)                                                                                  \
    namespace GroupName {                                                                                              \
    inline QString groupPath(const QString& key) { return #GroupName "/" + key; }

#define END_CONFIG_GROUP() }

// 用于定义配置项的宏
#define CONFIG_KEY(Type, Name, DefaultValue) DEFINE_CONFIG_KEY(Name, Type, groupPath(#Name), DefaultValue)