// Copyright 2017 Annotator Team
#ifndef PASCALVOCXMLPLUGIN_H
#define PASCALVOCXMLPLUGIN_H

#include <AnnotatorLib/Storage/StoragePlugin.h>

class PascalVocXMLPlugin : public AnnotatorLib::Storage::StoragePlugin {
 public:
  PascalVocXMLPlugin();

  virtual const std::string name() { return "pascalvocxml"; }
  virtual const std::string description() { return "PascalVoc XML Storage"; }

  virtual bool hasLoader();
  virtual bool hasSaver();
  virtual bool hasStorage();

  virtual shared_ptr<AnnotatorLib::Loader::AbstractLoader> createLoader();
  virtual shared_ptr<AnnotatorLib::Saver::AbstractSaver> createSaver();
  virtual shared_ptr<AnnotatorLib::Storage::AbstractStorage> createStorage();
};

#endif  // PASCALVOCXMLPLUGIN_H
