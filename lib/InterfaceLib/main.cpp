#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "debug.h"
#include "../src/emul_ppc.h"
#include "defs.h"

#include <QtWidgets>

// FIXME: these must be part of the returned window ptr for multi-window apps to work
static Point pen;

// application may give us this at some point
static QDGlobals *qd;

static char *argv[] = { "" };
static int argc = 0;
static QApplication *app;
static QColor color;
static QImage *img;

class CWindow : public QWidget
{
public:
    CWindow(int width, int height) : QWidget(nullptr) {
        image = new QImage(width, height, QImage::Format_RGB32);
        resize(width, height);
        painter = new QPainter(this);
    }

    ~CWindow() {
        free(painter);
        free(image);
    }

    void paintEvent(QPaintEvent *event) {
        static QPoint point(0, 0);
        painter->begin(this);
        painter->drawImage(point, *image);
        painter->end();
    }

    QImage *image; // XXX AAAAAaaaaaaaa

private:
    QPainter *painter;
};

static CWindow *win;

extern "C" int ppc_GetApplLimit(emul_ppc_state *cpu)
{
    uint32_t ret = 0x0130;
    FIXME("() stub -> %08X", ret);
    PPC_RETURN_INT(cpu, ret);
}

extern "C" int ppc_SetApplLimit(emul_ppc_state *cpu)
{
    FIXME("(%08X) stub", PPC_ARG_INT(cpu, 1));
    return 0;
}

extern "C" int ppc_MaxApplZone(emul_ppc_state *cpu)
{
    FIXME("() stub");
    return 0;
}

extern "C" int ppc_MoreMasters(emul_ppc_state *cpu)
{
    FIXME("() stub");
    return 0;
}

extern "C" extern "C" int ppc_SysEnvirons(emul_ppc_state *cpu)
{
    uint32_t versionRequested = PPC_ARG_INT(cpu, 1);
    SysEnvRec *theWorld = (SysEnvRec*)PPC_ARG_PTR(cpu, 2);

    FIXME("(versionRequested=%d, theWorld=%p [0x%08X])", versionRequested, (void *)theWorld, PPC_ARG_INT(cpu, 2));

    theWorld->environsVersion = 0xFFFF;
    theWorld->machineType = 0xFFFF;
    theWorld->systemVersion = 0xFFFF;
    theWorld->processor = 0xFFFF;
    theWorld->hasFPU = 0;
    theWorld->hasColorQD = 1;

    PPC_RETURN_INT(cpu, 0);
}

extern "C" int ppc_InitGraf(emul_ppc_state *cpu)
{
    void *thePort = PPC_ARG_PTR(cpu, 1);

    FIXME("(thePort=%p [0x%08X]) stub", thePort, PPC_ARG_INT(cpu, 1));

    qd = (QDGlobals *)(((uint8_t *)thePort + sizeof(uint32_t)) - sizeof(QDGlobals));

    memset(qd, 0, sizeof(*qd));

    qd->screenBits.bounds.right = PPC_SHORT(1024);
    qd->screenBits.bounds.bottom = PPC_SHORT(768);

    FIXME("Setting screen size to %dx%d", PPC_SHORT(qd->screenBits.bounds.right), PPC_SHORT(qd->screenBits.bounds.bottom));
    return 0;
}

extern "C" int ppc_InitFonts(emul_ppc_state *cpu)
{
    FIXME("() stub");
    return 0;
}

extern "C" int ppc_InitWindows(emul_ppc_state *cpu)
{
    FIXME("() stub");
    app = new QApplication(argc, argv);
    return 0;
}

extern "C" int ppc_InitMenus(emul_ppc_state *cpu)
{
    FIXME("() stub");
    return 0;
}

extern "C" int ppc_TEInit(emul_ppc_state *cpu)
{
    FIXME("() stub");
    return 0;
}

extern "C" int ppc_InitDialogs(emul_ppc_state *cpu)
{
    FIXME("(...) stub");
    return 0;
}

extern "C" int ppc_InitCursor(emul_ppc_state *cpu)
{
    FIXME("() stub");
    return 0;
}

extern "C" int ppc_GetDateTime(emul_ppc_state *cpu)
{
    uint32_t *seconds = (uint32_t *)PPC_ARG_PTR(cpu, 1);

    FIXME("using UNIX time instead of correct epoch");
    *seconds = time(NULL);

    return 0;
}

extern "C" int ppc_InsetRect(emul_ppc_state *cpu)
{
    Rect *r = (Rect*)PPC_ARG_PTR(cpu, 1);
    int16_t dv = PPC_ARG_SHORT(cpu, 2);
    int16_t dh = PPC_ARG_SHORT(cpu, 3);

    r->top = PPC_SHORT(PPC_SHORT(r->top) + dv);
    r->bottom = PPC_SHORT(PPC_SHORT(r->bottom) - fminl(dv, PPC_SHORT(r->bottom)));

    r->left = PPC_SHORT(PPC_SHORT(r->left) + dh);
    r->right = PPC_SHORT(PPC_SHORT(r->right) - fminl(dh, PPC_SHORT(r->right)));

    return 0;
}

extern "C" int ppc_NewCWindow(emul_ppc_state *cpu)
{
    void *wStorage = PPC_ARG_PTR(cpu, 1);
    Rect *boundsRect = (Rect*)PPC_ARG_PTR(cpu, 2);
    const Str255 *title = (const Str255*)PPC_ARG_PTR(cpu, 3);
    uint32_t visible = PPC_ARG_INT(cpu, 4);
    uint32_t procID = PPC_ARG_INT(cpu, 5);
    void *behind = PPC_ARG_PTR(cpu, 6);
    uint32_t goAwayFlag = PPC_ARG_INT(cpu, 7);
    uint32_t refCon = PPC_ARG_INT(cpu, 8);

    FIXME("(wStorage=%p, boundsRect=%p, title = '%s' (len=%d), visible=%d, procID=%d, behind=%p, goAwayFlag=%d, refCon=%d) stub",
            wStorage, boundsRect, title->str, title->len, visible, procID, behind, goAwayFlag, refCon);

    int w = PPC_SHORT(boundsRect->right) - PPC_SHORT(boundsRect->left);
    int h = PPC_SHORT(boundsRect->bottom) - PPC_SHORT(boundsRect->top);
    win = new CWindow(w, h);
    win->setWindowTitle(title->str);
    win->show();

    return 0;
}

extern "C" int ppc_GetNewCWindow(emul_ppc_state *cpu)
{
    uint32_t windowID = PPC_ARG_INT(cpu, 1);
    void *wStorage = PPC_ARG_PTR(cpu, 2);
    void *behind = PPC_ARG_PTR(cpu, 3);

    FIXME("(windowID=0x%08X, wStorage=%p, behind=%p) stub", windowID, wStorage, behind);

    PPC_RETURN_INT(cpu, -1);
}

extern "C" int ppc_SetPort(emul_ppc_state *cpu)
{
    FIXME("(...) stub");
    return 0;
}

extern "C" int ppc_TextSize(emul_ppc_state *cpu)
{
    uint16_t size = PPC_ARG_INT(cpu, 1);

    if (size != 8)
    {
        WARN("requested %d size text, we only have 8");
    }

    return 0;
}

// FIXME: implement custom Mersenne Twister that uses qd->randSeed directly
static int rand_init = 0;
extern "C" int ppc_Random(emul_ppc_state *cpu)
{
    if (!rand_init && qd)
    {
        srand(qd->randSeed);
        rand_init = 1;
    }

    PPC_RETURN_INT(cpu, (rand() % 65536) - 32767);
}

extern "C" int ppc_RGBForeColor(emul_ppc_state *cpu)
{
    RGBColor *rgb = (RGBColor*)PPC_ARG_PTR(cpu, 1);

    color.setRgb(PPC_SHORT(rgb->red) >> 8, PPC_SHORT(rgb->green) >> 8, PPC_SHORT(rgb->blue) >> 8);

    return 0;
}

extern "C" int ppc_SetRect(emul_ppc_state *cpu)
{
    Rect *r = (Rect*)PPC_ARG_PTR(cpu, 1);
    uint16_t left = PPC_ARG_INT(cpu, 2);
    uint16_t top = PPC_ARG_INT(cpu, 3);
    uint16_t right = PPC_ARG_INT(cpu, 4);
    uint16_t bottom = PPC_ARG_INT(cpu, 5);

    r->left = PPC_SHORT(left);
    r->top = PPC_SHORT(top);
    r->right = PPC_SHORT(right);
    r->bottom = PPC_SHORT(bottom);
    return 0;
}

extern "C" int ppc_MoveTo(emul_ppc_state *cpu)
{
    uint16_t h = PPC_ARG_INT(cpu, 1);
    uint16_t v = PPC_ARG_INT(cpu, 2);

    pen.h = h;
    pen.v = v;

    return 0;
}

extern "C" int ppc_PaintOval(emul_ppc_state *cpu)
{
    Rect *r = (Rect*)PPC_ARG_PTR(cpu, 1);

    int x = PPC_SHORT(r->left),
        y = PPC_SHORT(r->top),
        w = PPC_SHORT(r->right) - PPC_SHORT(r->left),
        h = PPC_SHORT(r->bottom) - PPC_SHORT(r->top);

    QPainter painter(win->image);
    QPen qpen(color);
    QBrush brush(color);
    painter.setPen(qpen);
    painter.setBrush(brush);
    painter.drawEllipse(x, y, w, h);

    return 0;
}

extern "C" int ppc_InvertColor(emul_ppc_state *cpu)
{
    RGBColor *rgb = (RGBColor*)PPC_ARG_PTR(cpu, 1);

    rgb->red = PPC_SHORT(0xFFFF - PPC_SHORT(rgb->red));
    rgb->green = PPC_SHORT(0xFFFF - PPC_SHORT(rgb->green));
    rgb->blue = PPC_SHORT(0xFFFF - PPC_SHORT(rgb->blue));

    return 0;
}

extern "C" int ppc_DrawString(emul_ppc_state *cpu)
{
    Str255 *s = (Str255*)PPC_ARG_PTR(cpu, 1);

    QPainter painter(win->image);
    QPen qpen(color);
    QFont sansFont("Helvetica", 8);
    painter.setFont(sansFont);
    painter.setPen(qpen);
    painter.drawText(QPoint(pen.h, pen.v), s->str);

    return 0;
}

extern "C" int ppc_Button(emul_ppc_state *cpu)
{
    win->repaint();
    app->processEvents();

    PPC_RETURN_INT(cpu, 0);
}

extern "C" int ppc_FlushEvents(emul_ppc_state *cpu)
{
    FIXME("(...) stub");
    return 0;
}

extern "C" int ppc_GetMainDevice(emul_ppc_state *cpu)
{
    FIXME("() stub");
    return 0;
}

extern "C" int ppc_SetGDevice(emul_ppc_state *cpu)
{
    FIXME("(...) stub");
    return 0;
}

extern "C" int ppc_GetCTable(emul_ppc_state *cpu)
{
    FIXME("(...) stub");
    return 0;
}

extern "C" int ppc_GetIndString(emul_ppc_state *cpu)
{
    FIXME("(...) stub");
    return 0;
}

extern "C" int ppc_ParamText(emul_ppc_state *cpu)
{
    FIXME("(...) stub");
    return 0;
}

extern "C" int ppc_GetResource(emul_ppc_state *cpu)
{
    FIXME("(...) stub");
    PPC_RETURN_INT(cpu, 0);
}

extern "C" int ppc_GetMBarHeight(emul_ppc_state *cpu)
{
    FIXME("() stub");
    PPC_RETURN_INT(cpu, 8);
}

extern "C" int ppc_Alert(emul_ppc_state *cpu)
{
    FIXME("(...) stub");
    PPC_RETURN_INT(cpu, -1);
}

extern "C" int ppc_ExitToShell(emul_ppc_state *cpu)
{
    FIXME("()");
    app->quit();
    return 1;
}

extern "C" int ppc_GetMenu(emul_ppc_state *cpu)
{
    FIXME("(...) stub");
    return 0;
}

extern "C" int ppc_AppendResMenu(emul_ppc_state *cpu)
{
    FIXME("(...) stub");
    return 0;
}

extern "C" int ppc_InsertMenu(emul_ppc_state *cpu)
{
    FIXME("(...) stub");
    return 0;
}

extern "C" int ppc_DrawMenuBar(emul_ppc_state *cpu)
{
    FIXME("(...) stub");
    return 0;
}

extern "C" int ppc_SystemTask(emul_ppc_state *cpu)
{
    FIXME("(...) stub");
    return 0;
}
