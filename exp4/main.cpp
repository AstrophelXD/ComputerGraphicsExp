#ifndef _AFXDLL
#define _AFXDLL
#endif

#include <afxwin.h>

#include <array>
#include <cmath>

#include "Face.h"
#include "P3.h"

// build_mfc.bat compiles main.cpp only, so we include Face.cpp here.
#include "Face.cpp"

static constexpr double kPi = 3.14159265358979323846;

class CExp4View : public CView {
    DECLARE_DYNCREATE(CExp4View)

public:
    CExp4View() = default;
    ~CExp4View() override = default;

protected:
    std::array<CP3, 20> m_vertices = {};
    CFace m_faces[12];

    double m_r = 1200.0;
    double m_d = 1000.0;
    double m_thetaDeg = 35.0;
    double m_phiDeg = 58.0;
    double m_tk[9] = {};

    double m_rotY = 0.0;
    double m_rotX = 0.0;

    void InitDodecahedron() {
        const double phi = (1.0 + std::sqrt(5.0)) / 2.0;
        const double invPhi = 1.0 / phi;
        const double s = 120.0;

        m_vertices = {{
            {-1, -1, -1}, {-1, -1, 1}, {-1, 1, -1}, {-1, 1, 1},
            {1, -1, -1},  {1, -1, 1},  {1, 1, -1},  {1, 1, 1},
            {0, -invPhi, -phi}, {0, -invPhi, phi}, {0, invPhi, -phi}, {0, invPhi, phi},
            {-invPhi, -phi, 0}, {-invPhi, phi, 0}, {invPhi, -phi, 0}, {invPhi, phi, 0},
            {-phi, 0, -invPhi}, {phi, 0, -invPhi}, {-phi, 0, invPhi}, {phi, 0, invPhi},
        }};

        for (CP3& p : m_vertices) {
            p.x *= s;
            p.y *= s;
            p.z *= s;
        }

        static constexpr int faceIndex[12][5] = {
            {0, 8, 10, 2, 16}, {0, 16, 18, 1, 12}, {0, 12, 14, 4, 8}, {8, 4, 17, 6, 10},
            {10, 6, 15, 13, 2}, {2, 13, 3, 18, 16}, {1, 9, 11, 3, 18}, {1, 12, 14, 5, 9},
            {4, 14, 5, 19, 17}, {6, 17, 19, 7, 15}, {3, 11, 7, 15, 13}, {5, 9, 11, 7, 19},
        };

        for (int i = 0; i < 12; ++i) {
            m_faces[i].SetNum(5);
            for (int j = 0; j < 5; ++j) {
                m_faces[i].vI[j] = faceIndex[i][j];
            }
        }
    }

    void InitParameter() {
        const double theta = m_thetaDeg * kPi / 180.0;
        const double phi = m_phiDeg * kPi / 180.0;
        const double st = std::sin(theta);
        const double ct = std::cos(theta);
        const double sp = std::sin(phi);
        const double cp = std::cos(phi);

        m_tk[0] = -st;
        m_tk[1] = ct;
        m_tk[2] = 0.0;

        m_tk[3] = -ct * cp;
        m_tk[4] = -st * cp;
        m_tk[5] = sp;

        m_tk[6] = -ct * sp;
        m_tk[7] = -st * sp;
        m_tk[8] = -cp;
    }

    CP3 RotateModel(const CP3& p) const {
        const double cy = std::cos(m_rotY);
        const double sy = std::sin(m_rotY);
        const double cx = std::cos(m_rotX);
        const double sx = std::sin(m_rotX);

        const double x1 = p.x * cy + p.z * sy;
        const double z1 = -p.x * sy + p.z * cy;
        const double y2 = p.y * cx - z1 * sx;
        const double z2 = p.y * sx + z1 * cx;

        return {x1, y2, z2};
    }

    CP3 PerProject(const CP3& p) const {
        const CP3 q = RotateModel(p);
        const double xView = m_tk[0] * q.x + m_tk[1] * q.y + m_tk[2] * q.z;
        const double yView = m_tk[3] * q.x + m_tk[4] * q.y + m_tk[5] * q.z;
        const double zView = m_tk[6] * q.x + m_tk[7] * q.y + m_tk[8] * q.z + m_r;

        if (std::abs(zView) <= 1e-8) return {0.0, 0.0, zView};
        return {m_d * xView / zView, m_d * yView / zView, zView};
    }

    void DrawAxis(CDC* pDC) const {
        const CP3 o = PerProject({0.0, 0.0, 0.0});
        const CP3 x = PerProject({280.0, 0.0, 0.0});
        const CP3 y = PerProject({0.0, 280.0, 0.0});
        const CP3 z = PerProject({0.0, 0.0, 280.0});

        pDC->MoveTo(static_cast<int>(o.x), static_cast<int>(o.y));
        pDC->LineTo(static_cast<int>(x.x), static_cast<int>(x.y));
        pDC->MoveTo(static_cast<int>(o.x), static_cast<int>(o.y));
        pDC->LineTo(static_cast<int>(y.x), static_cast<int>(y.y));
        pDC->MoveTo(static_cast<int>(o.x), static_cast<int>(o.y));
        pDC->LineTo(static_cast<int>(z.x), static_cast<int>(z.y));

        pDC->TextOut(static_cast<int>(x.x), static_cast<int>(x.y), _T("X"));
        pDC->TextOut(static_cast<int>(y.x), static_cast<int>(y.y), _T("Y"));
        pDC->TextOut(static_cast<int>(z.x), static_cast<int>(z.y), _T("Z"));
    }

    void DrawObject(CDC* pDC) const {
        for (const CFace& face : m_faces) {
            for (int i = 0; i < face.vN; ++i) {
                const int a = face.vI[i];
                const int b = face.vI[(i + 1) % face.vN];
                const CP3 p1 = PerProject(m_vertices[a]);
                const CP3 p2 = PerProject(m_vertices[b]);
                pDC->MoveTo(static_cast<int>(p1.x), static_cast<int>(p1.y));
                pDC->LineTo(static_cast<int>(p2.x), static_cast<int>(p2.y));
            }
        }
    }

    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct) {
        if (CView::OnCreate(lpCreateStruct) == -1) return -1;
        InitDodecahedron();
        SetTimer(1, 30, nullptr);
        return 0;
    }

    afx_msg void OnDestroy() {
        KillTimer(1);
        CView::OnDestroy();
    }

    afx_msg void OnTimer(UINT_PTR nIDEvent) {
        if (nIDEvent == 1) {
            m_rotY += 0.03;
            m_rotX += 0.015;
            if (m_rotY > 2.0 * kPi) m_rotY -= 2.0 * kPi;
            if (m_rotX > 2.0 * kPi) m_rotX -= 2.0 * kPi;
            Invalidate(FALSE);
        }
        CView::OnTimer(nIDEvent);
    }

    void OnDraw(CDC* pDC) override {
        CRect rect;
        GetClientRect(&rect);
        pDC->FillSolidRect(rect, RGB(255, 255, 255));

        pDC->SetMapMode(MM_ANISOTROPIC);
        pDC->SetWindowExt(rect.Width(), rect.Height());
        pDC->SetViewportExt(rect.Width(), -rect.Height());
        pDC->SetViewportOrg(rect.Width() / 2, rect.Height() / 2);

        InitParameter();
        DrawAxis(pDC);
        DrawObject(pDC);
        pDC->TextOut(-260, 260, _T("Exp4 (21+): Dodecahedron Perspective Wireframe Animation"));
    }

    DECLARE_MESSAGE_MAP()
};

IMPLEMENT_DYNCREATE(CExp4View, CView)
BEGIN_MESSAGE_MAP(CExp4View, CView)
ON_WM_CREATE()
ON_WM_DESTROY()
ON_WM_TIMER()
END_MESSAGE_MAP()

class CExp4Frame : public CFrameWnd {
public:
    CExp4Frame() {
        Create(nullptr, _T("Exp4 (21+) - Dodecahedron Projection Animation"), WS_OVERLAPPEDWINDOW, CRect(120, 80, 1220, 860));
    }
};

class CExp4App : public CWinApp {
public:
    BOOL InitInstance() override {
        CWinApp::InitInstance();
        auto* frame = new CExp4Frame();
        auto* view = new CExp4View();
        if (!view->Create(nullptr, nullptr, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), frame, AFX_IDW_PANE_FIRST, nullptr)) {
            return FALSE;
        }
        frame->SetActiveView(view);
        m_pMainWnd = frame;
        frame->ShowWindow(SW_SHOW);
        frame->UpdateWindow();
        return TRUE;
    }
};

CExp4App theApp;
