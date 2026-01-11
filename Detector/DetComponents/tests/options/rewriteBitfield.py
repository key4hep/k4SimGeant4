from Gaudi.Configuration import *
from Configurables import EventDataSvc
from k4FWCore import ApplicationMgr, IOSvc

# IOSvc for output
iosvc = IOSvc("IOSvc")
iosvc.Output = "rewrittenBitfield_ecalEndcapSim.root"
iosvc.outputCommands = ["keep *"]

# DD4hep geometry service
from Configurables import GeoSvc
geoservice = GeoSvc("GeoSvc", detectors=[ 'file:Detector/DetFCChhBaseline1/compact/FCChh_DectEmptyMaster.xml',
                                          'file:Detector/DetFCChhCalDiscs/compact/Endcaps_coneCryo.xml'],
                    OutputLevel = INFO)

# Geant4 service
# Configures the Geant simulation: geometry, physics list and user actions
from Configurables import SimG4Svc
geantservice = SimG4Svc("SimG4Svc")

# Geant4 algorithm
# Translates EDM to G4Event, passes the event to G4, writes out outputs via tools
# and a tool that saves the calorimeter hits
from Configurables import SimG4Alg, SimG4SaveCalHits
savecaltool = SimG4SaveCalHits("saveECalHits", readoutNames = ["EMECPhiEta"])
savecaltool.positionedCaloHits.Path = "positionedCaloHits"
savecaltool.caloHits.Path = "caloHits"
from Configurables import SimG4SingleParticleGeneratorTool
pgun=SimG4SingleParticleGeneratorTool("SimG4SingleParticleGeneratorTool",saveEdm=True,
                particleName="e-",energyMin=50000,energyMax=50000,etaMin=2,etaMax=2)
geantsim = SimG4Alg("SimG4Alg", outputs= ["SimG4SaveCalHits/saveECalHits"], eventProvider=pgun)

from Configurables import RewriteBitfield
rewrite = RewriteBitfield("Rewrite",
                          # old bitfield (readout)
                          oldReadoutName = "EMECPhiEta",
                          # specify which fields are going to be deleted
                          removeIds = ["sublayer"],
                          # new bitfield (readout), with new segmentation
                          newReadoutName = "EMECPhiEtaReco",
                          debugPrint = 10,
                          OutputLevel = DEBUG)
# clusters are needed, with deposit position and cellID in bits
rewrite.inhits.Path = "caloHits"
rewrite.outhits.Path = "caloRecoHits"

# ApplicationMgr
ApplicationMgr( TopAlg = [geantsim, rewrite],
                EvtSel = 'NONE',
                EvtMax   = 1,
                # order is important, as GeoSvc is needed by G4SimSvc
                ExtSvc = [EventDataSvc("EventDataSvc"), geoservice, geantservice],
                OutputLevel=INFO)
