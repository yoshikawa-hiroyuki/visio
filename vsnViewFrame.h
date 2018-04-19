//
// vsnViewFrame
//
#ifndef _VSN_VIEW_FRAME_H_
#define _VSN_VIEW_FRAME_H_

#include "vsnFrameBase.h"
#include "vsnGfxView.h"
#include "vsnUiView.h"
#include "vsnScene.h"
#include "vsnViewPoint.h"
#include "vsnRotScaleCenter.h"
#include <string>
#include <deque>

namespace VSN {
  // menu ids
  enum {ViewFrameMenu_File_New = 1100,
	ViewFrameMenu_File_Open,
	ViewFrameMenu_File_Import,
	ViewFrameMenu_File_Imp_VsnXML,
	ViewFrameMenu_File_Imp_Sph,
	ViewFrameMenu_File_Imp_P3dF,
	ViewFrameMenu_File_Imp_FdvStr,
	ViewFrameMenu_File_Imp_Dfi,
	ViewFrameMenu_File_Imp_OctVol,
	ViewFrameMenu_File_Imp_Scatter,
	ViewFrameMenu_File_Imp_ScatterTS,
	ViewFrameMenu_File_Imp_TriaCells,
	ViewFrameMenu_File_Imp_TriaCellsTS,
	ViewFrameMenu_File_Imp_Shape,
	ViewFrameMenu_File_Imp_ShapeTS,
	ViewFrameMenu_File_Imp_ShapeLOD,
	// MHIR append begin
	ViewFrameMenu_File_Imp_ParaXVX,
	// MHIR append end
	ViewFrameMenu_File_Imp_SvSQL,
	ViewFrameMenu_File_Save,
	ViewFrameMenu_File_SaveAs,
	ViewFrameMenu_File_OprProp,
	ViewFrameMenu_File_NewParamCanvas,
	ViewFrameMenu_File_ParamCanvasList,
	ViewFrameMenu_File_ExecCommand,
	ViewFrameMenu_File_Quit,
	ViewFrameMenu_View_New,
	ViewFrameMenu_View_Close,
	ViewFrameMenu_View_Geometry,
	ViewFrameMenu_View_Layout,
	ViewFrameMenu_View_UiShow,
	ViewFrameMenu_View_AuxUiShow,
	ViewFrameMenu_View_ShowToolBar,
	ViewFrameMenu_View_Perspective,
	ViewFrameMenu_View_Normalize,
	ViewFrameMenu_View_NormalizeScene,
	ViewFrameMenu_View_SetViewPoint,
	ViewFrameMenu_View_SyncViewPoint,
	ViewFrameMenu_View_SetCenter,
	ViewFrameMenu_View_CenterShow,
	ViewFrameMenu_View_NormCenter,
	ViewFrameMenu_View_FrAxisShow,
	ViewFrameMenu_View_SetBgColor,
	ViewFrameMenu_View_Shot,
	ViewFrameMenu_View_ShotAndExec,
	ViewFrameMenu_Scene_Add,
	ViewFrameMenu_Scene_Del,
	ViewFrameMenu_Scene_Rename,
	ViewFrameMenu_Scene_Anchor,
	ViewFrameMenu_Scene_LightAttr,
	ViewFrameMenu_Help_Manual,
	ViewFrameMenu_Help_About,
	ViewFrameTooBar_NormView,
	ViewFrameTooBar_ProjPers,
	ViewFrameTooBar_ProjOrtho,
	ViewFrameTooBar_Front,
	ViewFrameTooBar_Back,
	ViewFrameTooBar_Right,
	ViewFrameTooBar_Left,
	ViewFrameTooBar_Top,
	ViewFrameTooBar_Bottom,
	ViewFrameTooBar_AnimPlayBtn,
	ViewFrameTooBar_AnimStopBtn,
	ViewFrameTooBar_AnimRewBtn,
	ViewFrameTooBar_AnimFwdBtn,
	ViewFrameTooBar_AnimSelCombo,
  };
};


//----------------------------------------------------------------
// class vsnViewFrame
//----------------------------------------------------------------
class vsnViewFrame : public vsnFrameBase, public vsnIoObject {
public:
  vsnViewFrame(wxWindow* parent, const wxString& title,
	       const std::string& name = std::string(VFR_NONAME),
	       const wxPoint& pos =wxDefaultPosition,
	       const wxSize& size =wxDefaultSize,
	       long style = wxDEFAULT_FRAME_STYLE);
  virtual ~vsnViewFrame();

  void reset();
  void refresh(const bool reSelObj =false);
  bool closeView();
  size_t getSeqNo() const {return m_seq;}

  void setName(const std::string& name) {m_name = name;}
  std::string getName() const {return m_name;}

  vsnGfxView* getGfxView() {return m_pGfxView;}
  vsnUiView* getUiView() {return m_pUiView;}
  void setShowUiView(const bool suivm);
  bool getShowUiView() const {return m_showUiView;}

  void invalidateAuxUiFrame(class vsnAuxUiFrame* pauf);
  class vsnAuxUiFrame* getAuxUiFrame() {return m_pAuxUiFrame;}
  void setShowAuxUiFrame(const bool sauifm);
  bool getShowAuxUiFrame() const {return m_showAuxUiFrame;}

  void setShowToolBar(const bool stb);
  bool getShowToolBar() const {return m_showToolBar;}

  bool updateSceneList(const std::deque<vsnScene*>& scl);
  bool setGeometry(const int x, const int y, const int w, const int h);

  std::deque<wxColor>& getWxColorList() {return m_wxColorLst;}
  bool setBgColor(const vector4 bgc);
  void getBgColor(vector4 bgc) const;

  bool setPerspective(const bool pm);
  bool getPerspective() const;

  bool isSetCenterMode() const {return m_setCenterMode;}
  bool enterSetCenterMode();
  void escapeSetCenterMode();

  void setShowCenter(const bool scm);
  bool getShowCenter() const {return m_showCenter;}
  void setNormCenter(const bool scm);
  bool getNormCenter() const {return m_normalizeCenter;}
  void setShowFrAxis(const bool sfa);
  bool getShowFrAxis() const {return m_showFrAxis;}

  std::map<std::string, vsnViewPoint>& getViewPointList() {
    return m_viewPointLst;
  }
  vsnViewPoint getViewPoint(const std::string& vpname) const;
  bool addViewPoint(const std::string& vpname);
  bool delViewPoint(const std::string& vpname);
  bool updateViewPoint(const std::string& vpname, const bool xanim =true);
  void clearViewpointList();

  bool addViewPointSlave(vsnViewFrame* slave);
  bool delViewPointSlave(vsnViewFrame* slave);
  bool setViewPointMaster(vsnViewFrame* master);
  vsnViewFrame* getViewPointMaster() {return p_vpRefMaster;}
  const std::set<vsnViewFrame*>& getViewPointSlaveList() const {
    return m_vpRefSlaveLst;
  }

  void normalizeViewport(const bool tgtScene);
  vsnLightAttributeDlg* getLightAttDlg() {return m_pLightAttrDlg;}

  Point2 getScreenShotSize() const {
    if ( ! m_pGfxView ) return Point2();
    return m_pGfxView->getScreenShotSize();
  }
  unsigned char* getScreenShotImage(const bool flip) {
    if ( ! m_pGfxView ) return NULL;
    return m_pGfxView->screenShot(flip);
  }

  std::string getSshotCommStr() const {return m_scshotCommStr;}
  void setSshotCommStr(const std::string& str) {m_scshotCommStr = str;}

  // from vsnFrameBase
  virtual void setApp(class vsnApp* pp);
  virtual void setScene(class vsnScene* ps);
  virtual bool adjustSelection(vsnTreeItem* pti);
  virtual bool setStatusText(const std::string& msg);

  // from vsnIoObject
  virtual bool parseXML(xmlNodePtr xnp);
  virtual bool outputXML(std::ostream& os, const size_t ts =0);
  virtual bool commandXML(xmlNodePtr xnp);

  // static utils
  static void UpdateAllLightAttrDlg();

private:
  static size_t    s_viewSeq;

  size_t           m_seq;
  std::string      m_name;
  vsnGfxView*      m_pGfxView;
  vsnUiView*       m_pUiView; // Wx object, don't delete
  class vsnAuxUiFrame*
                   m_pAuxUiFrame; // Wx object, don't delete
  vector4          m_bgColor;
  bool             m_showUiView;
  bool             m_showAuxUiFrame;
  bool             m_showToolBar;
  bool             m_setCenterMode;
  bool             m_showCenter;
  bool             m_normalizeCenter;
  bool             m_showFrAxis;
  std::map<std::string, vsnViewPoint>
                   m_viewPointLst;
  std::deque<wxColor>
                   m_wxColorLst;
  std::string      m_scshotCommStr;

  vsnViewPointDlg* m_pViewPointDlg; // Wx object, don't delete
  vsnViewFrame*    p_vpRefMaster;
  std::set<vsnViewFrame*>
                   m_vpRefSlaveLst;
  mutable std::map<std::string, vsnViewPoint>
                   m_lastViewPointLst;

  vsnLightAttributeDlg*
                   m_pLightAttrDlg; // Wx object, don't delete

  bool updateTBAnimSelector();

  // GfxView actions for SetCenter
  vsnGfxAct_RotScene        gfxAct_Rot;
  vsnGfxAct_RollScene       gfxAct_Roll;
  vsnGfxAct_ScaleScene      gfxAct_Scale;
  vsnGfxAct_TransScene      gfxAct_Trans;
  vsnGfxAct_KeyEscCenter    gfxAct_KEC;
  vsnGfxAct_ClickCenter     gfxAct_CC;
  vsnGfxAct_DragTransCenter gfxAct_DTC;
  vsnGfxAct_DragDollyCenter gfxAct_DDC;
  vsnGfxAct_DragEndCenter   gfxAct_DEC;

  // create menu-bar, status-bar
  bool setupMenuBar();
  bool setupStatusBar();
  bool setupToolBar();

  // event handlers
  void OnMenuFile_New(wxCommandEvent& event);
  void OnMenuFile_Open(wxCommandEvent& event);
  void OnMenuFile_Imp_VsnXML(wxCommandEvent& event);
  void OnMenuFile_Imp_Sph(wxCommandEvent& event);
  void OnMenuFile_Imp_P3dF(wxCommandEvent& event);
  void OnMenuFile_Imp_FdvStr(wxCommandEvent& event);
  void OnMenuFile_Imp_Dfi(wxCommandEvent& event);
  void OnMenuFile_Imp_OctVol(wxCommandEvent& event);
  void OnMenuFile_Imp_Scatter(wxCommandEvent& event);
  void OnMenuFile_Imp_ScatterTS(wxCommandEvent& event);
  void OnMenuFile_Imp_TriaCells(wxCommandEvent& event);
  void OnMenuFile_Imp_TriaCellsTS(wxCommandEvent& event);
  void OnMenuFile_Imp_Shape(wxCommandEvent& event);
  void OnMenuFile_Imp_ShapeTS(wxCommandEvent& event);
  void OnMenuFile_Imp_ShapeLOD(wxCommandEvent& event);
  void OnMenuFile_Imp_SvSQL(wxCommandEvent& event);
  void OnMenuFile_Save(wxCommandEvent& event);
  void OnMenuFile_SaveAs(wxCommandEvent& event);
  void OnMenuFile_OprProp(wxCommandEvent& event);
  void OnMenuFile_NewParamCanvas(wxCommandEvent& event);
  void OnMenuFile_ParamCanvasList(wxCommandEvent& event);
  void OnMenuFile_ExecCommand(wxCommandEvent& event);
  void OnMenuFile_Quit(wxCommandEvent& event);
  void OnMenuView_New(wxCommandEvent& event);
  void OnMenuView_Close(wxCommandEvent& event);
  void OnMenuView_Geometry(wxCommandEvent& event);
  void OnMenuView_Layout(wxCommandEvent& event);
  void OnMenuView_UiShow(wxCommandEvent& event);
  void OnMenuView_AuxUiShow(wxCommandEvent& event);
  void OnMenuView_ShowToolBar(wxCommandEvent& event);
  void OnMenuView_Perspective(wxCommandEvent& event);
  void OnMenuView_SetBgColor(wxCommandEvent& event);
  void OnMenuView_Normalize(wxCommandEvent& event);
  void OnMenuView_NormalizeScene(wxCommandEvent& event);
  void OnMenuView_SetViewPoint(wxCommandEvent& event);
  void OnMenuView_SyncViewPoint(wxCommandEvent& event);
  void OnMenuView_SetCenter(wxCommandEvent& event);
  void OnMenuView_CenterShow(wxCommandEvent& event);
  void OnMenuView_NormCenter(wxCommandEvent& event);
  void OnMenuView_FrAxisShow(wxCommandEvent& event);
  void OnMenuView_Shot(wxCommandEvent& event);
  void OnMenuView_ShotAndExec(wxCommandEvent& event);
  void OnMenuScene_Add(wxCommandEvent& event);
  void OnMenuScene_Del(wxCommandEvent& event);
  void OnMenuScene_Rename(wxCommandEvent& event);
  void OnMenuScene_Anchor(wxCommandEvent& event);
  void OnMenuScene_LightAttr(wxCommandEvent& event);
  void OnMenuHelp_Manual(wxCommandEvent& event);
  void OnMenuHelp_About(wxCommandEvent& event);
  void OnUpdateMenuScene_Anchor(wxUpdateUIEvent& event);
  void OnToolBar_Normalize(wxCommandEvent& event);
  void OnToolBar_Projection(wxCommandEvent& event);
  void OnToolBar_ViewDirs(wxCommandEvent& event);
  void OnToolBar_AnimControl(wxCommandEvent& event);
  void OnClose(wxCloseEvent& event);
  // MHIR append begin
  void OnMenuFile_Imp_ParaXVX(wxCommandEvent& event);
  // MHIR append end

  DECLARE_EVENT_TABLE()
};

#endif // _VSN_VIEW_FRAME_H_
