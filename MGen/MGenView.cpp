// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MGenView.cpp : implementation of the CMGenView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "MGen.h"
#endif

#include "MGenDoc.h"
#include "MGenView.h"
#include "MainFrm.h"
#include "InfoDlg.h"
#include "GLibrary/MemDC2.cpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMGenView

IMPLEMENT_DYNCREATE(CMGenView, CScrollView)

BEGIN_MESSAGE_MAP(CMGenView, CScrollView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMGenView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CMGenView construction/destruction

CMGenView::CMGenView()
{
	v_color = vector<vector<unsigned char>>(MAX_VOICE, vector<unsigned char>(3));
	v_color[0] = {   0,   0, 255 };
	v_color[1] = { 255,   0, 255 };
	v_color[2] = { 90, 170, 255 };
	v_color[3] = {   0, 255,   0 };
	v_color[4] = { 120, 120,   0 };
	v_color[5] = { 0, 120, 120 };
	v_color[6] = { 180, 100, 100 };
	v_color[7] = { 90, 255, 170 };
	v_color[8] = { 150, 0, 150 };
	v_color[9] = { 0, 150, 0 };
	v_color[10] = { 120, 120, 255 };
	v_color[11] = { 255, 100, 160 };
	v_color[12] = { 255, 160, 100 };
	v_color[13] = { 0, 255, 255 };
	v_color[14] = { 130, 200, 40 };
	v_color[15] = { 160, 100, 255 };
	// Dummy set to avoid assertion failure on first OnPaint
	SetScrollSizes(MM_TEXT, CSize(20000, 20000));
}

CMGenView::~CMGenView()
{
}

BOOL CMGenView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CScrollView::PreCreateWindow(cs);
}

// CMGenView drawing

void CMGenView::OnDraw(CDC* pDC)
{
	int time_start = CGLib::time();
	int time_stop = time_start;
	int time_stop2 = time_start;
	int time_stop3 = time_start;
	int time_stop4 = time_start;
	int time_stop5 = time_start;
	CMainFrame *mf = (CMainFrame *)AfxGetMainWnd();
	CGMidi *pGen = mf->pGen;
	//mf->WriteLog(2, "OnDraw start");
	if ((mf->m_state_gen > 0) && (pGen != 0)) if (pGen->t_generated > 0) {
		if (!pGen->mutex_output.try_lock_for(chrono::milliseconds(50))) {
			mf->WriteLog(2, "OnDraw mutex timed out: drawing postponed");
			return;
		}
	}
  //CMGenDoc* pDoc = GetDocument();
	//ASSERT_VALID(pDoc);
	//if (!pDoc) return;

	// This is invalidated rect
	CRect ClipBox;
	pDC->GetClipBox(&ClipBox);
	// Total client rect
	CRect ClientRect;
	GetClientRect(&ClientRect);
	// Total scrollable size
	CSize Size = GetTotalSize();

	CMemDC2 dc(pDC);
	dc->FillRect(ClipBox, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));

	Graphics g(dc->m_hDC);
	//CClientDC aDC(this); //получить контекст устройства
	//OnPrepareDC(&aDC); //уточнить начальную точку в логических координатах
	//aDC.DPtoLP(&point); //перевод из клиентских координат в логические

	SolidBrush brush_gray(Color(255 /*A*/, 247 /*R*/, 247 /*G*/, 247 /*B*/));
	SolidBrush brush_dgray(Color(255 /*A*/, 220 /*R*/, 220 /*G*/, 220 /*B*/));
	SolidBrush brush_ddgray(Color(255 /*A*/, 180 /*R*/, 180 /*G*/, 180 /*B*/));
	SolidBrush brush_dddgray(Color(255 /*A*/, 120 /*R*/, 120 /*G*/, 120 /*B*/));
	SolidBrush brush_black(Color(255 /*A*/, 0 /*R*/, 0 /*G*/, 0 /*B*/));
	SolidBrush brush_red(Color(255 /*A*/, 255 /*R*/, 0 /*G*/, 0 /*B*/));
	SolidBrush brush_agray(Color(20 /*A*/, 0 /*R*/, 0 /*G*/, 0 /*B*/));
	SolidBrush brush_ared(Color(20 /*A*/, 255 /*R*/, 0 /*G*/, 0 /*B*/));
	Pen pen_agray(Color(100 /*A*/, 0 /*R*/, 0 /*G*/, 0 /*B*/), 1);
	Pen pen_ablue(Color(90 /*A*/, 0 /*R*/, 0 /*G*/, 170 /*B*/), 1);
	Pen pen_ared(Color(127 /*A*/, 255 /*R*/, 0 /*G*/, 0 /*B*/), 1);
	Pen pen_aared(Color(70 /*A*/, 255 /*R*/, 0 /*G*/, 0 /*B*/), 1);
	Pen pen_dgray(Color(255 /*A*/, 220 /*R*/, 220 /*G*/, 220 /*B*/), 1);
	Pen pen_ddgray(Color(255 /*A*/, 180 /*R*/, 180 /*G*/, 180 /*B*/), 1);
	Pen pen_dddgray(Color(255 /*A*/, 120 /*R*/, 120 /*G*/, 120 /*B*/), 1);
	Pen pen_black(Color(255 /*A*/, 0 /*R*/, 0 /*G*/, 0 /*B*/), 1);

	Gdiplus::Font font(&FontFamily(L"Arial"), 10);
	Gdiplus::Font font_small(&FontFamily(L"Arial"), 8);
	Gdiplus::Font font_small2(&FontFamily(L"Arial"), 6);
	int current_time = CGLib::time();
  USES_CONVERSION;
	CString st;

	if (mf->m_state_gen == 0) st = "Generation: not started";
	if (mf->m_state_gen == 1) st = "Generation: started";
	if (mf->m_state_gen == 2) st = "Generation: finished";
	g.DrawString(A2W(st), -1, &font, PointF(0, 0), &brush_black);
	if (mf->m_state_play == 0) st = "Playback: stopped";
	if (mf->m_state_play == 1) st = "Playback:";
	if (mf->m_state_play == 2) st = "Playback ";
	g.DrawString(A2W(st), -1, &font, PointF(800, 0), &brush_black);

	time_stop2 = CGLib::time();
	if ((mf->m_state_gen > 0) && (pGen != 0)) if (pGen->t_generated > 0) {
		CString time_st = "";
		if (pGen->t_sent > 0) time_st = CGLib::FormatTime(pGen->etime[pGen->t_sent - 1] / pGen->m_pspeed / 10);
		if (mf->m_state_gen == 1) st.Format("(%d/%d of %d meas. / %s in %.1f sec.)", 
			pGen->t_generated/8, pGen->t_sent/8, pGen->t_cnt/8, time_st, 
			((float)(CGLib::time() - pGen->time_started)) / 1000);
		if (mf->m_state_gen == 2) st.Format("(%d meas. / %s in %.1f sec.)", 
			pGen->t_sent/8, time_st, ((float)(pGen->time_stopped - pGen->time_started)) / 1000);
		g.DrawString(A2W(st), -1, &font, PointF(250, 0), &brush_black);
		int play_step = 0;
		if (mf->m_state_play > 0) play_step = pGen->GetPlayStep();
		if (play_step > 0) {
			int play_time = CGLib::time() - pGen->midi_start_time;
			st.Format("(%02d:%02d - meas. %d)", play_time/60000, (play_time/1000) % 60, play_step/8 + 1);
			g.DrawString(A2W(st), -1, &font, PointF(900, 0), &brush_black);
		}
		if (pGen->need_exit == 1)
			g.DrawString(L"INTERRUPTED", -1, &font, PointF(640, 0), &brush_red);
		nwidth = 4 * mf->zoom_x / 100;
		if (nwidth == 0) nwidth = 1;
		if (mf->view_single_track) {
			// Count tempo window
			double tg_min = MIN_TEMPO_DISPLAY;
			double tg_max = MAX_TEMPO_DISPLAY;
			// Increase if generated bigger tempo window
			if (pGen->tg_min < tg_min) tg_min = pGen->tg_min - 1;
			if (pGen->tg_max > tg_max) tg_max = pGen->tg_max + 1;
			// Get generator window
			int ng_min = pGen->ng_min;
			int ng_max = pGen->ng_max;
			int ncount = ng_max - ng_min + 1;
			// Copy generator window
			int ng_min2 = ng_min;
			int ng_max2 = ng_max;
			int ncount2 = ncount;
			// Load last window
			if (mf->ng_max != 0) {
				ng_min2 = mf->ng_min;
				ng_max2 = mf->ng_max;
				// Switch to generator window if it is bigger than last window
				if ((ng_min < ng_min2) || (ng_max > ng_max2)) {
					ng_min2 = ng_min;
					ng_max2 = ng_max;
				}
				ncount2 = ng_max2 - ng_min2 + 1;
			}
			// Check if window is too small
			if (ncount2 < SINGLETRACK_MINNOTES - 2) {
				ncount2 = SINGLETRACK_MINNOTES;
				ng_min2 = ng_min - (ncount2 - ncount) / 2;
				ng_max2 = ng_max + (ncount2 - ncount) / 2;
				ncount2 = ng_max2 - ng_min2 + 1;
			}
			// Save window
			mf->ng_min = ng_min2;
			mf->ng_max = ng_max2;
			// Count height
			nheight = (ClientRect.bottom - ClientRect.top - Y_HEADER - Y_FOOTER) / ncount2;
			y_start = ClientRect.top + Y_HEADER + nheight*ncount2;
			// Select steps to show
			int step1 = max(0, (ClipBox.left - X_FIELD) / nwidth - 1);
			int step2_2 = min((ClipBox.right - X_FIELD) / nwidth + 1, 32000 / nwidth); // For horizontal bars
			if (step2_2 < step1) step2_2 = step1;
			int step2 = min(pGen->t_generated, step2_2); // For notes
			int step2_3 = step2 / 8 * 8+8; // For vertical lines
			// Show grid
			RectF sizeRect;
			for (int i = ng_min2; i <= ng_max2; i++) {
				int pos = y_start - (i - ng_min2 + 1) * nheight;
				if (diatonic[i % 12] == 0) {
					g.FillRectangle(&brush_gray, max(X_FIELD, ClipBox.left), pos,	ClipBox.right-ClipBox.left, nheight);
				}
				if (i % 12 == 4) g.DrawLine(&pen_dgray, max(X_FIELD, ClipBox.left),	pos, ClipBox.right,	pos);
				if (i % 12 == 11) g.DrawLine(&pen_ddgray, max(X_FIELD, ClipBox.left), pos, ClipBox.right,	pos);
				// Show piano
				if (ClipBox.left < X_FIELD) {
					if (diatonic[i % 12] == 0) {
						g.DrawLine(&pen_dddgray, X_FIELD/2, pos + nheight / 2, X_FIELD, pos + nheight / 2);
						g.FillRectangle(&brush_dddgray, 0, pos, X_FIELD / 2, nheight);
					}
					if (i % 12 == 11) {
						if ((nheight > 5) && (i < ng_max2)) {
							g.DrawLine(&pen_dddgray, 0, pos, X_FIELD/2-2, pos);
							st.Format("%d", i / 12);
							g.MeasureString(A2W(st), -1, &font_small, sizeRect, &sizeRect);
							g.DrawString(A2W(st), -1, &font_small, PointF(X_FIELD-18, pos - sizeRect.Height / 2), &brush_dddgray);
						}
						else {
							g.DrawLine(&pen_dddgray, 0, pos, X_FIELD, pos);
						}
					}
					if (i % 12 == 4) {
						g.DrawLine(&pen_ddgray, 0, pos, X_FIELD, pos);
					}
				}
			}
			// Show horizontal lines
			g.DrawLine(&pen_ddgray, max(X_FIELD, ClipBox.left), y_start, ClipBox.right, y_start);
			g.DrawLine(&pen_ddgray, max(X_FIELD, ClipBox.left), ClientRect.top + Y_HEADER, ClipBox.right, ClientRect.top + Y_HEADER);
			g.DrawLine(&pen_ddgray, max(X_FIELD, ClipBox.left), ClientRect.top + Y_HEADER - Y_TIMELINE, 
				ClipBox.right, ClientRect.top + Y_HEADER - Y_TIMELINE);
			time_stop3 = CGLib::time();
			// Draw vertical lines
			for (int i = max(0, step1-16); i < step2_3; i++) {
				if (i % 8 == 0) {
					if ((i % 16 == 0) || (mf->zoom_x > 120)) {
						if ((i % 64 == 0) || (mf->zoom_x > 80)) {
							g.DrawLine(&pen_ddgray, X_FIELD + i * nwidth, ClientRect.top + Y_HEADER,
								X_FIELD + i * nwidth, ClientRect.top + Y_HEADER - Y_TIMELINE);
							st.Format("%d", i / 8 + 1);
							g.DrawString(A2W(st), -1, &font_small, PointF(X_FIELD + i * nwidth, ClientRect.top + Y_HEADER - Y_TIMELINE + 1), &brush_dddgray);
						}
						g.DrawLine(&pen_ddgray, X_FIELD + i * nwidth, y_start - 1,
							X_FIELD + i * nwidth, ClientRect.top + Y_HEADER + 1);
					}
					else if (mf->zoom_x >= 100) {
						g.DrawLine(&pen_ddgray, X_FIELD + i * nwidth, y_start - 1,
							X_FIELD + i * nwidth, ClientRect.top + Y_HEADER + 1);
					}
				}
				else if ((mf->zoom_x >= 200) && (i % 2 == 0)) g.DrawLine(&pen_dgray, X_FIELD + i * nwidth, y_start,
					X_FIELD + i * nwidth, ClientRect.top + Y_HEADER);
			}
			Color ncolor, mcolor;
			// Add step to the left and to the right
			int step1t = step1;
			int step2t = step2;
			if (step1t > 0) step1t--;
			if (step2t < pGen->t_generated - 1) step2t++;
			// Show scan range
			if (mf->show_curve) {
				for (int v = 0; v < pGen->v_cnt; v++) {
					ncolor = Color(10, v_color[v][0] /*R*/, v_color[v][1] /*G*/, v_color[v][2] /*B*/);
					SolidBrush brush(ncolor);
					for (int i = step1t; i < step2t; i++) if (pGen->nsr1[i][v] && pGen->nsr2[i][v]) {
						g.FillRectangle(&brush, X_FIELD + i * nwidth,
							y_start - (pGen->nsr2[i][v] + pGen->show_transpose[v] - ng_min2 + 1) * nheight,
							nwidth, (pGen->nsr2[i][v] - pGen->nsr1[i][v] + 1) * nheight);
					}
				}
			}
			// Show notes
			time_stop4 = CGLib::time();
			int cutend = 0;
			int alpha;
			int step_dyn = mf->m_step_dyn;
			int ci_old = -1;
			for (int v = 0; v < pGen->v_cnt; v++) {
				int ci = v;
				if (pGen->midifile_loaded && pGen->track_id[v] > 0) ci = pGen->track_id[v] - 1;
				// Show instrument name
				if (ci_old != ci) {
					ci_old = ci;
					ncolor = Color(255 /*A*/, v_color[ci][0] /*R*/, v_color[ci][1] /*G*/, v_color[ci][2] /*B*/);
					SolidBrush brush_v(ncolor);
					st = pGen->InstGName[pGen->instr[v]];
					g.DrawString(A2W(st), -1, &font, PointF(1150 + 100 * ci, 0), &brush_v);
				}
				for (int i = step1; i < step2; i++) if ((pGen->pause[i][v] == 0) && (pGen->note[i][v] > 0)) {
					if (i == step1) if (pGen->coff[i][v] > 0) i = i - pGen->coff[i][v];
					// Check if note steps have different dynamics
					int step_dyn2 = 0;
					if ((step_dyn) && (pGen->len[i][v] > 1)) {
						for (int x = i + 1; x < i + pGen->len[i][v]; x++) {
							if (pGen->dyn[x][v] != pGen->dyn[x - 1][v]) {
								step_dyn2 = 1;
								break;
							}
						}
					}
					// Show without step dynamics
					if (!step_dyn2 || !mf->show_vel) {
						if (mf->show_vel) alpha = 40 + (80 * pGen->dyn[i][v] / 127);
						else alpha = 100;
						if (mf->show_notecolors && pGen->color[i][v].GetValue() != 0) {
							if (pGen->color[i][v].GetAlpha() == 0) ncolor = Color(alpha, pGen->color[i][v].GetR(), pGen->color[i][v].GetG(), pGen->color[i][v].GetB());
							else ncolor = pGen->color[i][v];
						}
						else {
							ncolor = Color(alpha /*A*/, v_color[ci][0] /*R*/, v_color[ci][1] /*G*/, v_color[ci][2] /*B*/);
						}
						SolidBrush brush(ncolor);
						cutend = 0;
						// Cut long notes end to prevent glueing with other voices
						if (pGen->len[i][v] * nwidth > 2) cutend = 1;
						g.FillRectangle(&brush, X_FIELD + i * nwidth,
							y_start - (pGen->note[i][v] + pGen->show_transpose[v] - ng_min2 + 1) * nheight,
							pGen->len[i][v] * nwidth - cutend, nheight);
						// Highlight selected note
						if ((mouse_step >= i) && (mouse_step < i + pGen->len[i][v]) && (mouse_voice == v)) {
							g.FillRectangle(&brush, X_FIELD + i * nwidth,
								y_start - (pGen->note[i][v] + pGen->show_transpose[v] - ng_min2 + 1) * nheight,
								pGen->len[i][v] * nwidth - cutend, nheight);
						}
					}
					// Show with step dynamics
					else {
						for (int x = i; x < i + pGen->len[i][v]; x++) {
							alpha = 40 + (80 * pGen->dyn[x][v] / 127);
							if (mf->show_notecolors && pGen->color[x][v].GetValue() != 0) {
								if (pGen->color[x][v].GetAlpha() == 0)
									ncolor = Color(alpha, pGen->color[x][v].GetR(), pGen->color[x][v].GetG(), pGen->color[x][v].GetB());
								else ncolor = pGen->color[x][v];
							}
							else {
								ncolor = Color(alpha /*A*/, v_color[ci][0] /*R*/, v_color[ci][1] /*G*/, v_color[ci][2] /*B*/);
							}
							SolidBrush brush(ncolor);
							cutend = 0;
							if ((x == i + pGen->len[i][v] - 1) && (i + pGen->noff[i][v] < pGen->t_generated) &&
								(pGen->note[i + pGen->noff[i][v]][v] == pGen->note[i][v])) cutend = 1;
							g.FillRectangle(&brush, X_FIELD + x * nwidth,
								y_start - (pGen->note[i][v] + pGen->show_transpose[v] - ng_min2 + 1) * nheight,
								nwidth - cutend, nheight);
							// Highlight selected note
							if ((mouse_step >= i) && (mouse_step < i + pGen->len[i][v]) && (mouse_voice == v)) {
								g.FillRectangle(&brush, X_FIELD + x * nwidth,
									y_start - (pGen->note[i][v] + pGen->show_transpose[v] - ng_min2 + 1) * nheight,
									nwidth - cutend, nheight);
							}
						}
					}
					// Show lining
					if (mf->show_lining && pGen->lining[i][v] == 1) {
						g.DrawLine(&pen_black, X_FIELD + i * nwidth, y_start - (pGen->note[i][v] + pGen->show_transpose[v] - ng_min2 + 1) * nheight,
							X_FIELD + i * nwidth + pGen->len[i][v] * nwidth - cutend, y_start - (pGen->note[i][v] + pGen->show_transpose[v] - ng_min2) * nheight);
						g.DrawLine(&pen_black, X_FIELD + i * nwidth, y_start - (pGen->note[i][v] + pGen->show_transpose[v] - ng_min2) * nheight,
							X_FIELD + i * nwidth + pGen->len[i][v] * nwidth - cutend, y_start - (pGen->note[i][v] + pGen->show_transpose[v] - ng_min2 + 1) * nheight);
					}
					// Show comment
					if (mf->show_comments && pGen->comment[i][v] != "")
						g.DrawRectangle(&pen_black, X_FIELD + i * nwidth,
							y_start - (pGen->note[i][v] + pGen->show_transpose[v] - ng_min2 + 1) * nheight,
							pGen->len[i][v] * nwidth - cutend, nheight);
					if (pGen->noff[i][v] == 0) break;
					i = i + pGen->noff[i][v] - 1;
					// Protect from infinite loop
					if (i < step1) break;
				} // for i
			} // for v
			// Show marks
			CString mark;
			if (mf->show_marks) for (int v = 0; v < pGen->v_cnt; v++) {
				int step1m = max(0, step1 - MARK_BACK);
				for (int i = step1m; i < step2; i++) if (pGen->mark[i][v] != "") {
					if (pGen->mark_color[i][v].GetValue() != 0) {
						if (pGen->mark_color[i][v].GetAlpha() == 0)
							mcolor = Color(210, pGen->mark_color[i][v].GetR(), pGen->mark_color[i][v].GetG(), pGen->mark_color[i][v].GetB());
						else mcolor = pGen->mark_color[i][v];
					}
					else {
						mcolor = Color(210, ncolor.GetR(), ncolor.GetG(), ncolor.GetB());
					}
					SolidBrush brush_v(mcolor);
					mark = pGen->mark[i][v];
					if (dc.GetTextExtent(mark).cx > MARK_BACK * nwidth) {
						mark = "...";
						if (!warning_mark_long) {
							++warning_mark_long;
							mf->WriteLog(1, "Warning: Mark is too long for current zoom and was replaced with '...'. Solution: increase Zoom level, increase MARK_BACK or decrease mark string length in algorithm.");
						}
					}
					g.DrawString(A2W(mark), -1, &font_small2, PointF(X_FIELD + i * nwidth,
						y_start - (pGen->note[i][v] + pGen->show_transpose[v] - ng_min2) * nheight), &brush_v);
				}
			}
			// Show generated vertical lines
			if (mf->show_lines) for (int v = 0; v < pGen->v_cnt; v++) {
				for (int i = step1; i < step2; i++) {
					if (pGen->linecolor[i].GetAlpha() != 0) {
						Pen pen_line(pGen->linecolor[i]);
						g.DrawLine(&pen_line, X_FIELD + i * nwidth, y_start - (pGen->ng_min - ng_min2) * nheight,
							X_FIELD + i * nwidth, y_start - (pGen->ng_max - ng_min2 + 1) * nheight);
					}
				}
			}
			mouse_voice_old = mouse_voice;
			// Show note graph
			if (mf->show_curve) {
				for (int v = 0; v < pGen->v_cnt; v++) {
					ncolor = Color(100, v_color[v][0] /*R*/, v_color[v][1] /*G*/, v_color[v][2] /*B*/);
					Pen pen(ncolor, 1);
					for (int n = 0; n < pGen->ngraph_size; ++n) {
						for (int i = step1t; i < step2t; i++) if (i > 0 && pGen->ngraph[i][v][n] && pGen->ngraph[i - 1][v][n]) {
							g.DrawLine(&pen, X_FIELD + i * nwidth + nwidth / 2 - 1,
								(int)(y_start - (pGen->ngraph[i][v][n] + pGen->show_transpose[v] - ng_min2 + 0.5) * nheight),
								X_FIELD + (i - 1) * nwidth + nwidth / 2,
								(int)(y_start - (pGen->ngraph[i - 1][v][n] + pGen->show_transpose[v] - ng_min2 + 0.5) * nheight));
						}
					}
				}
			}
			// Show tempo
			if (mf->show_tempo) {
				for (int i = step1t; i < step2t; i++) if (i > 0) {
					g.DrawLine(&pen_ablue, X_FIELD + i * nwidth + nwidth / 2,
						y_start - 2 - (y_start - Y_HEADER - 4)*(pGen->tempo[i] - tg_min) / (tg_max - tg_min),
						X_FIELD + (i - 1) * nwidth + nwidth / 2,
						y_start - 2 - (y_start - Y_HEADER - 4)*(pGen->tempo[i - 1] - tg_min) / (tg_max - tg_min));
					if (pGen->tempo_src[i])
						g.DrawLine(&pen_aared, X_FIELD + i * nwidth + nwidth / 2,
							y_start - 2 - (y_start - Y_HEADER - 4)*(pGen->tempo_src[i] - tg_min) / (tg_max - tg_min),
							X_FIELD + (i - 1) * nwidth + nwidth / 2,
							y_start - 2 - (y_start - Y_HEADER - 4)*(pGen->tempo_src[i - 1] - tg_min) / (tg_max - tg_min));
				}
				if (step2t < pGen->t_generated - 1) {
					g.DrawLine(&pen_ablue, X_FIELD + step2t * nwidth + nwidth / 2,
						y_start - 2 - (y_start - Y_HEADER - 4)*(pGen->tempo[step2t] - tg_min) / (tg_max - tg_min),
						X_FIELD + (step2t + 1) * nwidth + nwidth / 2,
						y_start - 2 - (y_start - Y_HEADER - 4)*(pGen->tempo[step2t + 1] - tg_min) / (tg_max - tg_min));
					if (pGen->tempo_src[step2t + 1])
						g.DrawLine(&pen_aared, X_FIELD + step2t * nwidth + nwidth / 2,
							y_start - 2 - (y_start - Y_HEADER - 4)*(pGen->tempo_src[step2t] - tg_min) / (tg_max - tg_min),
							X_FIELD + (step2t + 1) * nwidth + nwidth / 2,
							y_start - 2 - (y_start - Y_HEADER - 4)*(pGen->tempo_src[step2t + 1] - tg_min) / (tg_max - tg_min));
				}
			}
			// Highlight draft notes
			time_stop5 = CGLib::time();
			if ((step2t > pGen->t_sent) || (pGen->need_exit == 1)) {
				int step3 = max((ClipBox.left - X_FIELD) / nwidth - 1, pGen->t_sent);
				g.FillRectangle(&brush_ared, X_FIELD + step3 * nwidth,
					ClientRect.top + Y_HEADER,
					(step2t - step3 + 1) * nwidth, y_start - ClientRect.top - Y_HEADER);
			}
			// Highlight selection
			if ((mouse_step > -1)) {
				//int step3 = max((ClipBox.left - X_FIELD) / nwidth - 1, pGen->t_sent);
				g.FillRectangle(&brush_agray, X_FIELD + mouse_step * nwidth,
					ClientRect.top + Y_HEADER + 1,
					nwidth, y_start - ClientRect.top - Y_HEADER - 1);
			}
			mouse_step_old = mouse_step;
			// Show play position
			if (play_step > 0) {
				g.DrawLine(&pen_ared, X_FIELD + play_step * nwidth, y_start - 1,
					X_FIELD + play_step * nwidth, ClientRect.top + Y_HEADER + 1);
			}
		}
		if (min(32000, nwidth*pGen->t_generated) > ClientRect.right) {
			CSize DocSize(min(32000, nwidth*pGen->t_generated) + 50, 0);
			SetScrollSizes(MM_TEXT, DocSize, CSize(500, 500), CSize(50, 50));
		}
		pGen->mutex_output.unlock();
	}
	time_stop = CGLib::time();
	if (time_stop - time_start > 80 && mf->m_debug_level > 1) {
		st.Format("OnDraw run time %d (%d / %d / %d / %d) ms", time_stop - time_start, time_stop2 - time_start, time_stop3 - time_start, time_stop4 - time_start, time_stop5 - time_start);
		mf->WriteLog(2, st);
	}
	if (time_stop - time_start > max_draw_time) {
		max_draw_time = time_stop - time_start;
		if (max_draw_time > WARN_DRAW * (double)(mf->m_view_timer)) {
			st.Format("Warning: drawing takes %d ms. Your delay between drawings (View_timer) is %d ms. It is recommended to increase zoom or increase View_timer or set step_dyn to 0 in settings.pl if you face performance issues.", time_stop - time_start, mf->m_view_timer);
			mf->WriteLog(2, st);
		}
	}
}


// CMGenView printing


void CMGenView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMGenView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CMGenView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CMGenView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CMGenView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMGenView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMGenView diagnostics

#ifdef _DEBUG
void CMGenView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CMGenView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CMGenDoc* CMGenView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMGenDoc)));
	return (CMGenDoc*)m_pDocument;
}
#endif //_DEBUG


// CMGenView message handlers


void CMGenView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	//Определить размер документа
	CSize DocSize(0, 0);
	//Установить режим отображения и размер документа
	SetScrollSizes(MM_TEXT, DocSize, CSize(500, 500), CSize(50, 50));
	
	// Tooltip
	//CMFCToolTipInfo params;
	//params.m_bVislManagerTheme = TRUE;
	//m_ToolTip.SetParams(&params);
	//m_ToolTip.AddTool(this, _T("."));
	//m_ToolTip.SetDelayTime(100);					// 0.5 secondS before tooltips pop up in view
	//m_ToolTip.SetDelayTime(TTDT_RESHOW, 500);		// 0.5 seconds before tooltip after moving
	//m_ToolTip.SetDelayTime(TTDT_AUTOPOP, INT_MAX);	// leave tooltip visible
}


void CMGenView::OnMouseMove(UINT nFlags, CPoint point)
{
	CMainFrame* mf = (CMainFrame *)AfxGetMainWnd();
	CGMidi *pGen = mf->pGen;
	if ((pGen != 0) && (nwidth > 0) && (nheight > 0)) if (pGen->t_generated > 0) {
		if (!pGen->mutex_output.try_lock_for(chrono::milliseconds(50))) {
			mf->WriteLog(2, "OnMouseMove mutex timed out: mouse not processed");
			CScrollView::OnMouseMove(nFlags, point);
			return;
		}
		CPoint scroll = GetScrollPosition();
		// Find step
		if ((point.y < Y_HEADER - Y_TIMELINE) || (point.y > y_start)) mouse_step = -1;
		else mouse_step = (scroll.x + point.x - X_FIELD) / nwidth;
		// Check if mouse is too left
		if (mouse_step < -1) mouse_step = -1;
		// Check if mouse step is in future
		if (mouse_step >= pGen->t_generated) mouse_step = -1;
		if (mouse_step_old >= pGen->t_generated) mouse_step_old = -1;
		// Set note limits
		int step1 = mouse_step;
		int step2 = mouse_step;
		int step21 = mouse_step_old;
		int step22 = mouse_step_old;
		// Find voice
		mouse_voice = -1;
		int off = 0;
		if ((mouse_step_old > -1) && (mouse_voice_old > -1) && mouse_voice_old < pGen->v_cnt) {
			step21 = mouse_step_old - pGen->coff[mouse_step_old][mouse_voice_old];
			step22 = step21 + pGen->len[mouse_step_old][mouse_voice_old];
		}
		if (mouse_step > -1) {
			mouse_note = (y_start - point.y) / nheight + mf->ng_min;
			mouse_voices.clear();
			for (int i = 0; i < pGen->v_cnt; i++)
				if ((pGen->note[mouse_step][i] + pGen->show_transpose[i] == mouse_note) && (pGen->pause[mouse_step][i] == 0)) {
					mouse_voice = i;
					mouse_voices.push_back(i);
					step1 = mouse_step - pGen->coff[mouse_step][i];
					step2 = step1 + pGen->len[mouse_step][i];
				}
			if ((point.y >= Y_HEADER - Y_TIMELINE) && (point.y <= Y_HEADER)) mouse_in_timeline = 1;
			else mouse_in_timeline = 0;
		}
		else mouse_note = -1;
		if (((mouse_step != -1) || (mouse_step_old != -1)) && ((mouse_step != mouse_step_old) || (mouse_voice != mouse_voice_old))) {
			if (mouse_step_old == -1)	Invalidate();
			else {
				int min_step = min(step1, min(step2, min(step21, step22)));
				int max_step = max(step1, max(step2, max(step21, step22)));
				//int step2 = max(mouse_step_old, mouse_step);
				InvalidateRect(CRect(X_FIELD + min_step*nwidth - scroll.x, 0,
					X_FIELD + (max_step + off + 1)*nwidth - scroll.x, 1080));
				//CString st;
				//st.Format("InvalidateRect %d, %d, %d, %d", X_FIELD, min_step, nwidth, scroll.x);
				//mf->WriteLog(2, st);
			}
		}
		CString st, st2;
		if (mouse_step > -1) {
			st2.Format("Step %d, time %s, tempo %d. ", mouse_step, CGLib::FormatTime(pGen->stime[mouse_step] / pGen->m_pspeed / 10), (int)pGen->tempo[mouse_step]);
			st += st2;
		}
		if (mouse_voice > -1) {
			st2.Format("Voice %d. ", mouse_voice);
			st += st2;
		}
		if (mouse_note > -1) {
			st2.Format("Note %s (%d). ", CGLib::GetNoteName(mouse_note), mouse_note);
			st += st2;
		}
		mf->m_wndStatusBar.GetElement(0)->SetText(st);
		mf->m_wndStatusBar.Invalidate(1);
		//mf->WriteLog(0, "Mouse move");
		pGen->mutex_output.unlock();
	}

	CScrollView::OnMouseMove(nFlags, point);
}

void CMGenView::OnMouseHover(UINT nFlags, CPoint point)
{
	CScrollView::OnMouseHover(nFlags, point);
}

void CMGenView::GetToolTipLabelText(POINT cursor, CString & labelText, CString & descriptionText) const
{
}

BOOL CMGenView::PreTranslateMessage(MSG* pMsg)
{
	return CScrollView::PreTranslateMessage(pMsg);
}

void CMGenView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CString st;
	int result;
	CMainFrame *mf = (CMainFrame *)AfxGetMainWnd();
	CGMidi *pGen = mf->pGen;
	if ((mouse_step > -1) && (mouse_voice > -1)) {
		if (mouse_voices.size() > 1) {
			CMenu *menu = new CMenu;
			menu->CreatePopupMenu();
			for (int i = 0; i < mouse_voices.size(); ++i) {
				int mv = mouse_voices[i];
				st.Format("Voice %d (%s)", mv, pGen->InstGName[pGen->instr[mv]] + "/" + pGen->InstCName[pGen->instr[mv]]);
				menu->AppendMenu(MF_STRING, mv+1, st);
			}
			ClientToScreen(&point);
			result = menu->TrackPopupMenu(TPM_CENTERALIGN | TPM_RIGHTBUTTON | TPM_NONOTIFY | TPM_RETURNCMD, point.x, point.y, this);
			if (!result) return;
			mouse_voice = result - 1;
		}
		CInfoDlg dlg;
		dlg.pGen = mf->pGen;
		dlg.DoModal();
	}
	else if ((mouse_step > -1) && (mouse_in_timeline)) {
		if ((mf->m_state_gen == 2) && (mf->m_state_play == 0))
			mf->StartPlay(mouse_step);
	}

	CScrollView::OnLButtonUp(nFlags, point);
}


BOOL CMGenView::OnEraseBkgnd(CDC* pDC)
{
	return false;

	//return CScrollView::OnEraseBkgnd(pDC);
}


void CMGenView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	CMainFrame* mf = (CMainFrame *)AfxGetMainWnd();
	if (nChar == VK_F1) {
		((CMGenApp*)::AfxGetApp())->OnAppAbout();
	}
	if (nChar == VK_F3) {
		mf->OnButtonAlgo();
	}
	if (nChar == VK_F4) {
		mf->OnButtonParams();
	}
	if (nChar == VK_F5) {
		mf->OnButtonGen();
	}
	if (nChar == VK_SPACE) {
		mf->OnButtonPlay();
	}
	if (nChar == VK_NEXT) OnHScroll(SB_PAGERIGHT, 0, NULL);
	if (nChar == VK_PRIOR) OnHScroll(SB_PAGELEFT, 0, NULL);
	if (nChar == VK_RIGHT) OnHScroll(SB_LINERIGHT, 0, NULL);
	if (nChar == VK_LEFT) OnHScroll(SB_LINELEFT, 0, NULL);
	if (nChar == VK_END) OnHScroll(SB_RIGHT, 0, NULL);
	if (nChar == VK_HOME) OnHScroll(SB_LEFT, 0, NULL);

	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}
