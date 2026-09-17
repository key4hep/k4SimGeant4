#include "MaterialScan.h"
#include "k4Interface/IGeoSvc.h"

#include "GaudiKernel/ITHistSvc.h"
#include "GaudiKernel/Service.h"

#include "DD4hep/Detector.h"
#include "DD4hep/Printout.h"
#include "DDRec/MaterialManager.h"
#include "DDRec/Vector3D.h"

#include "TFile.h"
#include "TTree.h"
#include "TVector3.h"

#include <array>
#include <cmath>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <vector>

MaterialScan::MaterialScan(const std::string& name, ISvcLocator* svcLoc)
    : Service(name, svcLoc), m_geoSvc("GeoSvc", name) {}

StatusCode MaterialScan::initialize() {
  if (Service::initialize().isFailure()) {
    return StatusCode::FAILURE;
  }

  if (!m_geoSvc) {
    error() << "Unable to find Geometry Service." << endmsg;
    return StatusCode::FAILURE;
  }

  if (m_etaBinning != -1) {
    warning() << "m_etaBinning is deprecated, use m_angleBinning instead." << endmsg;
    m_angleBinning = m_etaBinning;
  }

  if (m_etaMax != -1) {
    warning() << "m_etaMax is deprecated, use m_angleMax instead." << endmsg;
    m_angleMax = m_etaMax;
  }

  if (m_nPhiTrials != -1) {
    warning() << "m_nPhiTrials is deprecated, use m_nPhi instead." << endmsg;
    m_nPhi = m_nPhiTrials;
  }

  std::unique_ptr<TFile> rootFile(TFile::Open(m_filename.value().c_str(), "RECREATE"));
  // no smart pointers possible because TTree is owned by rootFile (root mem management FTW!)
  TTree* tree = new TTree("materials", "");
  double angle = 0;
  double phi = 0;
  unsigned nMaterials = 0;
  std::unique_ptr<std::vector<double>> nX0(new std::vector<double>);
  std::unique_ptr<std::vector<double>> nLambda(new std::vector<double>);
  std::unique_ptr<std::vector<double>> matDepth(new std::vector<double>);
  std::unique_ptr<std::vector<std::string>> material(new std::vector<std::string>);
  auto nX0Ptr = nX0.get();
  auto nLambdaPtr = nLambda.get();
  auto matDepthPtr = matDepth.get();
  auto materialPtr = material.get();

  tree->Branch("angle", &angle);
  tree->Branch("phi", &phi);
  tree->Branch("nMaterials", &nMaterials);
  tree->Branch("nX0", &nX0Ptr);
  tree->Branch("nLambda", &nLambdaPtr);
  tree->Branch("matDepth", &matDepthPtr);
  tree->Branch("material", &materialPtr);

  auto lcdd = m_geoSvc->getDetector();
  dd4hep::rec::MaterialManager matMgr(lcdd->detector(m_envelopeName).volume());
  dd4hep::rec::Vector3D beginning(0, 0, 0);
  auto boundaryVol = lcdd->detector(m_envelopeName).volume()->GetShape();
  std::array<Double_t, 3> pos = {0, 0, 0};
  std::array<Double_t, 3> dir = {0, 0, 0};
  TVector3 vec(0, 0, 0);

  for (angle = m_angleMin + 0.5 * m_angleBinning; angle < m_angleMax; angle += m_angleBinning) {
    std::cout << m_angleDef << ": " << angle << std::endl;

    for (phi = 0; phi < 2 * M_PI; phi += 2 * M_PI / m_nPhi) {
      nX0->clear();
      nLambda->clear();
      matDepth->clear();
      material->clear();

      if (m_angleDef == "eta")
        vec.SetPtEtaPhi(1, angle, phi);
      else if (m_angleDef == "theta")
        vec.SetPtThetaPhi(1, angle / 360.0 * 2 * M_PI, phi);
      else if (m_angleDef == "thetaRad")
        vec.SetPtThetaPhi(1, angle, phi);
      else if (m_angleDef == "cosTheta")
        vec.SetPtThetaPhi(1, acos(angle), phi);

      auto n = vec.Unit();
      dir = {n.X(), n.Y(), n.Z()};

      // if the start point (beginning) is inside the material-scan envelope (e.g. if envelope is world volume)
      double distance = boundaryVol->DistFromInside(pos.data(), dir.data());
      // if the start point (beginning) is not inside the envelope
      if (distance < std::numeric_limits<double>::epsilon()) {
        distance = boundaryVol->DistFromOutside(pos.data(), dir.data());
      }

      dd4hep::rec::Vector3D end(dir[0] * distance, dir[1] * distance, dir[2] * distance);
      debug() << "Calculating material between 0 and (" << end.x() << ", " << end.y() << ", " << end.z() << ") <=> "
              << m_angleDef << " = " << angle << ", phi =  " << phi << endmsg;
      const dd4hep::rec::MaterialVec& materials = matMgr.materialsBetween(beginning, end);
      std::map<dd4hep::Material, double> phiMaterialsBetween; // For phi scan
      for (const auto& [mat, depth] : materials) {
        phiMaterialsBetween[mat] += depth;
      }
      nMaterials = phiMaterialsBetween.size();
      for (const auto& matpair : phiMaterialsBetween) {
        TGeoMaterial* mat = matpair.first->GetMaterial();
        material->push_back(mat->GetName());
        matDepth->push_back(matpair.second);
        nX0->push_back(matpair.second / mat->GetRadLen());
        nLambda->push_back(matpair.second / mat->GetIntLen());
      }
      tree->Fill();
    }
  }
  tree->Write();
  rootFile->Close();

  return StatusCode::SUCCESS;
}

DECLARE_COMPONENT(MaterialScan)
