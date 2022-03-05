#include "broadcast/mpris/mediaplayer2tracklist.h"

#include <QApplication>

#include "moc_mediaplayer2tracklist.cpp"

MediaPlayer2TrackList::MediaPlayer2TrackList(QObject* parent)
        : QDBusAbstractAdaptor(parent) {
}

MediaPlayer2TrackList::~MediaPlayer2TrackList() {
}

MprisTrackIds MediaPlayer2TrackList::tracks() const {
    MprisTrackIds tracks;
    return tracks;
}

bool MediaPlayer2TrackList::canEditTracks() const {
    return false;
}

MprisTrackMetadata MediaPlayer2TrackList::GetTracksMetadata(
        const MprisTrackIds& tracks) const {
    Q_UNUSED(tracks);

    MprisTrackMetadata metadata;
    return metadata;
}

void MediaPlayer2TrackList::AddTrack(const QString& uri,
        const QDBusObjectPath& afterTrack,
        bool setAsCurrent) {
    Q_UNUSED(uri);
    Q_UNUSED(afterTrack);
    Q_UNUSED(setAsCurrent);
}

void MediaPlayer2TrackList::RemoveTrack(const QDBusObjectPath& trackId) {
    Q_UNUSED(trackId);
}

void MediaPlayer2TrackList::GoTo(const QDBusObjectPath& trackId) {
    Q_UNUSED(trackId);
}
