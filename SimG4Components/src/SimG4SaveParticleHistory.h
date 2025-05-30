#ifndef SIMG4COMPONENTS_SIMG4SAVEPARTICLEHISTORY_H
#define SIMG4COMPONENTS_SIMG4SAVEPARTICLEHISTORY_H

// Gaudi
#include "GaudiKernel/AlgTool.h"

// FCCSW
#include "SimG4Common/EventInformation.h"
#include "SimG4Interface/ISimG4SaveOutputTool.h"
#include "k4FWCore/DataHandle.h"

class IGeoSvc;

// datamodel
namespace edm4hep {
class MCParticleCollection;
}

namespace sim {
class EventInformation;
}

/** @class SimG4SaveParticleHistory SimG4Components/src/SimG4SaveParticleHistory.h SimG4SaveParticleHistory.h
 *
 *  This tool allows to save the particle history of particles decaying during the simulation
 *
 *  @author J. Lingemann
 *  @author V. Volkl
 */

class SimG4SaveParticleHistory : public AlgTool, virtual public ISimG4SaveOutputTool {
public:
  explicit SimG4SaveParticleHistory(const std::string& aType, const std::string& aName, const IInterface* aParent);
  virtual ~SimG4SaveParticleHistory() = default;

  /**  Save the history
   *   Creates particles and gen vertices that allow association of parents and children
   *   @param[in] aEvent The Geant Event conatining data to save.
   *   @return status code
   */
  StatusCode saveOutput(const G4Event& aEvent) override final;

private:
  /// Handle for collection of MC particles to create
  mutable k4FWCore::DataHandle<edm4hep::MCParticleCollection> m_mcParticles{"SimParticleSecondaries",
                                                                            Gaudi::DataHandle::Writer, this};
  /// Pointer to the particle collection, ownership should be handled in a algorithm / tool
  edm4hep::MCParticleCollection* m_mcParticleColl;
};

#endif /* SIMG4COMPONENTS_SIMG4SAVEPARTICLEHISTORY_H */
