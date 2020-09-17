/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_PLAYLIST_BROWSER_PLAYLIST_SERVICE_H_
#define BRAVE_COMPONENTS_PLAYLIST_BROWSER_PLAYLIST_SERVICE_H_

#include <memory>
#include <string>
#include <vector>

#include "base/callback_forward.h"
#include "base/macros.h"
#include "base/memory/scoped_refptr.h"
#include "base/memory/weak_ptr.h"
#include "base/observer_list.h"
#include "base/values.h"
#include "brave/components/playlist/browser/playlist_download_request_manager.h"
#include "brave/components/playlist/browser/playlist_media_file_download_manager.h"
#include "brave/components/playlist/browser/playlist_thumbnail_downloader.h"
#include "brave/components/playlist/browser/playlist_types.h"
#include "components/keyed_service/core/keyed_service.h"

namespace base {
class FilePath;
class SequencedTaskRunner;
}  // namespace base

namespace content {
class BrowserContext;
class WebContents;
}  // namespace content

class PrefService;

namespace playlist {

class PlaylistServiceObserver;
class PlaylistYoutubeDownComponentManager;

class PlaylistService : public KeyedService,
                        public PlaylistMediaFileDownloadManager::Delegate,
                        public PlaylistThumbnailDownloader::Delegate,
                        public PlaylistDownloadRequestManager::Delegate {
 public:
  PlaylistService(content::BrowserContext* context,
                  PlaylistYoutubeDownComponentManager* manager);
  ~PlaylistService() override;
  PlaylistService(const PlaylistService&) = delete;
  PlaylistService& operator=(const PlaylistService&) = delete;

  base::Value GetAllPlaylistItems();
  base::Value GetPlaylistItem(const std::string& id);
  void RecoverPlaylistItem(const std::string& id);
  void DeletePlaylistItem(const std::string& id);
  void DeleteAllPlaylistItems();

  void AddObserver(PlaylistServiceObserver* observer);
  void RemoveObserver(PlaylistServiceObserver* observer);

  bool GetThumbnailPath(const std::string& id, base::FilePath* thumbnail_path);

  base::FilePath GetPlaylistItemDirPath(const std::string& id) const;

  void RequestDownload(const std::string& url);

 private:
  // KeyedService overrides:
  void Shutdown() override;

  // PlaylistMediaFileDownloadManager::Delegate overrides:
  void OnMediaFileReady(base::Value playlist_value, bool partial) override;
  void OnMediaFileGenerationFailed(base::Value playlist_value) override;

  // PlaylistThumbnailDownloader::Delegate overrides:
  void OnThumbnailDownloaded(const std::string& id,
                             const base::FilePath& path) override;

  // PlaylistDownloadRequestManager::Delegate overrides:
  void OnPlaylistCreationParamsReady(
      const CreatePlaylistParams& params) override;
  void OnPlaylistItemDirCreated(const std::string& id, bool directory_ready);

  void CreatePlaylistItem(const CreatePlaylistParams& params);
  void DownloadThumbnail(const std::string& id);
  void GenerateMediafileForPlaylistItem(const std::string& id);

  base::SequencedTaskRunner* task_runner();

  // Delete orphaned playlist item directories that are not included in db.
  void CleanUp();
  void OnGetOrphanedPaths(const std::vector<base::FilePath> paths);

  void NotifyPlaylistChanged(const PlaylistChangeParams& params);

  void UpdatePlaylistValue(const std::string& id, base::Value value);
  void RemovePlaylist(const std::string& id);

  // index.html is only used for demo (brave://playlist)
  void GenerateIndexHTMLFile(const base::FilePath& playlist_path);
  void OnHTMLFileGenerated(bool generated);

  // Playlist creation can be ready to play two steps.
  // Step 1. When creation is requested, requested info is put to db and
  //         notification is delivered to user with basic infos like playlist
  //         name and titles if provided. playlist is visible to user but it
  //         doesn't have thumbnail.
  // Step 1. Request thumbnail.
  //         Request video files and audio files and combined as a single
  //         video and audio file.
  //         Whenever thumbnail is fetched or media files are ready,
  //         it is notified.

  void OnGetMetadata(base::Value value);

  const base::FilePath base_dir_;
  base::ObserverList<PlaylistServiceObserver> observers_;

  std::unique_ptr<PlaylistMediaFileDownloadManager>
      media_file_download_manager_;
  std::unique_ptr<PlaylistThumbnailDownloader> thumbnail_downloader_;
  std::unique_ptr<PlaylistDownloadRequestManager> download_request_manager_;
  scoped_refptr<base::SequencedTaskRunner> task_runner_;
  PrefService* prefs_;

  base::WeakPtrFactory<PlaylistService> weak_factory_;
};

}  // namespace playlist

#endif  // BRAVE_COMPONENTS_PLAYLIST_BROWSER_PLAYLIST_SERVICE_H_
