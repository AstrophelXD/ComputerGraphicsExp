#ifndef _AFXDLL
#define _AFXDLL
#endif

#include <afxwin.h>

#include <array>
#include <cmath>
#include <vector>

struct Point2 {
    double x;
    double y;
};

static constexpr double kPi = 3.14159265358979323846;

class CExp3View : public CView {
    DECLARE_DYNCREATE(CExp3View)

public:
    CExp3View() = default;
    ~CExp3View() override = default;

protected:
    std::vector<Point2> m_baseCtrl = {
        {-110.0, -20.0}, {-85.0, 45.0}, {-40.0, 100.0}, {0.0, 130.0}, {40.0, 100.0},
        {85.0, 45.0},    {110.0, -20.0}, {70.0, -80.0}, {0.0, -120.0}, {-70.0, -80.0},
        {-110.0, -20.0}, {-85.0, 45.0}, {-40.0, 100.0}};
    std::vector<Point2> m_animCtrl;
    double m_phase = 0.0;

    Point2 ComputeCenter(const std::vector<Point2>& pts) const {
        Point2 c{0.0, 0.0};
        if (pts.empty()) return c;
        for (const Point2& p : pts) {
            c.x += p.x;
            c.y += p.y;
        }
        c.x /= static_cast<double>(pts.size());
        c.y /= static_cast<double>(pts.size());
        return c;
    }

    Point2 TransformPoint(const Point2& p, const Point2& c, double sx, double sy, double angleRad, double tx, double ty) const {
        const double x0 = p.x - c.x;
        const double y0 = p.y - c.y;
        const double xs = x0 * sx;
        const double ys = y0 * sy;
        const double cs = std::cos(angleRad);
        const double sn = std::sin(angleRad);
        const double xr = xs * cs - ys * sn;
        const double yr = xs * sn + ys * cs;
        return {xr + c.x + tx, yr + c.y + ty};
    }

    void UpdateAnimatedControlPoints() {
        const Point2 center = ComputeCenter(m_baseCtrl);
        const double angle = 0.3 * std::sin(m_phase);
        const double sx = 1.0 + 0.12 * std::sin(m_phase * 1.6);
        const double sy = 1.0 - 0.08 * std::sin(m_phase * 1.6);
        const double tx = 35.0 * std::sin(m_phase * 0.7);
        const double ty = 18.0 * std::sin(m_phase * 1.2);

        m_animCtrl.clear();
        m_animCtrl.reserve(m_baseCtrl.size());
        for (const Point2& p : m_baseCtrl) {
            m_animCtrl.push_back(TransformPoint(p, center, sx, sy, angle, tx, ty));
        }
    }

    Point2 EvalUniformCubicBSpline(const Point2& p0, const Point2& p1, const Point2& p2, const Point2& p3, double u) const {
        const double u2 = u * u;
        const double u3 = u2 * u;

        const double b0 = (-u3 + 3.0 * u2 - 3.0 * u + 1.0) / 6.0;
        const double b1 = (3.0 * u3 - 6.0 * u2 + 4.0) / 6.0;
        const double b2 = (-3.0 * u3 + 3.0 * u2 + 3.0 * u + 1.0) / 6.0;
        const double b3 = u3 / 6.0;

        return {b0 * p0.x + b1 * p1.x + b2 * p2.x + b3 * p3.x, b0 * p0.y + b1 * p1.y + b2 * p2.y + b3 * p3.y};
    }

    void DrawControlPolygon(CDC* pDC, const std::vector<Point2>& ctrl) const {
        if (ctrl.size() < 2) return;

        CPen pen(PS_DOT, 1, RGB(120, 120, 120));
        CPen* oldPen = pDC->SelectObject(&pen);
        for (size_t i = 1; i < ctrl.size(); ++i) {
            pDC->MoveTo(static_cast<int>(ctrl[i - 1].x), static_cast<int>(ctrl[i - 1].y));
            pDC->LineTo(static_cast<int>(ctrl[i].x), static_cast<int>(ctrl[i].y));
        }
        pDC->SelectObject(oldPen);
    }

    void DrawBSpline(CDC* pDC, const std::vector<Point2>& ctrl) const {
        if (ctrl.size() < 4) return;

        CPen pen(PS_SOLID, 2, RGB(20, 90, 220));
        CPen* oldPen = pDC->SelectObject(&pen);

        const int stepsPerSeg = 30;
        bool firstPoint = true;
        for (size_t i = 0; i + 3 < ctrl.size(); ++i) {
            for (int s = 0; s <= stepsPerSeg; ++s) {
                const double u = static_cast<double>(s) / static_cast<double>(stepsPerSeg);
                const Point2 q = EvalUniformCubicBSpline(ctrl[i], ctrl[i + 1], ctrl[i + 2], ctrl[i + 3], u);
                const int x = static_cast<int>(q.x);
                const int y = static_cast<int>(q.y);
                if (firstPoint) {
                    pDC->MoveTo(x, y);
                    firstPoint = false;
                } else {
                    pDC->LineTo(x, y);
                }
            }
        }

        pDC->SelectObject(oldPen);
    }

    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct) {
        if (CView::OnCreate(lpCreateStruct) == -1) return -1;
        m_animCtrl = m_baseCtrl;
        SetTimer(1, 40, nullptr);
        return 0;
    }

    afx_msg void OnDestroy() {
        KillTimer(1);
        CView::OnDestroy();
    }

    afx_msg void OnTimer(UINT_PTR nIDEvent) {
        if (nIDEvent == 1) {
            m_phase += 0.07;
            if (m_phase > 2.0 * kPi) m_phase -= 2.0 * kPi;
            UpdateAnimatedControlPoints();
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

        DrawControlPolygon(pDC, m_animCtrl);
        DrawBSpline(pDC, m_animCtrl);
        pDC->TextOut(-200, 180, _T("Exp3 (21+): Uniform Cubic B-Spline Animation"));
    }

    DECLARE_MESSAGE_MAP()
};

IMPLEMENT_DYNCREATE(CExp3View, CView)
BEGIN_MESSAGE_MAP(CExp3View, CView)
ON_WM_CREATE()
ON_WM_DESTROY()
ON_WM_TIMER()
END_MESSAGE_MAP()

class CExp3Frame : public CFrameWnd {
public:
    CExp3Frame() {
        Create(nullptr, _T("Exp3 (21+) - B-Spline Curve Animation"), WS_OVERLAPPEDWINDOW, CRect(120, 80, 1220, 860));
    }
};

class CExp3App : public CWinApp {
public:
    BOOL InitInstance() override {
        CWinApp::InitInstance();
        auto* frame = new CExp3Frame();
        auto* view = new CExp3View();
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

CExp3App theApp;
