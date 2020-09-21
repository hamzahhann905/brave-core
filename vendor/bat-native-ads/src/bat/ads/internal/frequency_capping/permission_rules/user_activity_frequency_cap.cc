/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/frequency_capping/permission_rules/user_activity_frequency_cap.h"

#include "bat/ads/internal/frequency_capping/frequency_capping_util.h"
#include "bat/ads/internal/time_util.h"

namespace ads {

UserActivityFrequencyCap::UserActivityFrequencyCap(
    const UserActivityHistoryMap& history)
    : history_(history) {
}

UserActivityFrequencyCap::~UserActivityFrequencyCap() = default;

bool UserActivityFrequencyCap::IsAllowed() {
  if (!DoesRespectCap()) {
    // TODO(tmancey): Remove following line before merging
    last_message_ = "Ad cannot be shown";

    return false;
  }

  return true;
}

std::string UserActivityFrequencyCap::get_last_message() const {
  return last_message_;
}

bool UserActivityFrequencyCap::DoesRespectCap() const {
  const uint64_t time_constraint = base::Time::kSecondsPerHour;

  const uint64_t cap = 1;

  int count = 0;

  for (const auto& entry : history_) {
    const UserActivityHistory user_activity_history = entry.second;

    if (!DoesHistoryRespectCapForRollingTimeConstraint(user_activity_history,
        time_constraint, cap)) {
      count++;
    }
  }

  if (count < 2) {
    return false;
  }

  return true;
}

}  // namespace ads
