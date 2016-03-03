//---------------------------------------------------------------------------

#ifndef TagsDictDlgH
#define TagsDictDlgH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Buttons.hpp>
#include <CheckLst.hpp>
#include <Menus.hpp>

#include <string>
#include <vector>

//---------------------------------------------------------------------------

struct TTagsDictionaryEntry {
  std::string TagName;
  int TagID;

  // Default constructor
  TTagsDictionaryEntry(void) {
    TagID = 0;
  }

  // Initialization constructor
  TTagsDictionaryEntry(const std::string& TagName_,int TagID_) {
    TagName = TagName_;
    TagID = TagID_;
  }
};

typedef std::vector<TTagsDictionaryEntry> TTagsDictionary;

// Vector of tags
typedef std::vector<int> TTagList;


class TTagsDictForm : public TForm
{
__published:	// IDE-managed Components
        TBitBtn *BitBtn1;
        TBitBtn *BitBtn2;
        TCheckListBox *TagsCheckListBox;
        TPopupMenu *PopupMenu1;
        TMenuItem *ClearAllMenuItem;
        TMenuItem *SelectAllMenuItem;
        void __fastcall ClearAllMenuItemClick(TObject *Sender);
        void __fastcall SelectAllMenuItemClick(TObject *Sender);
private:	// User declarations
  TTagsDictionary *m_TagsDict;
  
public:		// User declarations
        __fastcall TTagsDictForm(TComponent* Owner);

  void SetTagsDictionary(TTagsDictionary& TagsDict);
  AnsiString GetTagsString(void);
  void MarkTags(const TTagList& TagList);
};
//---------------------------------------------------------------------------
extern PACKAGE TTagsDictForm *TagsDictForm;
//---------------------------------------------------------------------------
#endif
