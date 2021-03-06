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
// Copyright (c) 1995-2020 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//============================================================================

#ifndef BROWSER_DIALOG_HXX
#define BROWSER_DIALOG_HXX

class GuiObject;
class ButtonWidget;
class EditTextWidget;
class FileListWidget;
class StaticTextWidget;
class FilesystemNode;

#include "Dialog.hxx"
#include "Command.hxx"
#include "bspf.hxx"

class BrowserDialog : public Dialog, public CommandSender
{
  public:
    enum ListMode {
      FileLoad,   // File selector, no input from user
      FileSave,   // File selector, filename changable by user
      Directories // Directories only, no input from user
    };

  public:
    BrowserDialog(GuiObject* boss, const GUI::Font& font, int max_w, int max_h,
                  const string& title = "");
    virtual ~BrowserDialog() = default;

    /** Place the browser window onscreen, using the given attributes */
    void show(const string& startpath,
              BrowserDialog::ListMode mode, int cmd, const string& ext = "");

    /** Get resulting file node (called after receiving kChooseCmd) */
    const FilesystemNode& getResult() const;

  private:
    void handleCommand(CommandSender* sender, int cmd, int data, int id) override;
    void updateUI();

  private:
    enum {
      kChooseCmd  = 'CHOS',
      kGoUpCmd    = 'GOUP',
      kBaseDirCmd = 'BADR'
    };

    int	_cmd{0};

    FileListWidget*   _fileList{nullptr};
    EditTextWidget*   _currentPath{nullptr};
    StaticTextWidget* _type{nullptr};
    EditTextWidget*   _selected{nullptr};
    ButtonWidget*     _goUpButton{nullptr};
    ButtonWidget*     _basedirButton{nullptr};

    BrowserDialog::ListMode _mode{Directories};

  private:
    // Following constructors and assignment operators not supported
    BrowserDialog() = delete;
    BrowserDialog(const BrowserDialog&) = delete;
    BrowserDialog(BrowserDialog&&) = delete;
    BrowserDialog& operator=(const BrowserDialog&) = delete;
    BrowserDialog& operator=(BrowserDialog&&) = delete;
};

#endif
