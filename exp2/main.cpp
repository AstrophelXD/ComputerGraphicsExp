#ifndef _AFXDLL
#define _AFXDLL
#endif

#include <afxwin.h>

#include <array>
#include <cmath>

struct CP3 {
    double x;
    double y;
    double z;
};

struct Edge {
    int s;
    int e;
};

static constexpr double kPi = 3.14159265358979323846;

class CExp2View : public CView {
    DECLARE_DYNCREATE(CExp2View)

public:
    CExp2View() = default;
    ~CExp2View() override = default;

protected:
    double m_r = 1000.0;
    double m_d = 1000.0;
    double m_thetaDeg = 30.0;
    double m_phiDeg = 60.0;
    double m_tk[9] = {};

    static std::array<CP3, 6> OctaVertices() {
        return {{{0.0, 0.0, 170.0}, {0.0, 0.0, -170.0}, {170.0, 0.0, 0.0}, {-170.0, 0.0, 0.0}, {0.0, 170.0, 0.0}, {0.0, -170.0, 0.0}}};
    }

    static std::array<Edge, 12> OctaEdges() {
        return {{{0, 2}, {0, 3}, {0, 4}, {0, 5}, {1, 2}, {1, 3}, {1, 4}, {1, 5}, {2, 4}, {4, 3}, {3, 5}, {5, 2}}};
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

    CP3 PerProject(const CP3& currP) const {
        const double xView = m_tk[0] * currP.x + m_tk[1] * currP.y + m_tk[2] * currP.z;
        const double yView = m_tk[3] * currP.x + m_tk[4] * currP.y + m_tk[5] * currP.z;
        const double zView = m_tk[6] * currP.x + m_tk[7] * currP.y + m_tk[8] * currP.z + m_r;

        CP3 screenP = {0.0, 0.0, zView};
        if (std::abs(zView) > 1e-8) {
            screenP.x = m_d * xView / zView;
            screenP.y = m_d * yView / zView;
        }
        return screenP;
    }

    void DrawAxis(CDC* pDC) const {
        CP3 origin = PerProject({0.0, 0.0, 0.0});
        CP3 xEnd = PerProject({260.0, 0.0, 0.0});
        CP3 yEnd = PerProject({0.0, 260.0, 0.0});
        CP3 zEnd = PerProject({0.0, 0.0, 260.0});

        pDC->MoveTo(static_cast<int>(origin.x), static_cast<int>(origin.y));
        pDC->LineTo(static_cast<int>(xEnd.x), static_cast<int>(xEnd.y));
        pDC->MoveTo(static_cast<int>(origin.x), static_cast<int>(origin.y));
        pDC->LineTo(static_cast<int>(yEnd.x), static_cast<int>(yEnd.y));
        pDC->MoveTo(static_cast<int>(origin.x), static_cast<int>(origin.y));
        pDC->LineTo(static_cast<int>(zEnd.x), static_cast<int>(zEnd.y));

        pDC->TextOut(static_cast<int>(xEnd.x), static_cast<int>(xEnd.y), _T("X"));
        pDC->TextOut(static_cast<int>(yEnd.x), static_cast<int>(yEnd.y), _T("Y"));
        pDC->TextOut(static_cast<int>(zEnd.x), static_cast<int>(zEnd.y), _T("Z"));
    }

    template <size_t VCount, size_t ECount>
    void DrawWire(CDC* pDC, const std::array<CP3, VCount>& vertices, const std::array<Edge, ECount>& edges) const {
        for (const Edge& edge : edges) {
            const CP3 s2 = PerProject(vertices[edge.s]);
            const CP3 e2 = PerProject(vertices[edge.e]);
            pDC->MoveTo(static_cast<int>(s2.x), static_cast<int>(s2.y));
            pDC->LineTo(static_cast<int>(e2.x), static_cast<int>(e2.y));
        }
    }

    void DrawObject(CDC* pDC) const {
        DrawWire(pDC, OctaVertices(), OctaEdges());
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
    }

    DECLARE_MESSAGE_MAP()
};

IMPLEMENT_DYNCREATE(CExp2View, CView)
BEGIN_MESSAGE_MAP(CExp2View, CView)
END_MESSAGE_MAP()

class CExp2Frame : public CFrameWnd {
public:
    CExp2Frame() { Create(nullptr, _T("Exp2 - Polyhedron Perspective Projection"), WS_OVERLAPPEDWINDOW, CRect(100, 80, 1200, 820)); }
};

class CExp2App : public CWinApp {
public:
    BOOL InitInstance() override {
        CWinApp::InitInstance();
        auto* frame = new CExp2Frame();
        auto* view = new CExp2View();
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

CExp2App theApp;
