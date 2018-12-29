//============================================================================
//
//   SSSS    tt          lll  lll
//  SS  SS   tt           ll   ll
//  SS     tttttt  eeee   ll   ll   aaaa
//   SSSS    tt   ee  ee  ll   ll      aa
//      SS   tt   eeeeee  ll   ll   aaaaa  --  "An Atari 2600 VCS Emulator"
//  SS  SS   tt   ee      ll   ll  aa  aa
//   SSSS     ttt  eeeee llll llll  aaaaa
//
// Copyright (c) 1995-2018 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//============================================================================

#include "Console.hxx"
#include "TIA.hxx"
#include "Switches.hxx"
#include "Dialog.hxx"
#include "Font.hxx"
#include "EventHandler.hxx"
#include "StateManager.hxx"
#include "OSystem.hxx"
#include "Widget.hxx"
#include "CommandDialog.hxx"

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CommandDialog::CommandDialog(OSystem& osystem, DialogContainer& parent)
  : Dialog(osystem, parent, osystem.frameBuffer().font(), "Commands")
{
  const int HBORDER = 10;
  const int VBORDER = 10;
  const int HGAP = 8;
  const int VGAP = 4;
  const int buttonWidth = _font.getStringWidth("Stella Palette") + 16,
            buttonHeight = _font.getLineHeight() + 6,
            rowHeight = buttonHeight + VGAP;
                                                         
  // Set real dimensions
  _w = 3 * (buttonWidth + 5) + HBORDER * 2;
  _h = 6 * rowHeight - VGAP + VBORDER * 2 + _th;
  ButtonWidget* bw;
  WidgetArray wid;
  int xoffset = HBORDER, yoffset = VBORDER + _th;

  auto ADD_CD_BUTTON = [&](const string& label, int cmd)
  {
    ButtonWidget* bw = new ButtonWidget(this, _font, xoffset, yoffset,
            buttonWidth, buttonHeight, label, cmd);
    yoffset += buttonHeight + VGAP;
    return bw;
  };

  // Column 1
  bw = ADD_CD_BUTTON("Select", kSelectCmd);
  wid.push_back(bw);
  bw = ADD_CD_BUTTON("Reset", kResetCmd);
  wid.push_back(bw);
  myColorButton = ADD_CD_BUTTON("", kColorCmd);
  wid.push_back(myColorButton);
  myLeftDiffButton = ADD_CD_BUTTON("", kLeftDiffCmd);
  wid.push_back(myLeftDiffButton);
  myRightDiffButton = ADD_CD_BUTTON("", kLeftDiffCmd);
  wid.push_back(myRightDiffButton);

  // Column 2
  xoffset += buttonWidth + HGAP;
  yoffset = VBORDER + _th;

  mySaveStateButton = ADD_CD_BUTTON("", kSaveStateCmd);
  wid.push_back(mySaveStateButton);
  myStateSlotButton = ADD_CD_BUTTON("", kStateSlotCmd);
  wid.push_back(myStateSlotButton);
  myLoadStateButton = ADD_CD_BUTTON("", kLoadStateCmd);
  wid.push_back(myLoadStateButton);
  bw = ADD_CD_BUTTON("Snapshot", kSnapshotCmd);
  wid.push_back(bw);
  bw = ADD_CD_BUTTON("TODO", 0);
  wid.push_back(bw);
  bw = ADD_CD_BUTTON("Exit Game", kExitCmd);
  wid.push_back(bw);

  // Column 3
  xoffset += buttonWidth + HGAP;
  yoffset = VBORDER + _th;

  myTVFormatButton = ADD_CD_BUTTON("", kFormatCmd);
  wid.push_back(myTVFormatButton);
  myPaletteButton = ADD_CD_BUTTON("", kPaletteCmd);
  wid.push_back(myPaletteButton);
  bw = ADD_CD_BUTTON("TODO", 0);
  wid.push_back(bw);
  bw = ADD_CD_BUTTON("TODO", 0);
  wid.push_back(bw);
  bw = ADD_CD_BUTTON("Reload ROM", kReloadRomCmd);
  wid.push_back(bw);

  addToFocusList(wid);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CommandDialog::loadConfig()
{
  myColorButton->setLabel(instance().console().switches().tvColor() ? "Color Mode" : "B/W Mode");
  myLeftDiffButton->setLabel(instance().console().switches().leftDifficultyA() ? "Left Diff A" : "Left Diff B");
  myRightDiffButton->setLabel(instance().console().switches().rightDifficultyA() ? "Right Diff A" : "Right Diff B");

  updateSlot(instance().state().currentSlot());

  myTVFormatButton->setLabel(instance().console().getFormatString() + " Mode");
  string palette, label;
  palette = instance().settings().getString("palette");
  if(BSPF::equalsIgnoreCase(palette, "standard"))
    label = "Stella Palette";
  else if(BSPF::equalsIgnoreCase(palette, "z26"))
    label = "Z26 Palette";
  else
    label = "User Palette";
  myPaletteButton->setLabel(label);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CommandDialog::handleCommand(CommandSender* sender, int cmd,
                                  int data, int id)
{
  bool consoleCmd = false, stateCmd = false;
  Event::Type event = Event::NoType;

  switch(cmd)
  {
    case kSelectCmd:
      event = Event::ConsoleSelect;
      consoleCmd = true;
      break;

    case kResetCmd:
      event = Event::ConsoleReset;
      consoleCmd = true;
      break;

    case kColorCmd:
      event = Event::ConsoleColorToggle;
      consoleCmd = true;
      break;

    case kLeftDiffCmd:
      event = Event::ConsoleLeftDiffToggle;
      consoleCmd = true;
      break;

    case kRightDiffCmd:
      event = Event::ConsoleRightDiffToggle;
      consoleCmd = true;
      break;

    case kSaveStateCmd:
      event = Event::SaveState;
      consoleCmd = true;
      break;

    case kStateSlotCmd:
    {
      event = Event::ChangeState;
      stateCmd = true;
      int slot = (instance().state().currentSlot() + 1) % 10;
      updateSlot(slot);
      break;
    }
    case kLoadStateCmd:
      event = Event::LoadState;
      consoleCmd = true;
      break;

    case kSnapshotCmd:
      instance().eventHandler().leaveMenuMode();
      instance().eventHandler().handleEvent(Event::TakeSnapshot, 1);
      break;

    case kFormatCmd:
      instance().eventHandler().leaveMenuMode();
      instance().console().toggleFormat();
      break;

    case kPaletteCmd:
      instance().eventHandler().leaveMenuMode();
      instance().console().togglePalette();
      break;

    case kReloadRomCmd:
      instance().eventHandler().leaveMenuMode();
      instance().reloadConsole();
      break;

    case kExitCmd:
      instance().eventHandler().handleEvent(Event::LauncherMode, 1);
      break;
  }

  // Console commands show be performed right away, after leaving the menu
  // State commands require you to exit the menu manually
  if(consoleCmd)
  {
    instance().eventHandler().leaveMenuMode();
    instance().eventHandler().handleEvent(event, 1);
    instance().console().switches().update();
    instance().console().tia().update();
    instance().eventHandler().handleEvent(event, 0);
  }
  else if(stateCmd)
    instance().eventHandler().handleEvent(event, 1);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void CommandDialog::updateSlot(int slot)
{
  ostringstream buf;
  buf << " " << slot;

  mySaveStateButton->setLabel("Save State" + buf.str());
  myStateSlotButton->setLabel("State Slot" + buf.str());
  myLoadStateButton->setLabel("Load State" + buf.str());
}
