from Gaudi.Configuration import *
from Configurables import EventDataSvc
from k4FWCore import ApplicationMgr, IOSvc

# IOSvc for output
iosvc = IOSvc("IOSvc")
iosvc.Output = "positions_trackerSim.root"
iosvc.outputCommands = ["keep *"]

# DD4hep geometry service
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[ 'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                          'file:Detector/DetFCChhTrackerTkLayout/compact/Tracker.xml'
],
                    OutputLevel = INFO)

from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4FtfpBert", actions="SimG4FullSimActions")

from Configurables import SimG4Alg, SimG4SaveTrackerHits
savetool = SimG4SaveTrackerHits("saveHits", readoutNames = ["TrackerBarrelReadout"])
savetool.positionedTrackHits.Path = "PositionedHits"
savetool.trackHits.Path = "Hits"
savetool.digiTrackHits.Path = "digiHits"
from Configurables import SimG4SingleParticleGeneratorTool
pgun=SimG4SingleParticleGeneratorTool("SimG4SingleParticleGeneratorTool",saveEdm=True,
                                      particleName = "mu-", energyMin = 1000, energyMax = 1000, etaMin = 0, etaMax = 0,
                                      OutputLevel = DEBUG)
geantsim = SimG4Alg("SimG4Alg",
                    outputs= ["SimG4SaveTrackerHits/saveHits"],
                    eventProvider = pgun,
                    OutputLevel=DEBUG)

from Configurables import CreateVolumeTrackPositions
positions = CreateVolumeTrackPositions("positions", OutputLevel = VERBOSE)
positions.hits.Path = "Hits"
positions.positionedHits.Path = "Positions"

#CPU information
from Configurables import AuditorSvc, ChronoAuditor
chra = ChronoAuditor()
audsvc = AuditorSvc()
audsvc.Auditors = [chra]
geantsim.AuditExecute = True
positions.AuditExecute = True

# ApplicationMgr
ApplicationMgr( TopAlg = [geantsim, positions],
                EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [EventDataSvc("EventDataSvc"), geoservice, geantservice, audsvc],
                OutputLevel=DEBUG
)
