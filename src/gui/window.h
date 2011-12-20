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
    bool didUserQuit() const;
    virtual bool processMessages();
    virtual LRESULT handleMessage( UINT message, WPARAM wParam, LPARAM lParam );

    // Starts the exit process
    void exit();

    HWND windowHandle() const;
    HINSTANCE appInstance() const;

    void setWindowHandle( HWND hWnd );
    unsigned int width() const;
    unsigned int height() const;

private:
    const std::wstring mAppClassName;
    std::wstring mTitle;
    unsigned int mWidth;
    unsigned int mHeight;
    HINSTANCE mAppInstance;
    HWND mWindowHandle;
    bool mCreated;
    bool mUserQuit;
};

#endif