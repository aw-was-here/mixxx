#include <QtDebug>
#include <QMenu>
#include <QFile>
#include <QFileInfo>

#include "library/playlistfeature.h"

#include "widget/wlibrary.h"
#include "widget/wlibrarysidebar.h"
#include "widget/wlibrarytextbrowser.h"
#include "library/trackcollection.h"
#include "library/playlisttablemodel.h"
#include "library/treeitemmodel.h"
#include "library/queryutil.h"
#include "library/parser.h"
#include "controllers/keyboard/keyboardeventfilter.h"
#include "sources/soundsourceproxy.h"
#include "util/dnd.h"
#include "util/duration.h"

PlaylistFeature::PlaylistFeature(UserSettingsPointer pConfig,
                                 Library* pLibrary,
                                 QObject* parent,
                                 TrackCollection* pTrackCollection)
        : BasePlaylistFeature(pConfig, pLibrary, parent, pTrackCollection) {
    //construct child model
    TreeItem *rootItem = new TreeItem();
    m_childModel->setRootItem(rootItem);
    constructChildModel(-1);
}

PlaylistFeature::~PlaylistFeature() {
}

QVariant PlaylistFeature::title() {
    return tr("Playlists");
}

QString PlaylistFeature::getIconPath() {
    return ":/images/library/ic_library_playlist.png";
}

QString PlaylistFeature::getSettingsName() const {
    return "PlaylistFeature";
}

bool PlaylistFeature::isSinglePane() const {
    return false;
}

void PlaylistFeature::onRightClick(const QPoint& globalPos) {
    m_lastRightClickedIndex = QModelIndex();

    //Create the right-click menu
    QMenu menu(nullptr);
    menu.addAction(m_pCreatePlaylistAction);
    menu.addSeparator();
    menu.addAction(m_pCreateImportPlaylistAction);
    menu.exec(globalPos);
}

void PlaylistFeature::onRightClickChild(const QPoint& globalPos, QModelIndex index) {
    //Save the model index so we can get it in the action slots...
    m_lastRightClickedIndex = index;
    int playlistId = playlistIdFromIndex(index);

    bool locked = m_playlistDao.isPlaylistLocked(playlistId);
    m_pDeletePlaylistAction->setEnabled(!locked);
    m_pRenamePlaylistAction->setEnabled(!locked);

    m_pLockPlaylistAction->setText(locked ? tr("Unlock") : tr("Lock"));

    //Create the right-click menu
    QMenu menu(NULL);
    menu.addAction(m_pCreatePlaylistAction);
    menu.addSeparator();
    menu.addAction(m_pAddToAutoDJAction);
    menu.addAction(m_pAddToAutoDJTopAction);
    menu.addSeparator();
    menu.addAction(m_pRenamePlaylistAction);
    menu.addAction(m_pDuplicatePlaylistAction);
    menu.addAction(m_pDeletePlaylistAction);
    menu.addAction(m_pLockPlaylistAction);
    menu.addSeparator();
    menu.addAction(m_pAnalyzePlaylistAction);
    menu.addSeparator();
    menu.addAction(m_pImportPlaylistAction);
    menu.addAction(m_pExportPlaylistAction);
    menu.addAction(m_pExportTrackFilesAction);
    menu.exec(globalPos);
}


bool PlaylistFeature::dragMoveAccept(QUrl url) {
    return SoundSourceProxy::isUrlSupported(url) ||
            Parser::isPlaylistFilenameSupported(url.toLocalFile());
}


bool PlaylistFeature::dropAcceptChild(const QModelIndex& index, QList<QUrl> urls,
                                      QObject* pSource) {
    int playlistId = playlistIdFromIndex(index);
    //m_playlistDao.appendTrackToPlaylist(url.toLocalFile(), playlistId);

    QList<QFileInfo> files = DragAndDropHelper::supportedTracksFromUrls(urls, false, true);

    QList<TrackId> trackIds;
    if (pSource) {
        trackIds = m_pTrackCollection->getTrackDAO().getTrackIds(files);
        m_pTrackCollection->getTrackDAO().unhideTracks(trackIds);
    } else {
        // If a track is dropped onto a playlist's name, but the track isn't in the
        // library, then add the track to the library before adding it to the
        // playlist.
        // Adds track, does not insert duplicates, handles unremoving logic.
        trackIds = m_pTrackCollection->getTrackDAO().addMultipleTracks(files, true);
    }

    // remove tracks that could not be added
    for (int trackIdIndex = 0; trackIdIndex < trackIds.size(); ++trackIdIndex) {
        if (!trackIds.at(trackIdIndex).isValid()) {
            trackIds.removeAt(trackIdIndex--);
        }
    }

    // Request a name for the playlist if it's a new playlist
    if (playlistId < 0) {
        QString name = getValidPlaylistName();
        if (name.isNull()) {
            // The user has canceled
            return false;
        }
        
        playlistId = m_playlistDao.createPlaylist(name);
        // An error happened
        if (playlistId < 0) {
            return false;
        }
    }
    
    // Return whether appendTracksToPlaylist succeeded.
    return m_playlistDao.appendTracksToPlaylist(trackIds, playlistId);
}

bool PlaylistFeature::dragMoveAcceptChild(const QModelIndex& index, QUrl url) {    
    int playlistId = playlistIdFromIndex(index);
    bool locked = m_playlistDao.isPlaylistLocked(playlistId);

    bool formatSupported = SoundSourceProxy::isUrlSupported(url) ||
            Parser::isPlaylistFilenameSupported(url.toLocalFile());
    return !locked && formatSupported;
}

void PlaylistFeature::buildPlaylistList() {
    m_playlistList.clear();

    QString queryString = "SELECT "
                          "Playlists.id as id, "
                          "Playlists.name as name, "
                          "COUNT(library.id) as count, "
                          "SUM(library.duration) as durationSeconds "
                          "FROM Playlists "
                          "LEFT JOIN PlaylistTracks ON PlaylistTracks.playlist_id = Playlists.id "
                          "LEFT JOIN library ON PlaylistTracks.track_id = library.id "
                          "WHERE Playlists.hidden = 0 "
                          "GROUP BY Playlists.id "
                          "ORDER BY name";
    QSqlQuery query(m_pTrackCollection->getDatabase());
    if (!query.exec(queryString)) {
        LOG_FAILED_QUERY(query);
    }
    
    QSqlRecord record = query.record();
    int iName = record.indexOf("name");
    int iId = record.indexOf("id");
    int iCount = record.indexOf("count");
    int iDuration = record.indexOf("durationSeconds");
    
    while (query.next()) {
        int id = query.value(iId).toInt();
        QString name = query.value(iName).toString();
        int count = query.value(iCount).toInt();
        int duration = query.value(iDuration).toInt();
        QString itemName = "%1 (%2) %3";
        itemName = itemName.arg(name, 
                                QString::number(count),
                                mixxx::Duration::formatSeconds(duration));
        m_playlistList << PlaylistItem(id, itemName);
    }
}

void PlaylistFeature::decorateChild(TreeItem* item, int playlist_id) {    
    if (m_playlistDao.isPlaylistLocked(playlist_id)) {
        item->setIcon(QIcon(":/images/library/ic_library_locked.png"));
    } else {
        item->setIcon(QIcon());
    }
}

PlaylistTableModel* PlaylistFeature::constructTableModel() {
    return new PlaylistTableModel(this, m_pTrackCollection, "mixxx.db.model.playlist");
}

void PlaylistFeature::slotPlaylistTableChanged(int playlistId) {
    if (!m_pPlaylistTableModel) {
        return;
    }

    //qDebug() << "slotPlaylistTableChanged() playlistId:" << playlistId;
    enum PlaylistDAO::HiddenType type = m_playlistDao.getHiddenType(playlistId);
    if (type == PlaylistDAO::PLHT_NOT_HIDDEN ||
        type == PlaylistDAO::PLHT_UNKNOWN) { // In case of a deleted Playlist
        m_lastRightClickedIndex = constructChildModel(playlistId);
    }
}

void PlaylistFeature::slotPlaylistContentChanged(int playlistId) {
    if (!m_pPlaylistTableModel) {
        return;
    }

    //qDebug() << "slotPlaylistContentChanged() playlistId:" << playlistId;
    enum PlaylistDAO::HiddenType type = m_playlistDao.getHiddenType(playlistId);
    if (type == PlaylistDAO::PLHT_NOT_HIDDEN ||
        type == PlaylistDAO::PLHT_UNKNOWN) { // In case of a deleted Playlist
        updateChildModel(playlistId);
    }
}



void PlaylistFeature::slotPlaylistTableRenamed(int playlistId,
                                               QString /* a_strName */) {
    if (!m_pPlaylistTableModel) {
        return;
    }

    //qDebug() << "slotPlaylistTableChanged() playlistId:" << playlistId;
    enum PlaylistDAO::HiddenType type = m_playlistDao.getHiddenType(playlistId);
    if (type == PlaylistDAO::PLHT_NOT_HIDDEN ||
        type == PlaylistDAO::PLHT_UNKNOWN) { // In case of a deleted Playlist
        m_lastRightClickedIndex = constructChildModel(playlistId);
        if (type != PlaylistDAO::PLHT_UNKNOWN) {
            activatePlaylist(playlistId);
        }
    }
}

QString PlaylistFeature::getRootViewHtml() const {
    QString playlistsTitle = tr("Playlists");
    QString playlistsSummary = tr("Playlists are ordered lists of songs that allow you to plan your DJ sets.");
    QString playlistsSummary2 = tr("Some DJs construct playlists before they perform live, but others prefer to build them on-the-fly.");
    QString playlistsSummary3 = tr("When using a playlist during a live DJ set, remember to always pay close attention to how your audience reacts to the music you've chosen to play.");
    QString playlistsSummary4 = tr("It may be necessary to skip some songs in your prepared playlist or add some different songs in order to maintain the energy of your audience.");
    QString createPlaylistLink = tr("Create New Playlist");

    QString html;
    html.append(QString("<h2>%1</h2>").arg(playlistsTitle));
    html.append("<table border=\"0\" cellpadding=\"5\"><tr><td>");
    html.append(QString("<p>%1</p>").arg(playlistsSummary));
    html.append(QString("<p>%1</p>").arg(playlistsSummary2));
    html.append(QString("<p>%1 %2</p>").arg(playlistsSummary3,
                                            playlistsSummary4));
    html.append("</td></tr>");
    html.append(QString("<tr><td><a href=\"create\">%1</a>")
                .arg(createPlaylistLink));
    html.append("</td></tr></table>");
    return html;
}
