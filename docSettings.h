//This file contains some instructions to doxygen. Please do not delete.

/*! \mainpage VME trigger documentation 
 *
 * \section intro_sec Introduction
 *
 * This library was created in order to provide a flexible remote control system to a  <a href="https://dl.dropboxusercontent.com/u/33459975/cosmicTrigger/DocHardware.pdf">cosmic muon trigger system</a>  .
 * 
 * The library has the following structure:
 *
 * \image html Lib.png
 * 
 * \li VmeController is a virtual class. It has the definition of a few basic i/o functions. It also contains the verbosity level.
 * 
 * \li VmeUsbBridge is the instantiation of VmeController for a CAEN VME Controller.
 * 
 * \li VmeBoard is a virtual class containing the definition of a fiew read/write functions. It also contains the module's address and default AM/DW modes.
 * 
 * \li VmeBoard daughter classes: classes implementing the functions that can be used by the user. Each class is different and has different functionalities depending on the card.
 * 
 * \li CommonDef is a file containing functions and enum types that can be used by anyone.
 * 
 * To see a complete list of implemented boards, please go to the VmeBoard documention page.
 * 
 * \section dwl Download
 * 
 * The best way to download it is from <a href="https://github.com/md987/CosmicTrigger">github</a> where it will be updated.
 * 
 * You can also download it from <a href="https://dl.dropboxusercontent.com/u/33459975/cosmicTrigger/completeArchive.tar.gz">dropbox</a> or from a <a href="http://www.cosmictrigger.comeze.com/completeArchive.tar.gz">free hosting service</a>.
 * 
 * 
 * \section install_sec Installation
 *
 * For the moment, the only bridge implemented is from CAEN. To use it, you should first install CAENVME's driver.
 * 
 * When the library was first created, the driver used were  <a href="https://dl.dropboxusercontent.com/u/33459975/cosmicTrigger/datasheets/CAENUSBdrvB-1.2.tgz">CAENUSBdrvB-1.2</a>   (necessary) and  <a href="https://dl.dropboxusercontent.com/u/33459975/cosmicTrigger/datasheets/CAENVMELib-2.30.2.tgz">CAENVMELib-2.30.2</a>   (probably optional). These drivers worked on a 64bits distribution of linux. More drivers can be found on  <a href="http://www.caen.it/csite/CaenProd.jsp?idmod=417&parent=11">Caen's website</a>  
 * 
 * After installing those drivers, the library should be ready to use (yay!). A makefile is provided to compile a program named "main.cpp".
 * 
 * \section concl_sec Example
 * 
 * A few examples are provided in the folder /examples. 
 * 
 * One of them, \ref normalOp.cpp explains how the system can be used for a regular cosmic trigger system and is probably the best way to see concretely how the library work.
 * 
 * More examples can be found in the appropriate section.
 * 
 * \section contact Contact
 * 
 * For any further information, please feel free to contact me on <a href="https://github.com/md987/CosmicTrigger">github</a> or using my ucl student email if you know it.
 * 
 */
 /**
 *\example normalOp.cpp
 * This is example shows how the system can be used for a regular cosmic trigger system. 
 * 
 * Please note that the high voltage is not regulated in this program. Go to \ref Alim.cpp to see how it can be done.
 */ 
 /**
 *\example exampleTDC.cpp
 * This was how the tdc was used to analyse the phase between the clock and the trigger.
 */
 /**
 *\example Alim.cpp
 * This example shows you how the control of the High voltage was implemented.
 */ 
 /**
 *\example HvCalib.cpp
 * Shows how the scaler could be used to test the PMs at different voltages.
 */
