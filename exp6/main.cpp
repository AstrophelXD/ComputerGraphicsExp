#ifndef _AFXDLL
#define _AFXDLL
#endif

#include <afxwin.h>
#include <cmath>
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

struct Vec3 {
    float x;
    float y;
    float z;
};

static constexpr float PI = 3.14159265358979323846f;
static float g_angle = 0.0f;

static Vec3 sub(const Vec3& a, const Vec3& b) { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
static Vec3 cross(const Vec3& a, const Vec3& b) {
    return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x};
}
static Vec3 normalize(const Vec3& v) {
    const float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
    if (len < 1e-6f) return {0.0f, 1.0f, 0.0f};
    return {v.x / len, v.y / len, v.z / len};
}

static void emitQuad(const Vec3& v0, const Vec3& v1, const Vec3& v2, const Vec3& v3) {
    const Vec3 n = normalize(cross(sub(v1, v0), sub(v2, v0)));
    glNormal3f(n.x, n.y, n.z);
    glVertex3f(v0.x, v0.y, v0.z);
    glVertex3f(v1.x, v1.y, v1.z);
    glVertex3f(v2.x, v2.y, v2.z);
    glVertex3f(v3.x, v3.y, v3.z);
}

// 实验6精简版：仅实现 21 组及以后 => 圆环体 + 绿宝石材质
static void drawTorus(float r1, float r2, int nAlpha, int nBeta) {
    const float da = 2.0f * PI / static_cast<float>(nAlpha);
    const float db = 2.0f * PI / static_cast<float>(nBeta);
    glBegin(GL_QUADS);
    for (int i = 0; i < nAlpha; ++i) {
        const float a0 = i * da, a1 = ((i + 1) % nAlpha) * da;
        for (int j = 0; j < nBeta; ++j) {
            const float b0 = j * db, b1 = ((j + 1) % nBeta) * db;
            emitQuad(
                {(r1 + r2 * std::sin(b0)) * std::cos(a0), r2 * std::cos(b0), (r1 + r2 * std::sin(b0)) * std::sin(a0)},
                {(r1 + r2 * std::sin(b0)) * std::cos(a1), r2 * std::cos(b0), (r1 + r2 * std::sin(b0)) * std::sin(a1)},
                {(r1 + r2 * std::sin(b1)) * std::cos(a1), r2 * std::cos(b1), (r1 + r2 * std::sin(b1)) * std::sin(a1)},
                {(r1 + r2 * std::sin(b1)) * std::cos(a0), r2 * std::cos(b1), (r1 + r2 * std::sin(b1)) * std::sin(a0)});
        }
    }
    glEnd();
}

static void setupLightingAndMaterial() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    const GLfloat ambientLight[] = {200.0f / 255.0f, 200.0f / 255.0f, 200.0f / 255.0f, 1.0f};
    const GLfloat diffuseLight[] = {250.0f / 255.0f, 250.0f / 255.0f, 250.0f / 255.0f, 1.0f};
    const GLfloat specularLight[] = {250.0f / 255.0f, 250.0f / 255.0f, 250.0f / 255.0f, 1.0f};
    const GLfloat lightPos[] = {4.0f, 6.0f, 8.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    const GLfloat emeraldAmbient[] = {0.0215f, 0.1745f, 0.0215f, 1.0f};
    const GLfloat emeraldDiffuse[] = {0.07568f, 0.61424f, 0.07568f, 1.0f};
    const GLfloat emeraldSpecular[] = {0.633f, 0.727811f, 0.633f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, emeraldAmbient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, emeraldDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, emeraldSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 76.8f);
}

class CExp6View : public CView {
    DECLARE_DYNCREATE(CExp6View)
public:
    CExp6View() = default;
    ~CExp6View() override = default;

protected:
    HGLRC m_hRC = nullptr;

    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct) {
        if (CView::OnCreate(lpCreateStruct) == -1) return -1;
        CDC* pDC = GetDC();
        PIXELFORMATDESCRIPTOR pfd = {};
        pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
        pfd.nVersion = 1;
        pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
        pfd.iPixelType = PFD_TYPE_RGBA;
        pfd.cColorBits = 24;
        pfd.cDepthBits = 24;
        pfd.iLayerType = PFD_MAIN_PLANE;
        const int pf = ChoosePixelFormat(pDC->GetSafeHdc(), &pfd);
        if (pf == 0 || !SetPixelFormat(pDC->GetSafeHdc(), pf, &pfd)) {
            ReleaseDC(pDC);
            return -1;
        }
        m_hRC = wglCreateContext(pDC->GetSafeHdc());
        wglMakeCurrent(pDC->GetSafeHdc(), m_hRC);
        setupLightingAndMaterial();
        ReleaseDC(pDC);
        SetTimer(1, 16, nullptr);
        return 0;
    }

    afx_msg void OnDestroy() {
        KillTimer(1);
        if (m_hRC) {
            CDC* pDC = GetDC();
            wglMakeCurrent(pDC->GetSafeHdc(), nullptr);
            wglDeleteContext(m_hRC);
            m_hRC = nullptr;
            ReleaseDC(pDC);
        }
        CView::OnDestroy();
    }

    afx_msg void OnSize(UINT nType, int cx, int cy) {
        CView::OnSize(nType, cx, cy);
        if (!m_hRC || cx <= 0 || cy <= 0) return;
        CDC* pDC = GetDC();
        wglMakeCurrent(pDC->GetSafeHdc(), m_hRC);
        glViewport(0, 0, cx, cy);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, static_cast<double>(cx) / static_cast<double>(cy), 0.1, 100.0);
        glMatrixMode(GL_MODELVIEW);
        ReleaseDC(pDC);
    }

    afx_msg void OnTimer(UINT_PTR nIDEvent) {
        if (nIDEvent == 1) {
            g_angle = std::fmod(g_angle + 0.6f, 360.0f);
            Invalidate(FALSE);
        }
        CView::OnTimer(nIDEvent);
    }

    void OnDraw(CDC* pDC) override {
        if (!m_hRC) return;
        wglMakeCurrent(pDC->GetSafeHdc(), m_hRC);
        glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glLoadIdentity();
        gluLookAt(4.0, 3.0, 8.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
        glRotatef(-20.0f, 1.0f, 0.0f, 0.0f);
        glRotatef(g_angle, 0.0f, 1.0f, 0.0f);
        drawTorus(1.1f, 0.4f, 64, 32);
        SwapBuffers(pDC->GetSafeHdc());
    }

    DECLARE_MESSAGE_MAP()
};

IMPLEMENT_DYNCREATE(CExp6View, CView)
BEGIN_MESSAGE_MAP(CExp6View, CView)
ON_WM_CREATE()
ON_WM_DESTROY()
ON_WM_SIZE()
ON_WM_TIMER()
END_MESSAGE_MAP()

class CExp6Frame : public CFrameWnd {
public:
    CExp6Frame() {
        Create(nullptr, _T("Exp6 (21+) - MFC Minimal"), WS_OVERLAPPEDWINDOW, CRect(100, 80, 1200, 820));
    }
};

class CExp6App : public CWinApp {
public:
    BOOL InitInstance() override {
        CWinApp::InitInstance();
        auto* frame = new CExp6Frame();
        auto* view = new CExp6View();
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

CExp6App theApp;
