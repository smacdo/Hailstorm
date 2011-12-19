#ifndef SCOTT_MAIN_WINDOW_H
#define SCOTT_MAIN_WINDOW_H

#include "window.h"

class MainWindow : public Window
{
public:
    MainWindow( HINSTANCE hInstance,
                const std::wstring& windowTitle,
                unsigned int width,
                unsigned int height );
    virtual ~MainWindow();

private:
};

#endif