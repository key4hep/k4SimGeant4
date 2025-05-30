#ifndef SIMG4COMMON_EVENTINFORMATION_H
#define SIMG4COMMON_EVENTINFORMATION_H

#include "G4VUserEventInformation.hh"

#include <iostream>
#include <map>

class G4Track;
namespace edm4hep {
class MCParticleCollection;
}

/** @class sim::EventInformation SimG4Common/SimG4Common/EventInformation.h EventInformation.h
 *
 * Additional event information.
 *
 * Currently holds the particle history in form of edm particles and vertices
 *
 * @author J. Lingemann
 */

namespace sim {
class EventInformation : public G4VUserEventInformation {
public:
  /// Default constructor
  EventInformation();
  /// Destructor
  virtual ~EventInformation() = default;
  /** Set external pointers to point at the particle and vertex collections.
   * @param[in] aGenVertexCollection  pointer to a collection that should take ownership of the particles saved here
   * @param[in] aMCParticleCollection  pointer to a collection that should take ownership of the particles saved here
   */
  void setCollections(edm4hep::MCParticleCollection*& aMcParticleCollection);
  /// Add a particle to be tracked in the EDM collections
  void addParticle(const G4Track* aSecondary);

  void Print() const {};

private:
  /// Pointer to the particle collection, ownership is intended to be transfered to SaveTool
  edm4hep::MCParticleCollection* m_mcParticles;
  /// Map to get the edm end vertex id from a Geant4 unique particle ID
  std::map<size_t, size_t> m_g4IdToEndVertexMap;
};
} // namespace sim
#endif /* define SIMG4COMMON_EVENTINFORMATION_H */
