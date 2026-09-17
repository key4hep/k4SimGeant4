#include "k4Interface/IGeoSvc.h"

#include "Gaudi/Property.h"
#include "GaudiKernel/Service.h"

/**
 *
 *  Service that facilitates material scan on initialize
 *  This service outputs a ROOT file containing a TTree with radiation lengths and material thickness
 *
 */

class MaterialScan : public Service {
public:
  explicit MaterialScan(const std::string& name, ISvcLocator* svcLoc);

  StatusCode initialize() override;

private:
  /// name of the output file
  Gaudi::Property<std::string> m_filename{this, "filename", "", "file name to save the tree to"};
  /// Handle to the geometry service from which the detector is retrieved
  ServiceHandle<IGeoSvc> m_geoSvc;

  /// Step size in eta (deprecated, use angleBinning instead)
  Gaudi::Property<double> m_etaBinning{this, "etaBinning", -1, "eta bin size"};
  /// Step size in eta/theta/cosTheta/thetaRad
  Gaudi::Property<double> m_angleBinning{this, "angleBinning", 0.05, "eta/theta/cosTheta/thetaRad bin size"};

  /// Maximum eta until which to scan, scan is performed from -m_etaMax to +m_etaMax (deprecated, use angleMax instead)
  Gaudi::Property<double> m_etaMax{this, "etaMax", -1, "maximum eta value"};
  /// Maximum eta/theta/cosTheta/thetaRad until which to scan
  Gaudi::Property<double> m_angleMax{this, "angleMax", 6, "maximum eta/theta/cosTheta/thetaRad value"};

  /// Minimum eta/theta/cosTheta/thetaRad until which to scan
  Gaudi::Property<double> m_angleMin{this, "angleMin", -6, "minimum eta/theta/cosTheta/thetaRad value"};

  /// number of random, uniformly distributed phi values to average over (deprecated, use nPhi instead)
  Gaudi::Property<int> m_nPhiTrials{this, "nPhiTrials", -1,
                                    "number of random, uniformly distributed phi values to average over"};
  /// number of phi values to run over, evenly distributed from 0 to 2 pi
  Gaudi::Property<int> m_nPhi{this, "nPhi", 100, "number of phi values to run over, evenly distributed from 0 to 2 pi"};

  /// angle definition to use: eta, theta, cosTheta or thetaRad default: eta
  Gaudi::Property<std::string> m_angleDef{
      this, "angleDef", "eta",
      "angle definition to use: 'eta', 'theta' (in degrees), 'cosTheta' or 'thetaRad' (in rad)"};

  /// Name of the envelope within which the material is measured (by default: world volume)
  Gaudi::Property<std::string> m_envelopeName{this, "envelopeName", "world",
                                              "name of the envelope within which the material is measured"};
};
