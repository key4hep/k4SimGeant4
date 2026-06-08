#ifndef DETSTUDIES_SAMPLINGFRACTIONINLAYERTHETATREE_H
#define DETSTUDIES_SAMPLINGFRACTIONINLAYERTHETATREE_H

// GAUDI
#include "Gaudi/Algorithm.h"
#include "GaudiKernel/ServiceHandle.h"

// FCCSW
#include "k4FWCore/DataHandle.h"

#include "THnSparse.h"

#include <cstddef>
#include <vector>

class IGeoSvc;
class ITHistSvc;
class TH2D;
class TTree;

// datamodel
namespace edm4hep {
class SimCalorimeterHitCollection;
}

/** @class SamplingFractionInLayerThetaTree SamplingFractionInLayerThetaTree.h
 *
 *  Accumulates layer-theta sampling fraction constants and writes final summary outputs.
 *  It can also keep the legacy event-level TTree for workflows that still need per-event entries.
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
  Gaudi::Property<bool> m_writeEventTree{this, "writeEventTree", true,
                                         "Write the legacy event-level layer-theta TTree"};
  Gaudi::Property<bool> m_writeSummaryTree{this, "writeSummaryTree", true,
                                           "Write one final row per non-empty layer-theta bin"};
  Gaudi::Property<bool> m_writeMergeableHistograms{this, "writeMergeableHistograms", true,
                                                   "Write additive TH2D energy/count histograms for hadd merging"};
  Gaudi::Property<bool> m_writeSfDistribution{this, "writeSfDistribution", false,
                                              "Write THnSparseD(layer, theta, event-level SF) for per-bin fits"};
  Gaudi::Property<unsigned int> m_sfDistributionBins{this, "sfDistributionBins", 200,
                                                     "Number of bins in the THnSparseD SF axis"};
  Gaudi::Property<double> m_sfDistributionMin{this, "sfDistributionMin", 0.,
                                              "Minimum value of the THnSparseD SF axis"};
  Gaudi::Property<double> m_sfDistributionMax{this, "sfDistributionMax", 1.0,
                                              "Maximum value of the THnSparseD SF axis"};
  Gaudi::Property<std::string> m_treePath{this, "treePath", "/rec/ecal_sf_layer_theta_tree",
                                          "THistSvc path for the legacy event-level TTree"};
  Gaudi::Property<std::string> m_summaryTreePath{this, "summaryTreePath", "/rec/ecal_sf_layer_theta_summary",
                                                 "THistSvc path for the final summary TTree"};
  Gaudi::Property<std::string> m_totalEnergyHistPath{this, "totalEnergyHistPath",
                                                     "/rec/ecal_sf_layer_theta_total_energy",
                                                     "THistSvc path for the additive total-energy TH2D"};
  Gaudi::Property<std::string> m_activeEnergyHistPath{this, "activeEnergyHistPath",
                                                      "/rec/ecal_sf_layer_theta_active_energy",
                                                      "THistSvc path for the additive active-energy TH2D"};
  Gaudi::Property<std::string> m_eventCountHistPath{this, "eventCountHistPath",
                                                    "/rec/ecal_sf_layer_theta_event_count",
                                                    "THistSvc path for the additive non-empty event-bin count TH2D"};
  Gaudi::Property<std::string> m_samplingFractionHistPath{this, "samplingFractionHistPath",
                                                          "/rec/ecal_sf_layer_theta",
                                                          "THistSvc path for the final derived sampling-fraction TH2D"};
  Gaudi::Property<std::string> m_sfDistributionPath{this, "sfDistributionPath",
                                                   "/rec/ecal_sf_layer_theta_sf_distribution",
                                                   "THistSvc path for the TTree holding the THnSparseD SF distribution"};

  TTree* m_eventTree;
  TTree* m_summaryTree;
  TTree* m_sfDistributionTree;
  TH2D* m_totalEnergyHist;
  TH2D* m_activeEnergyHist;
  TH2D* m_eventCountHist;
  TH2D* m_samplingFractionHist;
  THnSparseD* m_sfDistribution;

  mutable int m_event;
  mutable int m_layer;
  mutable int m_theta;
  mutable int m_nHits;
  mutable int m_nActiveHits;
  mutable long long m_summaryNHits;
  mutable long long m_summaryNActiveHits;
  mutable long long m_nEventBins;
  mutable double m_totalEnergy;
  mutable double m_activeEnergy;
  mutable double m_samplingFraction;

  mutable std::vector<double> m_sumEnergy;
  mutable std::vector<double> m_sumActiveEnergy;
  mutable std::vector<long long> m_sumHits;
  mutable std::vector<long long> m_sumActiveHits;
  mutable std::vector<long long> m_sumEventBins;

  std::size_t flatIndex(unsigned int layerIndex, unsigned int thetaIndex) const;
};

#endif /* DETSTUDIES_SAMPLINGFRACTIONINLAYERTHETATREE_H */
