#pragma once

#include <vector>

#include <QObject>

#include "analyzer/plugins/analyzerplugin.h"
#include "analyzer/plugins/buffering_utils.h"
#include "util/memory.h"
#include "util/types.h"

class GetKeyMode;

namespace mixxx {

class AnalyzerQueenMaryKey : public AnalyzerKeyPlugin {
  public:
    static AnalyzerPluginInfo pluginInfo() {
        return AnalyzerPluginInfo(
                // Don't change this ID. It was auto generated by VAMP until
                // Mixxx 2.1 and we keep it for a compatible config.
                "qm-keydetector:2",
                QObject::tr("Queen Mary University London"),
                QObject::tr("Queen Mary Key Detector"),
                false);
    }

    AnalyzerQueenMaryKey();
    ~AnalyzerQueenMaryKey() override;

    AnalyzerPluginInfo info() const override {
        return pluginInfo();
    }

    bool initialize(mixxx::audio::SampleRate sampleRate) override;
    bool processSamples(const CSAMPLE* pIn, const int iLen) override;
    bool finalize() override;

    KeyChangeList getKeyChanges() const override {
        return m_resultKeys;
    }

  private:
    std::unique_ptr<GetKeyMode> m_pKeyMode;
    DownmixAndOverlapHelper m_helper;
    size_t m_currentFrame;
    KeyChangeList m_resultKeys;
    mixxx::track::io::key::ChromaticKey m_prevKey;
};

} // namespace mixxx
