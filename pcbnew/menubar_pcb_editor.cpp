/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2017 Jean-Pierre Charras, jp.charras at wanadoo.fr
 * Copyright (C) 2012 SoftPLC Corporation, Dick Hollenbeck <dick@softplc.com>
 * Copyright (C) 2012 Wayne Stambaugh <stambaughw@gmail.com>
 * Copyright The KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include <advanced_config.h>
#include <bitmaps.h>
#include <file_history.h>
#include <kiface_base.h>
#include <pcb_edit_frame.h>
#include <pcbnew_id.h>
#include <python_scripting.h>
#include <tool/action_manager.h>
#include <tool/actions.h>
#include <tool/tool_manager.h>
#include <tools/pcb_actions.h>
#include <tools/pcb_selection_tool.h>
#include <widgets/wx_menubar.h>


void PCB_EDIT_FRAME::doReCreateMenuBar()
{
    PCB_SELECTION_TOOL* selTool = m_toolManager->GetTool<PCB_SELECTION_TOOL>();
    // wxWidgets handles the Mac Application menu behind the scenes, but that means
    // we always have to start from scratch with a new wxMenuBar.
    wxMenuBar*  oldMenuBar = GetMenuBar();
    WX_MENUBAR* menuBar    = new WX_MENUBAR();

    // Recreate all menus:

    //-- File menu -----------------------------------------------------------
    //
    ACTION_MENU*   fileMenu = new ACTION_MENU( false, selTool );
    static ACTION_MENU* openRecentMenu;

    // RESTRICTED MODE: File > New, Open, Recent disabled
    // if( Kiface().IsSingle() )   // not when under a project mgr
    // {
    //     ... removed for restricted mode
    // }

    // fileMenu->Add( PCB_ACTIONS::appendBoard );  // RESTRICTED MODE: disabled
    fileMenu->AppendSeparator();

    fileMenu->Add( ACTIONS::save );

    // Save as menu:
    // under a project mgr we do not want to modify the board filename
    // to keep consistency with the project mgr which expects files names same as prj name
    // for main files
    if( Kiface().IsSingle() )
        fileMenu->Add( ACTIONS::saveAs );
    else
        fileMenu->Add( ACTIONS::saveCopy );

    fileMenu->Add( ACTIONS::revert );

    // RESTRICTED MODE: Rescue, Import, Export, Fabrication Outputs disabled
    // fileMenu->AppendSeparator();
    // fileMenu->Add( _( "Resc&ue" ), ... );
    // fileMenu->Add( submenuImport );
    // fileMenu->Add( submenuExport );
    // fileMenu->Add( submenuFabOutputs );

    // fileMenu->AppendSeparator();
    // fileMenu->Add( PCB_ACTIONS::boardSetup );  // RESTRICTED MODE: disabled

    // RESTRICTED MODE: Page Settings, Print, Plot disabled
    // fileMenu->AppendSeparator();
    // fileMenu->Add( ACTIONS::pageSettings );
    // fileMenu->Add( ACTIONS::print );
    // fileMenu->Add( ACTIONS::plot );

    fileMenu->AppendSeparator();
    fileMenu->AddQuitOrClose( &Kiface(), _( "PCB Editor" ) );

    //-- Edit menu -----------------------------------------------------------
    //
    ACTION_MENU* editMenu = new ACTION_MENU( false, selTool );

    editMenu->Add( ACTIONS::undo );
    editMenu->Add( ACTIONS::redo );

    editMenu->AppendSeparator();
    editMenu->Add( ACTIONS::cut );
    editMenu->Add( ACTIONS::copy );
    editMenu->Add( ACTIONS::paste );
    editMenu->Add( ACTIONS::pasteSpecial );
    editMenu->Add( ACTIONS::doDelete );

    editMenu->AppendSeparator();
    editMenu->Add( ACTIONS::selectAll );
    editMenu->Add( ACTIONS::unselectAll );

    editMenu->AppendSeparator();
    editMenu->Add( ACTIONS::find );

    // RESTRICTED MODE: Edit property dialogs disabled
    // editMenu->AppendSeparator();
    // editMenu->Add( PCB_ACTIONS::editTracksAndVias );
    // editMenu->Add( PCB_ACTIONS::editTextAndGraphics );
    // editMenu->Add( PCB_ACTIONS::editTeardrops );
    // editMenu->Add( PCB_ACTIONS::changeFootprints );
    // editMenu->Add( PCB_ACTIONS::swapLayers );
    // editMenu->Add( ACTIONS::gridOrigin );

    editMenu->AppendSeparator();
    editMenu->Add( PCB_ACTIONS::zoneFillAll );
    editMenu->Add( PCB_ACTIONS::zoneUnfillAll );
    editMenu->Add( PCB_ACTIONS::regenerateAllTuning );

    // RESTRICTED MODE: Delete tool and global deletions disabled
    // editMenu->AppendSeparator();
    // editMenu->Add( ACTIONS::deleteTool );
    // editMenu->Add( PCB_ACTIONS::globalDeletions );


    //----- View menu -----------------------------------------------------------
    //
    ACTION_MENU* viewMenu = new ACTION_MENU( false, selTool );

    ACTION_MENU* showHidePanels = new ACTION_MENU( false, selTool );
    showHidePanels->SetTitle( _( "Panels" ) );
    showHidePanels->Add( ACTIONS::showProperties,                 ACTION_MENU::CHECK );
    showHidePanels->Add( PCB_ACTIONS::showSearch,                 ACTION_MENU::CHECK );
    showHidePanels->Add( PCB_ACTIONS::showLayersManager,          ACTION_MENU::CHECK );
    showHidePanels->Add( PCB_ACTIONS::showNetInspector,           ACTION_MENU::CHECK );
    viewMenu->Add( showHidePanels );

    // RESTRICTED MODE: Footprint Browser and 3D Viewer disabled
    // viewMenu->AppendSeparator();
    // viewMenu->Add( ACTIONS::showFootprintBrowser );
    // viewMenu->Add( ACTIONS::show3DViewer );

    viewMenu->AppendSeparator();
    viewMenu->Add( ACTIONS::zoomInCenter );
    viewMenu->Add( ACTIONS::zoomOutCenter );
    viewMenu->Add( ACTIONS::zoomFitScreen );
    viewMenu->Add( ACTIONS::zoomFitObjects );
    viewMenu->Add( ACTIONS::zoomTool );
    viewMenu->Add( ACTIONS::zoomRedraw );

    viewMenu->AppendSeparator();
    // Drawing Mode Submenu
    ACTION_MENU* drawingModeSubMenu = new ACTION_MENU( false, selTool );
    drawingModeSubMenu->SetTitle( _( "&Drawing Mode" ) );
    drawingModeSubMenu->SetIcon( BITMAPS::add_zone );

    drawingModeSubMenu->Add( PCB_ACTIONS::zoneDisplayFilled,   ACTION_MENU::CHECK );
    drawingModeSubMenu->Add( PCB_ACTIONS::zoneDisplayOutline,  ACTION_MENU::CHECK );

    if( ADVANCED_CFG::GetCfg().m_ExtraZoneDisplayModes )
    {
        drawingModeSubMenu->Add( PCB_ACTIONS::zoneDisplayFractured,    ACTION_MENU::CHECK );
        drawingModeSubMenu->Add( PCB_ACTIONS::zoneDisplayTriangulated, ACTION_MENU::CHECK );
    }

    drawingModeSubMenu->AppendSeparator();
    drawingModeSubMenu->Add( PCB_ACTIONS::padDisplayMode,      ACTION_MENU::CHECK );
    drawingModeSubMenu->Add( PCB_ACTIONS::viaDisplayMode,      ACTION_MENU::CHECK );
    drawingModeSubMenu->Add( PCB_ACTIONS::trackDisplayMode,    ACTION_MENU::CHECK );

    drawingModeSubMenu->AppendSeparator();
    drawingModeSubMenu->Add( PCB_ACTIONS::graphicsOutlines,    ACTION_MENU::CHECK );
    drawingModeSubMenu->Add( PCB_ACTIONS::textOutlines,        ACTION_MENU::CHECK );

    viewMenu->Add( drawingModeSubMenu );

    // Contrast Mode Submenu
    ACTION_MENU* contrastModeSubMenu = new ACTION_MENU( false, selTool );
    contrastModeSubMenu->SetTitle( _( "&Contrast Mode" ) );
    contrastModeSubMenu->SetIcon( BITMAPS::contrast_mode );

    contrastModeSubMenu->Add( ACTIONS::highContrastMode,    ACTION_MENU::CHECK );
    contrastModeSubMenu->Add( PCB_ACTIONS::layerAlphaDec );
    contrastModeSubMenu->Add( PCB_ACTIONS::layerAlphaInc );
    viewMenu->Add( contrastModeSubMenu );

    viewMenu->Add( PCB_ACTIONS::flipBoard,                  ACTION_MENU::CHECK );

#ifdef __APPLE__
    viewMenu->AppendSeparator();
#endif

    //-- Place Menu ----------------------------------------------------------
    //
    ACTION_MENU* placeMenu = new ACTION_MENU( false, selTool );

    // RESTRICTED MODE: Place Footprint disabled
    // placeMenu->Add( PCB_ACTIONS::placeFootprint );
    placeMenu->Add( PCB_ACTIONS::drawVia );
    placeMenu->Add( PCB_ACTIONS::drawZone );
    placeMenu->Add( PCB_ACTIONS::drawRuleArea );

    ACTION_MENU* muwaveSubmenu = new ACTION_MENU( false, selTool );
    muwaveSubmenu->SetTitle( _( "Draw Microwave Shapes" ) );
    muwaveSubmenu->SetIcon( BITMAPS::mw_add_line );
    muwaveSubmenu->Add( PCB_ACTIONS::microwaveCreateLine );
    muwaveSubmenu->Add( PCB_ACTIONS::microwaveCreateGap );
    muwaveSubmenu->Add( PCB_ACTIONS::microwaveCreateStub );
    muwaveSubmenu->Add( PCB_ACTIONS::microwaveCreateStubArc );
    muwaveSubmenu->Add( PCB_ACTIONS::microwaveCreateFunctionShape );
    placeMenu->Add( muwaveSubmenu );

    placeMenu->AppendSeparator();
    placeMenu->Add( PCB_ACTIONS::drawLine );
    placeMenu->Add( PCB_ACTIONS::drawArc );
    placeMenu->Add( PCB_ACTIONS::drawRectangle );
    placeMenu->Add( PCB_ACTIONS::drawCircle );
    placeMenu->Add( PCB_ACTIONS::drawPolygon );
    placeMenu->Add( PCB_ACTIONS::drawBezier );
    // RESTRICTED MODE: Reference image, text, textbox, table, dimensions, characteristics, stackup removed
    // placeMenu->Add( PCB_ACTIONS::placeReferenceImage );
    // placeMenu->Add( PCB_ACTIONS::placeText );
    // placeMenu->Add( PCB_ACTIONS::drawTextBox );
    // placeMenu->Add( PCB_ACTIONS::drawTable );

    // RESTRICTED MODE: Dimensions submenu removed
    // placeMenu->AppendSeparator();
    // ACTION_MENU* dimensionSubmenu = new ACTION_MENU( false, selTool );
    // dimensionSubmenu->SetTitle( _( "Draw Dimensions" ) );
    // dimensionSubmenu->SetIcon( BITMAPS::add_aligned_dimension );
    // dimensionSubmenu->Add( PCB_ACTIONS::drawOrthogonalDimension );
    // dimensionSubmenu->Add( PCB_ACTIONS::drawAlignedDimension );
    // dimensionSubmenu->Add( PCB_ACTIONS::drawCenterDimension );
    // dimensionSubmenu->Add( PCB_ACTIONS::drawRadialDimension );
    // dimensionSubmenu->Add( PCB_ACTIONS::drawLeader );
    // placeMenu->Add( dimensionSubmenu );

    // RESTRICTED MODE: Board characteristics and stackup removed
    // placeMenu->AppendSeparator();
    // placeMenu->Add( PCB_ACTIONS::placeCharacteristics );
    // placeMenu->Add( PCB_ACTIONS::placeStackup );

    // RESTRICTED MODE: Grid/Drill origin and Auto-Place disabled
    // placeMenu->AppendSeparator();
    // placeMenu->Add( PCB_ACTIONS::drillOrigin );
    // placeMenu->Add( PCB_ACTIONS::drillResetOrigin );
    // placeMenu->Add( ACTIONS::gridSetOrigin );
    // placeMenu->Add( ACTIONS::gridResetOrigin );
    // placeMenu->Add( autoplaceSubmenu );

    //-- Route Menu ----------------------------------------------------------
    //
    ACTION_MENU* routeMenu = new ACTION_MENU( false, selTool );

    routeMenu->Add( PCB_ACTIONS::selectLayerPair );

    routeMenu->AppendSeparator();
    routeMenu->Add( PCB_ACTIONS::routeSingleTrack );
    routeMenu->Add( PCB_ACTIONS::routeDiffPair );

    routeMenu->AppendSeparator();
    routeMenu->Add( PCB_ACTIONS::tuneSingleTrack );
    routeMenu->Add( PCB_ACTIONS::tuneDiffPair );
    routeMenu->Add( PCB_ACTIONS::tuneSkew );

    // RESTRICTED MODE: Router Settings disabled
    // routeMenu->AppendSeparator();
    // routeMenu->Add( PCB_ACTIONS::routerSettingsDialog );


    //-- Inspect Menu --------------------------------------------------------
    //
    ACTION_MENU* inspectMenu = new ACTION_MENU( false, selTool );

    inspectMenu->Add( PCB_ACTIONS::boardStatistics );
    inspectMenu->Add( ACTIONS::measureTool );

    inspectMenu->AppendSeparator();
    inspectMenu->Add( PCB_ACTIONS::runDRC );
    inspectMenu->Add( ACTIONS::prevMarker );
    inspectMenu->Add( ACTIONS::nextMarker );
    inspectMenu->Add( ACTIONS::excludeMarker );

    inspectMenu->AppendSeparator();
    inspectMenu->Add( PCB_ACTIONS::inspectClearance );
    inspectMenu->Add( PCB_ACTIONS::inspectConstraints );
    // RESTRICTED MODE: Footprint associations and compare removed
    // inspectMenu->Add( PCB_ACTIONS::showFootprintAssociations );
    // inspectMenu->Add( PCB_ACTIONS::diffFootprint );


    //-- Tools menu ----------------------------------------------------------
    //
    ACTION_MENU* toolsMenu = new ACTION_MENU( false, selTool );

    // RESTRICTED MODE: Most Tools menu items disabled - only keeping Zones Manager
    //Zones management
    toolsMenu->Add( PCB_ACTIONS::zonesManager );

    // RESTRICTED MODE: Preferences menu removed entirely
    // ACTION_MENU* prefsMenu = new ACTION_MENU( false, selTool );
    // AddMenuLanguageList( prefsMenu, selTool );


    //--MenuBar -----------------------------------------------------------
    //
    menuBar->Append( fileMenu,    _( "&File" ) );
    menuBar->Append( editMenu,    _( "&Edit" ) );
    menuBar->Append( viewMenu,    _( "&View" ) );
    menuBar->Append( placeMenu,   _( "&Place" ) );
    menuBar->Append( routeMenu,   _( "Ro&ute" ) );
    menuBar->Append( inspectMenu, _( "&Inspect" ) );
    menuBar->Append( toolsMenu,   _( "&Tools" ) );
    // RESTRICTED MODE: Preferences and Help menus removed
    // menuBar->Append( prefsMenu,   _( "P&references" ) );
    // AddStandardHelpMenu( menuBar );

    SetMenuBar( menuBar );
    delete oldMenuBar;

}
