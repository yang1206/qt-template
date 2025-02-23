// settings_helper.h
#pragma once

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QVariant>
#include <QVector>

/**
 * @brief 配置助手类，用于统一管理应用程序的配置项
 *
 * 使用单例模式，提供通用的配置读写接口
 * 支持分组管理、数组操作、配置导入导出等功能
 */
class SettingsHelper : public QObject {
  Q_OBJECT

public:
  /**
   * @brief 获取单例实例
   * @return SettingsHelper& 单例引用
   */
  static SettingsHelper &instance() {
    static SettingsHelper instance;
    return instance;
  }

  /**
   * @brief 初始化配置系统
   * @param filePath 配置文件路径
   * @param format 配置文件格式，默认INI格式
   */
  void init(const QString &filePath,
            QSettings::Format format = QSettings::IniFormat) {
    if (m_settings) {
      delete m_settings;
      m_settings = nullptr;
    }
    m_settings = new QSettings(filePath, format);
  }

  /**
   * @brief 获取配置文件路径
   * @return QString 配置文件路径
   */
  QString getFilePath() const {
    return m_settings ? m_settings->fileName() : QString();
  }
  /**
   * @brief 设置是否使用易读格式
   */
  void setReadableFormat(bool enabled) { m_useReadableFormat = enabled; }

  /**
   * @brief 获取当前是否使用易读格式
   */
  bool isReadableFormat() const { return m_useReadableFormat; }

  /**
   * @brief 设置配置项的值
   * @param group 配置组名
   * @param key 配置项键名
   * @param value 配置项值
   */
  template <typename T>
  void setValue(const QString &group, const QString &key, const T &value) {
    if (!checkSettings())
      return;
    m_settings->beginGroup(group);
    if (m_useReadableFormat) {
      // 使用易读格式
      m_settings->setValue(key, value.toString());
    } else {
      // 使用QVariant格式
      m_settings->setValue(key, QVariant::fromValue(value));
    }
    m_settings->endGroup();
    m_settings->sync();
    emit configChanged(group, key);
  }

  /**
   * @brief 获取配置项的值
   * @param group 配置组名
   * @param key 配置项键名
   * @param defaultValue 默认值
   * @return T 配置项的值
   */
  template <typename T>
  T getValue(const QString &group, const QString &key,
             const T &defaultValue = T()) {
    if (!checkSettings())
      return defaultValue;
    m_settings->beginGroup(group);
    QVariant value = m_settings->value(key);
    m_settings->endGroup();

    if (m_useReadableFormat && !value.toString().isEmpty()) {
      return T::fromString(value.toString());
    } else if (!value.isNull()) {
      return value.value<T>();
    }
    return defaultValue;
  }

  /**
   * @brief 保存数组类型的配置
   * @param group 配置组名
   * @param arrayName 数组名
   * @param array 数组数据
   */
  template <typename T>
  void setArray(const QString &group, const QString &arrayName,
                const QVector<T> &array) {
    if (!checkSettings())
      return;

    m_settings->beginGroup(group);
    m_settings->beginWriteArray(arrayName, array.size());
    for (int i = 0; i < array.size(); ++i) {
      m_settings->setArrayIndex(i);
      if (m_useReadableFormat) {
        m_settings->setValue("value", array[i].toString());
      } else {
        m_settings->setValue("value", QVariant::fromValue(array[i]));
      }
    }
    m_settings->endArray();
    m_settings->endGroup();
    m_settings->sync();
    emit configChanged(group, arrayName);
  }

  /**
   * @brief 读取数组类型的配置
   * @param group 配置组名
   * @param arrayName 数组名
   * @return QVector<T> 数组数据
   */
  template <typename T>
  QVector<T> getArray(const QString &group, const QString &arrayName) {
    QVector<T> result;
    if (!checkSettings())
      return result;

    m_settings->beginGroup(group);
    int size = m_settings->beginReadArray(arrayName);
    for (int i = 0; i < size; ++i) {
      m_settings->setArrayIndex(i);
      QVariant value = m_settings->value("value");
      if (m_useReadableFormat && !value.toString().isEmpty()) {
        result.append(T::fromString(value.toString()));
      } else if (!value.isNull()) {
        result.append(value.value<T>());
      }
    }
    m_settings->endArray();
    m_settings->endGroup();
    return result;
  }

  /**
   * @brief 删除指定组的所有配置
   * @param group 组名
   */
  void removeGroup(const QString &group) {
    if (!checkSettings())
      return;
    m_settings->remove(group);
    m_settings->sync();
    emit groupRemoved(group);
  }

  /**
   * @brief 获取所有配置组名
   * @return QStringList 配置组名列表
   */
  QStringList getGroups() {
    return m_settings ? m_settings->childGroups() : QStringList();
  }

  /**
   * @brief 检查配置项是否存在
   * @param group 配置组名
   * @param key 配置项键名
   * @return bool 是否存在
   */
  bool contains(const QString &group, const QString &key) {
    if (!checkSettings())
      return false;
    m_settings->beginGroup(group);
    bool exists = m_settings->contains(key);
    m_settings->endGroup();
    return exists;
  }

  /**
   * @brief 清除所有配置
   */
  void clear() {
    if (!checkSettings())
      return;
    m_settings->clear();
    m_settings->sync();
    emit configCleared();
  }

  /**
   * @brief 导出配置到文件
   * @param filePath 导出文件路径
   * @return bool 是否成功
   */
  bool exportConfig(const QString &filePath) {
    if (!checkSettings())
      return false;
    return QFile::copy(m_settings->fileName(), filePath);
  }

  /**
   * @brief 从文件导入配置
   * @param filePath 导入文件路径
   * @return bool 是否成功
   */
  bool importConfig(const QString &filePath) {
    if (!checkSettings())
      return false;
    if (QFile::exists(filePath)) {
      clear();
      return QFile::copy(filePath, m_settings->fileName());
    }
    return false;
  }

  /**
   * @brief 检查配置文件是否存在
   * @return bool 是否存在
   */
  bool fileExists() const {
    if (!m_settings)
      return false;
    return QFile::exists(m_settings->fileName());
  }

signals:
  /**
   * @brief 配置变更信号
   * @param group 变更的配置组
   * @param key 变更的配置键
   */
  void configChanged(const QString &group, const QString &key);

  /**
   * @brief 配置组删除信号
   * @param group 被删除的配置组
   */
  void groupRemoved(const QString &group);

  /**
   * @brief 配置清空信号
   */
  void configCleared();

private:
  bool m_useReadableFormat{false}; // 默认使用易读格式
  SettingsHelper() : m_settings(nullptr) {}
  ~SettingsHelper() {
    if (m_settings) {
      delete m_settings;
      m_settings = nullptr;
    }
  }

  /**
   * @brief 检查设置对象是否有效
   * @return bool 是否有效
   */
  bool checkSettings() {
    if (!m_settings) {
      qWarning() << "Settings not initialized! Call init() first.";
      return false;
    }
    return true;
  }

  QSettings *m_settings; // 配置对象指针

  // 禁用拷贝和赋值
  SettingsHelper(const SettingsHelper &) = delete;
  SettingsHelper &operator=(const SettingsHelper &) = delete;
};