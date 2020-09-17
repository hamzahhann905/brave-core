/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_PLAYLIST_BROWSER_PLAYLIST_DOWNLOAD_REQUEST_MANAGER_H_
#define BRAVE_COMPONENTS_PLAYLIST_BROWSER_PLAYLIST_DOWNLOAD_REQUEST_MANAGER_H_

#include <list>
#include <memory>
#include <string>

#include "base/memory/weak_ptr.h"
#include "base/scoped_observer.h"
#include "brave/components/playlist/browser/playlist_types.h"
#include "brave/components/playlist/browser/playlist_youtubedown_component_manager.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_contents_observer.h"

namespace base {
class Value;
}  // namespace base

namespace content {
class BrowserContext;
}  // namespace content

namespace playlist {

struct CreatePlaylistParams;

class PlaylistDownloadRequestManager :
    public PlaylistYoutubeDownComponentManager::Observer,
    public content::WebContentsObserver {
 public:
  class Delegate {
   public:
    virtual void OnPlaylistCreationParamsReady(
        const CreatePlaylistParams& params) = 0;
  };
  PlaylistDownloadRequestManager(content::BrowserContext* context,
                                 Delegate* delegate,
                                 PlaylistYoutubeDownComponentManager* manager);
  ~PlaylistDownloadRequestManager() override;
  PlaylistDownloadRequestManager(
      const PlaylistDownloadRequestManager&) = delete;
  PlaylistDownloadRequestManager& operator=(
      const PlaylistDownloadRequestManager&) = delete;

  // Delegate will get called with generated param.
  void GeneratePlaylistCreateParamsForYoutubeURL(const std::string& url);

 private:
  // PlaylistYoutubeDownComponentManager::Observer overrides:
  void OnYoutubeDownScriptReady(const std::string& youtubedown_script) override;

  // content::WebContentsObserver overrides:
  void RenderFrameCreated(content::RenderFrameHost* render_frame_host) override;

  // Will get date from youtube for downloading media files of |url| by
  // injecting youtubedown.js.
  void FetchYoutubeDownData(const std::string& url) const;
  void OnGetYoutubeDownData(base::Value value);
  bool ReadyToRunYoutubeDownJS() const;
  void CreateWebContents();
  void FetchAllPendingYoutubeURLs();

  std::list<std::string> pending_youtube_urls_;
  // Used to inject youtubedown.js to get playlist item metadata to download
  // its media files/thumbnail mages and get titile.
  std::unique_ptr<content::WebContents> webcontents_;
  bool webcontents_ready_ = false;
  std::string youtubedown_script_;
  content::BrowserContext* context_;
  Delegate* delegate_;
  PlaylistYoutubeDownComponentManager* youtubedown_component_manager_;
  ScopedObserver<PlaylistYoutubeDownComponentManager,
                 PlaylistYoutubeDownComponentManager::Observer> observed_{this};

  base::WeakPtrFactory<PlaylistDownloadRequestManager> weak_factory_{this};
};

}  // namespace playlist

#endif  // BRAVE_COMPONENTS_PLAYLIST_BROWSER_PLAYLIST_DOWNLOAD_REQUEST_MANAGER_H_
