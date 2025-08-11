#include "GeoSvc.h"
#include "GeoConstruction.h"

#include <GaudiKernel/IMessageSvc.h>
#include <GaudiKernel/Service.h>

#include <TGeoManager.h>

#include <DD4hep/Detector.h>
#include <DD4hep/Printout.h>

GeoSvc::GeoSvc(const std::string& name, ISvcLocator* svc) : base_class(name, svc) {}

StatusCode GeoSvc::initialize() {
  {
    StatusCode sc = Service::initialize();
    if (!sc.isSuccess()) {
      return sc;
    }
  }

  // Turn off TGeo printouts if appropriate for the msg level
  if (msgLevel() >= MSG::INFO) {
    TGeoManager::SetVerboseLevel(0);
  }
  uint printoutLevel = msgLevel();
  dd4hep::setPrintLevel(dd4hep::PrintLevel(printoutLevel));

  // Build DD4hep Geometry
  {
    StatusCode sc = buildDD4HepGeo();
    if (sc.isFailure()) {
      error() << "Could not build DD4hep geometry!" << endmsg;
      return sc;
    } else {
      info() << "DD4hep geometry SUCCESSFULLY built." << endmsg;
    }
  }

  // Build Geant4 Geometry
  if (m_buildGeant4Geo) {
    StatusCode sc = buildGeant4Geo();
    if (sc.isFailure()) {
      error() << "Could not build Geant4 geometry!" << endmsg;
    } else {
      info() << "Geant4 geometry SUCCESSFULLY built." << endmsg;
    }
  } else {
    debug() << "Conversion to Geant4 Geometry is disabled" << endmsg;
  }

  return StatusCode::SUCCESS;
}

StatusCode GeoSvc::finalize() { return StatusCode::SUCCESS; }

StatusCode GeoSvc::buildDD4HepGeo() {
  // Retrieve the static instance of the DD4HEP::Geometry
  m_dd4hepgeo = &dd4hep::Detector::getInstance();
  m_dd4hepgeo->addExtension<IGeoSvc>(this);

  // Load geometry
  info() << "Detector geometry will be loaded from the following file(s):" << endmsg;
  for (const auto& filename : m_xmlFileNames) {
    info() << "  - " << filename << endmsg;
  }
  for (const auto& filename : m_xmlFileNames) {
    info() << "Loading geometry from file: " << filename << endmsg;
    m_dd4hepgeo->fromCompact(filename);
  }
  if (!m_dd4hepgeo->volumeManager().isValid()) {
    m_dd4hepgeo->apply("DD4hepVolumeManager", 0, nullptr);
  }

  return StatusCode::SUCCESS;
}

dd4hep::Detector* GeoSvc::getDetector() { return m_dd4hepgeo; }

dd4hep::DetElement GeoSvc::getDD4HepGeo() { return m_dd4hepgeo->world(); }

StatusCode GeoSvc::buildGeant4Geo() {
  if (!m_buildGeant4Geo) {
    return StatusCode::SUCCESS;
  }
  m_geant4geo = std::make_shared<det::GeoConstruction>(*m_dd4hepgeo, m_sensitive_types);
  if (m_geant4geo) {
    return StatusCode::SUCCESS;
  }

  return StatusCode::FAILURE;
}

G4VUserDetectorConstruction* GeoSvc::getGeant4Geo() { return m_geant4geo.get(); }

std::string GeoSvc::constantAsString(const std::string& name) { return m_dd4hepgeo->constantAsString(name); }

DECLARE_COMPONENT(GeoSvc)
