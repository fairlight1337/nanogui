/*
    nanogui/glcanvas.h -- Canvas widget for rendering GL

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <nanogui/window.h>
#include <nanogui/screen.h>
#include <nanogui/glcanvas.h>
#include <nanogui/theme.h>
#include <nanogui/opengl.h>
#include <nanogui/serializer/core.h>

NAMESPACE_BEGIN(nanogui)

GLCanvas::GLCanvas(Widget *parent)
  : Widget(parent), mBackgroundColor(Vector4i(128, 128, 128, 255)), mDrawingCallback([](){}), mMouseButtonCallback([](const Vector2i&, int, bool, int){}), mMouseMotionCallback([](const Vector2i&, const Vector2i&, int, int){}), mDrawBorder(true) {
    mSize = Vector2i(250, 250);
}

void GLCanvas::drawWidgetBorder(NVGcontext* ctx) const {
    nvgBeginPath(ctx);
    nvgStrokeWidth(ctx, 1.0f);
    nvgRoundedRect(ctx, mPos.x() - 0.5f, mPos.y() - 0.5f,
                   mSize.x() + 1, mSize.y() + 1, mTheme->mWindowCornerRadius);
    nvgStrokeColor(ctx, mTheme->mBorderLight);
    nvgRoundedRect(ctx, mPos.x() - 1.0f, mPos.y() - 1.0f,
                   mSize.x() + 2, mSize.y() + 2, mTheme->mWindowCornerRadius);
    nvgStrokeColor(ctx, mTheme->mBorderDark);
    nvgStroke(ctx);
}

void GLCanvas::draw(NVGcontext *ctx) {
    Widget::draw(ctx);
    nvgEndFrame(ctx);

    if (mDrawBorder)
        drawWidgetBorder(ctx);

    const Screen* screen = dynamic_cast<const Screen*>(this->window()->parent());
    assert(screen);

    Vector2f screenSize = screen->size().cast<float>();
    Vector2i positionInScreen = absolutePosition();
    Vector2i imagePosition = Vector2i(positionInScreen[0], screenSize[1] - positionInScreen[1] - mSize[1]);

    GLint arrnViewport[4];
    glGetIntegerv(GL_VIEWPORT, arrnViewport);

    glViewport(imagePosition[0], imagePosition[1], mSize[0], mSize[1]);

    glEnable(GL_SCISSOR_TEST);
    glScissor(imagePosition[0], imagePosition[1], mSize[0], mSize[1]);
    glClearColor(mBackgroundColor[0], mBackgroundColor[1], mBackgroundColor[2], mBackgroundColor[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    mDrawingCallback();
    glDisable(GL_SCISSOR_TEST);

    glViewport(arrnViewport[0], arrnViewport[1], arrnViewport[2], arrnViewport[3]);
}

void GLCanvas::setGLDrawingCallback(std::function<void()> fncDraw) {
    mDrawingCallback = fncDraw;
}

bool GLCanvas::mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers) {
    mMouseButtonCallback(p, button, down, modifiers);

    return true;
}

bool GLCanvas::mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers) {
    mMouseMotionCallback(p, rel, button, modifiers);

    return true;
}

void GLCanvas::setMouseButtonCallback(std::function<void(const Vector2i&, int, bool, int)> fncMouseButtonCallback) {
    mMouseButtonCallback = fncMouseButtonCallback;
}

void GLCanvas::setMouseMotionCallback(std::function<void(const Vector2i&, const Vector2i&, int, int)> fncMouseMotionCallback) {
    mMouseMotionCallback = fncMouseMotionCallback;
}

void GLCanvas::save(Serializer &s) const {
    Widget::save(s);
    s.set("backgroundColor", mBackgroundColor);
    s.set("drawBorder", mDrawBorder);
}

bool GLCanvas::load(Serializer &s) {
    if (!Widget::load(s)) return false;
    if (!s.get("backgroundColor", mBackgroundColor)) return false;
    if (!s.get("drawBorder", mDrawBorder)) return false;
    return true;
}

NAMESPACE_END(nanogui)
