#include <memory>
#include <vector>
#include <ctime>
#include <random>

using namespace std;

#define NOMINMAX
//#define RAND_MAX 255

#include <wrl.h>
#include <atlbase.h>
#include <atlwin.h>
#define ASSERT ATLASSERT
#define VERIFY ATLVERIFY
#define TRACE ATLTRACE

#include <d2d1.h>
#pragma comment(lib, "d2d1")
using namespace D2D1;
using namespace Microsoft::WRL;

#ifdef DEBUG
#define HR(expression) ASSERT(S_OK == (expression))
#else
struct ComException
{
	HRESULT const hr;
	ComException(HRESULT const value) : hr(value) {}
};

inline void HR(HRESULT hr)
{
	if (S_OK != hr) throw ComException(hr);
}
#endif

#include "Scene.h"
#include "CircleRenderer.h"
#include "SquareRenderer.h"
#include "Engine.h"

template <typename T>
class DesktopWindow : public CWindowImpl<T, CWindow, CWinTraits<WS_OVERLAPPEDWINDOW | WS_VISIBLE>> {
public:
	ComPtr<ID2D1Factory> _factory;
	ComPtr<ID2D1HwndRenderTarget> _target;

	DECLARE_WND_CLASS_EX(L"window", CS_HREDRAW | CS_VREDRAW, -1);

	BEGIN_MSG_MAP(SampleWindow)
		MESSAGE_HANDLER(WM_PAINT, PaintHandler)
		MESSAGE_HANDLER(WM_DESTROY, DestroyHandler)
		MESSAGE_HANDLER(WM_SIZE, SizeHandler)
		MESSAGE_HANDLER(WM_DISPLAYCHANGE, DisplayChangeHandler)
	END_MSG_MAP()

	LRESULT DisplayChangeHandler(UINT, WPARAM, LPARAM, BOOL&)
	{
		Invalidate();
		return 0;
	}

	LRESULT SizeHandler(UINT, WPARAM, LPARAM lparam, BOOL&)
	{
		if (_target)
		{
			if (S_OK != _target->Resize(SizeU(LOWORD(lparam), HIWORD(lparam))))
			{
				_target.Reset();
			}
		}

		return 0;
	}

	LRESULT PaintHandler(UINT, WPARAM, LPARAM, BOOL&)
	{
		PAINTSTRUCT ps;
		VERIFY(BeginPaint(&ps));

		Render();

		EndPaint(&ps);
		return 0;
	}

	LRESULT DestroyHandler(UINT, WPARAM, LPARAM, BOOL&)
	{
		PostQuitMessage(0);
		return 0;
	}

	void Invalidate()
	{
		VERIFY(InvalidateRect(nullptr, false));
	}

	void Create()
	{
		D2D1_FACTORY_OPTIONS fo = {};
#ifdef DEBUG
		fo.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

		HR(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, fo, _factory.GetAddressOf()));

		CreateDeviceIndependentResources();

		VERIFY(__super::Create(nullptr, 0, L"C SCI 151"));
	}

	void CreateDeviceIndependentResources()
	{
	}

	virtual void CreateDeviceResources() = 0;

	void Render()
	{
		// create render target if needed
		if (!_target)
		{
//			_target->Resize(SizeU(640, 480)&);
			RECT rect;
			VERIFY(GetClientRect(&rect));
//			_windowsize = D2D1_SIZE_F{ float(rect.right), float(rect.bottom) };


			// Below is the relative size of the window. Still looking for the absolute original size.
			 auto size = SizeU(rect.right, rect.bottom);
			// auto size = SizeU(640, 480); 

			HR(_factory->CreateHwndRenderTarget(RenderTargetProperties(),
				HwndRenderTargetProperties(m_hWnd, size),
				_target.GetAddressOf()));

			CreateDeviceResources();
		}

		// draw the scene
		if (!(D2D1_WINDOW_STATE_OCCLUDED & _target->CheckWindowState()))
		{
			_target->BeginDraw();

			Draw();

			if (D2DERR_RECREATE_TARGET == _target->EndDraw())
			{
				_target.Reset();
			}
		}
	}

	virtual void Update(double deltaTime) = 0;
	virtual void Draw() = 0;

	int processFrame(double deltaTime)
	{
		Update(deltaTime);

		PAINTSTRUCT ps;
		VERIFY(BeginPaint(&ps));

		Render();

		EndPaint(&ps);
		return 0;
	}

	int Run()
	{
		const int FRAMES_PER_SECOND = 60;
		bool finished = false;
		MSG message;
		while (!finished) {
			DWORD currentTick = GetTickCount();
			DWORD endTick = currentTick + 1000 / FRAMES_PER_SECOND;

			while (currentTick < endTick) {
				if (PeekMessage(&message, m_hWnd, 0, 0, PM_REMOVE)) {
					if (WM_QUIT == message.message) {
						finished = true;
						break;
					}
					else {
						TranslateMessage(&message);
						DispatchMessage(&message);
					}
					currentTick = GetTickCount();
				}
				else {
					break;
				}
			}

			processFrame(1000 / FRAMES_PER_SECOND);
		}

		return 0;
	}

	D2D1_SIZE_F GetWindowSize()
	{
		return D2D1_SIZE_F{ RenderTargetProperties().dpiX, RenderTargetProperties().dpiY };
	}
//
//private:
//	D2D1_SIZE_F _windowsize;
};

class SampleWindow : public DesktopWindow<SampleWindow> {
public:
	SampleWindow()
		: _scene{}, _engine{}
	{
		// initialize device resources
		brushDeviceResource = make_shared<BrushDeviceResource>();
		_engine.AddDeviceResource(static_pointer_cast<IDeviceResource, BrushDeviceResource>(brushDeviceResource));



		// setup scene
		//auto gameObject = make_unique<GameObject>(static_pointer_cast<IRenderer, CircleRenderer>(yellowCircleRenderer), MPoint2F{ 150.f, 150.f });
		//_scene.insertGameObject(gameObject);
		//for (auto it = objects.begin(); it != objects.end(); ++it)
		//{

		//	float x = float(rand() % 500);
		//	float y = float(rand() % 500);
		//	auto gameObject = make_unique<GameObject>(static_pointer_cast<IRenderer, CircleRenderer>(*it), MPoint2F{ x, y });
		//	_scene.insertGameObject(gameObject);
		//}
		// TODO: add more objects to the scene --> DONE (randomly generated)
	}

	virtual void MakeShapes(int width, int height)
	{
		// initialize renderers
		auto yellowCircleRenderer = make_shared<CircleRenderer>(brushDeviceResource, Color4F{ 1.f, 1.f, 0.f, 1.f }, 50.f);
		//// TODO: add more renderers here
		//auto redCircleRenderer = make_shared<CircleRenderer>(brushDeviceResource, Color4F{ 255.f, 1.f, 1.f, 1.f }, 50.f);

		circleObjects.push_back(yellowCircleRenderer);
		random_device device;
		mt19937 engine(device());
		uniform_real_distribution<> dist(0.0, 1.0);
		while (circleObjects.size() <= 10)
		{
			circleObjects.push_back(make_shared<CircleRenderer>(brushDeviceResource, Color4F{ float(dist(engine)), float(dist(engine)), float(dist(engine)), 1.f }, float(rand() % 100)));
			squareObjects.push_back(make_shared<SquareRenderer>(brushDeviceResource, Color4F{ float(dist(engine)), float(dist(engine)), float(dist(engine)), 1.f }, float(rand() % 100)));

		}

		uniform_real_distribution<> dist2(-1.0, 1.0);
		for (auto it = circleObjects.begin(); it != circleObjects.end(); ++it)
		{

			float x = float(rand() % int(width));
			float y = float(rand() % int(height));
			float spx = float(10 * dist2(engine));
			float spy = float(10 * dist2(engine));
			auto gameObject = make_unique<GameObject>(static_pointer_cast<IRenderer, CircleRenderer>(*it), MPoint2F{ x, y }, Speed2DF{ spx, spy });
			_scene.insertGameObject(gameObject);
		}

		for (auto it = squareObjects.begin(); it != squareObjects.end(); ++it)
		{

			float x = float(rand() % int(width));
			float y = float(rand() % int(height));
			float spx = float(10 * dist(engine));
			float spy = float(10 * dist(engine));
			auto gameObject = make_unique<GameObject>(static_pointer_cast<IRenderer, SquareRenderer>(*it), MPoint2F{ x, y }, Speed2DF{ spx, spy });
			_scene.insertGameObject(gameObject);
		}
	}

	virtual void CreateDeviceResources() override
	{
		_engine.ResetDeviceResource(*_target.Get());
	}

	virtual void Draw() override
	{
		_target->Clear(D2D_COLOR_F{ 0.26f, 0.56f, 0.87f, 1.0f });
		_scene.Draw(*_target.Get());
	}


	virtual void Update(double deltaTime) override
	{
		_scene.Update(deltaTime);
	}

private:
	Scene _scene;
	Engine _engine;
	vector<shared_ptr<CircleRenderer>> circleObjects;
	vector<shared_ptr<SquareRenderer>> squareObjects;
	shared_ptr<BrushDeviceResource> brushDeviceResource;
};

int __stdcall wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
	SampleWindow window;
	
	window.Create();
	window.MakeShapes(500, 500);
	return window.Run();
}