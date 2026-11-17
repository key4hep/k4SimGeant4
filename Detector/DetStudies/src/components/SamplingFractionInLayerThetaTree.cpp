#include "SamplingFractionInLayerThetaTree.h"

// FCCSW
#include "k4Interface/IGeoSvc.h"

// datamodel
#include "edm4hep/SimCalorimeterHitCollection.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TH2D.h"
#include "TTree.h"

// DD4hep
#include "DD4hep/Detector.h"
#include "DD4hep/Readout.h"

#include <vector>

DECLARE_COMPONENT(SamplingFractionInLayerThetaTree)

SamplingFractionInLayerThetaTree::SamplingFractionInLayerThetaTree(const std::string& aName, ISvcLocator* aSvcLoc)
    : Gaudi::Algorithm(aName, aSvcLoc), m_histSvc("THistSvc", aName), m_geoSvc("GeoSvc", aName), m_eventTree(nullptr),
      m_summaryTree(nullptr), m_sfDistributionTree(nullptr), m_totalEnergyHist(nullptr), m_activeEnergyHist(nullptr),
      m_eventCountHist(nullptr), m_samplingFractionHist(nullptr), m_sfDistribution(nullptr), m_event(0), m_layer(0),
      m_theta(0), m_nHits(0), m_nActiveHits(0), m_summaryNHits(0), m_summaryNActiveHits(0), m_nEventBins(0),
      m_totalEnergy(0.), m_activeEnergy(0.), m_samplingFraction(0.) {
  declareProperty("deposits", m_deposits, "Energy deposits in sampling calorimeter (input)");
}

SamplingFractionInLayerThetaTree::~SamplingFractionInLayerThetaTree() {}

StatusCode SamplingFractionInLayerThetaTree::initialize() {
  if (Gaudi::Algorithm::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }

  if (!m_geoSvc) {
    error() << "Unable to locate Geometry Service!" << endmsg;
    return StatusCode::FAILURE;
  }

  if (m_geoSvc->getDetector()->readouts().find(m_readoutName) == m_geoSvc->getDetector()->readouts().end()) {
    error() << "Readout <<" << m_readoutName << ">> does not exist." << endmsg;
    return StatusCode::FAILURE;
  }

  const std::size_t nBins = static_cast<std::size_t>(m_numLayers) * static_cast<std::size_t>(m_numThetaBins);
  m_sumEnergy.assign(nBins, 0.);
  m_sumActiveEnergy.assign(nBins, 0.);
  m_sumHits.assign(nBins, 0);
  m_sumActiveHits.assign(nBins, 0);
  m_sumEventBins.assign(nBins, 0);

  if (m_writeEventTree) {
    m_eventTree = new TTree("ecal_sf_layer_theta_tree", "Event-level layer-theta sampling fraction");
    m_eventTree->Branch("event", &m_event, "event/I");
    m_eventTree->Branch("layer", &m_layer, "layer/I");
    m_eventTree->Branch("theta", &m_theta, "theta/I");
    m_eventTree->Branch("total_energy", &m_totalEnergy, "total_energy/D");
    m_eventTree->Branch("active_energy", &m_activeEnergy, "active_energy/D");
    m_eventTree->Branch("sf", &m_samplingFraction, "sf/D");
    m_eventTree->Branch("n_hits", &m_nHits, "n_hits/I");
    m_eventTree->Branch("n_active_hits", &m_nActiveHits, "n_active_hits/I");

    if (m_histSvc->regTree(m_treePath, m_eventTree).isFailure()) {
      error() << "Couldn't register TTree at " << m_treePath << endmsg;
      return StatusCode::FAILURE;
    }
  }

  if (m_writeSummaryTree) {
    m_summaryTree = new TTree("ecal_sf_layer_theta_summary", "Final layer-theta sampling fraction summary");
    m_summaryTree->Branch("layer", &m_layer, "layer/I");
    m_summaryTree->Branch("theta", &m_theta, "theta/I");
    m_summaryTree->Branch("total_energy", &m_totalEnergy, "total_energy/D");
    m_summaryTree->Branch("active_energy", &m_activeEnergy, "active_energy/D");
    m_summaryTree->Branch("sf", &m_samplingFraction, "sf/D");
    m_summaryTree->Branch("n_hits", &m_summaryNHits, "n_hits/L");
    m_summaryTree->Branch("n_active_hits", &m_summaryNActiveHits, "n_active_hits/L");
    m_summaryTree->Branch("n_event_bins", &m_nEventBins, "n_event_bins/L");

    if (m_histSvc->regTree(m_summaryTreePath, m_summaryTree).isFailure()) {
      error() << "Couldn't register TTree at " << m_summaryTreePath << endmsg;
      return StatusCode::FAILURE;
    }
  }

  if (m_writeMergeableHistograms) {
    const double layerMin = static_cast<double>(m_firstLayerId) - 0.5;
    const double layerMax = static_cast<double>(m_firstLayerId + static_cast<int>(m_numLayers)) - 0.5;
    const double thetaMin = static_cast<double>(m_firstThetaId) - 0.5;
    const double thetaMax = static_cast<double>(m_firstThetaId + static_cast<int>(m_numThetaBins)) - 0.5;

    m_totalEnergyHist = new TH2D("ecal_sf_layer_theta_total_energy", "Total energy by layer and theta;layer;theta",
                                 m_numLayers, layerMin, layerMax, m_numThetaBins, thetaMin, thetaMax);
    m_activeEnergyHist = new TH2D("ecal_sf_layer_theta_active_energy", "Active energy by layer and theta;layer;theta",
                                  m_numLayers, layerMin, layerMax, m_numThetaBins, thetaMin, thetaMax);
    m_eventCountHist = new TH2D("ecal_sf_layer_theta_event_count", "Non-empty event bins by layer and theta;layer;theta",
                                m_numLayers, layerMin, layerMax, m_numThetaBins, thetaMin, thetaMax);
    m_samplingFractionHist = new TH2D("ecal_sf_layer_theta", "Final sampling fraction by layer and theta;layer;theta",
                                      m_numLayers, layerMin, layerMax, m_numThetaBins, thetaMin, thetaMax);

    if (m_histSvc->regHist(m_totalEnergyHistPath, m_totalEnergyHist).isFailure() ||
        m_histSvc->regHist(m_activeEnergyHistPath, m_activeEnergyHist).isFailure() ||
        m_histSvc->regHist(m_eventCountHistPath, m_eventCountHist).isFailure() ||
        m_histSvc->regHist(m_samplingFractionHistPath, m_samplingFractionHist).isFailure()) {
      error() << "Couldn't register layer-theta TH2D outputs" << endmsg;
      return StatusCode::FAILURE;
    }
  }

  if (m_writeSfDistribution) {
    if (m_sfDistributionMax <= m_sfDistributionMin) {
      error() << "sfDistributionMax must be larger than sfDistributionMin." << endmsg;
      return StatusCode::FAILURE;
    }

    const int bins[3] = {static_cast<int>(m_numLayers), static_cast<int>(m_numThetaBins),
                         static_cast<int>(m_sfDistributionBins)};
    const double min[3] = {static_cast<double>(m_firstLayerId) - 0.5, static_cast<double>(m_firstThetaId) - 0.5,
                           m_sfDistributionMin};
    const double max[3] = {static_cast<double>(m_firstLayerId + static_cast<int>(m_numLayers)) - 0.5,
                           static_cast<double>(m_firstThetaId + static_cast<int>(m_numThetaBins)) - 0.5,
                           m_sfDistributionMax};
    m_sfDistribution = new THnSparseD("ecal_sf_layer_theta_sf_distribution",
                                      "Event-level SF distribution by layer and theta;layer;theta;sf", 3, bins, min,
                                      max);
    m_sfDistributionTree = new TTree("ecal_sf_layer_theta_sf_distribution",
                                     "THnSparseD event-level SF distribution by layer and theta");
    m_sfDistributionTree->Branch("sf_distribution", &m_sfDistribution);
    if (m_histSvc->regTree(m_sfDistributionPath, m_sfDistributionTree).isFailure()) {
      error() << "Couldn't register THnSparseD holder TTree at " << m_sfDistributionPath << endmsg;
      return StatusCode::FAILURE;
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode SamplingFractionInLayerThetaTree::execute(const EventContext& ctx) const {
  auto decoder = m_geoSvc->getDetector()->readout(m_readoutName).idSpec().decoder();

  std::vector<std::vector<double>> sumE(m_numLayers, std::vector<double>(m_numThetaBins, 0.));
  std::vector<std::vector<double>> sumEactive(m_numLayers, std::vector<double>(m_numThetaBins, 0.));
  std::vector<std::vector<int>> nHits(m_numLayers, std::vector<int>(m_numThetaBins, 0));
  std::vector<std::vector<int>> nActiveHits(m_numLayers, std::vector<int>(m_numThetaBins, 0));

  const auto deposits = m_deposits.get();
  for (const auto& hit : *deposits) {
    dd4hep::DDSegmentation::CellID cID = hit.getCellID();
    const int layerId = decoder->get(cID, m_layerFieldName);
    const int thetaId = decoder->get(cID, m_thetaFieldName);
    const int layerIndex = layerId - m_firstLayerId;
    const int thetaIndex = thetaId - m_firstThetaId;

    if (layerIndex < 0 || layerIndex >= static_cast<int>(m_numLayers) || thetaIndex < 0 ||
        thetaIndex >= static_cast<int>(m_numThetaBins)) {
      continue;
    }

    const double energy = hit.getEnergy();
    sumE[layerIndex][thetaIndex] += energy;
    ++nHits[layerIndex][thetaIndex];

    const int activeField = decoder->get(cID, m_activeFieldName);
    if (activeField == m_activeFieldValue) {
      sumEactive[layerIndex][thetaIndex] += energy;
      ++nActiveHits[layerIndex][thetaIndex];
    }
  }

  m_event = static_cast<int>(ctx.evt());
  for (unsigned int layerIndex = 0; layerIndex < m_numLayers; ++layerIndex) {
    for (unsigned int thetaIndex = 0; thetaIndex < m_numThetaBins; ++thetaIndex) {
      if (sumE[layerIndex][thetaIndex] <= 0.) {
        continue;
      }

      m_layer = static_cast<int>(layerIndex) + m_firstLayerId;
      m_theta = static_cast<int>(thetaIndex) + m_firstThetaId;
      m_totalEnergy = sumE[layerIndex][thetaIndex];
      m_activeEnergy = sumEactive[layerIndex][thetaIndex];
      m_samplingFraction = m_activeEnergy / m_totalEnergy;
      m_nHits = nHits[layerIndex][thetaIndex];
      m_nActiveHits = nActiveHits[layerIndex][thetaIndex];

      const std::size_t index = flatIndex(layerIndex, thetaIndex);
      m_sumEnergy[index] += m_totalEnergy;
      m_sumActiveEnergy[index] += m_activeEnergy;
      m_sumHits[index] += m_nHits;
      m_sumActiveHits[index] += m_nActiveHits;
      ++m_sumEventBins[index];

      if (m_writeMergeableHistograms) {
        m_totalEnergyHist->Fill(m_layer, m_theta, m_totalEnergy);
        m_activeEnergyHist->Fill(m_layer, m_theta, m_activeEnergy);
        m_eventCountHist->Fill(m_layer, m_theta);
      }

      if (m_writeSfDistribution) {
        const double coordinates[3] = {static_cast<double>(m_layer), static_cast<double>(m_theta), m_samplingFraction};
        m_sfDistribution->Fill(coordinates);
      }

      if (m_writeEventTree) {
        m_eventTree->Fill();
      }
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode SamplingFractionInLayerThetaTree::finalize() {
  for (unsigned int layerIndex = 0; layerIndex < m_numLayers; ++layerIndex) {
    for (unsigned int thetaIndex = 0; thetaIndex < m_numThetaBins; ++thetaIndex) {
      const std::size_t index = flatIndex(layerIndex, thetaIndex);
      if (m_sumEnergy[index] <= 0.) {
        continue;
      }

      m_layer = static_cast<int>(layerIndex) + m_firstLayerId;
      m_theta = static_cast<int>(thetaIndex) + m_firstThetaId;
      m_totalEnergy = m_sumEnergy[index];
      m_activeEnergy = m_sumActiveEnergy[index];
      m_samplingFraction = m_activeEnergy / m_totalEnergy;
      m_summaryNHits = m_sumHits[index];
      m_summaryNActiveHits = m_sumActiveHits[index];
      m_nEventBins = m_sumEventBins[index];

      if (m_writeSummaryTree) {
        m_summaryTree->Fill();
      }
      if (m_writeMergeableHistograms) {
        m_samplingFractionHist->SetBinContent(layerIndex + 1, thetaIndex + 1, m_samplingFraction);
      }
    }
  }

  if (m_writeSfDistribution) {
    m_sfDistributionTree->Fill();
  }

  return Gaudi::Algorithm::finalize();
}

std::size_t SamplingFractionInLayerThetaTree::flatIndex(unsigned int layerIndex, unsigned int thetaIndex) const {
  return static_cast<std::size_t>(layerIndex) * static_cast<std::size_t>(m_numThetaBins) +
         static_cast<std::size_t>(thetaIndex);
}
