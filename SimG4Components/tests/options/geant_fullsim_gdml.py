

### \file
### \ingroup SimulationTests
### | **input (alg)**                 | other algorithms                   |                                                           |                          |                                    | **output (alg)**                                |
### | ------------------------------- | ---------------------------------- | --------------------------------------------------------- | ------------------------ | ---------------------------------- | ----------------------------------------------- |
### | read events from a HepMC file   | convert `HepMC::GenEvent` to EDM   | geometry taken from GDML (no sensitive detectors!) | FTFP_BERT physics list   | no action initialization list      | write the EDM output to ROOT file using PODIO   |


from Gaudi.Configuration import DEBUG

from Configurables import EventDataSvc
from k4FWCore import ApplicationMgr, IOSvc

# Get lists of algorithms and services that provide input to the simulation
from common_config import mc_particle_algs, mc_particle_svcs

iosvc = IOSvc("IOSvc")
iosvc.outputCommands = ["keep *"]

from Configurables import SimG4Svc, SimG4GdmlDetector
## Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
# first create tool that builds geometry from GDML
det = SimG4GdmlDetector("SimG4GdmlDetector", gdml = "Sim/SimG4Common/gdml/example.xml")
geantservice = SimG4Svc("SimG4Svc", detector=det, physicslist="SimG4FtfpBert", actions="SimG4FullSimActions")

from Configurables import SimG4Alg, SimG4PrimariesFromEdmTool
## Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4
particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.genParticles.Path = "allGenParticles"
geantsim = SimG4Alg("SimG4Alg",
                    eventProvider=particle_converter)


ApplicationMgr(
    TopAlg=mc_particle_algs + [geantsim],
    EvtSel='NONE',
    EvtMax=1,
    ## order is important, as GeoSvc is needed by SimG4Svc
    ExtSvc=mc_particle_svcs + [EventDataSvc("EventDataSvc"), geantservice],
    OutputLevel=DEBUG,
)
