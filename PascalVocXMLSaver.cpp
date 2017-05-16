// Copyright 2016-2017 Annotator Team
#define Annotator_AnnotatorLib_Saver_PascalVocXMLSaver_BODY

/************************************************************
 XMLSaver class body
 ************************************************************/

// include associated header file
#include "PascalVocXMLSaver.h"
#include <AnnotatorLib/Algo/InterpolateAnnotation.h>
#include <AnnotatorLib/Frame.h>
#include <AnnotatorLib/Object.h>
#include <AnnotatorLib/Session.h>

#include <fstream>
#include <memory>
#include <unordered_map>

#include <boost/filesystem.hpp>

#include <Poco/DOM/DOMWriter.h>
#include <Poco/DOM/Text.h>
#include <Poco/XML/XMLWriter.h>

using namespace std;

void PascalVocXMLSaver::saveFrame(const AnnotatorLib::Session *session,
                                  const shared_ptr<AnnotatorLib::Frame> frame) {
  std::string number = std::to_string(frame->getFrameNumber());
  // prepend zeros
  number = std::string(8 - number.length(), '0') + number;

  boost::filesystem::path image_path(
      project->getImageSet()->getImagePath(frame->getFrameNumber()));
  std::string filename =
      path + "/" + image_path.filename().stem().string() + ".xml";

  std::ofstream ostr(filename, std::ios::out);
  Poco::XML::DOMWriter writer;
  writer.setNewLine("\n");
  writer.setOptions(Poco::XML::XMLWriter::PRETTY_PRINT);
  document = new Poco::XML::Document;

  Poco::AutoPtr<Poco::XML::Element> root =
      document->createElement("annotation");

  // folder
  Poco::AutoPtr<Poco::XML::Element> folder = document->createElement("folder");
  if (project) {
    folder->appendChild(
        document->createTextNode(image_path.parent_path().filename().string()));
  } else
    folder->appendChild(document->createTextNode("."));
  root->appendChild(folder);

  // filename
  Poco::AutoPtr<Poco::XML::Element> f = document->createElement("filename");
  if (project) {
    f->appendChild(
        document->createTextNode(image_path.filename().stem().string()));
  } else
    f->appendChild(document->createTextNode(number));
  root->appendChild(f);

  // path
  Poco::AutoPtr<Poco::XML::Element> p = document->createElement("path");
  if (project)
    p->appendChild(document->createTextNode(
        project->getImageSet()->getImagePath(frame->getFrameNumber())));
  else
    p->appendChild(document->createTextNode(path + "/" + number));
  root->appendChild(p);

  // source
  Poco::AutoPtr<Poco::XML::Element> source = document->createElement("source");
  Poco::AutoPtr<Poco::XML::Element> database =
      document->createElement("database");
  database->appendChild(document->createTextNode("Unknown"));
  source->appendChild(database);
  root->appendChild(source);

  // size
  Poco::AutoPtr<Poco::XML::Element> size = document->createElement("size");
  // width
  Poco::AutoPtr<Poco::XML::Element> size_width =
      document->createElement("width");
  int w = project->getImageSet()->getImage(frame->getFrameNumber()).cols;
  size_width->appendChild(document->createTextNode(std::to_string(w)));
  size->appendChild(size_width);
  // height
  Poco::AutoPtr<Poco::XML::Element> size_height =
      document->createElement("height");
  int h = project->getImageSet()->getImage(frame->getFrameNumber()).rows;
  size_height->appendChild(document->createTextNode(std::to_string(h)));
  size->appendChild(size_height);
  // depth
  Poco::AutoPtr<Poco::XML::Element> size_depth =
      document->createElement("depth");
  int d = project->getImageSet()->getImage(frame->getFrameNumber()).channels();
  size_depth->appendChild(document->createTextNode(std::to_string(d)));
  size->appendChild(size_depth);
  root->appendChild(size);

  // segmented
  Poco::AutoPtr<Poco::XML::Element> segmented =
      document->createElement("segmented");
  segmented->appendChild(document->createTextNode("0"));
  root->appendChild(segmented);

  // attributes
  for (auto &attribute : frame->getAttributes()) {
    Poco::AutoPtr<Poco::XML::Element> attrElement =
        document->createElement(attribute.second->getName());
    attrElement->appendChild(
        document->createTextNode(attribute.second->getValue()->toString()));
    root->appendChild(attrElement);
  }

  // object
  for (auto &pair : session->getObjects()) {
    if (session->getAnnotation(frame, pair.second))  // appears in frame?
      root->appendChild(fromObject(session, pair.second, frame));
  }

  document->appendChild(root);

  writer.writeNode(ostr, document);
}

void PascalVocXMLSaver::saveAnnotation(AnnotatorLib::Annotation) {}

void PascalVocXMLSaver::setPath(std::string path) { this->path = path; }

AnnotatorLib::StorageType PascalVocXMLSaver::getType() {
  return AnnotatorLib::StorageType::PASCALVOCXML;
}

void PascalVocXMLSaver::saveSession(const AnnotatorLib::Session *session) {
  for (auto &pair : session->getFrames()) {
    saveFrame(session, pair.second);
  }
}

void PascalVocXMLSaver::saveProject(
    std::shared_ptr<AnnotatorLib::Project> project) {
  this->project = project;
  saveSession(project->getSession().get());
}

bool PascalVocXMLSaver::close() { return true; }

Poco::AutoPtr<Poco::XML::Element> PascalVocXMLSaver::fromObject(
    const AnnotatorLib::Session *session,
    const shared_ptr<AnnotatorLib::Object> object,
    const shared_ptr<AnnotatorLib::Frame> frame) {
  Poco::AutoPtr<Poco::XML::Element> element = document->createElement("object");
  // name
  Poco::AutoPtr<Poco::XML::Element> name = document->createElement("name");
  name->appendChild(document->createTextNode(object->getClass()->getName()));
  element->appendChild(name);

  // bndbox
  shared_ptr<AnnotatorLib::Annotation> annotation =
      session->getAnnotation(frame, object);
  Poco::AutoPtr<Poco::XML::Element> bndbox = document->createElement("bndbox");

  Poco::AutoPtr<Poco::XML::Element> xmin = document->createElement("xmin");
  xmin->appendChild(
      document->createTextNode(std::to_string((int)annotation->getX())));
  Poco::AutoPtr<Poco::XML::Element> ymin = document->createElement("ymin");
  ymin->appendChild(
      document->createTextNode(std::to_string((int)annotation->getY())));
  Poco::AutoPtr<Poco::XML::Element> xmax = document->createElement("xmax");
  xmax->appendChild(document->createTextNode(
      std::to_string((int)(annotation->getX() + annotation->getWidth()))));
  Poco::AutoPtr<Poco::XML::Element> ymax = document->createElement("ymax");
  ymax->appendChild(document->createTextNode(
      std::to_string((int)(annotation->getY() + annotation->getHeight()))));

  bndbox->appendChild(xmin);
  bndbox->appendChild(ymin);
  bndbox->appendChild(xmax);
  bndbox->appendChild(ymax);

  element->appendChild(bndbox);

  // attributes
  for (shared_ptr<AnnotatorLib::Attribute> attribute :
       annotation->getAttributes()) {
    Poco::AutoPtr<Poco::XML::Element> attrElement =
        document->createElement(attribute->getName());
    attrElement->appendChild(
        document->createTextNode(attribute->getValue()->toString()));
    element->appendChild(attrElement);
  }

  return element;
}

/************************************************************
 End of XMLSaver class body
 ************************************************************/
