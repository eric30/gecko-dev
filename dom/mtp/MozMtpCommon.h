/* -*- Mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 40 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_mtp_mozmtpcommon_h__
#define mozilla_dom_mtp_mozmtpcommon_h__

#ifdef MOZ_WIDGET_GONK
#include <android/log.h>

#define MTP_LOG(msg, ...)                                            \
  __android_log_print(ANDROID_LOG_INFO, "GeckoMtp",                  \
                      "%s: " msg, __FUNCTION__, ##__VA_ARGS__)       \

#define BEGIN_MTP_NAMESPACE \
  namespace mozilla { namespace dom { namespace mtp {
#define END_MTP_NAMESPACE \
  } /* namespace mtp */ } /* namespace dom */ } /* namespace mozilla */
#define USING_MTP_NAMESPACE \
  using namespace mozilla::dom::mtp;

#endif
#endif // mozilla_dom_mtp_mtpcommon_h__

