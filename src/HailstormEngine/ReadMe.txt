
Developer Notes
===============
- Rename DemoScene to Scene.
- Build out Scene hierarchy like:
     Scene  ->  BasicScene   ->   Chapter6DemoScene, etc
	        ->  LoadableScene
- Scenes should be responsible for a lot of the DirectX code, rather than DXRenderer (I think).
   This will allow me to easily add demo samples in each class. BasicScene should have device, swap, etc creation but
   each method should be overloadable.
   * Not sure yet what this means about DirectXRenderer's ultimate use and fate.
- Rename MainWindow to DesktopRenderingWindow.
- Remove IWindow implementaiton, merge into DesktopRenderingWindow
- Move both DirectXRenderer and DesktopRenderingWindow to hailstorm engine library.