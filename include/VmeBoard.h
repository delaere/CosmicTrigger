#ifndef __VMEBOARD
#define __VMEBOARD

#include "CommonDef.h"
#include "CAENVMEtypes.h"
#include "VmeController.h"

  /**
   * \brief Mother class for any board type class.
   * 
   * This class should be used to create any daughter board class.
   * 
   * It is used to store basic communication functions with a virtual vmeController and to store associated default values.
   *
   * A few read/write functions are implemented here in order to simplify communications. Those functions use the virtual read/write functions of the vmeController and the DataWidth/AddressModifier are either specified when calling the function or the default values will be used.
   * 
   * 
   * 
   * If the default values where not specified when creating the vmeBoard object, A32_U_DATA and D16 will be used.
   * 
   * 
   */

class vmeBoard{
  
public:
  vmeBoard(vmeController* cont,CVAddressModifier AM=cvA32_U_DATA,CVDataWidth DW=cvD16);
  /**< \brief Constructor
   * 
   *  When a daughter card is created, it will call this constructor. The vmeController pointer is always given, and if the card uses special AddressModifier or DataWidth, those argument should be sent then.
   * 
   * If, for some reason, those default AM/DW values have to be changed, the functions setDW() or setAM() should be used.
   * 
   * 
   */
  
protected:
  vmeController* Controller(void) { return cont; }/**<\brief Allows daughter class to get the controller*/
  
  void writeData(long unsigned int add, void *DATA);
  /**<\brief Uses default values
  *
  *This function will use the virtual controller functions with the stored data in order to communicate with a daughter board.
  * 
  *The add parameter is the address of the register (not the board!) to be accessed and *DATA a pointer towards any type and will be used to send the appropriate number of bits to the VME Board.
  * 
  * The amount of bits send is stored in DW and can be modified with setDW.
  *
  */
  
  void readData(long unsigned int add, void *DATA);
  /**<\brief Uses default values
  *
  *This function will use the virtual controller functions with the stored data in order to communicate with a daughter board.
  * 
  * The add parameter is the address of the register (not the board!) to be accessed and *DATA a pointer towards any type and will be used to read the appropriate number of bits to the VME Board.
  * 
  * The default DW and AM are set when the vmeBoard is created but can be changed with setDW or setAM.
  *
  */  
  
  void writeData(long unsigned int add, void *DATA,CVAddressModifier tAM, CVDataWidth tDW);
  /**<\brief Uses temporary values
   *
   * This write function will use the virtual controller's write function with the given parameters.
   *
   */
  
  void readData(long unsigned int add, void *DATA,CVAddressModifier tAM, CVDataWidth tDW);
    /**<\brief Uses temporary values
   *
   * This read function will use the virtual controller's read function with the given parameters.
   *
   */
  
  void setAM(CVAddressModifier AM);
  /**< \brief Saves default value
   *
   * This function allows the user to change the stored AddressModifier value that will be used to communicate with the Board if no parameter is specified.
   *
   */
  
  void setDW(CVDataWidth DW);
  /**< \brief Saves default value
   *
   * This function allows the user to change the stored DataWidth value that will be used to communicate with the Board if no parameter is specified.
   *
   */

  bool vLevel(coutLevel level);
  /**< \brief Checks the verbosity level.
   * 
   * This function will access the verbosity level of the vmeController and return 1 if the given level is above the controller's level.
   * 
   * It allows the controller to choose what messages should be displayed.
   * 
   * The possible values for the level are: SILENT, ERROR, WARNING, NORMAL and DEBUG.
   * 
   */
  
  void setAdd(int add){this->add=add;}///<Changes the address of the board.
  
  int add;
private:
  CVAddressModifier AM; ///< Stored AM value
  CVDataWidth DW; ///<Stored DW value
  vmeController *cont; ///<Pointer to the controller. Only this class can access it.
};

#endif
