/* Copyright (c) 2020 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_UI_BRAVE_CUSTOM_NOTIFICATION_PUBLIC_CPP_NOTIFICATION_DELEGATE_H_
#define BRAVE_UI_BRAVE_CUSTOM_NOTIFICATION_PUBLIC_CPP_NOTIFICATION_DELEGATE_H_

#include <memory>
#include <string>

#include "base/callback.h"
#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "base/memory/weak_ptr.h"
#include "base/optional.h"
#include "base/strings/string16.h"

namespace brave_custom_notification {

// Handles actions performed on a notification.
class NotificationObserver {
 public:
  // Called when the desktop notification is closed. If closed by a user
  // explicitly (as opposed to timeout/script), |by_user| should be true.
  virtual void Close(bool by_user) {}

  // Called when a desktop notification is clicked. |button_index| is filled in
  // if a button was clicked (as opposed to the body of the notification) while
  // |reply| is filled in if there was an input field associated with the
  // button.
  virtual void Click(const base::Optional<int>& button_index,
                     const base::Optional<base::string16>& reply) {}
};

// Ref counted version of NotificationObserver, required to satisfy
// brave_custom_notification::Notification::delegate_.
class NotificationDelegate : public NotificationObserver,
  public base::RefCountedThreadSafe<NotificationDelegate> {
 protected:
  virtual ~NotificationDelegate() = default;

 private:
  friend class base::RefCountedThreadSafe<NotificationDelegate>;
};

// A pass-through which converts the RefCounted requirement to a WeakPtr
// requirement. This class replaces the need for individual delegates that pass
// through to an actual controller class, and which only exist because the
// actual controller has a strong ownership model.
class ThunkNotificationDelegate
    : public NotificationDelegate {
 public:
  explicit ThunkNotificationDelegate(base::WeakPtr<NotificationObserver> impl);

  // NotificationDelegate:
  void Close(bool by_user) override;
  void Click(const base::Optional<int>& button_index,
             const base::Optional<base::string16>& reply) override;

 protected:
  ~ThunkNotificationDelegate() override;

 private:
  base::WeakPtr<NotificationObserver> impl_;

  DISALLOW_COPY_AND_ASSIGN(ThunkNotificationDelegate);
};

// A simple notification delegate which invokes the passed closure when the body
// or a button is clicked.
class HandleNotificationClickDelegate
    : public NotificationDelegate {
 public:
  // The parameter is the index of the button that was clicked, or nullopt if
  // the body was clicked.
  using ButtonClickCallback =
      base::RepeatingCallback<void(base::Optional<int>)>;

  // Creates a delegate that handles clicks on a button or on the body.
  explicit HandleNotificationClickDelegate(const ButtonClickCallback& callback);

  // Creates a delegate that only handles clicks on the body of the
  // notification.
  explicit HandleNotificationClickDelegate(
      const base::RepeatingClosure& closure);

  // Overrides the callback with one that handles clicks on a button or on the
  // body.
  void SetCallback(const ButtonClickCallback& callback);

  // Overrides the callback with one that only handles clicks on the body of the
  // notification.
  void SetCallback(const base::RepeatingClosure& closure);

  // NotificationDelegate overrides:
  void Click(const base::Optional<int>& button_index,
             const base::Optional<base::string16>& reply) override;

 protected:
  ~HandleNotificationClickDelegate() override;

 private:
  ButtonClickCallback callback_;

  DISALLOW_COPY_AND_ASSIGN(HandleNotificationClickDelegate);
};

}  // namespace brave_custom_notification

#endif  // BRAVE_UI_BRAVE_CUSTOM_NOTIFICATION_PUBLIC_CPP_NOTIFICATION_DELEGATE_H_
