//
// vsnMethod_Shape_scalarMap
//
#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include <stdexcept>

#include "vsnMethod_Shape_scalarMap.h"
#include "vsnData_Shape.h"

#include "vsnIsosurf.h"
#include "vsnOctTree.h" // for decomp vecIdx
#include "vsnUiView.h"
#include "vsnColorBar.h"
#include "vsnError.h"
#include "vsnData_Sph.h"
#include "vsnGridUtilSv.h"


//----------------------------------------------------------------
// class vsnMPP_Shape_scalarMap
//----------------------------------------------------------------

// ScalarMap共通のイベントテーブル
//
BEGIN_EVENT_TABLE(vsnMPP_Shape_scalarMap, wxPanel)
  EVT_COMBOBOX(MPP_Shape_scalarMap_SphLst,
               vsnMPP_Shape_scalarMap::OnSphLst)
  EVT_RADIOBOX(MPP_Shape_scalarMap_InterpolateRadio,
	       vsnMPP_Shape_scalarMap::OnInterpolateRadio)
  EVT_RADIOBOX(MPP_Shape_scalarMap_RenderingModeRadio,
	       vsnMPP_Shape_scalarMap::OnRenderingModeRadio)
  EVT_COMBOBOX(MPP_Shape_scalarMap_PhysLst,
               vsnMPP_Shape_scalarMap::OnPhysLst)
  EVT_CHECKBOX(MPP_Shape_scalarMap_UpdMinMaxChk,
	       vsnMPP_Shape_scalarMap::OnUpdMinMaxChk)
  EVT_RADIOBOX(MPP_Shape_scalarMap_NormalDirectionRadio,
	       vsnMPP_Shape_scalarMap::OnNormalDirectionRadio)
//  EVT_CHECKBOX(MPP_Shape_scalarMap_DivideChk,
//	       vsnMPP_Shape_scalarMap::OnDivideChk)
  EVT_CHECKBOX(MPP_Shape_scalarMap_ExUseTrilinearChk,
	       vsnMPP_Shape_scalarMap::OnExUseTrilinearChk)
  EVT_BUTTON(MPP_Shape_scalarMap_OutputFileBtn,
             vsnMPP_Shape_scalarMap::OnOutputFileBtn)
END_EVENT_TABLE()

// スカラー値
//
BEGIN_EVENT_TABLE(vsnMPP_Shape_scalarMap_Phys_scalar, wxPanel)
  EVT_COMBOBOX(MPP_Shape_scalarMap_Phys_scalar_ScalarCombo,
	       vsnMPP_Shape_scalarMap_Phys_scalar::OnScalarLst)
  EVT_TEXT_ENTER(MPP_Shape_scalarMap_Phys_scalar_NumLinesTxt,
		 vsnMPP_Shape_scalarMap_Phys_scalar::OnNumLinesTxt)
  EVT_TEXT_ENTER(MPP_Shape_scalarMap_Phys_scalar_MinTxt,
		 vsnMPP_Shape_scalarMap_Phys_scalar::OnMinMaxTxt)
  EVT_TEXT_ENTER(MPP_Shape_scalarMap_Phys_scalar_MaxTxt,
		 vsnMPP_Shape_scalarMap_Phys_scalar::OnMinMaxTxt)
  EVT_CHECKBOX(MPP_Shape_scalarMap_Phys_scalar_UseCMapChk,
 	       vsnMPP_Shape_scalarMap_Phys_scalar::OnUseCMapChk)
  EVT_TEXT_ENTER(MPP_Shape_scalarMap_Phys_scalar_LineWidthTxt,
 		 vsnMPP_Shape_scalarMap_Phys_scalar::OnLineWidthTxt)
  EVT_CHECKBOX(MPP_Shape_scalarMap_Phys_scalar_AntiAliasChk,
 	       vsnMPP_Shape_scalarMap_Phys_scalar::OnAntiAliasChk)
  EVT_CHECKBOX(MPP_Shape_scalarMap_Phys_scalar_DispContourLineCB,
 	       vsnMPP_Shape_scalarMap_Phys_scalar::OnDispContourLineCB)
END_EVENT_TABLE()

// 圧力寄与率
//
BEGIN_EVENT_TABLE(vsnMPP_Shape_scalarMap_Phys_pressDrag, wxPanel)
  EVT_COMBOBOX(MPP_Shape_scalarMap_Phys_pressDrag_PressCombo,
	       vsnMPP_Shape_scalarMap_Phys_pressDrag::OnPressCombo)
  EVT_RADIOBOX(MPP_Shape_scalarMap_Phys_pressDrag_DirRadio,
	       vsnMPP_Shape_scalarMap_Phys_pressDrag::OnDirRadio)
END_EVENT_TABLE()

// 剪断応力
//
BEGIN_EVENT_TABLE(vsnMPP_Shape_scalarMap_Phys_shearing, wxPanel)
  EVT_TEXT_ENTER(MPP_Shape_scalarMap_Phys_shearing_CoeffValTxt,
		 vsnMPP_Shape_scalarMap_Phys_shearing::OnCoeffValTxt)
  EVT_TEXT_ENTER(MPP_Shape_scalarMap_Phys_shearing_StencilWidthTxt,
		 vsnMPP_Shape_scalarMap_Phys_shearing::OnStencilWidthTxt)
  EVT_TEXT_ENTER(MPP_Shape_scalarMap_Phys_shearing_SurfVelocXTxt,
		 vsnMPP_Shape_scalarMap_Phys_shearing::OnSurfVelocXTxt)
  EVT_TEXT_ENTER(MPP_Shape_scalarMap_Phys_shearing_SurfVelocYTxt,
		 vsnMPP_Shape_scalarMap_Phys_shearing::OnSurfVelocYTxt)
  EVT_TEXT_ENTER(MPP_Shape_scalarMap_Phys_shearing_SurfVelocZTxt,
		 vsnMPP_Shape_scalarMap_Phys_shearing::OnSurfVelocZTxt)
  EVT_RADIOBOX(MPP_Shape_scalarMap_Phys_shearing_MapCompoRadio,
	       vsnMPP_Shape_scalarMap_Phys_shearing::OnMapCompoRadio)
  EVT_COMBOBOX(MPP_Shape_scalarMap_Phys_shearing_PressSphCB,
	       vsnMPP_Shape_scalarMap_Phys_shearing::OnPressSphCB)
  EVT_COMBOBOX(MPP_Shape_scalarMap_Phys_shearing_PressCompoCB,
	       vsnMPP_Shape_scalarMap_Phys_shearing::OnPressCompoCB)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------

// ----------------------------------------------------------------------------
// 剪断応力
//

/**
 * コンストラクタ
 * @param parent 親のパネル
 * @param vsnMethodObj メソッド
 */
vsnMPP_Shape_scalarMap_Phys_shearing::
vsnMPP_Shape_scalarMap_Phys_shearing(wxPanel* parent, vsnMethodObj* pm)
: vsnMethodPP(parent, pm)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_Shape_scalarMap*>(pm));

  wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  /**
   * GUI のコンポーネントを作成する
   */
  {
    // 圧力データの指定
    // 全抵抗の時だけ使用する
    //
    wxSizer* sizerV = new wxBoxSizer(wxVERTICAL);
    topSizer -> Add(sizerV, 0, wxALL, 0);

    {
      wxBoxSizer* sizerH1 = new wxBoxSizer(wxHORIZONTAL);
      assert(sizerH1);
      sizerV -> Add(sizerH1, 0, wxALL, 0);

      m_pPressSphTxt = new wxStaticText(this, -1, wxT("Pressure"));
      assert(m_pPressSphTxt);
      sizerH1 -> Add(m_pPressSphTxt, 0, wxALIGN_LEFT | wxALL, 3);

      m_pPressSphCB
	= new wxComboBox(this, MPP_Shape_scalarMap_Phys_shearing_PressSphCB,
			 wxT(""), wxDefaultPosition, wxDefaultSize,
			 0, NULL, wxCB_READONLY);
      assert(m_pPressSphCB);
      sizerH1 -> Add(m_pPressSphCB, 0, wxEXPAND | wxALL, 3);
    }

    {
      wxBoxSizer* sizerH2 = new wxBoxSizer(wxHORIZONTAL);
      assert(sizerH2);
      sizerV -> Add(sizerH2, 0, wxALL, 0);

      m_pPressCompoTxt = new wxStaticText(this, -1, wxT("Component"));
      assert(m_pPressCompoTxt);
      sizerH2 -> Add(m_pPressCompoTxt, 0, wxALIGN_LEFT | wxALL, 3);
      
      m_pPressCompoCB
	= new wxComboBox(this, MPP_Shape_scalarMap_Phys_shearing_PressCompoCB,
			 wxT(""), wxDefaultPosition, wxDefaultSize,
			 0, NULL, wxCB_READONLY);
      assert(m_pPressCompoCB);
      sizerH2 -> Add(m_pPressCompoCB, 0, wxEXPAND | wxALL, 3);
    }
					
  }

  {
    // 粘性係数のデータ指定
    //
    sizerH = new wxBoxSizer(wxHORIZONTAL);

    sizerH -> Add(new wxStaticText(this, -1, wxT("Coefficient of Viscosity")),
		  0, wxALIGN_LEFT|wxALL, 3);

    m_pCoeffValTxt
      = new wxTextCtrl(this, MPP_Shape_scalarMap_Phys_shearing_CoeffValTxt,
		       wxT("0"), wxDefaultPosition, wxDefaultSize,
		       wxTE_PROCESS_ENTER);
    sizerH -> Add(m_pCoeffValTxt, 1, wxEXPAND|wxALL, 3);

    topSizer -> Add(sizerH, 0, wxALL, 0);
  }

  {
    // ステンシル幅
    //
    sizerH = new wxBoxSizer(wxHORIZONTAL);

    sizerH -> Add(new wxStaticText(this, -1, wxT("Stencil Width")),
		  0, wxALIGN_LEFT|wxALL, 3);

    m_pStencilWidthTxt
      = new wxTextCtrl(this, MPP_Shape_scalarMap_Phys_shearing_StencilWidthTxt,
		       wxT(""),	wxDefaultPosition, wxDefaultSize,
		       wxTE_PROCESS_ENTER);
    sizerH -> Add(m_pStencilWidthTxt, 1, wxEXPAND|wxALL, 3);

    topSizer -> Add(sizerH, 0, wxALL, 0);
  }

  {
    // 表面速度ベクトル
    //
    sizerH = new wxBoxSizer(wxVERTICAL);

    sizerH -> Add(new wxStaticText(this, -1, wxT("Surface Veloc Vector")),
		  0, wxALIGN_LEFT|wxALL, 3);

    {
      wxBoxSizer* tmp = new wxBoxSizer(wxHORIZONTAL);
      tmp -> Add(new wxStaticText(this, -1, wxT("     X")),
		 0, wxALIGN_LEFT|wxALL, 3);
      m_pSurfVelocTxt[0]
	= new wxTextCtrl(this, MPP_Shape_scalarMap_Phys_shearing_SurfVelocXTxt,
			 wxT(""), wxDefaultPosition, wxDefaultSize,
			 wxTE_PROCESS_ENTER);
      tmp -> Add(m_pSurfVelocTxt[0], 1, wxEXPAND|wxALL, 3);

      sizerH -> Add(tmp, 1, wxEXPAND | wxALL, 3);
    }

    {
      wxBoxSizer* tmp = new wxBoxSizer(wxHORIZONTAL);
      tmp -> Add(new wxStaticText(this, -1, wxT("     Y")),
		 0, wxALIGN_LEFT|wxALL, 3);
      m_pSurfVelocTxt[1]
	= new wxTextCtrl(this, MPP_Shape_scalarMap_Phys_shearing_SurfVelocYTxt,
			 wxT(""), wxDefaultPosition, wxDefaultSize,
			 wxTE_PROCESS_ENTER);
      tmp -> Add(m_pSurfVelocTxt[1], 1, wxEXPAND|wxALL, 3);

      sizerH -> Add(tmp, 1, wxEXPAND|wxALL, 3);
    }

    {
      wxBoxSizer* tmp = new wxBoxSizer(wxHORIZONTAL);
      tmp -> Add(new wxStaticText(this, -1, wxT("     Z")),
		 0, wxALIGN_LEFT|wxALL, 3);
      m_pSurfVelocTxt[2]
	= new wxTextCtrl(this, MPP_Shape_scalarMap_Phys_shearing_SurfVelocZTxt,
			 wxT(""), wxDefaultPosition, wxDefaultSize,
			 wxTE_PROCESS_ENTER);
      tmp -> Add(m_pSurfVelocTxt[2], 1, wxEXPAND|wxALL, 3);

      sizerH -> Add(tmp, 1, wxEXPAND|wxALL, 3);
    }

    topSizer -> Add(sizerH, 0, wxALL, 0);
  }

  {
    // ポリンゴンにマップする成分
    //
    sizerH = new wxBoxSizer(wxHORIZONTAL);

    wxString items[] = {wxString(wxT("u")), wxString(wxT("v")),
			wxString(wxT("w")), wxString(wxT("vecLen"))};
    m_pMapCompoRadio
      = new wxRadioBox(this, MPP_Shape_scalarMap_Phys_shearing_MapCompoRadio,
		       wxT("MapComponent"),
		       wxDefaultPosition, wxDefaultSize,
		       4, items, 1, wxRA_SPECIFY_ROWS);
    sizerH -> Add(m_pMapCompoRadio, 0, wxEXPAND|wxALL, 3);

    topSizer -> Add(sizerH, 0, wxALL, 0);
  }

  {
    // 全抵抗値の表示
    //
    wxBoxSizer* sizerV = new wxBoxSizer(wxVERTICAL);
    assert(sizerV);
    m_pTotalResistLabel[3]
      = new wxStaticText(this, -1, wxT("Total Resistance"));
    sizerV -> Add(m_pTotalResistLabel[3]);

    sizerH = new wxBoxSizer(wxHORIZONTAL);
    assert(sizerH);
    m_pTotalResistLabel[0] = new wxStaticText(this, -1, wxT("X"));
    sizerH -> Add(m_pTotalResistLabel[0]);
    m_pTotalResistTxt[0]
      = new wxTextCtrl(this, MPP_Shape_scalarMap_Phys_shearing_TotalResistX,
		       wxT("0"), wxDefaultPosition, wxDefaultSize);
    assert(m_pTotalResistTxt[0]);
    sizerH -> Add(m_pTotalResistTxt[0], 0, wxEXPAND|wxALL, 3);
    sizerV -> Add(sizerH);

    sizerH = new wxBoxSizer(wxHORIZONTAL);
    assert(sizerH);
    m_pTotalResistLabel[1] = new wxStaticText(this, -1, wxT("Y"));
    sizerH -> Add(m_pTotalResistLabel[1]);
    m_pTotalResistTxt[1]
      = new wxTextCtrl(this, MPP_Shape_scalarMap_Phys_shearing_TotalResistY,
		       wxT("0"), wxDefaultPosition, wxDefaultSize);
    assert(m_pTotalResistTxt[1]);
    sizerH -> Add(m_pTotalResistTxt[1], 1, wxEXPAND | wxALL, 3);
    sizerV -> Add(sizerH);

    sizerH = new wxBoxSizer(wxHORIZONTAL);
    assert(sizerH);
    m_pTotalResistLabel[2] = new wxStaticText(this, -1, wxT("Z"));
    sizerH -> Add(m_pTotalResistLabel[2]);
    m_pTotalResistTxt[2]
      = new wxTextCtrl(this, MPP_Shape_scalarMap_Phys_shearing_TotalResistZ,
		       wxT("0"), wxDefaultPosition, wxDefaultSize);
    assert(m_pTotalResistTxt[2]);
    sizerH -> Add(m_pTotalResistTxt[2], 1, wxEXPAND | wxALL, 3);
    sizerV -> Add(sizerH);
    
    topSizer -> Add(sizerV, 0, wxALL, 0);
  }

  // post process
  SetSizer(topSizer);
  addTo(parent);
  topSizer->Layout();
  (void)update();
}

/**
 * デストラクタ
 */
vsnMPP_Shape_scalarMap_Phys_shearing::
~vsnMPP_Shape_scalarMap_Phys_shearing(void)
{
}

/**
 * 表示の更新
 */
bool
vsnMPP_Shape_scalarMap_Phys_shearing::update(void)
{

  if (! m_pPressSphCB) return false;
  if (! m_pPressCompoCB) return false;
  if (! m_pPressSphTxt) return false;
  if (! m_pPressCompoTxt) return false;
  if (! m_pTotalResistTxt[0]) return false;
  if (! m_pTotalResistTxt[1]) return false;
  if (! m_pTotalResistTxt[2]) return false;


  if (! m_pCoeffValTxt) return false;
  if (! m_pStencilWidthTxt) return false;
  if (! m_pSurfVelocTxt[0]) return false;
  if (! m_pSurfVelocTxt[1]) return false;
  if (! m_pSurfVelocTxt[2]) return false;
  if (! m_pMapCompoRadio) return false;

  vsnMethod_Shape_scalarMap* pm
    = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);
  if (! pm) return false;

  if (pm -> getPhysIdx() == 4) {
    // 全抵抗
    //
    m_pPressSphTxt   -> Show(true);
    m_pPressSphCB    -> Show(true);
    m_pPressCompoTxt -> Show(true);
    m_pPressCompoCB  -> Show(true);

    m_pPressSphCB -> SetSelection(wxNOT_FOUND);
    //if (! m_pPressSphCB -> IsEmpty()) m_pPressSphCB -> Clear();
    m_pPressSphCB -> Append(wxT("None"));
    for (size_t i = 0; i < pm -> sphSize(); ++i) {
      m_pPressSphCB -> Append(vsnApp::ConvSysToWx(pm->getSph(i)->getName()));
    }
    int pressSphIdx = pm -> getPressSphIdx();
    if (pressSphIdx < m_pPressSphCB -> GetCount())
      m_pPressSphCB -> SetSelection(pressSphIdx);

    m_pPressCompoCB -> SetSelection(wxNOT_FOUND);
    //if (! m_pPressCompoCB -> IsEmpty()) m_pPressCompoCB -> Clear();
    vsnData_Sph* pressSph = pm -> getSph(pressSphIdx - 1);
    if (pressSph) {
      int dlen = pressSph -> getDataLen();
      for (int i = 0; i < dlen; i++) {
	char txt[64];
	sprintf(txt, "data%d", i);
	m_pPressCompoCB -> Append(vsnApp::ConvSysToWx(txt));
      }

      int pressCompoIdx = pm -> getPressCompoIdx();
      if (pressCompoIdx < m_pPressCompoCB -> GetCount())
 	m_pPressCompoCB -> SetSelection(pressCompoIdx);
    } else {
      std::cout << "no sph" << std::endl;
    }

    for (size_t i = 0; i < 3; ++i) {
      char buf[BUFSIZ];
      sprintf(buf, "%g", pm -> getTotalResistValue(i));
      m_pTotalResistTxt[i] -> SetValue(vsnApp::ConvSysToWx(buf));
    }

  } else {
    m_pPressSphTxt   -> Show(false);
    m_pPressSphCB    -> Show(false);
    m_pPressCompoTxt -> Show(false);
    m_pPressCompoCB  -> Show(false);

    m_pTotalResistTxt[0] -> Show(false);
    m_pTotalResistTxt[1] -> Show(false);
    m_pTotalResistTxt[2] -> Show(false);

    if (m_pTotalResistLabel[0]) m_pTotalResistLabel[0] -> Show(false);
    if (m_pTotalResistLabel[1]) m_pTotalResistLabel[1] -> Show(false);
    if (m_pTotalResistLabel[2]) m_pTotalResistLabel[2] -> Show(false);
    if (m_pTotalResistLabel[3]) m_pTotalResistLabel[3] -> Show(false);
  }

  char buf[BUFSIZ];

  // 粘性係数
  //
  sprintf(buf, "%g", pm -> getShearingCoeffVal());
  m_pCoeffValTxt -> SetValue(vsnApp::ConvSysToWx(buf));

  // ステンシル幅
  //
  sprintf(buf, "%g", pm -> getShearingStencilWidth());
  m_pStencilWidthTxt -> SetValue(vsnApp::ConvSysToWx(buf));

  // 表面速度ベクトル
  //
  for (int dir = 0; dir < 3; ++dir) {
    sprintf(buf, "%g", pm -> getShearingSurfVeloc(dir));
    m_pSurfVelocTxt[dir] -> SetValue(vsnApp::ConvSysToWx(buf));
  }

  // マップの方向
  //
  m_pMapCompoRadio -> SetSelection(pm -> getShearingMapCompo());
	  
  return true;
}

/**
 * 粘性係数の値が変更されたときのコールバック関数
 * @param event イベント
 */
void
vsnMPP_Shape_scalarMap_Phys_shearing::OnCoeffValTxt(wxCommandEvent& event)
{
  if (! m_pCoeffValTxt) return;
  vsnMethod_Shape_scalarMap* pm
    = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);
  if (! pm) return;

  wxString val = m_pCoeffValTxt -> GetValue();
  const char* ptr = val.mb_str();
  if (! ptr) return;

  if (pm -> setShearingCoeffVal(atof(ptr)))
    pm -> chkNotice();
}

/**
 * ステンシル幅の入力が変更されたときのコールバック
 * @param event イベント
 */
void 
vsnMPP_Shape_scalarMap_Phys_shearing::OnStencilWidthTxt(wxCommandEvent& event)
{
  if (! m_pStencilWidthTxt) return;

  vsnMethod_Shape_scalarMap* pm
    = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);
  if (! pm) return;

  wxString valStr = m_pStencilWidthTxt -> GetValue();
  if (valStr.IsEmpty()) return;
  const char* ptr = valStr.mb_str();
  if (! ptr) return;

  if (pm -> IsShearingMode()) {
    if (pm -> setShearingStencilWidth(atof(ptr))) {
      pm -> chkNotice();
    }
  } else if (pm -> IsFricResistMode()) {
    if (pm -> setFricResistStencilWidth(atof(ptr))) {
      pm -> chkNotice();
    }
  }
}

/**
 * 表面速度ベクトルの設定
 * @param dir 方向(=0: X方向、=1:Y方向、=2:Z方向)
 */
void
vsnMPP_Shape_scalarMap_Phys_shearing::OnSurfVelocTxt(size_t dir)
{
  if (! m_pSurfVelocTxt[dir]) return;
  vsnMethod_Shape_scalarMap* pm
    = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);
  if (! pm) return;

  wxString valStr = m_pSurfVelocTxt[dir] -> GetValue();
  if (valStr.IsEmpty()) return;
  const char* ptr = valStr.mb_str();
  if (! ptr) return;

  if (pm -> IsShearingMode()) {
    if (pm -> setShearingSurfVeloc(atof(ptr), dir)) {
      pm -> chkNotice();
    }
  } else {
    if (pm -> setFricResistSurfVeloc(atof(ptr), dir)) {
      pm -> chkNotice();
    }
  }
}

/**
 * 表面速度ベクトル(X)の入力のコールバック関数
 * @param event イベント
 */
void 
vsnMPP_Shape_scalarMap_Phys_shearing::OnSurfVelocXTxt(wxCommandEvent& event)
{
  OnSurfVelocTxt(0);
}

/**
 * 表面速度ベクトル(Y)の入力のコールバック関数
 * @param event イベント
 */
void 
vsnMPP_Shape_scalarMap_Phys_shearing::OnSurfVelocYTxt(wxCommandEvent& event)
{
  OnSurfVelocTxt(1);
}

/**
 * 表面速度ベクトル(Z)の入力のコールバック関数
 * @param event イベント
 */
void 
vsnMPP_Shape_scalarMap_Phys_shearing::OnSurfVelocZTxt(wxCommandEvent& event)
{
  OnSurfVelocTxt(2);
}

/**
 * マップするコンポーネントのコールバック関数
 * @param event イベント
 */
void 
vsnMPP_Shape_scalarMap_Phys_shearing::OnMapCompoRadio(wxCommandEvent& event)
{
  if (! m_pMapCompoRadio) return;
  vsnMethod_Shape_scalarMap* pm
    = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);
  if (! pm) return;

  if (pm -> IsShearingMode()) {
    if (pm -> setShearingMapCompo(m_pMapCompoRadio -> GetSelection()))
      pm -> chkNotice();
  } else if (pm -> IsFricResistMode()) {
    if (pm -> setFricResistMapCompo(m_pMapCompoRadio -> GetSelection()))
      pm -> chkNotice();
  } else {
    if (pm -> setShearingMapCompo(m_pMapCompoRadio -> GetSelection()))
      pm -> chkNotice();
  }

  pm -> chkNotice();
}

/**
 * 全抵抗
 * 圧力のSPHが変更されたときのコールバック関数
 * @param event イベント
 */
void
vsnMPP_Shape_scalarMap_Phys_shearing::OnPressSphCB(wxCommandEvent& event)
{
  if (! m_pPressSphCB) return;
  vsnMethod_Shape_scalarMap* pm
    = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);
  if (! pm) return;

  int sel = event.GetInt();
  if (sel < 0) return;

  if (sel == pm -> getPressSphIdx()) return;

  if (pm -> setPressSphIdx(sel)) {
    pm -> chkNotice();
  }
}

/**
 * 全抵抗
 * 圧力データのコンポーネントが選択されたときのコールバック関数
 * @param event イベント
 */
void
vsnMPP_Shape_scalarMap_Phys_shearing::OnPressCompoCB(wxCommandEvent& event)
{
  if (! m_pPressCompoCB) return;
  vsnMethod_Shape_scalarMap* pm
    = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);
  if (! pm) return;

  int sel = event.GetInt();
  if (sel < 0) return;

  if (pm -> getPressCompoIdx() == sel) return;

  if (pm -> setPressCompoIdx(sel))
    pm -> chkNotice();
}

// ----------------------------------------------------------------------------
// 圧力抵抗寄与率
//

/**
 * コンストラクタ
 * @param paent 親のパネル
 * @param pm メソッド
 */
vsnMPP_Shape_scalarMap_Phys_pressDrag::
vsnMPP_Shape_scalarMap_Phys_pressDrag(wxPanel* parent, vsnMethodObj* pm)
: vsnMethodPP(parent, pm)
{
  /**
   * 圧力抵抗寄与率のパネルを作成する
   */
  assert(parent);
  assert(dynamic_cast<vsnMethod_Shape_scalarMap*>(pm));

  wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  {
    // pressure selector
    sizerH = new wxBoxSizer(wxVERTICAL);

    sizerH -> Add(new wxStaticText(this, -1, wxT("Pressure Select")),
		  0, wxALIGN_LEFT|wxALL, 3);
    m_pPressCombo
      = new wxComboBox(this, MPP_Shape_scalarMap_Phys_pressDrag_PressCombo,
		       wxT(""), wxDefaultPosition, wxDefaultSize,
		       0, NULL, wxCB_READONLY);
    sizerH -> Add(m_pPressCombo, 0, wxEXPAND|wxALL, 3);

    topSizer -> Add(sizerH, 0, wxALL, 0);
  }

  {
    // Direction
    sizerH = new wxBoxSizer(wxVERTICAL);

    wxString items[]
      = {wxString(wxT("X")), wxString(wxT("Y")), wxString(wxT("Z"))};
    m_pDirRadio = new wxRadioBox(this,
				 MPP_Shape_scalarMap_Phys_pressDrag_DirRadio,
				 wxT("Direction"),
				 wxDefaultPosition, wxDefaultSize,
				 3, items, 1, wxRA_SPECIFY_ROWS);
    sizerH -> Add(m_pDirRadio, 0, wxEXPAND | wxALL, 3);

    topSizer -> Add(sizerH, 0, wxALL, 0);
  }

  // post process
  SetSizer(topSizer);
  addTo(parent);
  topSizer->Layout();
  (void)update();
}

/**
 * デストラクタ
 */
vsnMPP_Shape_scalarMap_Phys_pressDrag::
~vsnMPP_Shape_scalarMap_Phys_pressDrag(void)
{
}

/**
 * 表示の更新
 */
bool
vsnMPP_Shape_scalarMap_Phys_pressDrag::update(void)
{
  vsnMethod_Shape_scalarMap* pm
    = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);
  if (! pm) return false;
  vsnData_Shape* pdo = dynamic_cast<vsnData_Shape*>(pm->getRefData());
  if ( ! pdo ) return false;

  if (! m_pPressCombo) return false;
  if (! m_pDirRadio) return false;

  m_pPressCombo -> Clear();
  vsnData_Sph* sph = pm -> getSelectedSph();
  if (sph) {
    int dlen = sph -> getDataLen();
    for (int i = 0; i < dlen; i++) {
      char txt[64];
      sprintf(txt, "data%d", i);
      m_pPressCombo -> Append(vsnApp::ConvSysToWx(txt));
    }
    if (dlen == 3) m_pPressCombo -> Append(wxT("VecLen"));
  }
  m_pPressCombo -> SetSelection(pm -> getPressValIdx());

  m_pDirRadio -> SetSelection(pm -> getPressDir());

  return true;
}

/**
 * 圧力値選択のコールバック関数
 * @param event イベント
 */
void
vsnMPP_Shape_scalarMap_Phys_pressDrag::OnPressCombo(wxCommandEvent& event)
{
  if (! m_pPressCombo) return;

  vsnMethod_Shape_scalarMap* pm
    = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);
  if (! pm) return;

  int sel = event.GetInt();
  if (sel < 0) return;

  if (sel == pm -> getPressValIdx()) return;
  pm -> setPressValIdx(sel);

  // 再表示
  //
  pm -> chkNotice();
}

/**
 * マップする方向選択のコールバック関数
 * @param event イベント
 */
void
vsnMPP_Shape_scalarMap_Phys_pressDrag::OnDirRadio(wxCommandEvent& event)
{
  if (! m_pDirRadio) return;
  vsnMethod_Shape_scalarMap* pm
    = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);
  if (! pm) return;

  int dir = m_pDirRadio -> GetSelection();
  if (pm -> getPressDir() == dir) return;

  pm -> setPressDir(m_pDirRadio -> GetSelection());

  // 再表示
  //
  pm -> chkNotice();
  //  update();
}

// ----------------------------------------------------------------------------
// スカラー値
//

/**
 * コンストラクタ
 * @param parent 親のパネル
 * @param pm メソッド
 */
vsnMPP_Shape_scalarMap_Phys_scalar::
vsnMPP_Shape_scalarMap_Phys_scalar(wxPanel* parent, vsnMethodObj* pm)
  : vsnMethodPP(parent, pm)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_Shape_scalarMap*>(pm));

  wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
  wxBoxSizer* sizerH;

  {
    // Scalar selector
    sizerH = new wxBoxSizer(wxVERTICAL);

    sizerH -> Add(new wxStaticText(this, -1, wxT("Scalar Select")),
		  0, wxALIGN_LEFT|wxALL, 3);
    m_pScalarLst = new wxComboBox(this,
				  MPP_Shape_scalarMap_Phys_scalar_ScalarCombo,
				  wxT(""), wxDefaultPosition, wxDefaultSize,
				  0, NULL, wxCB_READONLY);
    sizerH -> Add(m_pScalarLst, 0, wxEXPAND|wxALL, 3);

    topSizer -> Add(sizerH, 0, wxALL, 0);
  }

  {
    sizerH = new wxBoxSizer(wxHORIZONTAL);
    topSizer -> Add(sizerH, 0, wxALL, 0);

    sizerH -> Add(new wxStaticText(this, -1, wxT("Show Contour Line")),
		  0, wxALIGN_LEFT|wxALL, 3);
    m_pDispContourLineCB
      = new wxCheckBox(this, MPP_Shape_scalarMap_Phys_scalar_DispContourLineCB,
		       wxT(""));
    sizerH -> Add(m_pDispContourLineCB, 0, wxEXPAND|wxALL, 3);
  }

  {
    // num lines, bilinear
    sizerH = new wxBoxSizer(wxHORIZONTAL);
    topSizer -> Add(sizerH, 0, wxALL, 3);

    sizerH -> Add(new wxStaticText(this, -1, wxT("#of lines")),
		  0, wxALIGN_LEFT|wxALL, 3);
    m_pNumLinesTxt = new wxTextCtrl(this,
				    MPP_Shape_scalarMap_Phys_scalar_NumLinesTxt,
				    wxT(""), wxDefaultPosition, wxDefaultSize,
				    wxTE_PROCESS_ENTER);
    sizerH->Add(m_pNumLinesTxt, 0, wxALIGN_LEFT|wxALL, 3);

    //m_pBiLinearChk = new wxCheckBox(this,
    //                              PP_Shape_scalarMap_Phys_scalar_BiLinearChk,
    // 				    wxT("bilinear"));
    //sizerH->Add(m_pBiLinearChk, 0, wxALIGN_LEFT|wxALL, 3);
  }

  {
    // range
    sizerH = new wxBoxSizer(wxHORIZONTAL);
    topSizer -> Add(sizerH, 0, wxALL, 3);

    sizerH -> Add(new wxStaticText(this, -1, wxT("range")),
		  0, wxALIGN_LEFT|wxALL, 3);
    m_pMinTxt = new wxTextCtrl(this, MPP_Shape_scalarMap_Phys_scalar_MinTxt,
			       wxT(""), wxDefaultPosition, wxDefaultSize,
			       wxTE_PROCESS_ENTER);
    sizerH -> Add(m_pMinTxt, 0, wxALIGN_LEFT|wxALL, 3);
    sizerH -> Add(new wxStaticText(this, -1, wxT("to")),
		  0, wxALIGN_LEFT|wxALL, 3);
    m_pMaxTxt = new wxTextCtrl(this, MPP_Shape_scalarMap_Phys_scalar_MaxTxt,
			       wxT(""), wxDefaultPosition, wxDefaultSize,
			       wxTE_PROCESS_ENTER);
    sizerH -> Add(m_pMaxTxt, 0, wxALIGN_LEFT|wxALL, 3);
  }


  {
    // use CMap / update minmax mode
    sizerH = new wxBoxSizer(wxHORIZONTAL);
    topSizer->Add(sizerH, 0, wxALL, 3);
    m_pUseCMapChk = new wxCheckBox(this,
				   MPP_Shape_scalarMap_Phys_scalar_UseCMapChk,
				   wxT("use cmap"));
    m_pUseCMapChk->SetValue(TRUE);
    sizerH->Add(m_pUseCMapChk, 0, wxALL, 3);

    //m_pUpdMinMaxChk = new wxCheckBox(this,
    //                             MPP_Shape_scalarMap_Phys_scalar_UpdMinMaxChk,
    // 			           wxT("update minmax"));
    //m_pUpdMinMaxChk->SetValue(TRUE);
    //sizerH->Add(m_pUpdMinMaxChk, 0, wxALL, 3);

    // line width
    sizerH = new wxBoxSizer(wxHORIZONTAL);
    topSizer->Add(sizerH, 0, wxALL, 3);
    sizerH->Add(new wxStaticText(this, -1, wxT("line width")),
		0, wxALIGN_LEFT|wxALL, 3);
    m_pLineWidthTxt
      = new wxTextCtrl(this, MPP_Shape_scalarMap_Phys_scalar_LineWidthTxt,
		       wxT(""), wxDefaultPosition, wxDefaultSize,
		       wxTE_PROCESS_ENTER);
    sizerH->Add(m_pLineWidthTxt, 0, wxALIGN_LEFT|wxALL, 3);

    // anti-alias
    m_pAntiAliasChk
      = new wxCheckBox(this, MPP_Shape_scalarMap_Phys_scalar_AntiAliasChk,
		       wxT("anti-alias line"));
    topSizer->Add(m_pAntiAliasChk, 0, wxALL, 3);
  }


  // post process
  SetSizer(topSizer);
  addTo(parent);
  topSizer->Layout();
  (void)update();
}

/**
 * デストラクタ
 */
vsnMPP_Shape_scalarMap_Phys_scalar::~vsnMPP_Shape_scalarMap_Phys_scalar(void)
{
}

/**
 * 表示の更新
 */
bool
vsnMPP_Shape_scalarMap_Phys_scalar::update(void)
{
  vsnMethod_Shape_scalarMap* pm
    = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);
  if (! pm) return false;
  vsnData_Shape* pdo = dynamic_cast<vsnData_Shape*>(pm->getRefData());
  if ( ! pdo ) return false;

  if (! m_pScalarLst) return false;

  // vector data indices
  m_pScalarLst -> Clear();

  char txt[64];

  vsnData_Sph* sph = pm -> getSelectedSph();
  if (sph) {
    int dlen = sph -> getDataLen();
    for (int i = 0; i < dlen; i++) {
      sprintf(txt, "data%d", i);
      m_pScalarLst -> Append(vsnApp::ConvSysToWx(txt));
    }
    if (dlen == 3) m_pScalarLst -> Append(wxT("VecLen"));
  }
  m_pScalarLst -> SetSelection(pm -> getScalarValIdx());

  // disp contour line
  if (m_pDispContourLineCB) {
    m_pDispContourLineCB -> SetValue(pm -> getDispContourLine());
  }

  // range
  if (m_pMinTxt && m_pMaxTxt) {
    float min, max; pm->getRange(min, max);
    sprintf(txt, "%g", min); m_pMinTxt->SetValue(vsnApp::ConvSysToWx(txt));
    sprintf(txt, "%g", max); m_pMaxTxt->SetValue(vsnApp::ConvSysToWx(txt));

    m_pMinTxt -> Enable(pm -> getDispContourLine());
    m_pMaxTxt -> Enable(pm -> getDispContourLine());
  }

  // num lines
  if (m_pNumLinesTxt) {
    size_t nl = pm -> getNumLines();
    sprintf(txt, "%lu", nl); m_pNumLinesTxt->SetValue(vsnApp::ConvSysToWx(txt));

    m_pNumLinesTxt -> Enable(pm -> getDispContourLine());
  }

  // line width
  if (m_pLineWidthTxt) {
    sprintf(txt, "%g", pm -> getLineWidth());
    m_pLineWidthTxt -> SetValue(vsnApp::ConvSysToWx(txt));

    m_pLineWidthTxt -> Enable(pm -> getDispContourLine());
  }

  // use CMap
  if (m_pUseCMapChk) {
    m_pUseCMapChk->SetValue(pm->getUseCMap());

    m_pUseCMapChk -> Enable(pm -> getDispContourLine());
  }

  // anti-alias
  if (m_pAntiAliasChk) {
    m_pAntiAliasChk->SetValue(pm->getAntiAliasMode());

    m_pAntiAliasChk -> Enable(pm -> getDispContourLine());
  }

  return true;
}

/**
 * AntiAlias のコールバック関数
 * @param event イベント
 */
void 
vsnMPP_Shape_scalarMap_Phys_scalar::OnAntiAliasChk(wxCommandEvent& event)
{
  if ( ! m_pAntiAliasChk ) return;
  vsnMethod_Shape_scalarMap* pm
    = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);
  if ( ! pm ) return;

  bool val = m_pAntiAliasChk->GetValue();
  if ( pm->setAntiAliasMode(val) )
    pm->chkNotice();
}

/**
 * コンターライン本数のコールバック関数
 * @param event イベント
 */
void 
vsnMPP_Shape_scalarMap_Phys_scalar::OnDispContourLineCB(wxCommandEvent& event)
{
  if (! m_pDispContourLineCB) return;
  vsnMethod_Shape_scalarMap* pm
    = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);
  if (! pm) return;

  bool val = m_pDispContourLineCB -> GetValue();
  if (pm -> setDispContourLine(val))
    pm->chkNotice();
}

/**
 * UseCMapのコールバック関数
 * @param event イベント
 */
void 
vsnMPP_Shape_scalarMap_Phys_scalar::OnUseCMapChk(wxCommandEvent& event)
{
  if (! m_pUseCMapChk) return;
  bool val = m_pUseCMapChk -> GetValue();

  vsnMethod_Shape_scalarMap* pm
    = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);
  if (! pm) return;

  if (pm -> setUseCMap(val))
    pm -> chkNotice();
}

/**
 * LineWidth のコールバック関数
 * @param event イベント
 */
void 
vsnMPP_Shape_scalarMap_Phys_scalar::OnLineWidthTxt(wxCommandEvent& event)
{
  if (! m_pLineWidthTxt) return;
  vsnMethod_Shape_scalarMap* pm
    = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);
  if (! pm) return;

  wxString valStr = m_pLineWidthTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  float val = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
  if (val <= 0.f) return;

  if (pm -> setLineWidth(val))
    pm -> chkNotice();
}

/**
 * 最小値・最大値のコールバック関数
 * @param event イベント
 */
void 
vsnMPP_Shape_scalarMap_Phys_scalar::OnMinMaxTxt(wxCommandEvent& event)
{
  if (! m_pMinTxt || ! m_pMaxTxt) return;

  vsnMethod_Shape_scalarMap* pm
    = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);
  if (! pm) return;

  wxString valStr = m_pMinTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  float min = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());
  valStr = m_pMaxTxt->GetValue();
  if ( valStr.IsEmpty() ) return;
  float max = (float)atof(vsnApp::ConvWxToSys(valStr).c_str());

  if (pm -> setContourRange(min, max))
    pm -> chkNotice();
  else {
    pm -> getContourRange(&min, &max);
    char txt[64];
    sprintf(txt, "%g", min); m_pMinTxt->SetValue(vsnApp::ConvSysToWx(txt));
    sprintf(txt, "%g", max); m_pMaxTxt->SetValue(vsnApp::ConvSysToWx(txt));
  }
}

/**
 * 本数のコールバック関数
 * @param event イベント
 */
void 
vsnMPP_Shape_scalarMap_Phys_scalar::OnNumLinesTxt(wxCommandEvent& event)
{
  if ( ! m_pNumLinesTxt ) return;

  vsnMethod_Shape_scalarMap* pm
    = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);
  if (! pm) return;

  wxString valStr = m_pNumLinesTxt -> GetValue();
  if (valStr.IsEmpty()) return;
  int val = atoi(vsnApp::ConvWxToSys(valStr).c_str());
  if (val < 1) {
    char txt[64]; sprintf(txt, "%lu", pm -> getNumLines());
    m_pNumLinesTxt -> SetValue(vsnApp::ConvSysToWx(txt));
    return;
  }

  if (pm -> setNumLines((size_t)val))
    pm -> chkNotice();
}

// ----------------------------------------------------------------------------

/**
 * コンストラクタ
 * @param paent 親のパネル
 * @param pm メソッド
 */
vsnMPP_Shape_scalarMap::vsnMPP_Shape_scalarMap(wxPanel* parent,
					       vsnMethodObj* pm)
: vsnMethodPP(parent, pm)
{
  assert(parent);
  assert(dynamic_cast<vsnMethod_Shape_scalarMap*>(pm));

  m_parentPanel = parent;
  m_mpp = NULL;

  wxBoxSizer* topSizer = new wxBoxSizer(wxVERTICAL);
  assert(topSizer);

  wxBoxSizer* sizerH;

  { // Shape
    sizerH = new wxBoxSizer(wxHORIZONTAL);
    assert(sizerH);

    sizerH -> Add(new wxStaticText(this, -1, wxT("SPH")),
		  0, wxALIGN_LEFT | wxALL, 3);

    m_pSphLst = new wxComboBox(this, MPP_Shape_scalarMap_SphLst,
			       wxT(""), wxDefaultPosition, wxDefaultSize,
			       0, NULL, wxCB_READONLY);
    assert(m_pSphLst);

    sizerH -> Add(m_pSphLst, 0, wxEXPAND | wxALL, 3);

    topSizer -> Add(sizerH, 0, wxALL, 0);
  }

  { // RenderngMode
    sizerH = new wxBoxSizer(wxHORIZONTAL);
    assert(sizerH);

    wxString items[] = { wxString(wxT("Const")), wxString(wxT("Linear")) };
    m_pRenderingModeRadio
      = new wxRadioBox(this, MPP_Shape_scalarMap_RenderingModeRadio,
		       wxT("RenderingMode"),
		       wxDefaultPosition, wxDefaultSize,
		       2, items, 1, wxRA_SPECIFY_ROWS);
    assert(m_pRenderingModeRadio);

    sizerH -> Add(m_pRenderingModeRadio, 0, wxEXPAND | wxALL, 3);

    //m_pDivideChk = new wxCheckBox(this, MPP_Shape_scalarMap_DivideChk,
    //				    wxT("divide"));
    //assert(m_pDivideChk);
    //sizerH -> Add(m_pDivideChk, 0, wxEXPAND | wxALL, 3);

    topSizer -> Add(sizerH, 0, wxALL, 0);
  }

  { // Interpolate
    sizerH = new wxBoxSizer(wxVERTICAL);
    assert(sizerH);

    wxString items[] = {wxString(wxT("Zero")), wxString(wxT("trilinear")),
			wxString(wxT("ext1")), wxString(wxT("ext2"))};
    m_pInterpolateRadio = new wxRadioBox(this,
					 MPP_Shape_scalarMap_InterpolateRadio,
					 wxT("Interpolate"), 
					 wxDefaultPosition, wxDefaultSize,
					 4, items, 1, wxRA_SPECIFY_ROWS);
    assert(m_pInterpolateRadio);

    sizerH -> Add(m_pInterpolateRadio, 0, wxEXPAND | wxALL, 3);


    m_pExUseTrilinearChk = new wxCheckBox(this,
					  MPP_Shape_scalarMap_ExUseTrilinearChk,
					  wxT("UseTrilinear"));
    sizerH -> Add(m_pExUseTrilinearChk, wxEXPAND | wxALL, 3);

    topSizer -> Add(sizerH, 0, wxALL, 0);
  }

  { // NormalDirection
    sizerH = new wxBoxSizer(wxHORIZONTAL);
    assert(sizerH);

    wxString items[] = { wxString(wxT("Normal")), wxString(wxT("Reverse")) };
    m_pNormalDirectionRadio
      = new wxRadioBox(this, MPP_Shape_scalarMap_NormalDirectionRadio,
		       wxT("NormalDirection"),
		       wxDefaultPosition, wxDefaultSize,
		       2, items, 1, wxRA_SPECIFY_ROWS);
    assert(m_pNormalDirectionRadio);

    sizerH -> Add(m_pNormalDirectionRadio, 0, wxEXPAND | wxALL, 3);

    topSizer -> Add(sizerH, 0, wxALL, 0);

    //m_pNormalDirectionRadio -> Disable();
  }

  { // Phys
    sizerH = new wxBoxSizer(wxHORIZONTAL);
    assert(sizerH);

    sizerH -> Add(new wxStaticText(this, -1, wxT("Phys")),
		  0, wxALIGN_LEFT | wxALL, 3);

    wxString items[] = {
      wxString(wxT("Scalar")),
      wxString(wxT("Pressure Drag")),
      wxString(wxT("Shearing Stress")),
      wxString(wxT("Frictional Resistance")),
      wxString(wxT("Total Resistance"))
    };

    m_pPhysLst = new wxComboBox(this, MPP_Shape_scalarMap_PhysLst,
				wxT(""), wxDefaultPosition, wxDefaultSize,
				5, items, wxCB_READONLY);
    assert(m_pPhysLst);

    sizerH -> Add(m_pPhysLst, 0, wxEXPAND | wxALL, 3);

    m_pPhysLst -> SetSelection(0);

    topSizer -> Add(sizerH, 0, wxALL, 0);
  }

  {
    sizerH = new wxBoxSizer(wxHORIZONTAL);
    assert(sizerH);

    // update minmax mode
    m_pUpdMinMaxChk = new wxCheckBox(this, MPP_Shape_scalarMap_UpdMinMaxChk,
				     wxT("update minmax"));
    assert(m_pUpdMinMaxChk);
    m_pUpdMinMaxChk->SetValue(TRUE);
    sizerH -> Add(m_pUpdMinMaxChk, 0, wxEXPAND | wxALL, 3);

    // OutputFile
    m_pOutputFileBtn = new wxButton(this, MPP_Shape_scalarMap_OutputFileBtn,
				    wxT("OutputFile..."));
    assert(m_pOutputFileBtn);
    sizerH -> Add(m_pOutputFileBtn, 0, wxEXPAND | wxALL, 3);

    topSizer -> Add(sizerH, 0, wxALL, 0);
  }

//   {
//     // ファイル出力
//     sizerH = new wxBoxSizer(wxHORIZONTAL);
//     assert(sizerH);

//     m_pOutputFileBtn = new wxButton(this, MPP_Shape_scalarMap_OutputFileBtn,
//                                     wxT("OutputFile"));
//     assert(m_pOutputFileBtn);
//     sizerH -> Add(m_pOutputFileBtn, 0, wxEXPAND | wxALL, 3);
//     topSizer -> Add(sizerH, 0, wxALL, 0);
//   }

  // Line
  topSizer -> Add(new wxStaticLine(this, -1, wxDefaultPosition, wxSize(3,3),
				   wxHORIZONTAL), 0, wxEXPAND|wxALL, 0);

  // phys param area
  //  m_pPhysParamArea = new wxScrolledWindow(this,
  //                                      MPP_Shape_scalarMap_PhysParamArea,
  //					  wxDefaultPosition, wxDefaultSize);
  //					  wxScrolledWindowStyle);
  //  m_pPhysParamArea -> SetScrollbars(1, 1, 100, 600);
  m_pPhysParamArea = new wxPanel(this, MPP_Shape_scalarMap_PhysParamArea);
  topSizer -> Add(m_pPhysParamArea, 1, wxEXPAND | wxALL, 3);
  wxBoxSizer* ppaSizer = new wxBoxSizer(wxVERTICAL);
  m_pPhysParamArea -> SetSizer(ppaSizer);

  // post process
  SetSizer(topSizer);
  addTo(parent);
  topSizer->Layout();
  (void)update();
}

/**
 * デストラクタ
 */
vsnMPP_Shape_scalarMap::~vsnMPP_Shape_scalarMap()
{
  if (m_mpp) { m_mpp -> Destroy(); m_mpp = NULL; }
}

// void
// vsnMPP_Shape_scalarMap::OnDivideChk(wxCommandEvent& event)
// {
//   if (! m_pDivideChk) return;
//   bool val = m_pDivideChk -> GetValue();

//   vsnMethod_Shape_scalarMap* pm
//            = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);
//   if (! pm) return;

//   if (pm -> setDivideMode(val))
//     pm -> chkNotice();
// }

/**
 * 外挿で trilinear がチェックされたときのコールバック関数
 * @param event イベント
 */
void
vsnMPP_Shape_scalarMap::OnExUseTrilinearChk(wxCommandEvent& event)
{
  if (! m_pExUseTrilinearChk) return;
  bool val = m_pExUseTrilinearChk -> GetValue();

  vsnMethod_Shape_scalarMap* pm
    = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);
  if (! pm) return;

  if (pm -> setExUseTrilinear(val))
    pm -> chkNotice();
}

/**
 * 法線方向のコールバック関数
 * @param event イベント
 */
void
vsnMPP_Shape_scalarMap::OnNormalDirectionRadio(wxCommandEvent& event)
{
  if (! m_pNormalDirectionRadio) return;
  int val = m_pNormalDirectionRadio -> GetSelection();

  vsnMethod_Shape_scalarMap* pm
    = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);
  if (! pm) return;

  if (pm -> setNormalDirection(val))
    pm -> chkNotice();
}

/**
 * UpdateMinMax のコールバック関数
 * @param event イベント
 */
void
vsnMPP_Shape_scalarMap::OnUpdMinMaxChk(wxCommandEvent& event)
{
  if (! m_pUpdMinMaxChk) return;
  bool val = m_pUpdMinMaxChk -> GetValue();

  vsnMethod_Shape_scalarMap* pm
    = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);
  if (! pm) return;

  if (pm -> setUpdateMinMaxMode(val))
    pm -> chkNotice();
}

//#include <boost/format.hpp>

/**
 * 操作パネルのアップデート
 * vsnMethod_Shape_scalarMap の値により画面のアップデートを行う
 */
bool
vsnMPP_Shape_scalarMap::update(void)
{
  //   // Get Method
  vsnMethod_Shape_scalarMap* pm
    = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);
  if (! pm) return false;

  if (! m_pSphLst) return false;
  if (! m_pRenderingModeRadio) return false;
  if (! m_pPhysLst) return false;
  if (! m_pInterpolateRadio) return false;
  if (! m_pUpdMinMaxChk) return false;
  if (! m_pNormalDirectionRadio) return false;

  pm -> loadSph();

  m_pSphLst -> Clear();
  m_pSphLst -> Append(wxT("None"));
  for (size_t i = 0; i < pm -> sphSize(); ++i) {
    m_pSphLst -> Append(vsnApp::ConvSysToWx(pm -> getSph(i) -> getName()));
  }
  m_pSphLst -> SetSelection(pm -> getSphIdx());

  // RenderingMode
  m_pRenderingModeRadio -> SetSelection(pm -> getRenderingMode());

  // Interpolate
  m_pInterpolateRadio -> SetSelection(pm -> getInterpolateMode());

  // 表示物理量
  //
  m_pPhysLst -> SetSelection(pm -> getPhysIdx());

  m_pUpdMinMaxChk -> SetValue(pm -> getUpdateMinMaxMode());

  m_pNormalDirectionRadio -> SetSelection(pm -> getNormalDirection());

//   if (m_pDivideChk) {
//     m_pDivideChk -> SetValue(pm -> getDivideMode());
//   }

  if (m_pExUseTrilinearChk) {
    m_pExUseTrilinearChk -> SetValue(pm -> getExUseTrilinear());
    if (pm -> getInterpolateMode() == 0 || pm -> getInterpolateMode() == 1) {
      m_pExUseTrilinearChk -> Disable();
    } else {
      m_pExUseTrilinearChk -> Enable();
    }
  }

  changePhysPanel();

  return true;
}

// ------------------- event handler ------------------------

/**
 * スカラー値選択のコールバック関数
 * @param event イベント
 */
void
vsnMPP_Shape_scalarMap_Phys_scalar::OnScalarLst(wxCommandEvent& event)
{
  if (! m_pScalarLst) return;
  vsnMethod_Shape_scalarMap* pm
    = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);
  if (! pm) return;

  int sel = event.GetInt();
  
  //  if (sel == pm -> getScalarValIdx()) return;
  if (pm -> setScalarValIdx(sel))
    pm -> chkNotice();
}

/**
 * OuptutFileボタンのコールバック関数
 * @param event イベント
 * @since 2.2.1
 */
void
vsnMPP_Shape_scalarMap::OnOutputFileBtn(wxCommandEvent& event)
{
  vsnMethod_Shape_scalarMap* pm
    = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);
  if (! pm) return;

  // 出力するファイルの選択
  //
  wxFileDialog dlg(this, wxT("select STLext file to save"),
		   wxT(""), wxT(""),
		   wxT("STLext file(*.stld)|*.stld|(*)|*"),
		   wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
  if (dlg.ShowModal() != wxID_OK) return;

  // パス名の取得
  //
  std::string outPath = vsnApp::ConvWxToSys(dlg.GetPath());
  if (outPath.empty()) return;
  outPath = vsnPath_normalize(outPath);

  try {
    pm -> SaveStlExt(outPath);
  } catch (std::runtime_error& e) {
    wxMessageDialog dlg(NULL, vsnApp::ConvSysToWx(e.what()),
			wxT("Save STLext File"), wxOK | wxICON_ERROR);
    dlg.ShowModal();
  }
}

/**
 * SPH選択のコールバック関数
 * @param event イベント
 */
void 
vsnMPP_Shape_scalarMap::OnSphLst(wxCommandEvent& event)
{
  if (! m_pSphLst) return;

  vsnMethod_Shape_scalarMap* pm
    = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);
  if (! pm) return;

  int sel = event.GetInt();
  if (sel < 0) return;

  //
  // 前と同じであれば何もしない
  //
  if (sel == pm -> getSphIdx()) return;
  if (pm -> setSphIdx(sel))
    pm -> chkNotice();
}

/**
 * 内挿方法のコールバック関数
 * @param event イベント
 */
void
vsnMPP_Shape_scalarMap::OnInterpolateRadio(wxCommandEvent& event)
{
  if (! m_pInterpolateRadio) return;
  vsnMethod_Shape_scalarMap* pm
    = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);
  if (! pm) return;

  int idx = m_pInterpolateRadio -> GetSelection();
  pm -> setInterpolateMode(idx);

  pm -> chkNotice();
}

/**
 * レンダリング方法のコールバック関数
 * @param event イベント
 */
void
vsnMPP_Shape_scalarMap::OnRenderingModeRadio(wxCommandEvent& event)
{
  if (! m_pRenderingModeRadio) return;
  vsnMethod_Shape_scalarMap* pm
    = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);

  pm -> setRenderingMode(m_pRenderingModeRadio -> GetSelection());

  // 再表示
  //
  pm -> chkNotice();
  //  update();
}

/**
 * 表示物理量の変更
 */
bool
vsnMPP_Shape_scalarMap::changePhysPanel(void)
{
  vsnMethod_Shape_scalarMap* pm
    = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);
  if (! pm) return false;

  if (! m_pPhysParamArea) return false;
  //  m_pPhysParamArea -> Scroll(0, 0);

  /**
   * パネルの変更を行う。
   * 同じパネルを使用するのであえば、新たなパネルは作成しない。
   */
  int physIdx = pm -> getPhysIdx();
  bool done = true;
  if (m_mpp) {
    if (physIdx == 0 &&
	dynamic_cast<vsnMPP_Shape_scalarMap_Phys_scalar*>(m_mpp)) 
      done = false;
    else if (physIdx == 1 &&
	     dynamic_cast<vsnMPP_Shape_scalarMap_Phys_pressDrag*>(m_mpp)) 
      done = false;
    else if (physIdx == 2 &&
	     dynamic_cast<vsnMPP_Shape_scalarMap_Phys_shearing*>(m_mpp)) 
      done = false;
    else if (physIdx == 3 &&
	     dynamic_cast<vsnMPP_Shape_scalarMap_Phys_shearing*>(m_mpp)) 
      done = false;
    else if (physIdx == 4 &&
	     dynamic_cast<vsnMPP_Shape_scalarMap_Phys_shearing*>(m_mpp)) {
      done = false;
      m_pPhysParamArea -> GetSizer() -> Layout(); // need on Windows
      GetSizer() -> Layout(); // need on GTK
      if (m_parentPanel) m_parentPanel -> FitInside();
    }
  }

  if (done) {
    if (m_mpp) { m_mpp -> Destroy(); m_mpp = NULL; }

    // パネルを変更する
    //
    m_mpp = pm -> getPhysPanel(m_pPhysParamArea);
    if (! m_mpp) return false;

    // re-layout
    m_pPhysParamArea -> GetSizer() -> Layout(); // need on Windows
    GetSizer() -> Layout(); // need on GTK
    if (m_parentPanel) m_parentPanel -> FitInside();
  }

  return true;
}

/**
 * 物理量選択のコールバック関数
 * @param event イベント
 */
void 
vsnMPP_Shape_scalarMap::OnPhysLst(wxCommandEvent& event)
{
  if (! m_pPhysLst) return;
  vsnMethod_Shape_scalarMap* pm
    = dynamic_cast<vsnMethod_Shape_scalarMap*>(p_method);
  if (! pm) return;

  int sel = event.GetInt();
  if (sel == pm -> getPhysIdx()) return;

  if (pm -> setPhysIdx(sel))
    pm -> chkNotice();
}

// ----------------------------------------------------------------

/**
 * コンストラクタ
 */
vsnMethod_Shape_scalarMap::vsnMethod_Shape_scalarMap(const std::string& name)
  : vsnMethodObj(name),
    m_trias(NULL), m_sphIdx(0), m_renderingMode(0), m_interpolateMode(0),
    m_physIdx(0), m_updateMinMax(true), 
    m_normalDirection(0), m_exUseTrilinear(false), m_scalarValIdx(0)
    //m_normalDirection(0), m_divideMode(false), m_exUseTrilinear(false),
    //m_scalarValIdx(0)
{
  m_minmax[0] = 0;
  m_minmax[1] = 1;

  // スカラー
  //
  m_dispContourLine = false;
  m_numLines = 5;
  m_numValidLines = 0;
  m_lineWidth = 1.0;
  m_useCMap = true;
  m_antiAlias = false;

  // 圧力寄与率
  //
  m_pressValIdx = 0;
  m_pressDir = 0;

  // 剪断応力の変数
  //
  m_shearingCoeffVal = 1.0;
  m_shearingStencil = 0.1;
  m_shearingMapCompo = 0;
  m_shearingSurfVeloc[0] = m_shearingSurfVeloc[1] = m_shearingSurfVeloc[2] =0.0;

  // 全抵抗
  //
  m_totalResistPressSphIdx = 2;
  m_totalResistPressCompoIdx = 0;

  m_numFaces = 0;
  m_faceGravity = NULL;
  m_faceArea = NULL;
  m_faceProjectionArea = NULL;
}

/**
 * デストラクタ
 */
vsnMethod_Shape_scalarMap::~vsnMethod_Shape_scalarMap(void)
{
  if (m_trias) delete m_trias;
  if (m_faceGravity) delete[] m_faceGravity;
  if (m_faceArea) delete[] m_faceArea;
  if (m_faceProjectionArea) {
    for (int i = 0; i < 3; ++i) delete[] m_faceProjectionArea[i];
    delete[] m_faceProjectionArea;
  }
}


/**
 * スカラー値の設定
 * m_allValues に値を設定する
 * @exception std::runtime_error エラー
 */
void
vsnMethod_Shape_scalarMap::setScalarValue(void)
{

  if (! m_trias) throw std::runtime_error("Can't Get Triangles");

  const size_t numVerts   = m_trias -> getNumVerts();
  if (numVerts < 3) throw std::runtime_error("No Triangle");

  vsnData_Shape* pdo = dynamic_cast<vsnData_Shape*>(getRefData());
  if (! pdo) throw std::runtime_error("Can't Get RefData");

  if (m_sphIdx == 0) return;
  vsnData_Sph* sph = m_sphList[m_sphIdx - 1];
  if (! sph) throw std::runtime_error("No SPH");

  int scalarValIdx = getScalarValIdx();

  CES::Vec3<int> vecDataIdx(0, 1, 2);
  std::vector<bool> didx;

  /**
   * ベクトル値を取得するのであれば、
   * didx の size は３になる。取得するコンポーネントに１を設定する。
   * VecLen の値の取得時は、全て１に設定する。
   * スカラー値の取得の時は、didx の size を１にする。
   */
  bool isVecLen = false;
  size_t vecCompo = scalarValIdx;
  if (sph -> getDataLen() == 3) {
    if (scalarValIdx == 0) { // Vector Component X
      didx.push_back(true); didx.push_back(false); didx.push_back(false);
    } else if (scalarValIdx == 1) { // Vector Component Y
      didx.push_back(false); didx.push_back(true); didx.push_back(false);
    } else if (scalarValIdx == 2) { // Vector Component Z
      didx.push_back(false); didx.push_back(false); didx.push_back(true);
    } else if (scalarValIdx == 3) { // VecLen
      didx.push_back(true); didx.push_back(true); didx.push_back(true);
      isVecLen = true;
    }

    // ファイル出力のため全ての値を取得する
    //
    didx[0] = didx[1] = didx[2] = 1;
  } else {
    if (scalarValIdx == 0) { // Scalar
      didx.push_back(true);
    }
  }

  /**
   * Trilinear での取得のために、vsnGridUtility の準備をしておく
   */
  int m_requestedStp = 0;
  vsnGridUtilSv _gus;
  if ( ! _gus.setup(sph -> getDims(), sph -> getDataLen(),
		    (float*)sph -> getCoord(m_requestedStp),
		    (float*)sph -> getData(m_requestedStp)) ) {
    return;
  }

  /**
   * mode が AT_PER_FACE の時は、面の塗りつぶし。
   * AT_PER_VERTEX の時は、頂点にマッピング。
   * コンターライン表示では、AT_PER_VERTEX にする。
   */
  VFR::AppearType mode = (m_renderingMode == 0) ? AT_PER_FACE : AT_PER_VERTEX;

  if (m_dispContourLine) mode = AT_PER_VERTEX;

  m_trias -> setColorMode(mode);
  m_trias -> setNormalMode(mode);
  m_trias -> generateNormals();

  // 頂点
  //
  VFR::vector3* verts = m_trias -> getVerts();
  if (! verts) throw std::runtime_error("Can't Get Verts");

  VFR::vector3* faceNormals = m_trias -> getNormals();

  int numPoints        = (mode == AT_PER_VERTEX) ? numVerts : m_numFaces;
  VFR::vector3* points = (mode == AT_PER_VERTEX) ? verts : m_faceGravity;

  // 法線ベクトルの向き
  //
  double normalDirection = (getNormalDirection() == 0) ? 1.0 : -1.0;

  std::vector<float> allValues(numPoints);
  std::vector<CES::Vec3<float> > allValues3;
  if (sph -> getDataLen() == 3) allValues3.resize(numPoints);
  else allValues3.clear();

  for (int i = 0; i < numPoints; ++i) {
    float x = points[i][0];
    float y = points[i][1];
    float z = points[i][2];

    CES::Vec3<float> n;
    if (mode == AT_PER_FACE) {
      n[0] = faceNormals[i][0] * normalDirection;
      n[1] = faceNormals[i][1] * normalDirection;
      n[2] = faceNormals[i][2] * normalDirection;
    } else {
      Vertex* v = m_vm.Find(x, y, z);
      n[0] = v -> m_normal[0] * normalDirection;
      n[1] = v -> m_normal[1] * normalDirection;
      n[2] = v -> m_normal[2] * normalDirection;
    }
    n.UnitVec();

    float val = 0;
    if (getInterpolateMode() == 1) {
      // trilinear
      DVec3 xp;
      DVec4 x_i;
      xp[0] = x;
      xp[1] = y;
      xp[2] = z;
      CES::Vec3<float> vv;
      if (didx.size() == 3) {
	if (_gus.ConvXtoI(xp, x_i) >= 0 &&
	    _gus.InterpolateData(x_i, vecDataIdx, vv.m_v)) {

	  val = (isVecLen) ? vv.Length() : vv[vecCompo];
	  allValues3[i] = vv;

#ifdef OLD
	  if (didx[0] && didx[1] && didx[2]) val = vv.Length();
	  else val = (didx[0]) ? vv[0] : (didx[1]) ? vv[1] : vv[2];
#endif // OLD
	}
      } else {
	float dval;
	if (_gus.ConvXtoI(xp, x_i) >= 0 &&
	    _gus.InterpolateData(x_i, 0, dval)) {
	  val = dval;
	}	
      }
    } else {
      // 形状頂点の値を取得
      //
      CES::Vec3<float> pos(x, y, z);
      std::vector<float> tmp
	= sph -> interpolateData(pos, didx, getInterpolateMode(),
				 n, getExUseTrilinear());
      if (tmp.size() != didx.size())
	throw std::runtime_error("interpolate data failed");
    
      //      float val = 0;
      if (didx.size() == 1) { // Scalar
	val = tmp[0];
      } else if (didx.size() == 3) {
	val = (isVecLen) ? \
	  sqrt(tmp[0] * tmp[0] + tmp[1] * tmp[1] + tmp[2] * tmp[2]) : \
	  tmp[vecCompo];

	allValues3[i][0] = tmp[0];
	allValues3[i][1] = tmp[1];
	allValues3[i][2] = tmp[2];

#ifdef OLD
	if (didx[0] && didx[1] && didx[2]) { // VecLen
	  val = sqrt(tmp[0] * tmp[0] + tmp[1] * tmp[1] + tmp[2] * tmp[2]);
	} else { // Vector Compoment
	  val = (didx[0]) ? tmp[0] : \
	    (didx[1]) ? tmp[1] : (didx[2]) ? tmp[2] : 0.0;
	}
#endif // OLD
      }
    }

    allValues[i] = val;
  }

  m_trias -> alcColors(allValues.size());
  m_trias -> setColorMode(mode);

  /**
   * コンターライン表示の時は、vfrLines を作成し、子供に加える
   */
  // コンターライン表示
  //
  if (m_dispContourLine) {
    int n = getNumContourLineChildren();

    if (m_numLines > n) {
      for (int i = n; i < m_numLines; ++i) {
	vfrLines* plns = new vfrLines("contour_line");
	if (! plns) {
	  throw std::runtime_error(getMethodType() + string("[") + getName() +
				   string("]: memory allocation failed"));
	}
	addChild(plns);
      }
    }

    std::vector<vfrLines*> linesV;

    m_numValidLines = 0;
    n = getNumChildren();
    for (int i = 0; i < n; i++ ) {
      vfrLines* p = dynamic_cast<vfrLines*>(getChild(i));
      if (! p) continue;
      p -> alcMaterial();
      p -> alcVerts(0);
      p -> notice();
      linesV.push_back(p);
    } // end of for(i)

    
//     getPrivateMaterial() -> setRenderMode(RT_NONE);
//     if (! m_show) { return true; }

    vector3 lp1, lp2;
    float dv = (m_numLines < 2) ? 0.f : \
      (m_minmax[1] - m_minmax[0]) / (float)(m_numLines - 1);
    float val;
    int k;
    int maxLineSegs = m_numFaces;
    for (val = m_minmax[0], k = 0; k < m_numLines; val += dv, k++) {
      //      vfrLines* plns = dynamic_cast<vfrLines*>(getChild(k));
      vfrLines* plns = linesV[k];
      if (! plns) continue;

      plns -> alcPools(maxLineSegs, -1, -1, -1);

      int idx = 0;
      for (int face = 0; face < m_numFaces; ++face) {
	VFR::vector3 p0;
	p0[0] = verts[face * 3 + 0][0];
	p0[1] = verts[face * 3 + 0][1];
	p0[2] = verts[face * 3 + 0][2];
	VFR::vector3 p1;
	p1[0] = verts[face * 3 + 1][0];
	p1[1] = verts[face * 3 + 1][1];
	p1[2] = verts[face * 3 + 1][2];
	VFR::vector3 p2;
	p2[0] = verts[face * 3 + 2][0];
	p2[1] = verts[face * 3 + 2][1];
	p2[2] = verts[face * 3 + 2][2];

	float val0 = allValues[face * 3 + 0];
	float val1 = allValues[face * 3 + 1];
	float val2 = allValues[face * 3 + 2];
	
	if (calcLineSegment(p0, p1, p2, val0, val1, val2, val, lp1, lp2)) {
	  plns -> alcVerts(idx + 2);
	  plns -> setVert(idx++, lp1, FALSE);
	  plns -> setVert(idx++, lp2, FALSE);
	}
      }

      if (m_useCMap)
	plns -> setColor3(0, &m_lut.lutEntry[m_lut.getValIdx(val) * 4]);
      else 
	plns -> setColor4(0, m_colour);

      // 先の幅を設定
      //
      plns -> getPrivateMaterial() -> setLineWidth(getLineWidth());
      //      plns -> setTransparency(m_antiAlias);
    }

    // anti alias line の設定
    //
    setAntiAliasMode(getAntiAliasMode());
  } else {
    deleteContourLines();
  }

  /**
   * カラーマップを利用して、値から色に変換し、定義点にマッピングする。
   */
  for (size_t i = 0; i < allValues.size(); ++i) {
    // 形状頂点の値を取得
    //
    int c = m_lut.getValIdx(allValues[i]);
    VFR::vector4 cv;
    memcpy(cv, &m_lut.lutEntry[c * 4], sizeof(float)*4);
    m_trias -> setColor3(i, cv);
  }

  // ファイルへの出力のため値を保持しておく
  //
  m_allValues = allValues;
  m_allValues3 = allValues3;
}

/**
 * コンターラインの線分を作成する
 * @param p0 三角形の頂点座標１
 * @param p1 三角形の頂点座標２
 * @param p2 三角形の頂点座標3
 * @param val0 p0 の値
 * @param val1 p1 の値
 * @param val2 p2 の値
 * @param val コンターラインの値
 * @param lp1 コンターラインセグメントの始点の座標値
 * @param lp2 コンターラインセグメントの終点の座標値
 * @return セグメントの作成に失敗した
 */
// STATIC
bool vsnMethod_Shape_scalarMap::
calcLineSegment(const vector3 p0, const vector3 p1, const vector3 p2,
		const float val0, const float val1, const float val2,
		const float val, vector3 lp1, vector3 lp2) {
  register int i, j, idx;
  char vFlg[3] = {0, 0, 0};

  j = 0;
  if ( val0 > val ) {vFlg[0] = 1; j++;}
  if ( val1 > val ) {vFlg[1] = 1; j++;}
  if ( val2 > val ) {vFlg[2] = 1; j++;}
  if ( j == 0 || j == 3 ) return false; // contour line not crossed

  CES::Vec3<float> pos[3] = {CES::Vec3<float>(p0),
			     CES::Vec3<float>(p1), CES::Vec3<float>(p2)};
  CES::Vec3<float> lp;
  float vals[3] = {val0, val1, val2};
  float t;
  int top = -1; int bot[2] = {-1, -1};
  if ( j == 1 ) {
    for ( idx = 0, i = 0; i < 3; i++ )
      if ( vFlg[i] == 1 ) top = i;
      else bot[idx++] = i;
  } else {
    for ( idx = 0, i = 0; i < 3; i++ )
      if ( vFlg[i] == 0 ) top = i;
      else bot[idx++] = i;
  }

  // lp1: top---bot[0]
  if ( vals[top] == vals[bot[0]] ) return false; // something wrong
  t = (val - vals[top]) / (vals[bot[0]] - vals[top]);
  lp = pos[top] + (pos[bot[0]] - pos[top]) * t;
  lp1[0] = lp[0]; lp1[1] = lp[1]; lp1[2] = lp[2];

  // lp2 : top---bot[1]
  if ( vals[top] == vals[bot[1]] ) return false; // something wrong
  t = (val - vals[top]) / (vals[bot[1]] - vals[top]);
  lp = pos[top] + (pos[bot[1]] - pos[top]) * t;
  lp2[0] = lp[0]; lp2[1] = lp[1]; lp2[2] = lp[2];

  return true;
}

/**
 * コンターラインの子供の数の取得
 * @return コンターラインの子供の数
 */
size_t
vsnMethod_Shape_scalarMap::getNumContourLineChildren(void)
{
  size_t result = 0;
  int n = getNumChildren();
  for (int i = 0; i < n; ++i) {
    vfrLines* line = dynamic_cast<vfrLines*>(getChild(i));
    if (line) result++;
  }
  return result;
}

/**
 * 三角形の面積を求める
 * @param p1 頂点座標
 * @param p2 頂点座標
 * @param p3 頂点座標
 * @return 三角形の面積
 */
static double
calcTriArea(const CES::Vec3<float>& p1,
	    const CES::Vec3<float>& p2, const CES::Vec3<float>& p3)
{
  CES::Vec3<float> v1 = p2 - p1;
  CES::Vec3<float> v2 = p3 - p1;

  // 外積
  //
  CES::Vec3<float> g = v1 ^ v2;

  // 面積
  //
  float area = sqrt(g[0] * g[0] + g[1] * g[1] + g[2] * g[2]) / 2.0;

  return area;
}

/**
 * 圧力寄与率データの取得。
 * m_minmax と m_allValues の値を更新する。
 * @retval true 失敗
 * @retval false 失敗
 */
bool
vsnMethod_Shape_scalarMap::getPressDragValue(void)
{
  if (! m_trias) return false;

  const size_t numVerts   = m_trias -> getNumVerts();
  if (numVerts < 3) return false;

  if (m_sphIdx == 0) return false;
  vsnData_Sph* sph = m_sphList[m_sphIdx - 1];
  if (! sph) return false;

  int scalarValIdx = getPressValIdx();

  bool isVecLen = false;
  size_t vecCompo = scalarValIdx;

  std::vector<bool> didx;
  if (sph -> getDataLen() == 3) {
    if (scalarValIdx == 0) { // Vector Component X
      didx.push_back(true); didx.push_back(false); didx.push_back(false);
    } else if (scalarValIdx == 1) { // Vector Component Y
      didx.push_back(false); didx.push_back(true); didx.push_back(false);
    } else if (scalarValIdx == 2) { // Vector Component Z
      didx.push_back(false); didx.push_back(false); didx.push_back(true);
    } else if (scalarValIdx == 3) { // VecLen
      didx.push_back(true); didx.push_back(true); didx.push_back(true);
      isVecLen = true;
    }

    // ファイルに保存するために全ての値を取得する
    //
    didx[0] = didx[1] = didx[2] = 1;
  } else {
    if (scalarValIdx == 0) { // Scalar
      didx.push_back(true);
    }
  }

  VFR::AppearType mode = (m_renderingMode == 0) ? AT_PER_FACE : AT_PER_VERTEX;

  // 頂点
  //
  VFR::vector3* verts = m_trias -> getVerts();
  if (! verts) return false;

  // 投影の向き
  //
  const int dir = getPressDir();

  // 面の数
  //
  int numFaces = numVerts / 3;

  // 面の法線ベクトル
  //
  VFR::vector3* faceNormals = m_trias -> getNormals();

  // レンダリングする点の数
  //
  int numPoints        = (mode == AT_PER_VERTEX) ? numVerts : numFaces;
  // レンダリングする点
  //
  VFR::vector3* points = (mode == AT_PER_VERTEX) ? verts : m_faceGravity;

  CES::Vec3<int> vecDataIdx(0, 1, 2);
  int m_requestedStp = 0;
  vsnGridUtilSv _gus;
  if ( ! _gus.setup(sph -> getDims(), sph -> getDataLen(),
		    (float*)sph -> getCoord(m_requestedStp),
		    (float*)sph -> getData(m_requestedStp)) ) {
    return false;
  }

  // 法線ベクトルの向き
  //
  float normalDirection = (getNormalDirection() == 0) ? 1.0 : -1.0;

  m_allValues.clear();
  m_allValues.resize(numPoints);
  std::vector<CES::Vec3<float> > allValues3;
  if (sph -> getDataLen() == 3) allValues3.resize(numPoints);
  else allValues3.clear();

  for (int i = 0; i < numPoints; ++i) {
    // 形状頂点の値を取得
    //
    CES::Vec3<float> pos(points[i][0], points[i][1], points[i][2]);

    // 法線ベクトル
    //
    CES::Vec3<float> n;
    // 投影面積
    //
    double projectionArea = 0.0;
    if (mode == AT_PER_FACE) {
      n[0] = faceNormals[i][0] * normalDirection;
      n[1] = faceNormals[i][1] * normalDirection;
      n[2] = faceNormals[i][2] * normalDirection;

      projectionArea = m_faceProjectionArea[dir][i];
    } else {
      Vertex* v = m_vm.Find(pos);
      n[0] = v -> m_normal[0] * normalDirection;
      n[1] = v -> m_normal[1] * normalDirection;
      n[2] = v -> m_normal[2] * normalDirection;

      projectionArea = v -> m_projectionArea[dir];
    }
    n.UnitVec();

    float val = 0;
    if (getInterpolateMode() == 1) {
      DVec3 xp;
      DVec4 x_i;
      xp[0] = pos[0];
      xp[1] = pos[1];
      xp[2] = pos[2];
      CES::Vec3<float> vv;
      if (didx.size() == 3) {
	if (_gus.ConvXtoI(xp, x_i) >= 0 &&
	    _gus.InterpolateData(x_i, vecDataIdx, vv.m_v)) {

	  val = (isVecLen) ? vv.Length() : vv[vecCompo];
	  allValues3[i] = vv;

#ifdef OLD
	  if (didx[0] && didx[1] && didx[2]) val = vv.Length();
	  else val = (didx[0]) ? vv[0] : (didx[1]) ? vv[1] : vv[2];
#endif // OLD

	}
      } else {
	float dval;
	if (_gus.ConvXtoI(xp, x_i) >= 0 &&
	    _gus.InterpolateData(x_i, 0, dval)) {
	  val = dval;
	}	
      }
    } else {    
      std::vector<float> tmp
	= sph -> interpolateData(pos, didx, getInterpolateMode(),
				 n, getExUseTrilinear());
      if (tmp.size() != didx.size()) return false;
    
      if (didx.size() == 1) { // Scalar
	val = tmp[0];
      } else if (didx.size() == 3) {

	val = (isVecLen) ? \
	  sqrt(tmp[0] * tmp[0] + tmp[1] * tmp[1] + tmp[2] * tmp[2]) : \
	  tmp[vecCompo];
	for (size_t k = 0; k < 3; ++k) allValues3[i][k] = tmp[k];

#ifdef OLD
	if (didx[0] && didx[1] && didx[2]) { // VecLen
	  val = sqrt(tmp[0] * tmp[0] + tmp[1] * tmp[1] + tmp[2] * tmp[2]);
	} else { // Vector Compoment
	  val = (didx[0]) ? tmp[0] : \
	    (didx[1]) ? tmp[1] : (didx[2]) ? tmp[2] : 0.0;
	}
#endif // OLD

      }
    }

    // 投影面積をかける
    //
    m_allValues[i] = val * projectionArea;
    // added by yoh@fns
    if (didx.size() == 3)
      for (size_t k = 0; k < 2; ++k) allValues3[i][k] *= projectionArea;

    // 最大最小値
    //
    if (i == 0) {
      m_minmax[0] = m_minmax[1] = m_allValues[i];
    } else {
      if (m_minmax[0] > m_allValues[i]) m_minmax[0] = m_allValues[i];
      if (m_minmax[1] < m_allValues[i]) m_minmax[1] = m_allValues[i];
    }
  }

  m_allValues3.clear();
  m_allValues3 = allValues3;

  return true;
}

/**
 * 圧力寄与率の設定
 */
void
vsnMethod_Shape_scalarMap::setPressDragValue(void)
{
  VFR::AppearType mode = (m_renderingMode == 0) ? AT_PER_FACE : AT_PER_VERTEX;
  m_trias -> alcColors(m_allValues.size());
  m_trias -> setColorMode(mode);

  /**
   * 定義点に色情報を設定する。
   * m_allValues の値を色情報に変換する。
   */
  for (size_t i = 0; i < m_allValues.size(); ++i) {
    // 形状頂点の値を取得
    //
    int c = m_lut.getValIdx(m_allValues[i]);
    VFR::vector4 cv;
    cv[0] = m_lut.lutEntry[c * 4 + 0];
    cv[1] = m_lut.lutEntry[c * 4 + 1];
    cv[2] = m_lut.lutEntry[c * 4 + 2];
    cv[3] = 1.f;
    //    memcpy(cv, &m_lut.lutEntry[c * 4], sizeof(float)*4);
    m_trias -> setColor3(i, cv);
  }
}

/**
 * せん断応力又は摩擦抵抗を求める
 * @param isShearingMode true ならばせん断応力を求める
 * @retval true 成功
 * @retval false 失敗
 */
bool
vsnMethod_Shape_scalarMap::getShearingValue(bool isShearingMode)
{
  m_allValues3.clear();
  m_allValues.clear();

  if (! m_trias) return false;

  // 頂点の数
  //
  const size_t numVerts   = m_trias -> getNumVerts();
  if (numVerts < 3) return false;

  if (m_sphIdx == 0) return false;
  vsnData_Sph* sph = m_sphList[m_sphIdx - 1];
  if (! sph) return false;

  // SPH の値がベクトルでなければ戻る
  //
  if (sph -> getDataLen() != 3) return false;

  VFR::AppearType mode = (m_renderingMode == 0) ? AT_PER_FACE : AT_PER_VERTEX;

  VFR::vector3* verts = m_trias -> getVerts();
  if (! verts) return false;

  // ステンシル幅
  //
  float stencilWidth = getShearingStencilWidth();
  if (stencilWidth == 0) return false;

  // 粘性係数
  //
  float coeffVal
    = (isShearingMode) ? getShearingCoeffVal() : getFricResistCoeffVal();

  // 表面速度ベクトル
  //
  CES::Vec3<float> surfVeloc
    = (isShearingMode) ? getShearingSurfVeloc() : getFricResistSurfVeloc();

  // 方向
  //
  int dir = (isShearingMode) ? getShearingMapCompo() : getFricResistMapCompo();
  
  // 面の数
  //
  int numFaces = numVerts / 3;

  // レンダリングする点の数
  //
  int numPoints        = (mode == AT_PER_VERTEX) ? numVerts : numFaces;
  // レンダリングする点の座標値
  //
  VFR::vector3* points = (mode == AT_PER_VERTEX) ? verts : m_faceGravity;

  // 面の法線ベクトル
  //
  VFR::vector3* faceNormals = m_trias -> getNormals();

  CES::Vec3<int> vecDataIdx(0, 1, 2);
  int m_requestedStp = 0;
  vsnGridUtilSv _gus;
  if ( ! _gus.setup(sph -> getDims(), sph -> getDataLen(),
		    (float*)sph -> getCoord(m_requestedStp),
		    (float*)sph -> getData(m_requestedStp)) ) {
    return false;
  }

  const CES::Vec3<float>* pbb = sph -> getBbox();

  // 値
  //
  //  float minmax[2];
  //  std::vector<float> values(numPoints);

  // 法線ベクトルの向き
  //
  float normalDirection = (getNormalDirection() == 0) ? 1.0 : -1.0;

  /**
   *　全抵抗で使うため、ベクトル値は m_allValues3 に設定する
   */
  m_allValues3.clear();
  m_allValues3.resize(numPoints);

  /**
   * 定義点にマップする値は m_allValues に設定する
   */
  m_allValues.clear();
  m_allValues.resize(numPoints);
  //  std::vector<CES::Vec3<float> > allValues(numPoints);
  for (int i = 0; i < numPoints; ++i) {

    // 法線ベクトル
    //
    CES::Vec3<float> n;
    if (mode == AT_PER_FACE) {
      n[0] = faceNormals[i][0] * normalDirection;
      n[1] = faceNormals[i][1] * normalDirection;
      n[2] = faceNormals[i][2] * normalDirection;
    } else {
      Vertex* v = m_vm.Find(points[i][0], points[i][1], points[i][2]);
      n[0] = v -> m_normal[0] * normalDirection;
      n[1] = v -> m_normal[1] * normalDirection;
      n[2] = v -> m_normal[2] * normalDirection;
    }
    n.UnitVec(); // UnitVector

    // 値を求める位置
    //
    CES::Vec3<float> pos(points[i][0] + n[0] * stencilWidth,
			 points[i][1] + n[1] * stencilWidth,
			 points[i][2] + n[2] * stencilWidth);

    // 全ての値を取得する
    //
    std::vector<bool> didx;
    didx.push_back(true); didx.push_back(true); didx.push_back(true);

    std::vector<float> tmp;
    if (getInterpolateMode() == 1) { // tilinear
      if (pos[0] < pbb[0].m_v[0] ||
	  pos[0] > pbb[1].m_v[0] ||
	  pos[1] < pbb[0].m_v[1] ||
	  pos[1] > pbb[1].m_v[1] ||
	  pos[2] < pbb[0].m_v[2] ||
	  pos[2] > pbb[1].m_v[2]) {
	tmp.push_back(0);
	tmp.push_back(0);
	tmp.push_back(0);
      } else {
	DVec3 xp;
	DVec4 x_i;
	xp[0] = pos[0];
	xp[1] = pos[1];
	xp[2] = pos[2];
	CES::Vec3<float> vv;
	if (_gus.ConvXtoI(xp, x_i) >= 0 &&
	    _gus.InterpolateData(x_i, vecDataIdx, vv.m_v)) {
	  tmp.push_back(vv[0]);
	  tmp.push_back(vv[1]);
	  tmp.push_back(vv[2]);
	} else {
	  tmp.push_back(0);
	  tmp.push_back(0);
	  tmp.push_back(0);
	}
      }
    } else {
      // Value
      tmp = sph -> interpolateData(pos, didx,
				   getInterpolateMode(),
				   n, getExUseTrilinear());
      if (tmp.size() != didx.size())
	throw std::runtime_error("interpolate data failed");
    }

    CES::Vec3<float> u;
    u[0] = tmp[0];
    u[1] = tmp[1];
    u[2] = tmp[2];

    // 内積
    //
    double v = n | u;

    // 値を求める
    //
    for (int j = 0; j < 3; ++j) {
      m_allValues3[i][j]
	= coeffVal * (u[j] - v * n[j] - surfVeloc[j]) / stencilWidth;
    }

    float val = 0.0;
    if (dir == 0 || dir == 1 || dir == 2) {
      val = m_allValues3[i][dir];
    } else if (dir == 3) {
      // VecLen
      val = m_allValues3[i].Length();
    }

    // 摩擦抵抗の時は面積をかける
    //
    if (! isShearingMode) {
      float area = 0.0;
      if (mode == AT_PER_FACE) {
	area = m_faceArea[i];
      } else {
	Vertex* v = m_vm.Find(verts[i]);
	area = v -> GetArea();
      }
      val *= area;

      m_allValues3[i][0] *= area;
      m_allValues3[i][1] *= area;
      m_allValues3[i][2] *= area;
    }

    m_allValues[i] = val;

    if (i == 0) m_minmax[0] = m_minmax[1] = val;
    else {
      if (val < m_minmax[0]) m_minmax[0] = val;
      else if (val > m_minmax[1]) m_minmax[1] = val;
    }
  }

  return true;
}

/**
 * 全抵抗の値を求める
 * @retval true 成功
 * @retval falsel 失敗
 *
 * m_totalValues, m_minmax
 * m_totalValues3, m_totalResist に値を設定する
 */
bool
vsnMethod_Shape_scalarMap::getTotalResistValue(void)
{
  bool success = true;
  // 摩擦抵抗を求める
  //
  if (! getShearingValue(false)) {
    std::cerr << "getShearingValue failed" << std::endl;
    success = false;
  }
  // 摩擦抵抗の値のバックアップ
  //
  std::vector<CES::Vec3<float> > fricResistValues3 = m_allValues3;
  std::vector<float> fricResistValues = m_allValues;

  //  float minmax[2] = { m_minmax[0], m_minmax[1] };

  // 圧力抵抗値の取得
  //
  if (! getPressResistValue()) {
    std::cerr << "getPressResist failed" << std::endl;
    success = false;
  }

  m_totalResistValue[0] = m_totalResistValue[1] = m_totalResistValue[2] = 0.0;

  if (! success) {
    m_allValues.clear();
    m_minmax[0] = m_minmax[1] = 0;
    return true;
  }

  // 圧力抵抗と摩擦抵抗を加算する
  //
  for (size_t i = 0; i < m_allValues.size(); ++i) {
    m_allValues[i] += fricResistValues[i];
    if (i == 0) { m_minmax[0] = m_minmax[1] = m_allValues[i]; }
    else {
      if (m_minmax[0] > m_allValues[i]) m_minmax[0] = m_allValues[i];
      if (m_minmax[1] < m_allValues[i]) m_minmax[1] = m_allValues[i];
    }

    CES::Vec3<float> tmp = m_allValues3[i] + fricResistValues3[i];
    m_totalResistValue[0] += tmp[0];
    m_totalResistValue[1] += tmp[1];
    m_totalResistValue[2] += tmp[2];
  }

  return true;
}

/**
 * 圧力抵抗の値を求める
 * @retval true 成功
 * @reval false 失敗
 *
 * m_totalValues, m_minmax
 * m_totalValues3, m_totalResist に値を設定する
 */
bool
vsnMethod_Shape_scalarMap::getPressResistValue(void)
{
  // 圧力データのSPH
  //
  int pressSphIdx = getPressSphIdx();
  if (pressSphIdx == 0) {
    std::cerr << "pressSphIdx == 0" << std::endl;
    return false;
  }
  vsnData_Sph* pressSph = getSph(pressSphIdx - 1);
  if (! pressSph) {
    std::cerr << "pressSph is NULL" << std::endl;
    return false;
  }

  // 圧力データのコンポーネント
  //
  int pressCompoIdx = getPressCompoIdx();
  if (!(pressCompoIdx == 0 || pressCompoIdx == 1 || pressCompoIdx == 2)) {
    std::cerr << "pressCompoIdx out of range " << pressCompoIdx << std::endl;
    return false;
  }

  // 形状データのチェック
  //
  if (! m_trias) {
    std::cerr << "triangle is NULL" << std::endl;
    return false;
  }

  // 頂点の数
  //
  const size_t numVerts   = m_trias -> getNumVerts();
  if (numVerts < 3) {
    std::cerr << "numVerts less 3" << std::endl;
    return false;
  }

  VFR::AppearType mode = (m_renderingMode == 0) ? AT_PER_FACE : AT_PER_VERTEX;

  VFR::vector3* verts = m_trias -> getVerts();
  if (! verts) return false;

  // マップする方向
  //
  int dir = getShearingMapCompo();
  
  // 面の数
  //
  int numFaces = numVerts / 3;

  // レンダリングする点の数
  //
  int numPoints        = (mode == AT_PER_VERTEX) ? numVerts : numFaces;
  // レンダリングする点の座標値
  //
  VFR::vector3* points = (mode == AT_PER_VERTEX) ? verts : m_faceGravity;

  // 面の法線ベクトル
  //
  VFR::vector3* faceNormals = m_trias -> getNormals();

  CES::Vec3<int> vecDataIdx(0, 1, 2);
  int m_requestedStp = 0;
  vsnGridUtilSv _gus;
  if ( ! _gus.setup(pressSph -> getDims(), pressSph -> getDataLen(),
		    (float*)pressSph -> getCoord(m_requestedStp),
		    (float*)pressSph -> getData(m_requestedStp)) ) {
    //    std::cerr << "gus failed" << std::endl;
    return false;
  }

  const CES::Vec3<float>* pbb = pressSph -> getBbox();

  // 法線ベクトルの向き
  //
  float normalDirection = (getNormalDirection() == 0) ? 1.0 : -1.0;

  // 全抵抗の成分
  //
  //  m_totalResist[0] = m_totalResist[1] = m_totalResist[2] = 0.0;

  m_allValues3.clear();
  m_allValues3.resize(numPoints);

  m_allValues.clear();
  m_allValues.resize(numPoints);
  //  std::vector<CES::Vec3<float> > allValues(numPoints);
  for (int i = 0; i < numPoints; ++i) {

    // 法線ベクトル
    //
    CES::Vec3<float> n;
    if (mode == AT_PER_FACE) {
      n[0] = faceNormals[i][0] * normalDirection;
      n[1] = faceNormals[i][1] * normalDirection;
      n[2] = faceNormals[i][2] * normalDirection;
    } else {
      Vertex* v = m_vm.Find(points[i][0], points[i][1], points[i][2]);
      n[0] = v -> m_normal[0] * normalDirection;
      n[1] = v -> m_normal[1] * normalDirection;
      n[2] = v -> m_normal[2] * normalDirection;
    }
    n.UnitVec(); // UnitVector

    // 値を求める位置
    //
    CES::Vec3<float> pos(points[i][0],
			 points[i][1],
			 points[i][2]);

    std::vector<bool> didx;
    if (pressSph -> getDataLen() == 3) {
      if (pressCompoIdx == 0) { // Vector Component X
	didx.push_back(true); didx.push_back(false); didx.push_back(false);
      } else if (pressCompoIdx == 1) { // Vector Component Y
	didx.push_back(false); didx.push_back(true); didx.push_back(false);
      } else if (pressCompoIdx == 2) { // Vector Component Z
	didx.push_back(false); didx.push_back(false); didx.push_back(true);
      }
    } else {
      didx.push_back(true);
    }

    float pressVal = 0;
    if (getInterpolateMode() == 1) { // tilinear
      if (pos[0] < pbb[0].m_v[0] ||
	  pos[0] > pbb[1].m_v[0] ||
	  pos[1] < pbb[0].m_v[1] ||
	  pos[1] > pbb[1].m_v[1] ||
	  pos[2] < pbb[0].m_v[2] ||
	  pos[2] > pbb[1].m_v[2]) {
	pressVal = 0;
      } else {
	DVec3 xp;
	DVec4 x_i;
	xp[0] = pos[0];
	xp[1] = pos[1];
	xp[2] = pos[2];

	if (didx.size() == 3) {
	  CES::Vec3<float> vv;
	  if (_gus.ConvXtoI(xp, x_i) >= 0 &&
	      _gus.InterpolateData(x_i, vecDataIdx, vv.m_v)) {
	    if (didx[0] && didx[1] && didx[2]) pressVal = vv.Length();
	    else pressVal = (didx[0]) ? vv[0] : (didx[1]) ? vv[1] : vv[2];
	  }
	} else {
	  float dval;
	  if (_gus.ConvXtoI(xp, x_i) >= 0 &&
	      _gus.InterpolateData(x_i, 0, dval)) {
	    pressVal = dval;
	  }
	}

      }
    } else {
      // Value
      std::vector<float> tmp
	= pressSph -> interpolateData(pos, didx, getInterpolateMode(),
				      n, getExUseTrilinear());
      if (tmp.size() != didx.size())
	throw std::runtime_error("interpolate data failed");
      if (didx.size() == 1) {
	pressVal = tmp[0];
      } else if (didx.size() == 3) {
	pressVal = (didx[0]) ? tmp[0] : \
	  (didx[1]) ? tmp[1] : (didx[2]) ? tmp[2] : 0.0;
      }
    }

    CES::Vec3<float> u;
    // 圧力値に単位ベクトルをかける
    //
    u[0] = pressVal * n[0];
    u[1] = pressVal * n[1];
    u[2] = pressVal * n[2];

    m_allValues3[i] = u;

    // マップする方向を考慮した計算
    //
    float val = 0.0;
    if (dir == 0 || dir == 1 || dir == 2) {
      val = m_allValues3[i][dir];
    } else if (dir == 3) {
      // VecLen
      val = m_allValues3[i].Length();
    }

    // 面積をかける
    //
    float area = 0.0;
    if (mode == AT_PER_FACE) {
      area = m_faceArea[i];
    } else {
      Vertex* v = m_vm.Find(verts[i]);
      area = v -> GetArea();
    }
    val *= area;
    m_allValues3[i][0] *= area;
    m_allValues3[i][1] *= area;
    m_allValues3[i][2] *= area;

    // スカラー値
    //
    m_allValues[i] = val;

    if (i == 0) m_minmax[0] = m_minmax[1] = val;
    else {
      if (val < m_minmax[0]) m_minmax[0] = val;
      else if (val > m_minmax[1]) m_minmax[1] = val;
    }
  }

  return true;
}

/**
 * せん断応力の設定
 * @param isSheingMode shearing モードのフラグ
 * @retval true 成功
 * @retval false 失敗
 */
bool
vsnMethod_Shape_scalarMap::setShearingValue(bool isShearingMode)
{
  if (m_allValues.empty()) {
    return false;
  }

  VFR::AppearType mode = (m_renderingMode == 0) ? AT_PER_FACE : AT_PER_VERTEX;

  m_trias -> alcColors(m_allValues.size());
  m_trias -> setColorMode(mode);

  /**
   * m_allValues の値の色情報を定義点に設定する
   */
  for (size_t i = 0; i < m_allValues.size(); ++i) {
    float val = m_allValues[i];

    int c = m_lut.getValIdx(val);
    VFR::vector4 cv;
    cv[0] = m_lut.lutEntry[c * 4 + 0];
    cv[1] = m_lut.lutEntry[c * 4 + 1];
    cv[2] = m_lut.lutEntry[c * 4 + 2];
    cv[3] = 1.f;
    //    memcpy(cv, &m_lut.lutEntry[c * 4], sizeof(float)*4);
    m_trias -> setColor3(i, cv);
  }

  return true;
}

#ifdef OLD
/**
 * 三角形の面積を求める
 * @param mode (=AT_PER_FACE) 面の面積を求める
 *             (=AT_PER_VERTEX) 頂点を共有する面の面積を求める
 * @return 面積のリスト。順番は面の順番。失敗すると空のリストを返す。
 */
std::vector<float>
vsnMethod_Shape_scalarMap::getAreas(VFR::AppearType mode)
{
  std::vector<float> result;
  
  if (! m_trias) return result;

  const size_t numVerts = m_trias -> getNumVerts();
  VFR::vector3* verts = m_trias -> getVerts();
  if (! verts) return result;

  if (mode == AT_PER_FACE) {
    const size_t numFaces = numVerts / 3;
    for (size_t i = 0; i < numFaces; ++i) {
      CES::Vec3<float> p1 = verts[i * 3 + 0];
      CES::Vec3<float> p2 = verts[i * 3 + 1];
      CES::Vec3<float> p3 = verts[i * 3 + 2];

      CES::Vec3<float> v1 = p2 - p1;
      CES::Vec3<float> v2 = p3 - p1;

      // 外積
      //
      CES::Vec3<float> g = v1 ^ v2;

      // 面積
      //
      float area = sqrt(g[0] * g[0] + g[1] * g[1] + g[2] * g[2]) / 2.0;
      if (area < 0) std::cerr << "area is negative" << std::endl;

      result.push_back(area);
    }
  } else if (mode == AT_PER_VERTEX) {
    
  }

  return result;
}
#endif // OLD

/**
 * shearingモードかどうか
 * @retval true shearingモードのとき
 * @retval false shearingモードでないとき
 */
bool
vsnMethod_Shape_scalarMap::IsShearingMode(void) const
{
  return m_physIdx == 2;
}

/**
 * fricResistモードかどうか
 * @retval true fricResistモードのとき
 * @retval false fricResistモードでないとき
 */
bool
vsnMethod_Shape_scalarMap::IsFricResistMode(void) const
{
  return m_physIdx == 3;
}

/**
 * 値の設定
 * @retval true 成功
 * @retval false 失敗
 */
bool
vsnMethod_Shape_scalarMap::setValue(void)
{
  /**
   * 表示する物理量により関数を切り替える
   */

  bool result = true;
  try {
    if (m_physIdx == 0) {
      // スカラー値
      //
      setScalarValue();
    } else if (m_physIdx == 1) {
      // 圧力抵抗の寄与率
      //
      setPressDragValue();
    } else if (m_physIdx == 2) {
      // 剪断応力
      //
      result = setShearingValue(IsShearingMode());
    } else if (m_physIdx == 3) {
      // 摩擦抵抗
      //
      result = setShearingValue(IsShearingMode());
    } else if (m_physIdx == 4) {
      // 全抵抗
      //
      result = setShearingValue(true);
    }

  } catch (std::runtime_error& e) {
    ErrMsg(VSN::MsgERR, e.what());
    std::cerr << e.what() << std::endl;
    result = false;
  }

  return result;
}

/* vsnMethodObj methods */

/**
 * 更新
 */
bool 
vsnMethod_Shape_scalarMap::update(const bool force)
{
  return updateStep(m_requestedStp, force);
}

/**
 * 読み込まれているSPHのリストを作成する
 */
bool
vsnMethod_Shape_scalarMap::loadSph(void)
{
  // Get ReferenceData
  vsnData_Shape* pdo = dynamic_cast<vsnData_Shape*>(getRefData());
  if (! pdo) return false;

  // Get Scene
  vsnScene* psc = pdo -> getAncestorScene();
  if (! psc) return false;

  // Make Shape List
  clearSph();
  for (size_t j = 0; j < psc -> getNumDataObj(); ++j) {
    vsnData_Sph* data = dynamic_cast<vsnData_Sph*>(psc -> getDataObj(j));
    if (data) addSph(data);
  }

  return true;
}

/**
 * 2点間の距離を求める
 * @param v1 点
 * @param v2 点
 * @return 距離
 */
double
len(const CES::Vec3<float>& v1, const CES::Vec3<float>& v2)
{
  return sqrt((v1[0] - v2[0]) * (v1[0] - v2[0]) + 
	      (v1[1] - v2[1]) * (v1[1] - v2[1]) + 
	      (v1[2] - v2[2]) * (v1[2] - v2[2]));
}

/**
 * ２点間の中点を求める
 * @param v1 点
 * @param v2 点
 * @return 中点
 */
CES::Vec3<float>
mid(const CES::Vec3<float>& v1, const CES::Vec3<float>& v2)
{
  return CES::Vec3<float>((v1[0]+v2[0])/2, (v1[1]+v2[1])/2, (v1[2]+v2[2])/2);
}

#ifdef OLD
/**
 * STLを細分化した新しい面
 */
class NewFace {
public:
  /**
   * コンストラクタ
   * @param v1 点
   * @param v2 点
   * @param v3 点
   * @param normal 法線ベクトル
   */
  NewFace(const CES::Vec3<float>& v1,
	  const CES::Vec3<float>& v2, const CES::Vec3<float>& v3,
	  const CES::Vec3<float>& normal) {
    m_v[0] = v1;
    m_v[1] = v2;
    m_v[2] = v3;
    m_v[3] = normal;
  }
  /**
   * 点の座標値[0..2]
   * 面の法線ベクトル[3]
   */
  CES::Vec3<float> m_v[4];
};

/**
 * 三角形の分割
 * 底辺の長さと高さの比を求め、アスペクト比の大きな三角形は分割する
 * 高さ/底辺　の比を求め、比の一番小さい底面を２分割し、反対側の頂点と結ぶ
 */
void
divideTri(const CES::Vec3<float>& v1,
	  const CES::Vec3<float>& v2, const CES::Vec3<float>& v3,
	  const CES::Vec3<float>& normal,
	  std::vector<NewFace>* new_faces, int count)
{
  if (count > 3) {
    new_faces -> push_back(NewFace(v1, v2, v3, normal));
    return;
  }

  double area = calcTriArea(v1, v2, v3);

  CES::Vec3<float> vv[3] = { v1, v2, v3 };

  int minIdx = 0;
  double minAspect = 0;
  for (size_t i = 0; i < 3; ++i) {
    double bottom = len(vv[i], vv[(i + 1) % 3]);
    double height = area / bottom * 2;
    double aspect = height / bottom;
    if (i == 0) minAspect = aspect;
    if (minAspect > aspect) {
      minAspect = aspect;
      minIdx = i;
    }
  }

  if (minAspect < 0.5) {
    CES::Vec3<float> midV = mid(vv[minIdx], vv[(minIdx + 1) % 3]);
      
    divideTri(vv[minIdx], midV, vv[(minIdx + 2) % 3],
	      normal, new_faces, count + 1);
    divideTri(midV, vv[(minIdx + 1) % 3], vv[(minIdx + 2) % 3],
	      normal, new_faces, count + 1);
  } else {
    new_faces -> push_back(NewFace(v1, v2, v3, normal));
  }
}
#endif // OLD

/**
 * 三角形データを作成する
 * @param force true の時は、既存の三角形データを破棄する
 * @result メモリの確保に失敗、形状データが vfrTriangles ではない
 */
bool
vsnMethod_Shape_scalarMap::generateTrias(bool force)
{
  /**
   * 面毎の重心位置、面積、投影面積を求める。
   * 頂点毎に、法線ベクトル、頂点の面積、投影面積を求める。
   */

  vsnData_Shape* pData = dynamic_cast<vsnData_Shape*>(p_refData);
  if ( ! pData ) return false;

  if (force) {
    if (m_trias) {
      delete m_trias;
      m_trias = NULL;
    }
  }

  if (! m_trias) {
    vfrTriangles* trias = dynamic_cast<vfrTriangles*>(pData -> getShape());
    if (! trias) return false;

    m_trias = new vfrTriangles();
    if (! m_trias) {
      ErrMsg(MsgERR, getMethodType() + string("[") + getName() +
	     string("]: memory allocation failed"));
      return false;
    }

    size_t numVerts   = trias -> getNumVerts();
    size_t numNormals = trias -> getNumNormals();
    size_t numFaces   = numVerts / 3;

    // 細分割するかどうか
    //
    //    bool useSecond = getDivideMode();
    bool useSecond = false;
    
    if (useSecond) {
#ifdef OLD
#ifdef USE_SECOND
      m_trias -> alcVerts(numVerts * 4);
      m_trias -> alcNormals(numNormals * 4);
      m_trias -> alcMaterial();
#endif // USE_SECOND

      m_trias -> alcMaterial();
      m_trias -> setNormalMode(trias -> getNormalMode());
      m_trias -> getPrivateMaterial() -> setRenderMode(RT_NONE);

      std::vector<NewFace> new_faces;

      VFR::vector3* verts = trias -> getVerts();
      VFR::vector3* normals = trias -> getNormals();
      for (size_t i = 0; i < numFaces; ++i) {
	CES::Vec3<float> v1 = verts[i * 3 + 0];
	CES::Vec3<float> v2 = verts[i * 3 + 1];
	CES::Vec3<float> v3 = verts[i * 3 + 2];
	CES::Vec3<float> normal = normals[i];

	divideTri(v1, v2, v3, normal, &new_faces, 0);
      }	

      int idx = 0;
      size_t new_verts_size = new_faces.size() * 3;
      size_t new_faces_size = new_faces.size();
      m_trias -> alcVerts(new_verts_size);
      m_trias -> alcNormals(new_faces_size);
      for (size_t i = 0; i < new_faces_size; ++i) {
	VFR::vector3 tmp;

	new_faces[i].m_v[0].GetVal(tmp);
	m_trias -> setVert(idx++, tmp, FALSE);

	new_faces[i].m_v[1].GetVal(tmp);
	m_trias -> setVert(idx++, tmp, FALSE);

	new_faces[i].m_v[2].GetVal(tmp);
	m_trias -> setVert(idx++, tmp, FALSE);

	new_faces[i].m_v[3].GetVal(tmp);
	m_trias -> setNormal(i, tmp);
      }

      numVerts = new_verts_size;
      numFaces = new_faces_size;
      numNormals = new_faces_size;

#ifdef OLD
      int idx = 0;
      //      VFR::vector3* verts = trias -> getVerts();
      //      VFR::vector3* normals = trias -> getNormals();
      for (size_t i = 0; i < numFaces; ++i) {
	VFR::vector3 v1, v2, v3, v1_v2, v2_v3, v3_v1;
	v1[0] = verts[i * 3 + 0][0];
	v1[1] = verts[i * 3 + 0][1];
	v1[2] = verts[i * 3 + 0][2];

	v2[0] = verts[i * 3 + 1][0];
	v2[1] = verts[i * 3 + 1][1];
	v2[2] = verts[i * 3 + 1][2];

	v3[0] = verts[i * 3 + 2][0];
	v3[1] = verts[i * 3 + 2][1];
	v3[2] = verts[i * 3 + 2][2];

	v1_v2[0] = (v1[0] + v2[0]) / 2;
	v1_v2[1] = (v1[1] + v2[1]) / 2;
	v1_v2[2] = (v1[2] + v2[2]) / 2;

	v2_v3[0] = (v2[0] + v3[0]) / 2;
	v2_v3[1] = (v2[1] + v3[1]) / 2;
	v2_v3[2] = (v2[2] + v3[2]) / 2;

	v3_v1[0] = (v3[0] + v1[0]) / 2;
	v3_v1[1] = (v3[1] + v1[1]) / 2;
	v3_v1[2] = (v3[2] + v1[2]) / 2;


	m_trias -> setVert(idx++, v1, FALSE);
	m_trias -> setVert(idx++, v1_v2, FALSE);
	m_trias -> setVert(idx++, v3_v1, FALSE);

	m_trias -> setVert(idx++, v2, FALSE);
	m_trias -> setVert(idx++, v2_v3, FALSE);
	m_trias -> setVert(idx++, v1_v2, FALSE);

	m_trias -> setVert(idx++, v3, FALSE);
	m_trias -> setVert(idx++, v3_v1, FALSE);
	m_trias -> setVert(idx++, v2_v3, FALSE);

	m_trias -> setVert(idx++, v1_v2, FALSE);
	m_trias -> setVert(idx++, v2_v3, FALSE);
	m_trias -> setVert(idx++, v3_v1, FALSE);

	VFR::vector3 normal;
	normal[0] = normals[i][0];
	normal[1] = normals[i][1];
	normal[2] = normals[i][2];
	m_trias -> setNormal(i * 4 + 0, normal);
	m_trias -> setNormal(i * 4 + 1, normal);
	m_trias -> setNormal(i * 4 + 2, normal);
	m_trias -> setNormal(i * 4 + 3, normal);
      }

      numVerts *= 4;
      numFaces *= 4;
      numNormals *= 4;
#endif // OLD
#endif // OLD
    } else {
      m_trias -> alcVerts(numVerts);
      m_trias -> alcNormals(numNormals);
      m_trias -> alcMaterial();
      m_trias -> setNormalMode(trias -> getNormalMode());

      m_trias -> getPrivateMaterial() -> setRenderMode(RT_NONE);

      //
      // 頂点を設定する
      //
      m_trias -> setVerts(numVerts, trias -> getVerts());
      //
      // 法線ベクトルを設定する
      //
      m_trias -> setNormals(numNormals, trias -> getNormals());
    }

    //
    // BBox の作成
    //
    m_trias -> generateBbox();

    //
    // 子供に加える
    //
    addChild(m_trias);

    // 以下、面の重心位置、面積、投影面積を求める
    //
    VFR::vector3* verts = m_trias -> getVerts();
    allocFaces(numFaces);
    //    m_faceGravity.clear();
    for (size_t i = 0; i < numFaces; ++i) {
      VFR::vector3* ptr = verts + (i * 3);

      // 重心
      //
      m_faceGravity[i][0] = (ptr[0][0] + ptr[1][0] + ptr[2][0]) / 3;
      m_faceGravity[i][1] = (ptr[0][1] + ptr[1][1] + ptr[2][1]) / 3;
      m_faceGravity[i][2] = (ptr[0][2] + ptr[1][2] + ptr[2][2]) / 3;

      // 面積
      //
      CES::Vec3<float> p1(ptr[0]), p2(ptr[1]), p3(ptr[2]);
      m_faceArea[i] = calcTriArea(p1, p2, p3);

      // 投影面積
      //
      for (int dir = 0; dir < 3; ++dir) {
	CES::Vec3<float> pp1(p1), pp2(p2), pp3(p3);
	pp1[dir] = pp2[dir] = pp3[dir] = 0;
	m_faceProjectionArea[dir][i] = calcTriArea(pp1, pp2, pp3);
      }
    }

    // 以下頂点の情報を作成する
    //
    m_vm.Clear();
    VFR::vector3* faceNormals = m_trias -> getNormals();
    for (size_t i = 0; i < numFaces; ++i) {
      Vertex* v1 = m_vm.Insert(verts[(i * 3) + 0]);
      if (! v1) std::cerr << "insert failed v1" << std::endl;
      v1 -> AppendNormal(faceNormals[i]); // 法線ベクトル

      Vertex* v2 = m_vm.Insert(verts[(i * 3) + 1]);
      if (! v2) std::cerr << "insert failed v2" << std::endl;
      v2 -> AppendNormal(faceNormals[i]); // 法線ベクトル

      Vertex* v3 = m_vm.Insert(verts[(i * 3) + 2]);
      if (! v3) std::cerr << "insert failed v3" << std::endl;
      v3 -> AppendNormal(faceNormals[i]); // 法線ベクトル

      // 頂点
      //
      CES::Vec3<float> vv1(v1 -> m_p[0], v1 -> m_p[1], v1 -> m_p[2]);
      CES::Vec3<float> vv2(v2 -> m_p[0], v2 -> m_p[1], v2 -> m_p[2]);
      CES::Vec3<float> vv3(v3 -> m_p[0], v3 -> m_p[1], v3 -> m_p[2]);

      CES::Vec3<float> tmp;

      // 重心
      //
      tmp = vv1 + vv2 + vv3;
      CES::Vec3<float> gravity(tmp[0] / 3, tmp[1] / 3, tmp[2] / 3);

      // 辺の中点
      //
      tmp = vv1 + vv2;
      CES::Vec3<float> v1_v2(tmp[0] / 2, tmp[1] / 2, tmp[2] / 2);
      tmp = vv1 + vv3;
      CES::Vec3<float> v1_v3(tmp[0] / 2, tmp[1] / 2, tmp[2] / 2);
      tmp = vv2 + vv3;
      CES::Vec3<float> v2_v3(tmp[0] / 2, tmp[1] / 2, tmp[2] / 2);

      // 頂点の面積
      //
      v1 -> AppendArea(calcTriArea(vv1, v1_v2, gravity));
      v1 -> AppendArea(calcTriArea(vv1, gravity, v1_v3));

      v2 -> AppendArea(calcTriArea(vv2, v2_v3, gravity));
      v2 -> AppendArea(calcTriArea(vv2, gravity, v1_v2));

      v3 -> AppendArea(calcTriArea(vv3, v1_v3, gravity));
      v3 -> AppendArea(calcTriArea(vv3, gravity, v2_v3));

      // 射影面積
      //
      for (int i = 0; i < 3; ++i) {
	CES::Vec3<float> pp1(vv1), pp2(v1_v2), pp3(gravity);
	pp1[i] = pp2[i] = pp3[i] = 0;
	v1 -> m_projectionArea[i] += calcTriArea(pp1, pp2, pp3);

	pp1 = vv1; pp2 = gravity; pp3 = v1_v3;
	pp1[i] = pp2[i] = pp3[i] = 0;
	v1 -> m_projectionArea[i] += calcTriArea(pp1, pp2, pp3);

	pp1 = vv2; pp2 = v2_v3; pp3 =  gravity;
	pp1[i] = pp2[i] = pp3[i] = 0;
	v2 -> m_projectionArea[i] += calcTriArea(pp1, pp2, pp3);

	pp1 = vv2; pp2 = gravity; pp3 = v1_v2;
	pp1[i] = pp2[i] = pp3[i] = 0;
	v2 -> m_projectionArea[i] += calcTriArea(pp1, pp2, pp3);

	pp1 = vv3; pp2 = v1_v3; pp3 = gravity;
	pp1[i] = pp2[i] = pp3[i] = 0;
	v3 -> m_projectionArea[i] += calcTriArea(pp1, pp2, pp3);

	pp1 = vv3; pp2 = gravity; pp3 = v2_v3;
	pp1[i] = pp2[i] = pp3[i] = 0;
	v3 -> m_projectionArea[i] += calcTriArea(pp1, pp2, pp3);
      }
    }
  }

  return true;
}

/**
 * データの更新
 * @param stp ステップ
 * @param force 強制的に更新するかどうか
 * @param cascade 未使用
 * @retval true 成功
 * @retval false 失敗
 */
bool
vsnMethod_Shape_scalarMap::updateStep(const int stp,
				   const bool force, const bool cascade)
{
  vsnData_Shape* pData = dynamic_cast<vsnData_Shape*>(p_refData);
  if ( ! pData ) return false;
  m_requestedStp = stp;
  if ( m_requestedStp < 0 )
    m_requestedStp = pData->getCurrentStepIdx();

  if ( force ) m_updatedStp = -1;
  if ( m_updatedStp == m_requestedStp ) return true;
  m_updatedStp = -1;

  // 三角形形状の作成
  //
  generateTrias();

  m_trias -> getPrivateMaterial() -> setRenderMode(RT_NONE);
  if (! m_show) return true;

  // SPH のデータを読み込む
  //
  loadSph();

  // データが指定されていない
  //
  if (m_sphIdx == 0) {
    chkNotice();
    return true;
  }
  if (m_sphList.size() <= m_sphIdx - 1) return false;
  vsnData_Sph* sph = m_sphList[m_sphIdx - 1];
  if (! sph) return false;

  // 頂点や面に値を設定する
  //
  if (setValue()) {
    m_trias -> getPrivateMaterial() -> setRenderMode(RT_SMOOTH);
  }

  // スカラー表示のときだけ、コンターラインを表示する
  //
  if (getPhysIdx() != 0) {
    deleteContourLines();
  }

  // コンターライン表示のときは
  // スカラー表示を行わない
  //
  if (getPhysIdx() == 0 && getDispContourLine())
    m_trias -> getPrivateMaterial() -> setRenderMode(RT_NONE);
  //  getPrivateMaterial() -> setRenderMode(RT_WIRE);

  return true;
}

/**
 * reloaded の継承関数
 */
void
vsnMethod_Shape_scalarMap::reloaded(void)
{
  vsnMethodObj::reloaded();
}

/**
 * 表示する物理量によりパネルを作成する
 * @return パネル
 */
vsnMethodPP*
vsnMethod_Shape_scalarMap::getPhysPanel(wxPanel* pp)
{
  vsnMethodPP* result = NULL;
  if (m_physIdx == 0) {
    // Scalar
    result = new vsnMPP_Shape_scalarMap_Phys_scalar(pp, this);
  } else if (m_physIdx == 1) {
    // Pressure Drag
    result = new vsnMPP_Shape_scalarMap_Phys_pressDrag(pp, this);
  } else if (m_physIdx == 2) {
    // Shearing Stress
    result = new vsnMPP_Shape_scalarMap_Phys_shearing(pp, this);
  } else if (m_physIdx == 3) {
    // Fric Resist
    result = new vsnMPP_Shape_scalarMap_Phys_shearing(pp, this);
  } else if (m_physIdx == 4) {
    result = new vsnMPP_Shape_scalarMap_Phys_shearing(pp, this);
  }

  return result;
}

/**
 * 表示する物理量により切り替えるパネルの貼付け先のパネルを取得する
 * @return パネル
 */
vsnMethodPP*
vsnMethod_Shape_scalarMap::getParamPanel(wxPanel* pp)
{
  vsnMPP_Shape_scalarMap* result = new vsnMPP_Shape_scalarMap(pp, this);
  if (! result) return NULL;
  return result;
}

/**
 * 継承関数
 */
void 
vsnMethod_Shape_scalarMap::setBaseColor(const vector4 cv)
{
  /**
   * 何もしない
   */
}

/* serialize : from vsnIoObject */

/**
 * XMLファイルから状態を復元する
 * @param xnp ノード
 * @retval true 成功
 * @retval false 失敗
 */
bool 
vsnMethod_Shape_scalarMap::parseXML(xmlNodePtr xnp)
{
  static xmlChar* xs;
  xmlNodePtr cur;

  if (! vsnMethodObj::parseXML(xnp)) return false;
  string msgHdr
    = getMethodType() +string("[") +getName() +string("]: parseXML: ");

  // get children node
  cur = xnp->xmlChildrenNode;

  // traverse
  while (cur) {
    if ((cur = vsnIoObject::SkipCommentXML(cur)) == NULL) break;

    if (! strcmp((const char*)cur -> name, "param")) {
      std::string xsN, xsV;
      if (xs) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"name");
      if (! xs || strlen((const char*)xs) < 1) goto _NEXT_XML_NODE;
      xsN = (const char*)xs;
      if (xs) xmlFree(xs);
      xs = xmlGetProp(cur, (const xmlChar*)"value");
      if ( xs && strlen((const char*)xs) > 0 ) xsV = (const char*)xs;
      else xsV = "";

      if ( xsV.empty() ) {
        ErrMsg(MsgERR, msgHdr +string("no value in param ") +xsN);
        goto _NEXT_XML_NODE;
      }

      if (xsN == std::string("sphIdx")) {
	setSphIdx(atoi(xsV.c_str()));
      } else if (xsN == std::string("renderingMode")) {
	setRenderingMode(atoi(xsV.c_str()));
      } else if (xsN == std::string("interpolateMode")) {
	setInterpolateMode(atoi(xsV.c_str()));
      } else if (xsN == std::string("physIdx")) {
	setPhysIdx(atoi(xsV.c_str()));
      } else if (xsN == std::string("updateMinMax")) {
	setUpdateMinMaxMode((atoi(xsV.c_str()) == 1) ? true : false);
      } else if (xsN == std::string("normalDirection")) {
	setNormalDirection(atoi(xsV.c_str()));
	//      } else if (xsN == std::string("divideMode")) {
	//	setDivideMode((atoi(xsV.c_str()) == 1) ? true : false);
      } else if (xsN == std::string("exUseTrilinear")) {
	setExUseTrilinear((atoi(xsV.c_str()) == 1) ? true : false);
      } else if (xsN == std::string("scalarValIdx")) {
	setScalarValIdx(atoi(xsV.c_str()));
      } else if (xsN == std::string("dispContourLine")) {
	setDispContourLine((atoi(xsV.c_str()) == 1) ? true : false);
      } else if (xsN == std::string("numLines")) {
	setNumLines(atoi(xsV.c_str()));
      } else if (xsN == std::string("lineWidth")) {
	setLineWidth(atof(xsV.c_str()));
      } else if (xsN == std::string("useCMap")) {
	setUseCMap((atoi(xsV.c_str()) == 1) ? true : false);
      } else if (xsN == std::string("antiAlias")) {
	setAntiAliasMode((atoi(xsV.c_str()) == 1) ? true : false);
      } else if (xsN == std::string("pressValIdx")) {
	setPressValIdx(atoi(xsV.c_str()));
      } else if (xsN == std::string("pressDir")) {
	setPressDir(atoi(xsV.c_str()));
      } else if (xsN == std::string("shearingCoeffVal")) {
	setShearingCoeffVal(atof(xsV.c_str()));
      } else if (xsN == std::string("shearingStencil")) {
	setShearingStencilWidth(atof(xsV.c_str()));
      } else if (xsN == std::string("shearingSurfVelocX")) {
	setShearingSurfVeloc(atof(xsV.c_str()), 0);
      } else if (xsN == std::string("shearingSurfVelocY")) {
	setShearingSurfVeloc(atof(xsV.c_str()), 1);
      } else if (xsN == std::string("shearingSurfVelocZ")) {
	setShearingSurfVeloc(atof(xsV.c_str()), 2);
      } else if (xsN == std::string("shearingMapCompo")) {
	setShearingMapCompo(atoi(xsV.c_str()));
      } else if (xsN == std::string("fricResistCoeffVal")) {
	setFricResistCoeffVal(atof(xsV.c_str()));
      } else if (xsN == std::string("fricResistStencil")) {
	setFricResistStencilWidth(atof(xsV.c_str()));
      } else if (xsN == std::string("fricResistSurfVelocX")) {
	setFricResistSurfVeloc(atof(xsV.c_str()), 0);
      } else if (xsN == std::string("fricResistSurfVelocY")) {
	setFricResistSurfVeloc(atof(xsV.c_str()), 1);
      } else if (xsN == std::string("fricResistSurfVelocZ")) {
	setFricResistSurfVeloc(atof(xsV.c_str()), 2);
      } else if (xsN == std::string("fricResistMapCompo")) {
	setFricResistMapCompo(atoi(xsV.c_str()));
      } else if (xsN == std::string("totalResistPressSphIdx")) {
	setPressSphIdx(atoi(xsV.c_str()));
      } else if (xsN == std::string("totalResistPressCompoIdx")) {
	setPressCompoIdx(atoi(xsV.c_str()));
      }
    }

  _NEXT_XML_NODE:
    cur = cur->next;
  }

  return true;
}

/**
 * 状態をXMLファイルに出力する
 * @param os 出力用ストリーム
 * @param ts タブ
 * @retval true 成功
 * @retval false 失敗
 */
bool 
vsnMethod_Shape_scalarMap::outputXML(std::ostream& os, const size_t ts)
{
  std::string idts;
  for (size_t i = 0; i < ts; ++i) idts.push_back(' ');
  string idts_2 = idts; idts_2.push_back(' '); idts_2.push_back(' ');
  bool ret = true;
  string msgHdr = getMethodType() + std::string("[") + getName() + std::string("]: outputXML: ");

  // output
  os << idts << "<method type=\"" << getMethodType() << "\"";
  if (!_name.empty() && _name != string(VFR_NONAME))
    os << " name=\"" << _name << "\"";
  os << " >" << endl;

  // sphIdx
  os << idts_2 << "<param name=\"sphIdx\" value=\"" << m_sphIdx << "\" />"
     << std::endl; 
  // renderingMode
  os << idts_2 << "<param name=\"renderingMode\" value=\"" << m_renderingMode
     << "\" />" << std::endl;
  // interpolateMode
  os << idts_2 << "<param name=\"interpolateMode\" value=\""
     << m_interpolateMode << "\" />" << std::endl;
  // physIdx
  os << idts_2 << "<param name=\"physIdx\" value=\"" << m_physIdx << "\" />"
     << std::endl;
  // updateMinMax
  os << idts_2 << "<param name=\"updateMinMax\" value=\"" << m_updateMinMax
     << "\" />" << std::endl;
  // NormalDirection
  os << idts_2 << "<param name=\"normalDirection\" value=\""
     << m_normalDirection << "\" />" << std::endl;
  // Divide
  //  os << idts_2 << "<param name=\"divideMode\" value=\"" << m_divideMode
  //     << "\" />" << std::endl;
  os << idts_2 << "<param name=\"exUseTrilinear\" value=\""
     << m_exUseTrilinear << "\" />" << std::endl;
  

  // scalarValIdx
  os << idts_2 << "<param name=\"scalarValIdx\" value=\""
     << m_scalarValIdx << "\" />" << std::endl;
  os << idts_2 << "<param name=\"dispContourLine\" value=\""
     <<m_dispContourLine << "\" />" << std::endl;
  os << idts_2 << "<param name=\"numLines\" value=\""
     << m_numLines << "\" />" << std::endl;
  os << idts_2 << "<param name=\"lineWidth\" value=\""
     << m_lineWidth << "\" />" << std::endl;
  os << idts_2 << "<param name=\"useCMap\" value=\""
     << m_useCMap << "\" />" << std::endl;
  os << idts_2 << "<param name=\"antiAlias\" value=\""
     << m_antiAlias << "\" />" << std::endl;

  // pressDrag
  os << idts_2 << "<param name=\"pressValIdx\" value=\""
     << m_pressValIdx << "\" />" << std::endl;
  os << idts_2 << "<param name=\"pressDir\" value=\""
     << m_pressDir << "\" />" << std::endl;

  // shearing
  os << idts_2 << "<param name=\"shearingCoeffVal\" value=\""
     << m_shearingCoeffVal     << "\" />" << std::endl;
  os << idts_2 << "<param name=\"shearingStencil\" value=\""
     << m_shearingStencil      << "\" />" << std::endl;
  os << idts_2 << "<param name=\"shearingSurfVelocX\" value=\""
     << m_shearingSurfVeloc[0] << "\" />" << std::endl;
  os << idts_2 << "<param name=\"shearingSurfVelocY\" value=\""
     << m_shearingSurfVeloc[1] << "\" />" << std::endl;
  os << idts_2 << "<param name=\"shearingSurfVelocZ\" value=\""
     << m_shearingSurfVeloc[2] << "\" />" << std::endl;
  os << idts_2 << "<param name=\"shearingMapCompo\" value=\""
     << m_shearingMapCompo     << "\" />" << std::endl;
  os << idts_2 << "<param name=\"totalResistPressSphIdx\" value=\""
     << m_totalResistPressSphIdx   << "\" />" << std::endl;
  os << idts_2 << "<param name=\"totalResistPressCompoIdx\" value=\""
     << m_totalResistPressCompoIdx << "\" />" << std::endl;

#ifdef OLD
  // FricResist
  os << idts_2 << "<param name=\"fricResistCoeffVal\" value=\""
     << m_fricResistCoeffVal     << "\" />" << std::endl;
  os << idts_2 << "<param name=\"fricResistStencil\" value=\""
     << m_fricResistStencil      << "\" />" << std::endl;
  os << idts_2 << "<param name=\"fricResistSurfVelocX\" value=\""
     << m_fricResistSurfVeloc[0] << "\" />" << std::endl;
  os << idts_2 << "<param name=\"fricResistSurfVelocY\" value=\""
     << m_fricResistSurfVeloc[1] << "\" />" << std::endl;
  os << idts_2 << "<param name=\"fricResistSurfVelocZ\" value=\""
     << m_fricResistSurfVeloc[2] << "\" />" << std::endl;
  os << idts_2 << "<param name=\"fricResistMapCompo\" value=\""
     << m_fricResistMapCompo     << "\" />" << std::endl;
#endif // OLD

  // output base method params
  if ( ! vsnMethodObj::exportXMLNode(os, ts+2) ) {
    ErrMsg(MsgERR, msgHdr + string("base MethodObj exportXMLNode failed"));
    ret = false;
  }

  os << idts << "</method>" << endl;
  return true;
}

/**
 * 継承関数
 */
bool
vsnMethod_Shape_scalarMap::commandXML(xmlNodePtr xnp)
{
  /**
   * 何もしない
   */
  return true;
}

/**
 * スカラーを設定する
 * @param val 値
 * @retval true 成功
 * @retval false 失敗
 */
bool
vsnMethod_Shape_scalarMap::setScalarValIdx(int val)
{
  if (m_scalarValIdx == val) return true;
  m_scalarValIdx = val;

  /**
   * 表示範囲の設定も行う
   */
  adjustRange();

  if (! update()) return false;
  updateUI();
  return true;
}

/**
 * SPH を選択する
 * @param val SPH のインデックス
 * @retval true 成功
 * @retval false 失敗
 */
bool
vsnMethod_Shape_scalarMap::setSphIdx(int val)
{
  if (m_sphIdx == val) return true;
  m_sphIdx = val;

  /**
   * 表示範囲の設定も行う
   */
  adjustRange();

  if (! update()) return false;
  updateUI();
  return true;
}

/**
 * レンダリングモードを設定する
 * @param val レンダリングモード
 * @retval true 成功
 * @retval false 失敗
 */
bool
vsnMethod_Shape_scalarMap::setRenderingMode(int val)
{
  if (m_renderingMode == val) return true;
  m_renderingMode = val;

  /**
   * 表示範囲の設定も行う
   */
  adjustRange();

  if (! update()) return false;
  updateUI();
  return true;
}

/**
 * 補間のモードを設定する
 * @param val 補間モード
 * @retval true 成功
 * @retval false 失敗
 */
bool
vsnMethod_Shape_scalarMap::setInterpolateMode(int val)
{
  if (m_interpolateMode == val) return true;
  m_interpolateMode = val;

  /**
   * 表示範囲の設定も行う
   */
  adjustRange();

  if (! update()) return false;
  updateUI();
  return true;
}

/**
 * レンジを設定する
 * @param updval 強制的に更新するかどうか
 */
void 
vsnMethod_Shape_scalarMap::adjustRange(const bool updval)
{
  // 表示物理量がスカラー値の時だけ
  //

  /**
   * 表示物理量により処理を切り替える
   */

  bool needRangeUpd = false;

  if (m_physIdx == 0) {
    // スカラー値の表示
    //
    vsnData_Shape* pData = dynamic_cast<vsnData_Shape*>(p_refData);
    if (! pData) return;

    if (m_sphIdx == 0) return;

    if (m_sphList.empty()) return;
    if (!(m_sphList.size() > m_sphIdx - 1)) return;

    vsnData_Sph* sph = m_sphList[m_sphIdx - 1];
    if (! sph) return;

    int dlen = (int)sph -> getDataLen();
    float minmax[2];
    if (m_scalarValIdx == 3) {
      // VecLen
      CES::Vec3<int> vecDataIdx(0, 1, 2);
      sph -> getVectorMaxLen(vecDataIdx, minmax[1]);
      minmax[0] = 0;
      m_minmax[0] = minmax[0];
      m_minmax[1] = minmax[1];
      if (m_updateMinMax) {
	m_lut.minVal = minmax[0];
	m_lut.maxVal = minmax[1];
	needRangeUpd = true;
      }
    } else if (0 <= m_scalarValIdx && m_scalarValIdx < dlen) {
      // Component
      if (sph -> getMinMax(m_scalarValIdx, minmax)) {
	m_minmax[0] = minmax[0];
	m_minmax[1] = minmax[1];
	if (m_updateMinMax) {
	  m_lut.minVal = minmax[0];
	  m_lut.maxVal = minmax[1];
	  needRangeUpd = true;
	}
      }
      setUseLut(true);
    } else {
      setUseLut(false);
    }
  } else if (m_physIdx == 1) {
    if (getPressDragValue()) {
      if (m_updateMinMax) {
	m_lut.minVal = m_minmax[0];
	m_lut.maxVal = m_minmax[1];
	needRangeUpd = true;
      }
      setUseLut(true);
    } else {
      setUseLut(false);
    }
  } else if (m_physIdx == 2 || m_physIdx == 3) {
    // せん断応力　または　摩擦抵抗
    //
    if (getShearingValue(IsShearingMode())) {
      if (m_updateMinMax) {
	m_lut.minVal = m_minmax[0];
	m_lut.maxVal = m_minmax[1];
	needRangeUpd = true;
      }
      setUseLut(true);
    } else {
      setUseLut(false);
    }
  } else if (m_physIdx == 4) {
    // 全抵抗
    //
    if (getTotalResistValue()) {
      if (m_updateMinMax) {
	m_lut.minVal = m_minmax[0];
	m_lut.maxVal = m_minmax[1];
	needRangeUpd = true;
      }
      setUseLut(true);
    } else {
      setUseLut(false);
    }
  }

  if (needRangeUpd) {
    set<vsnMethodLutRefer*>::iterator it;
    for (it = m_mlrLst.begin(); it != m_mlrLst.end(); it++)
      if (*it) (*it) -> updateLut();
  }
}

/**
 * update minmax を更新
 * @param mode モード
 * @retval true 成功
 * @retval false 失敗
 */
bool
vsnMethod_Shape_scalarMap::setUpdateMinMaxMode(const bool mode)
{
  if (m_updateMinMax == mode) return true;
  m_updateMinMax = mode;

  // data range
  adjustRange(false);

  if ( ! update() ) return false;
  updateUI();

  return true;
}

/**
 * update 法線の向きを更新
 * @param val モード
 * @retval true 成功
 * @retval false 失敗
 */
bool
vsnMethod_Shape_scalarMap::setNormalDirection(int val)
{
  if (m_normalDirection == val) return true;
  m_normalDirection = val;

  // data range
  adjustRange(false);

  if ( ! update() ) return false;
  updateUI();

  return true;
}

/**
 * SPHを取得する
 * @param idx インデックス
 * @return SPHデータ
 */
vsnData_Sph*
vsnMethod_Shape_scalarMap::getSph(size_t idx)
{
  vsnData_Sph* result = NULL;
  if ( idx < m_sphList.size() ) {
    result = m_sphList[idx];
  }
  return result;
}

/**
 * 現在選択されている SPH を取得する
 * @return SOH
 */
vsnData_Sph*
vsnMethod_Shape_scalarMap::getSelectedSph(void)
{
  return getSph(m_sphIdx - 1);
#ifdef OLD
  vsnData_Sph* result = NULL;
  if (m_sphIdx != 0) {
    if (m_sphList.size() - 1 < m_sphIdx) {
      result = m_sphList[m_sphIdx - 1];
    }
  }
  return result;
#endif // OLD
}

/**
 * 表示する物理量を変更する
 * @param val インデックス
 * @retval true 成功
 * @retval false 失敗
 */
bool
vsnMethod_Shape_scalarMap::setPhysIdx(int val)
{
  if (m_physIdx == val) return true;
  m_physIdx = val;

  adjustRange();

  if (! update()) return false;
  updateUI();
  return true;
}

/**
 * 圧力値のインデックスを設定する
 * @param val 圧力値のインデックス
 * @retval true 成功
 * @retval false 失敗
 */
bool
vsnMethod_Shape_scalarMap::setPressValIdx(int val)
{
  if (m_pressValIdx == val) return true;
  m_pressValIdx = val;

  adjustRange();

  if (! update()) return false;
  updateUI();
  return true;
}

/**
 * 圧力の方向を設定する
 * @param val 方向
 * @retval true 成功
 * @retval false 失敗
 */
bool
vsnMethod_Shape_scalarMap::setPressDir(int val)
{
  if (m_pressDir == val) return true;
  m_pressDir = val;

  adjustRange();

  if (! update()) return false;
  updateUI();
  return true;
}

// ----------------------------------------------------------

/**
 * 剪断応力のステンシル幅を設定する
 * @param val 幅
 * @retval true 成功
 * @retval false 失敗
 */
bool
vsnMethod_Shape_scalarMap::setShearingStencilWidth(float val)
{
  if (m_shearingStencil == val) return true;
  m_shearingStencil = val;

  adjustRange();

  if (! update()) return false;
  updateUI();

  return true;
}

/**
 * 表面速度を設定する
 * @param val 値
 * @param dir 方向
 * @retval true 成功
 * @retval false 失敗
 */
bool
vsnMethod_Shape_scalarMap::setShearingSurfVeloc(float val, int dir)
{
  if (!(dir == 0 || dir == 1 || dir == 2)) return false;
  if (m_shearingSurfVeloc[dir] == val) return true;
  m_shearingSurfVeloc[dir] = val;

  adjustRange();

  if (! update()) return false;
  updateUI();

  return true;
}

/**
 * 粘性係数を設定する
 * @param val 値
 * @retval true 成功
 * @retval false 失敗
 */
bool
vsnMethod_Shape_scalarMap::setShearingCoeffVal(float val)
{
  if (m_shearingCoeffVal == val) return true;
  m_shearingCoeffVal = val;

  adjustRange();

  if (! update()) return false;
  updateUI();

  return true;
}

/**
 * マップするコンポーネントを設定する
 * @param val 値
 * @retval true 成功
 * @retval false 失敗
 */
bool
vsnMethod_Shape_scalarMap::setShearingMapCompo(int val)
{
  if (m_shearingMapCompo == val) return true;
  m_shearingMapCompo = val;

  adjustRange();

  if (! update()) return false;
  updateUI();

  return true;
}

/**
 * 全抵抗：SPHのインデックスを変更する
 * @param val 値
 * @retval true 成功
 * @retval false 失敗
 */
bool
vsnMethod_Shape_scalarMap::setPressSphIdx(int val)
{
  if (m_totalResistPressSphIdx == val) return true;
  m_totalResistPressSphIdx = val;

  /**
   * SPH のデータが変更されたら、圧力のコンポーネントは０で初期化する
   */
  m_totalResistPressCompoIdx = 0;

  adjustRange();

  if (! update()) return false;
  updateUI();

  return true;
}

/**
 * 圧力値のインデックスを設定する
 * @param val 値
 * @retval true 成功
 * @retval false 失敗
 */
bool
vsnMethod_Shape_scalarMap::setPressCompoIdx(int val)
{
  if (m_totalResistPressCompoIdx == val) return true;
  m_totalResistPressCompoIdx = val;

  adjustRange();

  if (! update()) return false;
  updateUI();

  return true;
}

// -----------------------------------------------------------

/**
 * 面の情報を保持するための領域を確保する
 * @param numFaces 面の数
 * @retval true 成功
 * @retval false 失敗
 */
bool
vsnMethod_Shape_scalarMap::allocFaces(size_t numFaces)
{
  if (m_faceGravity) {    delete[] m_faceGravity;    m_faceGravity = NULL;  }
  if ((m_faceGravity = new VFR::vector3[numFaces]) == NULL) return false;

  if (m_faceArea) {    delete[] m_faceArea;    m_faceArea = NULL;  }
  if ((m_faceArea = new double[numFaces]) == NULL) return false;

  if (m_faceProjectionArea) {
    for (int i = 0; i < 3; ++i) delete[] m_faceProjectionArea[i];
    delete[] m_faceProjectionArea;
  }

  if ((m_faceProjectionArea = new double*[3]) == NULL) return false;
  for (int i = 0; i < 3; ++i) {
    if ((m_faceProjectionArea[i] = new double[numFaces]) == NULL) return false;
  }

  m_numFaces = numFaces;
  return true;
}

/**
 * コンターラインの数を設定する
 * @param nl 値
 * @retval true 成功
 * @retval false 失敗
 */
bool 
vsnMethod_Shape_scalarMap::setNumLines(const size_t nl)
{
  if ( m_numLines == nl ) return true;
  if ( nl < 1 ) return false;
  m_numLines = nl;

  if ( ! update() ) return false;
  updateUI();
  return true;
}

/**
 * コンターラインのレンジを設定する
 * @param min 最小値
 * @param max 最大値
 * @retval true 成功
 * @retval false 失敗
 */
bool
vsnMethod_Shape_scalarMap::setContourRange(const float min, const float max) 
{
  if ( m_minmax[0] == min && m_minmax[1] == max ) return true;
  if ( min > max ) return false;
  m_minmax[0] = min; m_minmax[1] = max;

  if ( ! update() ) return false;
  updateUI();
  return true;
}

/**
 * コンターラインの表示範囲の取得
 * @param min 最小値
 * @param max 最大値
 */
void
vsnMethod_Shape_scalarMap::getContourRange(float* min, float* max) const
{
  *min = m_minmax[0];
  *max = m_minmax[1];
}

/**
 * コンターラインの線幅の設定
 * @param lw 値
 * @retval true 成功
 * @retval false 失敗
 */
bool 
vsnMethod_Shape_scalarMap::setLineWidth(const float lw)
{
  if (m_lineWidth == lw) return true;
  if (lw <= 0.f) return false;
  m_lineWidth = lw;

  if (! update()) return false;
  updateUI();
  return true;
}

/**
 * コンターラインのカラーマップ使用の有無を設定する
 * @param ucm 値
 * @retval true 成功
 * @retval false 失敗
 */
bool
vsnMethod_Shape_scalarMap::setUseCMap(const bool ucm)
{
  if (m_useCMap == ucm) return true;
  m_useCMap = ucm;

  //  setUseLut(m_useCMap);
  if (! update()) return false;
  updateUI();

  return true;
}

/**
 * コンターラインを表示するかどうかを設定する
 * @param val 値
 * @retval true 成功
 * @retval false 失敗
 */
bool
vsnMethod_Shape_scalarMap::setDispContourLine(const bool val)
{
  if (m_dispContourLine == val) return true;
  m_dispContourLine = val;

  if (! update()) return false;
  updateUI();

  return true;
}

// void 
// vsnMethod_Shape_scalarMap::setUseLutContourLine(const bool ulm)
// {
//   if (m_useLut == ulm) return;
//   m_useLut = ulm;

//   if (m_useLut) {
//     if (m_numLines == m_numValidLines) {
//       register float val,
// 	dv = (m_numLines < 2) ? 0.f : (m_max - m_min)/(float)(m_numLines - 1);
//       register int k;
//       for ( val = m_min, k = 0; k < m_numLines; val += dv, k++ ) {
// 	vfrLines* plns = dynamic_cast<vfrLines*>(getChild(k));
// 	if ( ! plns ) continue;
// 	plns -> setColor3(0, &m_lut.lutEntry[m_lut.getValIdx(val) * 4]);
//       } // end of for(val, k)
//     }
//     else {
//       update();
//     }
//   } else {
//     setBaseColor(m_colour);
//   }

//   chkNotice();
// }

/**
 * 設定されているパラメータを元に、表示を行う。
 * vsnScene::importDataXML から呼ばれる。
 */
void
vsnMethod_Shape_scalarMap::viewByParam(void)
{
  loadSph();
  adjustRange();
  update();
  updateUI();
  chkNotice();
}

/**
 * コンターラインの削除
 */
void
vsnMethod_Shape_scalarMap::deleteContourLines(void)
{
  size_t n = getNumChildren();
  std::vector<vfrNode*> linesV;
  for (size_t i = 0; i < n; i++ ) {
    vfrLines* p = dynamic_cast<vfrLines*>(getChild(i));
    if (! p) continue;
    linesV.push_back(p);
  } // end of for(i)
  for (std::vector<vfrNode*>::iterator it = linesV.begin();
       it != linesV.end(); ++it) {
    remChild(*it);
  }
}

#ifdef OLD
bool
vsnMethod_Shape_scalarMap::setDivideMode(bool mode)
{
  if (m_divideMode == mode) return true;
  m_divideMode = mode;

  generateTrias(true);
  adjustRange(false);

  if (! update()) return false;
  updateUI();

  return true;
}
#endif // OLD

/**
 * 外挿点の値を求めるときにtrilinear を使うかどうかを設定する
 * @param mode 値
 * @retval true 成功
 * @retval false 失敗
 */
bool
vsnMethod_Shape_scalarMap::setExUseTrilinear(bool mode)
{
  if (m_exUseTrilinear == mode) return true;
  m_exUseTrilinear = mode;

  //  generateTrias(true);
  adjustRange(false);

  if (! update()) return false;
  updateUI();

  return true;
}


#include "vsnSTLext.h"

/**
 * STLext フォーマットでのファイル出力
 * @param outPath 出力するパス名
 * @exception std::runtime_error エラー
 * @since 2.2.1
 */
void
vsnMethod_Shape_scalarMap::SaveStlExt(const std::string& outPath)
{
  if (m_allValues.empty()) throw std::runtime_error("No Data");

  STLD::STLext stlExt;

  VFR::AppearType mode = (m_renderingMode == 0) ? AT_PER_FACE : AT_PER_VERTEX;

  if (mode == AT_PER_FACE) {
    // 面毎
    //

    // 面モード
    //
    stlExt.SetFaceMode();
    
    // 面の数
    //
    const size_t valNum = m_allValues.size();

    // 面の数の設定
    //
    stlExt.SetFaceNum(valNum);

    if (m_allValues3.empty()) {
      // 物理量の定義
      //
      size_t physNum = stlExt.AppendPhysInfo(STLD::PhysInfo(2, 1));

      // 物理量の設定
      //
      for (size_t i = 0; i < valNum; ++i) {
	std::vector<float> tmp(1);
	tmp[0] = m_allValues[i];
	if (! stlExt.SetData(i, physNum, tmp)) {
	  throw std::runtime_error("set data failed");
	}
      }
    } else {
      size_t physNum = stlExt.AppendPhysInfo(STLD::PhysInfo(2, 3));

      for (size_t i = 0; i < valNum; ++i) {
	std::vector<float> tmp(3);
	tmp[0] = m_allValues3[i][0];
	tmp[1] = m_allValues3[i][1];
	tmp[2] = m_allValues3[i][2];
	if (! stlExt.SetData(i, physNum, tmp)) {
	  throw std::runtime_error("set data failed");
	}

      }
    }

    // ファイルへの保存
    //
    if (! stlExt.Save(outPath)) {
      throw std::runtime_error("Save failed");
    }

  } else {
    // 頂点毎
    //

    if (! m_trias) throw std::runtime_error("No STL Data");
    VFR::vector3* verts = m_trias -> getVerts();
    if (! verts) throw std::runtime_error("No Vertex Data");

    // 頂点の数
    //
    const size_t valNum = m_allValues.size();

    if (valNum != m_trias -> getNumVerts())
      throw std::runtime_error("ValueNum not equal VertexNum");

    // 頂点モード
    //
    stlExt.SetVertexMode();

    // 頂点数の設定
    //
    stlExt.SetFaceNum(valNum);
    
    if (m_allValues3.empty()) {
      // 物理量の定義
      //
      size_t physNum = stlExt.AppendPhysInfo(STLD::PhysInfo(2, 1));

      // 物理量の設定
      //
      for (size_t i = 0; i < valNum; ++i) {
	std::vector<float> tmp(1);
	tmp[0] = m_allValues[i];
	if (! stlExt.SetData(i, physNum, tmp)) {
	  throw std::runtime_error("set data failed");
	}
      }
    } else {
      // 物理量の定義
      //
      size_t physNum = stlExt.AppendPhysInfo(STLD::PhysInfo(2, 3));

      // 物理量の設定
      //
      for (size_t i = 0; i < valNum; ++i) {
	std::vector<float> tmp(3);
	tmp[0] = m_allValues3[i][0];
	tmp[1] = m_allValues3[i][1];
	tmp[2] = m_allValues3[i][2];
	if (! stlExt.SetData(i, physNum, tmp)) {
	  throw std::runtime_error("set data failed");
	}
      }
    }

    // ファイルへの保存
    //
    if (! stlExt.Save(outPath)) {
      throw std::runtime_error("Save failed");
    }

  }
}
