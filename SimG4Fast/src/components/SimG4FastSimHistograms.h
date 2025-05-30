#ifndef SIMG4FAST_G4FASTSIMHISTOGRAMS_H
#define SIMG4FAST_G4FASTSIMHISTOGRAMS_H

// GAUDI
#include "Gaudi/Algorithm.h"

// FCCSW
#include "k4FWCore/DataHandle.h"
class ITHistSvc;

#include "edm4hep/RecoMCParticleLinkCollection.h"

// datamodel
namespace edm4hep {
class ReconstructedParticleCollection;
}

class TH1F;

/** @class SimG4FastSimHistograms SimG4Components/src/SimG4FastSimHistograms.h SimG4FastSimHistograms.h
 *
 *  Fast simulation histograms algorithm.
 *  Fills validation histograms.
 *  It takes ParticleCollection (\b'smearedParticles') as the input.
 *
 *  @author Anna Zaborowska
 */

class SimG4FastSimHistograms : public Gaudi::Algorithm {
public:
  explicit SimG4FastSimHistograms(const std::string&, ISvcLocator*);
  virtual ~SimG4FastSimHistograms();
  /**  Initialize.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Fills the histograms.
   *   @return status code
   */
  virtual StatusCode execute(const EventContext&) const final;
  /**  Finalize.
   *   @return status code
   */
  virtual StatusCode finalize() final;

private:
  /// Handle for the EDM particles and MC particles associations to be read
  mutable k4FWCore::DataHandle<edm4hep::RecoMCParticleLinkCollection> m_particlesMCparticles{
      "ParticlesMCparticles", Gaudi::DataHandle::Reader, this};
  /// Pointer to the interface of histogram service
  SmartIF<ITHistSvc> m_histSvc;
  // Histogram of the smeared particle's momentum
  TH1F* m_p{nullptr};
  // Histogram of the smeared particle's pseudorapidity
  TH1F* m_eta{nullptr};
  // Histogram of the difference between MC particle's and smeared particle's momentum
  TH1F* m_diffP{nullptr};
  // Histogram of the smeared particle's PDG code
  TH1F* m_pdg{nullptr};
};
#endif /* SIMG4FAST_G4FASTSIMHISTOGRAMS_H */
