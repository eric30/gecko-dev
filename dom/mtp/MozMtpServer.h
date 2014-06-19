/* -*- Mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 40 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef mozilla_dom_mtp_mozmtpserver_h__
#define mozilla_dom_mtp_mozmtpserver_h__

#include "nsCOMPtr.h"
#include "nsIThread.h"

namespace android {
  class MOZ_EXPORT MtpServer;
}

namespace mozilla {
namespace dom {
namespace mtp {

class MozMtpServer
{
public:
  MozMtpServer();
  virtual ~MozMtpServer();

  void Run();
  void Write();

  nsCOMPtr<nsIThread> mServerThread;
  int mFd;
};

} // mtp
} // dom
} // mozilla

#endif

