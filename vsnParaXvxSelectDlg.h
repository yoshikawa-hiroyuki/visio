#ifndef _VSN_PARA_XVX_SELECT_DLG_HPP_
#define _VSN_PARA_XVX_SELECT_DLG_HPP_

#include <vector>
#include <string>

namespace VSN {
  enum {
    ParaXvxSelectDlg_OkBtn =7500,
    ParaXvxSelectDlg_CancelBtn,
    ParaXvxSelectDlg_SelectAllBtn,
    ParaXvxSelectDlg_DeselectAllBtn,
    ParaXvxSelectDlg_FilterTxt
  };
};

class vsnViewFrame;

/**
 * 複数の並列計算結果ファイルから表示するファイルを選択するダイアログ
 */
class vsnParaXvxSelectDlg : public wxDialog {
public:
  /**
   * コンストラクタ
   * @param parent 親ウィンドウ
   */
  vsnParaXvxSelectDlg(vsnViewFrame* paent);

  virtual ~vsnParaXvxSelectDlg(void) {
    if (m_filterTxt) m_filterTxt = NULL;
    if (m_listCtrl) m_listCtrl = NULL;
  }

  /**
   * インデックスファイルの読み込み
   */
  void LoadIndexFile(const std::string& indexPath);

  /**
   * OKボタンのイベントハンドラ
   */
  void OnOK(wxCommandEvent& event);

  /**
   * Cancel ボタンのイベントハンドラ
   */
  void OnCancel(wxCommandEvent& event);

  /**
   * SelectAll ボタンのイベントハンドラ
   */
  void OnSelectAll(wxCommandEvent& event);

  /**
   * DeselectAll ボタンのイベントハンドラ
   */
  void OnDeselectAll(wxCommandEvent& event);

  /**
   * Filter テキストでエンターが押されたときのイベントハンドラ
   */
  void OnEnterFilter(wxCommandEvent& event);

  /**
   * インデックスファイルに書かれている全てのファイル名
   */
  const std::vector<std::string>& GetAllNames(void) const {
    return m_allNames;
  }

  /**
   * 選択されたファイル名
   */
  const std::vector<std::string>& GetSelectedNames(void) const {
    return m_selectedNames;
  }


private:
  /**
   * ボタン類の配置
   * @param pBaseSize ダイアログのベースの Sizer
   */
  void createLayout(wxBoxSizer* pBaseSizer);

  std::string m_baseDir;
  std::string m_idxPath;

  wxListCtrl* m_listCtrl;
  wxTextCtrl* m_filterTxt;

  std::vector<std::string> m_allNames;
  std::vector<std::string> m_selectedNames;

  DECLARE_EVENT_TABLE()
};

#endif // _VSN_PARA_XVX_SELECT_DLG_HPP_
