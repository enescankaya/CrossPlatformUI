#include "TileLoader.h"
#include "qurl.h"

CustomTileLoader::CustomTileLoader(const QString &tileDirectory, QObject *parent)
    : QObject(parent) {
    m_tileDirectory = QDir::fromNativeSeparators(tileDirectory);
    if (!m_tileDirectory.endsWith('/')) {
        m_tileDirectory += '/';
    }
}

bool CustomTileLoader::validateTilePath(const QString &path) const {
    QFileInfo fileInfo(path);
    return fileInfo.exists() && fileInfo.isFile() && fileInfo.suffix().toLower() == "png";
}

QString CustomTileLoader::tilePath(int zoom, int x, int y) {
    QString path = QString("%1%2/%3/%4.png")
    .arg(m_tileDirectory)
        .arg(zoom)
        .arg(x)
        .arg(y);

    path = QDir::fromNativeSeparators(path);

    if (validateTilePath(path)) {
        return QUrl::fromLocalFile(path).toString();
    } else {
        qDebug() << "Tile bulunamadı:" << path;
        return QString();
    }
}
