#ifndef DETSTUDIES_SAMPLINGFRACTIONINLAYERTHETATREE_H
#define DETSTUDIES_SAMPLINGFRACTIONINLAYERTHETATREE_H

// GAUDI
#include "Gaudi/Algorithm.h"
#include "GaudiKernel/ServiceHandle.h"

// FCCSW
#include "k4FWCore/DataHandle.h"

class IGeoSvc;
class ITHistSvc;
class TTree;

// datamodel
namespace edm4hep {
class SimCalorimeterHitCollection;
}

/** @class SamplingFractionInLayerThetaTree SamplingFractionInLayerThetaTree.h
 *
 *  Writes event-level layer-theta sampling fraction entries into a ROOT TTree.
 *  Each tree row corresponds to one non-empty (layer, theta) bin in one event.
 */
class SamplingFractionInLayerThetaTree : public Gaudi::Algorithm {
public:
  explicit SamplingFractionInLayerThetaTree(const std::string&, ISvcLocator*);
  virtual ~SamplingFractionInLayerThetaTree();

  virtual StatusCode initialize() final;
  virtual StatusCode execute(const EventContext&) const final;
  virtual StatusCode finalize() final;

private:
  ServiceHandle<ITHistSvc> m_histSvc;
  ServiceHandle<IGeoSvc> m_geoSvc;

  mutable k4FWCore::DataHandle<edm4hep::SimCalorimeterHitCollection> m_deposits{"rec/caloHits",
                                                                                Gaudi::DataHandle::Reader, this};

  Gaudi::Property<std::string> m_activeFieldName{this, "activeFieldName", "", "Identifier of active material"};
  Gaudi::Property<int> m_activeFieldValue{this, "activeFieldValue", 0, "Value of identifier for active material"};
  Gaudi::Property<std::string> m_layerFieldName{this, "layerFieldName", "", "Identifier of layers"};
  Gaudi::Property<std::string> m_thetaFieldName{this, "thetaFieldName", "theta", "Identifier of theta bins"};
  Gaudi::Property<unsigned int> m_numLayers{this, "numLayers", 8, "Number of layers"};
  Gaudi::Property<int> m_firstLayerId{this, "firstLayerId", 0, "First layer id included in output"};
  Gaudi::Property<unsigned int> m_numThetaBins{this, "numThetaBins", 1024, "Number of theta bins"};
  Gaudi::Property<int> m_firstThetaId{this, "firstThetaId", 0, "First theta id included in output"};
  Gaudi::Property<std::string> m_readoutName{this, "readoutName", "", "Name of the detector readout"};
  Gaudi::Property<std::string> m_treePath{this, "treePath", "/rec/ecal_sf_layer_theta_tree",
                                          "THistSvc path for the output TTree"};

  TTree* m_tree;

  mutable int m_event;
  mutable int m_layer;
  mutable int m_theta;
  mutable int m_nHits;
  mutable int m_nActiveHits;
  mutable double m_totalEnergy;
  mutable double m_activeEnergy;
  mutable double m_samplingFraction;
};

#endif /* DETSTUDIES_SAMPLINGFRACTIONINLAYERTHETATREE_H */
