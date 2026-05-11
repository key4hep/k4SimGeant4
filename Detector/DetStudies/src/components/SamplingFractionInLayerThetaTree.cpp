#include "SamplingFractionInLayerThetaTree.h"

// FCCSW
#include "k4Interface/IGeoSvc.h"

// datamodel
#include "edm4hep/SimCalorimeterHitCollection.h"

#include "GaudiKernel/ITHistSvc.h"
#include "TTree.h"

// DD4hep
#include "DD4hep/Detector.h"
#include "DD4hep/Readout.h"

#include <vector>

DECLARE_COMPONENT(SamplingFractionInLayerThetaTree)

SamplingFractionInLayerThetaTree::SamplingFractionInLayerThetaTree(const std::string& aName, ISvcLocator* aSvcLoc)
    : Gaudi::Algorithm(aName, aSvcLoc), m_histSvc("THistSvc", aName), m_geoSvc("GeoSvc", aName), m_tree(nullptr),
      m_event(0), m_layer(0), m_theta(0), m_nHits(0), m_nActiveHits(0), m_totalEnergy(0.), m_activeEnergy(0.),
      m_samplingFraction(0.) {
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

  m_tree = new TTree("ecal_sf_layer_theta_tree", "Event-level layer-theta sampling fraction");
  m_tree->Branch("event", &m_event, "event/I");
  m_tree->Branch("layer", &m_layer, "layer/I");
  m_tree->Branch("theta", &m_theta, "theta/I");
  m_tree->Branch("total_energy", &m_totalEnergy, "total_energy/D");
  m_tree->Branch("active_energy", &m_activeEnergy, "active_energy/D");
  m_tree->Branch("sf", &m_samplingFraction, "sf/D");
  m_tree->Branch("n_hits", &m_nHits, "n_hits/I");
  m_tree->Branch("n_active_hits", &m_nActiveHits, "n_active_hits/I");

  if (m_histSvc->regTree(m_treePath, m_tree).isFailure()) {
    error() << "Couldn't register TTree at " << m_treePath << endmsg;
    return StatusCode::FAILURE;
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
      m_tree->Fill();
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode SamplingFractionInLayerThetaTree::finalize() { return Gaudi::Algorithm::finalize(); }
