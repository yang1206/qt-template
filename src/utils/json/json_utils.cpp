//
// Created by yang1206 on 2025/2/27.
//

#include "json_utils.h"
#include <QDir>
#include <QFile>
#include <QStandardPaths>

QString JsonUtils::getConfigFilePath(const QString& fileName) {
    QString configPath    = getAppConfigPath();
    QString cleanFileName = QFileInfo(fileName).fileName();
    return configPath + "/" + cleanFileName;
}

bool JsonUtils::saveToFile(const QString& filePath, const QJsonObject& jsonObj) {
    // 确保目录存在
    if (!ensureFileDirectoryExists(filePath)) {
        qWarning() << "Failed to create directory for file:" << filePath;
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qWarning() << "Failed to open file for writing:" << filePath;
        return false;
    }

    QJsonDocument doc(jsonObj);
    file.write(doc.toJson(QJsonDocument::Indented)); // 使用格式化的JSON
    return true;
}

bool JsonUtils::loadFromFile(const QString& filePath, QJsonObject& jsonObj) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        // qWarning() << "Failed to open file for reading:" << filePath;
        return false;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (doc.isNull()) {
        qWarning() << "Invalid JSON format in file:" << filePath;
        return false;
    }

    jsonObj = doc.object();
    return true;
}

QString JsonUtils::getAppConfigPath() { return QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation); }

bool JsonUtils::ensureDirectoryExists(const QString& path) {
    QDir dir(path);
    return dir.mkpath(path);
}

bool JsonUtils::ensureFileDirectoryExists(const QString& filePath) {
    QFileInfo fileInfo(filePath);
    return ensureDirectoryExists(fileInfo.absolutePath());
}
