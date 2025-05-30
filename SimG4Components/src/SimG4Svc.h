#ifndef SIMG4COMPONENTS_G4SIMSVC_H
#define SIMG4COMPONENTS_G4SIMSVC_H

// FCCSW
#include "SimG4Common/RunManager.h"
#include "SimG4Interface/ISimG4ActionTool.h"
#include "SimG4Interface/ISimG4DetectorConstruction.h"
#include "SimG4Interface/ISimG4MagneticFieldTool.h"
#include "SimG4Interface/ISimG4PhysicsList.h"
#include "SimG4Interface/ISimG4RegionTool.h"
#include "SimG4Interface/ISimG4Svc.h"

// Gaudi
#include "GaudiKernel/IRndmGenSvc.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/ToolHandle.h"

#include "G4UIsession.hh"
#include "G4UIterminal.hh"
#include "G4VisExecutive.hh"
#include "G4VisManager.hh"

/** @class SimG4Svc SimG4Components/SimG4Components/SimG4Svc.h SimG4Svc.h
 *
 *  Main Geant simulation service.
 *  It handles Geant initialization (via tools) and communication with the G4RunManager.
 *  [For more information please see](@ref md_sim_doc_geant4fullsim).
 *
 *  @author Anna Zaborowska
 */

class SimG4Svc : public extends1<Service, ISimG4Svc> {
public:
  /// Standard constructor
  explicit SimG4Svc(const std::string& aName, ISvcLocator* aSL);
  /// Standard destructor
  virtual ~SimG4Svc();
  /**  Initialize the Geant simulation service.
   *   Tools set in the configuration file need to specify the detector construction,
   *   physics list and user action initialization to initialize G4RunManager.
   *   @return status code
   */
  virtual StatusCode initialize() final;
  /**  Finalize the Geant simulation service.
   *   @return status code
   */
  virtual StatusCode finalize() final;
  /**  Simulate the event with Geant.
   *   @param[in] aEvent An event to be processed.
   *   @return status code
   */
  StatusCode processEvent(G4Event& aEvent);
  /**  Retrieve the processed event.
   *   @param[out] aEvent The processed event.
   *   @return status code
   */
  StatusCode retrieveEvent(G4Event*& aEvent);
  /**  Terminate the event simulation.
   *   @return status code
   */
  StatusCode terminateEvent();

private:
  /// Pointer to the tool service
  SmartIF<IToolSvc> m_toolSvc;
  /// Pointer to the random numbers service
  SmartIF<IRndmGenSvc> m_randSvc;
  /// Handle for the detector construction tool
  ToolHandle<ISimG4DetectorConstruction> m_detectorTool{"SimG4DD4hepDetector", this, true};
  /// Handle for the Geant physics list tool
  ToolHandle<ISimG4PhysicsList> m_physicsListTool{"SimG4FtfpBert", this, true};
  /// Handle for the user action initialization
  ToolHandle<ISimG4ActionTool> m_actionsTool{"SimG4FullSimActions", this, true};
  /// Handle for the magnetic field initialization
  ToolHandle<ISimG4MagneticFieldTool> m_magneticFieldTool{"SimG4ConstantMagneticFieldTool", this, true};
  /// Geant4 commands to be executed before user initialization
  Gaudi::Property<std::vector<std::string>> m_g4PreInitCommands{
      this, "g4PreInitCommands", {}, "Geant4 commands to be executed before user initialization"};
  /// Geant4 commands to be executed after user initialization
  Gaudi::Property<std::vector<std::string>> m_g4PostInitCommands{
      this, "g4PostInitCommands", {}, "Geant4 commands to be executed after user initialization"};
  /// Handles to the tools creating regions and fast simulation models
  /// to be replaced with the ToolHandleArray<ISimG4RegionTool> m_regionTools
  std::vector<ISimG4RegionTool*> m_regionTools;
  /// Names of the tools that create regions and fast simulation models
  /// to be deleted once the ToolHandleArray<ISimG4RegionTool> m_regionTools is in place
  Gaudi::Property<std::vector<std::string>> m_regionToolNames{
      this, "regions", {}, "Names of the tools that create regions and fast simulation models"};
  /// Flag whether random numbers seeds should be taken from Gaudi (default: true)
  Gaudi::Property<bool> m_rndmFromGaudi{this, "randomNumbersFromGaudi", true,
                                        "Whether random numbers should be taken from Gaudi"};
  Gaudi::Property<long> m_seedValue{
      this, "seedValue", 1234567, "Seed to be used in RndmGenSvc engine (randomNumbersFromGaudi must be set to false)"};

  Gaudi::Property<bool> m_interactiveMode{this, "InteractiveMode", false, "Enter the interactive mode"};

  /// Run Manager
  sim::RunManager m_runManager;

  std::unique_ptr<G4VisManager> m_visManager{nullptr};
  // Define UI terminal for interactive mode
  std::unique_ptr<G4UIsession> m_session{nullptr};
};

#endif /* SIMG4COMPONENTS_G4SIMSVC_H */
