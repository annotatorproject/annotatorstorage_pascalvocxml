// Copyright 2017 Annotator Team
#include "PascalVocXMLPlugin.h"
#include "PascalVocXMLSaver.h"

#include <memory>

#include <Poco/ClassLibrary.h>

PascalVocXMLPlugin::PascalVocXMLPlugin() {}

bool PascalVocXMLPlugin::hasLoader() { return false; }

bool PascalVocXMLPlugin::hasSaver() { return true; }

bool PascalVocXMLPlugin::hasStorage() { return false; }

shared_ptr<AnnotatorLib::Storage::AbstractLoader>
PascalVocXMLPlugin::createLoader() {
  return nullptr;
}

shared_ptr<AnnotatorLib::Storage::AbstractSaver>
PascalVocXMLPlugin::createSaver() {
  return std::make_shared<PascalVocXMLSaver>();
}

shared_ptr<AnnotatorLib::Storage::AbstractStorage>
PascalVocXMLPlugin::createStorage() {
  return nullptr;
}

ANNOTATORLIB_API POCO_BEGIN_MANIFEST(AnnotatorLib::Storage::StoragePlugin)
    POCO_EXPORT_CLASS(PascalVocXMLPlugin) POCO_END_MANIFEST
