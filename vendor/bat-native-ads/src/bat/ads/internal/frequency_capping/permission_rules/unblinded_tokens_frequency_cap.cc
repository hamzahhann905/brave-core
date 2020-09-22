/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "bat/ads/internal/frequency_capping/permission_rules/unblinded_tokens_frequency_cap.h"

#include "bat/ads/internal/ads_impl.h"

namespace ads {

UnblindedTokensFrequencyCap::UnblindedTokensFrequencyCap(
    const AdsImpl* const ads)
    : ads_(ads) {
  DCHECK(ads_);
}

UnblindedTokensFrequencyCap::~UnblindedTokensFrequencyCap() = default;

bool UnblindedTokensFrequencyCap::IsAllowed() {
  if (!DoesRespectCap()) {
    last_message_ = "You do not have any unblinded tokens";

    return false;
  }

  return true;
}

std::string UnblindedTokensFrequencyCap::get_last_message() const {
  return last_message_;
}

bool UnblindedTokensFrequencyCap::DoesRespectCap() const {
  const uint64_t count = ads_->get_unblinded_tokens()->Count();
  if (count < 3) {
    return false;
  }

  return true;
}

}  // namespace ads
