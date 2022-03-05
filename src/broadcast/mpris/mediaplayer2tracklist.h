#ifndef MEDIAPLAYER2TRACKLIST_H
#define MEDIAPLAYER2TRACKLIST_H

#include <QStringList>
#include <QtDBus/QDBusAbstractAdaptor>
#include <QtDBus/QDBusObjectPath>

// this implements the Version 2.2 of
// MPRIS D-Bus Interface Specification
// org.mpris.MediaPlayer2.TrackList
// http://specifications.freedesktop.org/mpris-spec/2.2/

typedef QList<QVariantMap> MprisTrackMetadata;
Q_DECLARE_METATYPE(MprisTrackMetadata)
typedef QList<QDBusObjectPath> MprisTrackIds;

class MediaPlayer2TrackList : public QDBusAbstractAdaptor {
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "org.mpris.MediaPlayer2.TrackList")
    Q_PROPERTY(MprisTrackIds Tracks READ tracks)
    Q_PROPERTY(bool CanEditTracks READ canEditTracks)

  public:
    MediaPlayer2TrackList(QObject* parent = 0);
    virtual ~MediaPlayer2TrackList();

    MprisTrackIds tracks() const;
    bool canEditTracks() const;
    MprisTrackMetadata GetTracksMetadata(const MprisTrackIds& tracks) const;

  public slots:
    void AddTrack(const QString& uri, const QDBusObjectPath& afterTrack, bool setAsCurrent);
    void RemoveTrack(const QDBusObjectPath& trackId);
    void GoTo(const QDBusObjectPath& trackId);

  signals:
    void TrackListReplaced(const MprisTrackIds& tracks, const QDBusObjectPath& currentTrack);
    void TrackAdded(const MprisTrackMetadata& metadata, const QDBusObjectPath& afterTrack);
    void TrackRemoved(const QDBusObjectPath& trackId);
    void TrackMetadataChanged(const QDBusObjectPath& trackId, const MprisTrackMetadata& metadata);
};

#endif // MEDIAPLAYER2TRACKLIST_H
