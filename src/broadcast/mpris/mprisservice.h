#pragma once


#include "broadcast/scrobblingservice.h"
#include "broadcast/mpris/mpris.h"

class MixxxMainWindow;

class MprisService : public ScrobblingService {
    Q_OBJECT
  public:
    explicit MprisService(MixxxMainWindow *pWindow,
                          PlayerManager *pPlayer);
    void slotBroadcastCurrentTrack(TrackPointer pTrack) override;
    void slotScrobbleTrack(TrackPointer pTrack) override;
    void slotAllTracksPaused() override;
  private:
    Mpris m_mpris;
};
