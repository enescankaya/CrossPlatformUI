#ifndef CUSTOMTILELOADER_H
#define CUSTOMTILELOADER_H

#include <QObject>
#include <QFile>
#include <QString>
#include <QDebug>
#include <QDir>

class CustomTileLoader : public QObject {
    Q_OBJECT
public:
    explicit CustomTileLoader(const QString &tileDirectory, QObject *parent = nullptr);
    Q_INVOKABLE QString tilePath(int zoom, int x, int y);

private:
    QString m_tileDirectory;
    bool validateTilePath(const QString &path) const;
};

#endif // CUSTOMTILELOADER_H
