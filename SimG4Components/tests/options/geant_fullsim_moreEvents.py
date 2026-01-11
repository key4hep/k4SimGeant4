


### \file
### \ingroup SimulationTests
### | **input (alg)**                 | other algorithms                   |                                     |                          |                                    | **output (alg)**                                |
### | ------------------------------- | ---------------------------------- | ----------------------------------- | ------------------------ | ---------------------------------- | ----------------------------------------------- |
### | read events from a HepMC file   | convert `HepMC::GenEvent` to EDM   | geometry taken from TestTracker.xml | FTFP_BERT physics list   | save Tracker and HCAL hits         | write the EDM output to ROOT file using PODIO   |


from Gaudi.Configuration import DEBUG

from Configurables import EventDataSvc
from k4FWCore import ApplicationMgr, IOSvc

iosvc = IOSvc("IOSvc")
iosvc.Output = "test_geant_fullsim_moreEvents.root"
iosvc.outputCommands = ["keep *"]

from Configurables import MomentumRangeParticleGun
from GaudiKernel import PhysicalConstants as constants
guntool = MomentumRangeParticleGun()
guntool.ThetaMin = 0
guntool.ThetaMax = 2 * constants.pi
guntool.PdgCodes = [11]
from Configurables import GenAlg
gen = GenAlg()
gen.SignalProvider=guntool
gen.hepmc.Path = "hepmc"


from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"

from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=['file:../../../Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
  'file:../../../Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml'],
                    OutputLevel = DEBUG)

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4FtfpBert", actions="SimG4FullSimActions", )

from Configurables import SimG4Alg, SimG4SaveTrackerHits, SimG4PrimariesFromEdmTool
savetrackertool = SimG4SaveTrackerHits("SimG4SaveTrackerHits", readoutNames=["TrackerBarrelReadout", "TrackerEndcapReadout"])
savetrackertool.positionedTrackHits.Path = "positionedHits"
savetrackertool.trackHits.Path = "hits"

particle_converter = SimG4PrimariesFromEdmTool("EdmConverter")
particle_converter.genParticles.Path = "allGenParticles"
geantsim = SimG4Alg("SimG4Alg",
                    outputs = ["SimG4SaveTrackerHits/SimG4SaveTrackerHits"],
                    eventProvider=particle_converter)


ApplicationMgr(
    TopAlg=[gen, hepmc_converter, geantsim],
    EvtSel='NONE',
    EvtMax=10,
    ## order! geo needed by geant
    ExtSvc=[EventDataSvc("EventDataSvc"), geoservice, geantservice],
    OutputLevel=DEBUG,
)
