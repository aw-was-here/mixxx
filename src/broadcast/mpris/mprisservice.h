#pragma once


#include "broadcast/scrobblingservice.h"
#include "broadcast/mpris/mpris.h"
#include "mixxx.h"

class PlayerManagerInterface;

class MprisService : public ScrobblingService {
    Q_OBJECT
  public:
    MprisService(MixxxMainWindow* pWindow,
            PlayerManagerInterface* pPlayer,
            UserSettingsPointer pSettings);
    void slotBroadcastCurrentTrack(TrackPointer pTrack) override;
    void slotScrobbleTrack(TrackPointer pTrack) override;
    void slotAllTracksPaused() override;
  private:
    Mpris m_mpris;
    ControlProxy* m_pCPAutoDJEnabled;
};
