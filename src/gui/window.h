#ifndef SCOTT_WINDOW_H
#define SCOTT_WINDOW_H

#include <string>
#include <boost/utility.hpp>

class Window : boost::noncopyable
{
public:
    Window( HINSTANCE hInstance,
            const std::wstring& windowTitle,
            unsigned int width,
            unsigned int height );
    virtual ~Window();
    void create();
    void show();
    virtual LRESULT handleMessage( UINT message, WPARAM wParam, LPARAM lParam );

    HWND windowHandle() const;
    HINSTANCE appInstance() const;

    void setWindowHandle( HWND hWnd );

private:
    const std::wstring mAppClassName;
    std::wstring mTitle;
    unsigned int mWidth;
    unsigned int mHeight;
    HINSTANCE mAppInstance;
    HWND mWindowHandle;
    bool mCreated;
};

#endif