/*=========================================================================

  Program:   AtamaiTracking for VTK
  Module:    $RCSfile: vtkNDITracker.h,v $
  Creator:   David Gobbi <dgobbi@atamai.com>
  Language:  C++
  Author:    $Author: dgobbi $
  Date:      $Date: 2004/02/17 20:37:35 $
  Version:   $Revision: 1.1 $

==========================================================================

Copyright (c) 2000-2004 Atamai, Inc.
All rights reserved.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
MODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

=========================================================================*/
// .NAME vtkNDITracker - VTK interface for Northern Digital's NDI
// .SECTION Description
// The vtkNDITracker class provides an  interface to the NDI
// (Northern Digital Inc., Waterloo, Canada) optical tracking system.
// It also works with the AURORA magnetic tracking system, using the
// NDI API.
// .SECTION Caveats
// This class refers to ports 1,2,3,A,B,C as ports 0,1,2,3,4,5
// .SECTION see also
// vtkTrackerTool vtkFlockTracker


#ifndef __vtkNDITracker_h
#define __vtkNDITracker_h

#include "vtkTracker.h"
#include "ndicapi.h"

class vtkFrameToTimeConverter;

// the number of tools the polaris can handle
#define VTK_NDI_NTOOLS 12
#define VTK_NDI_REPLY_LEN 2048

class VTK_EXPORT vtkNDITracker : public vtkTracker
{
public:

  static vtkNDITracker *New();
  vtkTypeMacro(vtkNDITracker,vtkTracker);
  void PrintSelf(ostream& os, vtkIndent indent);
 
  // Description:
  // Probe to see if the tracking system is present on the
  // specified serial port.  If the SerialPort is set to -1,
  // then all serial ports will be checked.
  int Probe();

  // Description:
  // Send a command to the NDI in the format INIT: or VER:0 (the
  // command should include a colon).  Commands can only be done after
  // either Probe() or StartTracking() has been called.
  // The text reply from the NDI is returned, without the CRC or
  // final carriage return.
  char *Command(const char *command);

  // Description:
  // Get the a string (perhaps a long one) describing the type and version
  // of the device.
  vtkGetStringMacro(Version);

  // Description:
  // Set which serial port to use, 1 through 4.
  vtkSetMacro(SerialPort, int);
  vtkGetMacro(SerialPort, int);

  // Description:
  // Set the desired baud rate.  Default: 9600.  
  vtkSetMacro(BaudRate, int);
  vtkGetMacro(BaudRate, int);

  // Description:
  // Enable a passive tool by uploading a virtual SROM for that
  // tool, where 'tool' is a number between 0 and 5.
  void LoadVirtualSROM(int tool, const char *filename);
  void ClearVirtualSROM(int tool);

  // Description:
  // Get an update from the tracking system and push the new transforms
  // to the tools.  This should only be used within vtkTracker.cxx.
  void InternalUpdate();

protected:
  vtkNDITracker();
  ~vtkNDITracker();

  // Description:
  // Set the version information.
  vtkSetStringMacro(Version);

  // Description:
  // Start the tracking system.  The tracking system is brought from
  // its ground state into full tracking mode.  The NDI will
  // only be reset if communication cannot be established without
  // a reset.
  int InternalStartTracking();

  // Description:
  // Stop the tracking system and bring it back to its ground state:
  // Initialized, not tracking, at 9600 Baud.
  int InternalStopTracking();

  // Description:
  // Cause the NDI to beep the specified number of times.
  int InternalBeep(int n);

  // Description:
  // Set the specified tool LED to the specified state.
  int InternalSetToolLED(int tool, int led, int state);

  // Description:
  // This is a low-level method for loading a virtual SROM.
  // You must halt the tracking thread and take the NDI
  // out of tracking mode before you use it.
  void InternalLoadVirtualSROM(int tool, const unsigned char data[1024]);
  void InternalClearVirtualSROM(int tool);

  // Description:
  // Methods for detecting which ports have tools in them, and
  // auto-enabling those tools.
  void EnableToolPorts();
  void DisableToolPorts();

  // Description:
  // Find the tool for a specific port handle (-1 if not found).
  int GetToolFromHandle(int handle);

  // Description:
  // Class for updating the virtual clock that accurately times the
  // arrival of each transform, more accurately than is possible with
  // the system clock alone because the virtual clock averages out the
  // jitter.
  vtkFrameToTimeConverter *Timer;

  ndicapi *Device;
  char *Version;

  vtkMatrix4x4 *SendMatrix;
  int SerialPort; 
  int BaudRate;
  int IsDeviceTracking;

  int PortHandle[VTK_NDI_NTOOLS];
  int PortEnabled[VTK_NDI_NTOOLS];
  unsigned char *VirtualSROM[VTK_NDI_NTOOLS];

  char CommandReply[VTK_NDI_REPLY_LEN];

private:
  vtkNDITracker(const vtkNDITracker&);
  void operator=(const vtkNDITracker&);  
};

#endif




