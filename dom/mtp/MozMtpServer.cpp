/* -*- Mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 40 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

// For open()
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

// For errno
#include <errno.h>

// For printf
#include <stdio.h>

// For read()
#include <unistd.h>

#include "MozMtpServer.h"
#include "MozMtpDatabase.h"
#include "MozMtpCommon.h"

#include "nsThreadUtils.h"

#include "mtp.h"
#include "MtpDatabase.h"
#include "MtpObjectInfo.h"
#include "MtpServer.h"
#include "MtpStorage.h"
#include "MtpTypes.h"

#if defined(MOZ_WIDGET_GONK)
#include "cutils/properties.h"
#endif

using namespace android;
using namespace mozilla::dom;
using namespace mozilla::dom::mtp;

class MtpServerRunnable : public nsRunnable
{
public:
  nsresult Run()
  {
    int fd = open("/dev/mtp_usb", O_RDWR);
    if (fd < 0) {
      MTP_LOG("MozMtpServer open failed");
      return NS_OK;
    }

    MTP_LOG("MozMtpServer open done, fd: %d. Start reading.", fd);
    mFd = fd;

    MtpDatabase* database = new MozMtpDatabase();
    MtpServer* server = new MtpServer(mFd, database, false, 1023, 0664, 0775);

    MTP_LOG("MozMtpServer is running");
    server->run();
    MTP_LOG("MozMtpServer is over");

    delete server;

    return NS_OK;
  }

private:
  int mFd;
};

MozMtpServer::MozMtpServer()
{
  MTP_LOG("MozMtpServer ctor");

  if (property_set("sys.usb.config", "mtp,adb")!= 0) {
    MTP_LOG("property set failed.");
  }

  MTP_LOG("property set success: mtp enabled");
}

MozMtpServer::~MozMtpServer()
{
  MTP_LOG("MozMtpServer dtor");

  if (property_set("sys.usb.config",
                   "diag,serial_smd,serial_tty,rmnet_bam,mass_storage,adb")!= 0) {
    MTP_LOG("property set failed.");
  }

  MTP_LOG("property set success: mtp disabled");
}

void
MozMtpServer::Run()
{
  nsresult rv =
    NS_NewNamedThread("MtpServer", getter_AddRefs(mServerThread));
  mServerThread->Dispatch(new MtpServerRunnable(), 0);
}

