#ifndef EXAMPLES_GEOTOGDMLDUMPSVC_H
#define EXAMPLES_GEOTOGDMLDUMPSVC_H

// Gaudi
#include "GaudiKernel/Service.h"

/** @class GeoToGdmlDumpSvc Examples/src/GeoToGdmlDumpSvc.h GeoToGdmlDumpSvc.h
 *
 *  Service that dumps the geometry retrieved from the DD4hep GeoSvc to GDML file.
 *
 *  @author Anna Zaborowska
 */

class GeoToGdmlDumpSvc : public Service {
public:
  /// Constructor.
  explicit GeoToGdmlDumpSvc(const std::string& name, ISvcLocator* svcLoc);
  /// Initialize.
  virtual StatusCode initialize();
  /// Finalize.
  virtual StatusCode finalize();
  /// Destructor
  virtual ~GeoToGdmlDumpSvc() {}

private:
  /// Name of the GDML output file
  Gaudi::Property<std::string> m_gdmlFileName{this, "gdml", "GeantDetector.gdml", "Output GDML file name"};
};

#endif /* EXAMPLES_GEOTOGDMLDUMPSVC_H */
