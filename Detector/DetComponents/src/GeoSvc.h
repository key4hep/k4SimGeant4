#ifndef K4SIMGEANT4_GEOSVC_H
#define K4SIMGEANT4_GEOSVC_H

#include <Gaudi/Property.h>
#include <GaudiKernel/Service.h>

#include "k4Interface/IGeoSvc.h"

#include "DD4hep/Detector.h"

class G4VUserDetectorConstruction;

class GeoSvc : public extends<Service, IGeoSvc> {

public:
  GeoSvc(const std::string& name, ISvcLocator* svc);

  virtual StatusCode initialize() final;
  virtual StatusCode finalize() final;

  // This function generates the DD4hep geometry
  StatusCode buildDD4HepGeo();
  // This function generates the Geant4 geometry
  StatusCode buildGeant4Geo();
  // receive DD4hep Geometry
  virtual dd4hep::DetElement getDD4HepGeo() override;
  virtual dd4hep::Detector* getDetector() override;
  virtual std::string constantAsString(std::string const& name) override;
  // receive Geant4 Geometry
  virtual G4VUserDetectorConstruction* getGeant4Geo() override;

private:
  // Pointer to the interface to the DD4hep geometry
  dd4hep::Detector* m_dd4hepgeo{nullptr};
  // Pointer to the detector construction of DDG4
  std::shared_ptr<G4VUserDetectorConstruction> m_geant4geo{nullptr};

  Gaudi::Property<std::vector<std::string>> m_xmlFileNames{this, "detectors", {}, "Detector descriptions XML-files"};
  Gaudi::Property<std::map<std::string, std::string>> m_sensitive_types{
      this, "sensitiveTypes", {{"tracker", "SimpleTrackerSD"}, {"calorimeter", "SimpleCalorimeterSD"}}};
  Gaudi::Property<bool> m_buildGeant4Geo{this, "EnableGeant4Geo", true,
                                         "If True the DD4hep geometry is converted for Geant4 Simulations"};
};

#endif // K4SIMGEANT4_GEOSVC_H
