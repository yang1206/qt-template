//
// Created by yang1206 on 2025/2/27.
//

#pragma once
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QString>

class JsonUtils {
  public:
    static bool saveToFile(const QString& filePath, const QJsonObject& jsonObj);
    static bool loadFromFile(const QString& filePath, QJsonObject& jsonObj);

    // 获取应用程序配置目录
    static QString getAppConfigPath();

    // 获取完整的配置文件路径
    static QString getConfigFilePath(const QString& fileName);

    // 创建目录（如果不存在）
    static bool ensureDirectoryExists(const QString& path);

    // 确保文件所在目录存在
    static bool ensureFileDirectoryExists(const QString& filePath);

  private:
    JsonUtils()  = default;
    ~JsonUtils() = default;
};
