from Gaudi.Configuration import INFO, DEBUG
from GaudiKernel import SystemOfUnits as units
from GaudiKernel import PhysicalConstants as constants

from Configurables import EventDataSvc
from k4FWCore import ApplicationMgr, IOSvc

iosvc = IOSvc("IOSvc")
iosvc.Output = "output_xAngleBoost.root"
iosvc.outputCommands = ["keep *"]


from Configurables import MomentumRangeParticleGun
guntool = MomentumRangeParticleGun()
guntool.ThetaMin = 45 * constants.pi / 180.
guntool.ThetaMax = 135 * constants.pi / 180.
guntool.PhiMin = 0.
guntool.PhiMax = 2. * constants.pi
guntool.MomentumMin = 10. * units.GeV
guntool.MomentumMax = 10. * units.GeV
guntool.PdgCodes = [11]

from Configurables import GenAlg
gun = GenAlg()
gun.SignalProvider = guntool
gun.hepmc.Path = "hepmc"


from Configurables import HepMCToEDMConverter
hepmc_converter = HepMCToEDMConverter()
hepmc_converter.hepmc.Path = "hepmc"
hepmc_converter.GenParticles.Path = "GenParticles"


from Configurables import SimG4CrossingAngleBoost
xAngleBoost = SimG4CrossingAngleBoost('xAngleBoost')
xAngleBoost.InParticles = 'GenParticles'
xAngleBoost.OutParticles = 'BoostedParticles'
xAngleBoost.CrossingAngle = 0.015  # rad
xAngleBoost.OutputLevel = DEBUG


ApplicationMgr(
    TopAlg=[gun, hepmc_converter, xAngleBoost],
    EvtSel='NONE',
    EvtMax=2,
    ExtSvc=[EventDataSvc("EventDataSvc")],
    OutputLevel=INFO,
)
