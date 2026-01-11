from Gaudi.Configuration import *
from Configurables import EventDataSvc
from k4FWCore import ApplicationMgr, IOSvc

# IOSvc for output
iosvc = IOSvc("IOSvc")
iosvc.Output = "positions_ecalInclinedSim.root"
iosvc.outputCommands = ["keep *"]

from Configurables import  HepMCFileReader, GenAlg
readertool = HepMCFileReader("Reader", Filename="Test/TestGeometry/data/testHepMCpositionsEMcal.dat")
reader = GenAlg("Reader", SignalProvider=readertool)
reader.hepmc.Path = "hepmc"

from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter("Converter")
hepmc_converter.hepmc.Path="hepmc"
hepmc_converter.genparticles.Path="allGenParticles"
hepmc_converter.genvertices.Path="allGenVertices"

from Configurables import HepMCDumper
hepmc_dump = HepMCDumper("hepmc")
hepmc_dump.hepmc.Path="hepmc"

# DD4hep geometry service
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[ 'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                          'file:Detector/DetFCChhECalInclined/compact/FCChh_ECalBarrel_withCryostat.xml'
],
                    OutputLevel = INFO)

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc", detector='SimG4DD4hepDetector', physicslist="SimG4TestPhysicsList", actions="SimG4FullSimActions")

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
# and a tool that saves the calorimeter hits
from Configurables import SimG4Alg, SimG4SaveCalHits
savetool = SimG4SaveCalHits("saveHits",readoutNames = ["ECalBarrelEta"])
savetool.positionedCaloHits.Path = "PositionedHits"
savetool.caloHits.Path = "Hits"

geantsim = SimG4Alg("SimG4Alg",
                       outputs= ["SimG4SaveCalHits/saveHits"],
                       OutputLevel=DEBUG)

from Configurables import CreateVolumeCaloPositions
positions = CreateVolumeCaloPositions("positions", OutputLevel = VERBOSE)
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
ApplicationMgr( TopAlg = [reader, hepmc_converter, hepmc_dump, geantsim, positions],
                EvtSel = 'NONE',
                EvtMax   = 5,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [EventDataSvc("EventDataSvc"), geoservice, geantservice, audsvc],
                OutputLevel=DEBUG
)
