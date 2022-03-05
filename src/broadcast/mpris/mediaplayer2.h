#pragma once

#include <QStringList>
#include <QtDBus/QDBusAbstractAdaptor>

class MixxxMainWindow;

// this implements the Version 2.2 of
// MPRIS D-Bus Interface Specification
// org.mpris.MediaPlayer2
// http://specifications.freedesktop.org/mpris-spec/2.2/

class MediaPlayer2 : public QDBusAbstractAdaptor {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2")
    Q_PROPERTY(bool CanQuit READ canQuit)
    Q_PROPERTY(bool HasTrackList READ hasTrackList)
    Q_PROPERTY(QString Identity READ identity)
    Q_PROPERTY(QString DesktopEntry READ desktopEntry) // optional
    Q_PROPERTY(QStringList SupportedUriSchemes READ supportedUriSchemes)
    Q_PROPERTY(QStringList SupportedMimeTypes READ supportedMimeTypes)

  public:
    explicit MediaPlayer2(QObject* parent = 0);

    bool canQuit() const;
    bool hasTrackList() const;
    QString identity() const;
    QString desktopEntry() const;
    QStringList supportedUriSchemes() const;
    QStringList supportedMimeTypes() const;

  public slots:
    void Quit();

};
