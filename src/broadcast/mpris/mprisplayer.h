#pragma once

#include <QObject>
#include <QtDBus/QDBusObjectPath>

#include "broadcast/mpris/mpris.h"
#include "control/controlproxy.h"
#include "library/autodj/autodjprocessor.h"
#include "mixxx.h"

class PlayerManagerInterface;

class MprisPlayer : public QObject {
    Q_OBJECT
  public:
    MprisPlayer(PlayerManagerInterface* pPlayerManager,
            Mpris* pMpris,
            UserSettingsPointer pSettings);

    ~MprisPlayer() override;
    QString playbackStatus() const;
    QString loopStatus() const;
    void setLoopStatus(const QString& value);
    double rate() const;
    void setRate(double value);
    QVariantMap metadata();
    double volume() const;
    void setVolume(double value);
    qlonglong position() const;
    bool canGoNext() const;
    bool canGoPrevious() const;
    bool canPlay() const;
    bool canPause() const;
    bool canSeek() const;
    void nextTrack();
    void pause();
    void playPause();
    void play();
    qlonglong seek(qlonglong offset, bool& success);
    qlonglong setPosition(const QDBusObjectPath& trackId, qlonglong position, bool& success);
    void openUri(const QString& uri);

  private slots:
    void slotChangeProperties(double enabled);
    void slotPlayChanged(DeckAttributes* pDeck, bool playing);
    void slotPlayPositionChanged(DeckAttributes* pDeck, double position);
    void slotVolumeChanged(double volume);
    void slotCoverArtFound(const QObject* requestor,
            const CoverInfoRelative& info,
            const QPixmap& pixmap,
            bool fromCache);

  private:
    void broadcastPropertiesChange(bool enabled);
    void requestMetadataFromTrack(TrackPointer pTrack, bool requestCover);
    void requestCoverartUrl(TrackPointer pTrack);
    void broadcastCurrentMetadata();
    QVariantMap getVariantMapMetadata();
    double getAverageVolume() const;
    DeckAttributes* findPlayingDeck() const;
    bool autoDjEnabled() const;
    bool autoDjIdle() const;
    const QString autoDJDependentProperties[4] = {
            "CanGoNext",
            "CanPlay",
            "CanPause",
            "CanSeek"};

    ControlProxy* m_pCPAutoDjEnabled;
    ControlProxy* m_pCPFadeNow;
    ControlProxy* m_pCPAutoDJIdle;
    QList<ControlProxy*> m_CPDeckVolumes;
    PlayerManagerInterface* m_pPlayerManager;
    QString m_pausedDeck;
    bool m_bPropertiesEnabled;
    Mpris* m_pMpris;
    QList<DeckAttributes*> m_deckAttributes;
    UserSettingsPointer m_pSettings;

    struct CurrentMetadata {
        QString trackPath;
        long long int trackDuration;
        QStringList artists;
        QString title;
        QString coverartUrl;
        QString album;
        int userRating;
        int useCount;
    };

    CurrentMetadata m_currentMetadata;
    QTemporaryFile m_currentCoverArtFile;
};
