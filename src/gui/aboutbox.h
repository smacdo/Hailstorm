#ifndef SCOTT_ABOUT_BOX_H
#define SCOTT_ABOUT_BOX_H

#include <boost/utility.hpp>

class Window;

class AboutBox : boost::noncopyable
{
public:
    AboutBox( HINSTANCE hInstance, HWND hWnd );
    ~AboutBox();

    void show();

private:
    HINSTANCE mAppInstance;
    HWND mParentWindow;
};

#endif