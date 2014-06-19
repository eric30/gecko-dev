/* -*- Mode: c++; c-basic-offset: 2; indent-tabs-mode: nil; tab-width: 40 -*- */
/* vim: set ts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */
#ifndef _MOZ_MTP_DATABASE_H
#define _MOZ_MTP_DATABASE_H

#include "MtpDatabase.h"

#include "MozMtpCommon.h"

namespace android {
  class MOZ_EXPORT MtpDatabase;
}

using namespace android;

namespace mozilla {
namespace dom {
namespace mtp {

class MozMtpDatabase : public MtpDatabase {
public:
  virtual ~MozMtpDatabase()
  {
    MTP_LOG("MozMtpDatabase dtor");
  }

  virtual MtpObjectHandle beginSendObject(const char* path,
    MtpObjectFormat format, MtpObjectHandle parent, MtpStorageID storage,
    uint64_t size, time_t modified)
  {
    MtpObjectHandle a;
    MTP_LOG("MozMtpDatabase beginSendObject");

    return a;
  }

  virtual void endSendObject(const char* path, MtpObjectHandle handle,
    MtpObjectFormat format, bool succeeded)
  {
    MTP_LOG("MozMtpDatabase endSendObject");
  }

  virtual MtpObjectHandleList* getObjectList(MtpStorageID storageID,
    MtpObjectFormat format, MtpObjectHandle parent)
  {
    MTP_LOG("MozMtpDatabase getObjectList");
    return nullptr;
  }

  virtual int getNumObjects(MtpStorageID storageID, MtpObjectFormat format,
                            MtpObjectHandle parent)
  {
    MTP_LOG("MozMtpDatabase getNumObjects");
    return 0;
  }

  // callee should delete[] the results from these
  // results can be NULL
  virtual MtpObjectFormatList* getSupportedPlaybackFormats()
  {
    MTP_LOG("MozMtpDatabase getSupportedPlaybackFormats");
    return nullptr;
  }

  virtual MtpObjectFormatList* getSupportedCaptureFormats()
  {
    MTP_LOG("MozMtpDatabase getSupportedCaptureFormats");
    return nullptr;
  }

  virtual MtpObjectPropertyList*
  getSupportedObjectProperties(MtpObjectFormat format)
  {
    MTP_LOG("MozMtpDatabase getSupportedObjectProperties");
    return nullptr;
  }

  virtual MtpDevicePropertyList* getSupportedDeviceProperties()
  {
    MTP_LOG("MozMtpDatabase getSupportedDeviceProperties");
    return nullptr;
  }

  virtual MtpResponseCode getObjectPropertyValue(MtpObjectHandle handle,
      MtpObjectProperty property,
      MtpDataPacket& packet)
  {
    MTP_LOG("MozMtpDatabase getObjectPropertyValue");
    return 0;
  }

  virtual MtpResponseCode setObjectPropertyValue(MtpObjectHandle handle,
    MtpObjectProperty property, MtpDataPacket& packet)
  {
    MTP_LOG("MozMtpDatabase setObjectPropertyValue");
    return 0;
  }

  virtual MtpResponseCode getDevicePropertyValue(MtpDeviceProperty property,
    MtpDataPacket& packet)
  {
    MTP_LOG("MozMtpDatabase getDevicePropertyValue");
    return 0;
  }

  virtual MtpResponseCode setDevicePropertyValue(MtpDeviceProperty property,
    MtpDataPacket& packet)
  {
    MTP_LOG("MozMtpDatabase setDevicePropertyValue");
    return 0;
  }

  virtual MtpResponseCode resetDeviceProperty(MtpDeviceProperty property)
  {
    MTP_LOG("MozMtpDatabase resetDeviceProperty");
    return 0;
  }

  virtual MtpResponseCode
  getObjectPropertyList(MtpObjectHandle handle,
                        uint32_t format, uint32_t property,
                        int groupCode, int depth,
                        MtpDataPacket& packet)
  {
    MTP_LOG("MozMtpDatabase getObjectPropertyList");
    return 0;
  }

  virtual MtpResponseCode
  getObjectInfo(MtpObjectHandle handle, MtpObjectInfo& info)
  {
    MTP_LOG("MozMtpDatabase getObjectInfo");
    return 0;
  }

  virtual void*
  getThumbnail(MtpObjectHandle handle, size_t& outThumbSize)
  {
    MTP_LOG("MozMtpDatabase getThunbnail");
    return nullptr;
  }

  virtual MtpResponseCode getObjectFilePath(MtpObjectHandle handle,
      MtpString& outFilePath,
      int64_t& outFileLength,
      MtpObjectFormat& outFormat)
  {
    MTP_LOG("MozMtpDatabase getObjectFilePath");
    return 0;
  }

  virtual MtpResponseCode
  deleteFile(MtpObjectHandle handle)
  {
    MTP_LOG("MozMtpDatabase deleteFile");
    return 0;
  }

  virtual MtpObjectHandleList* getObjectReferences(MtpObjectHandle handle)
  {
    MTP_LOG("MozMtpDatabase getObjectReferences");
    return nullptr;
  }

  virtual MtpResponseCode
  setObjectReferences(MtpObjectHandle handle,
                      MtpObjectHandleList* references)
  {
    MTP_LOG("MozMtpDatabase setObjectReferences");
    return 0;
  }

  virtual MtpProperty*
  getObjectPropertyDesc(MtpObjectProperty property,
                        MtpObjectFormat format)
  {
    return nullptr;
  }

  virtual MtpProperty*
  getDevicePropertyDesc(MtpDeviceProperty property)
  {
    return nullptr;
  }

  virtual void sessionStarted()
  {
    MTP_LOG("MozMtpDatabase sessionStarted");
  }

  virtual void sessionEnded()
  {
    MTP_LOG("MozMtpDatabase sessionEnded");
  }
};

} // mtp
} // dom
} // mozilla

#endif
