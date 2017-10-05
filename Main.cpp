#include <windows.h>
#include <Windows.h>
#include <d3d11.h>
#include <d3dx11.h>
#include <dxerr.h>

#define _XM_NO_INTRINSICS_
#define XM_NO_ALIGNMENT
#include <xnamath.h>

/** To use instead of an array of characters */
#include <string>

/** To assist in varying the screen-clear colour in Extra Exercise 01a */
#include <random>

/** This is where all constant values are stored, in order to tidy up this file */
#include "ConstantValues.h"

//////////////////////////////////////////////////////////////////////////////////////
//	Global Variables (seems as though g_ would equate to a global variable then
//////////////////////////////////////////////////////////////////////////////////////
HINSTANCE	g_hInst = NULL;
HWND		g_hWnd = NULL;

/** For Exercise 02 of Tutorial 01 */
D3D_DRIVER_TYPE         g_driverType = D3D_DRIVER_TYPE_NULL;
D3D_FEATURE_LEVEL       g_featureLevel = D3D_FEATURE_LEVEL_11_0;
ID3D11Device*           g_pD3DDevice = NULL;
ID3D11DeviceContext*    g_pImmediateContext = NULL;
IDXGISwapChain*         g_pSwapChain = NULL;

/** For Exercise 01 of Tutorial 03 */
ID3D11Buffer* g_pVertexBuffer;
ID3D11VertexShader* g_pVertexShader;
ID3D11PixelShader* g_pPixelShader;
ID3D11InputLayout* g_pInputLayout;

/** For Extra Exercise 01 of Tutorial 03 */

// 'Define vertex structure'
struct POS_COL_VERTEX
{
	XMFLOAT3 Pos;
	XMFLOAT4 Col;
};

// 'Define vertices of a triangle - screen coordinates -1.0f to +1.0'
POS_COL_VERTEX g_vertices[G_VERTICES_COUNT] =
{
	{ XMFLOAT3(0.9f, 0.9f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) },
	{ XMFLOAT3(0.9f, -0.9f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f) },
	{ XMFLOAT3(-0.9f, -0.9f, 0.0f), XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f) }
};



/** For exercise 01 of Tutorial 02 */
ID3D11RenderTargetView* g_pBackBufferRTView = NULL;

/** For clearing the back buffer (leave the alpha value at 1.0f) */
//float  g_clear_colour[G_CLEAR_COLOUR_ARRAY_SIZE] = { 0.0f, 0.0f, 0.0f, 1.0f };

/** For the Advanced Exercise of Tutorial 02 */
DXGI_SWAP_CHAIN_DESC DefaultSwapChainDescription;

/** For Extra Exercise 01 of Tutorial 03 */

/** Lock and unlock the buffer (using Map() and UnMap()) before updating the vertices */
void UpdateVertices();

//////////////////////////////////////////////////////////////////////////////////////
//	Forward declarations
//////////////////////////////////////////////////////////////////////////////////////
HRESULT InitialiseWindow(HINSTANCE InstanceHandle, int nCmdShow);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/** For Exercise 02 of Tutorial 01 */
HRESULT InitialiseD3D();
void ShutdownD3D();

/** For Exercise 01 of Tutorial 02 */
void RenderFrame(void); // (enquire about the use of void here, to identify no parameters)

						/** For exercise 01 of Tutorial 03 */
HRESULT InitialiseGraphics(void); // void is void ^^

								  /** For Extra Exercise 01 of Tutorial 03 */
void ManageKeyPressed(int VirtualKeyCode);

// For the range of position/colour values
void MaintainPositionValueRange();
void MaintainColourValueRange();

//////////////////////////////////////////////////////////////////////////////////////
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE InstanceHandle, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (FAILED(InitialiseWindow(InstanceHandle, nCmdShow)))
	{
		DXTRACE_MSG("Failed to create Window");
		return 0;
	}

	if (FAILED(InitialiseD3D()))
	{
		DXTRACE_MSG("Failed to create Device");
		return 0;
	}

	if (FAILED(InitialiseGraphics()))
	{
		DXTRACE_MSG("Failed to initialise graphics");
		return 0;
	}

	// Main message loop
	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			RenderFrame();
		}
	}

	// For closing Direct3D correctly:
	ShutdownD3D();
	return (int)msg.wParam;
}

// If the position or the colour of a certain vertex is altered:
void UpdateVertices()
{
	// 'Copy the vertices into the buffer'
	D3D11_MAPPED_SUBRESOURCE ms;

	// 'Lock the buffer to allow writing'
	g_pImmediateContext->Map(g_pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms);

	// 'Copy the data'
	memcpy(ms.pData, g_vertices, sizeof(g_vertices));

	// 'Unlock the buffer'
	g_pImmediateContext->Unmap(g_pVertexBuffer, NULL);
}

//////////////////////////////////////////////////////////////////////////////////////
// Register class and create window
//////////////////////////////////////////////////////////////////////////////////////
HRESULT InitialiseWindow(HINSTANCE InstanceHandle, int nCmdShow)
{
	// Register class
	WNDCLASSEX wcex = { 0 };
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = InstanceHandle;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	//   wcex.hbrBackground = (HBRUSH )( COLOR_WINDOW + 1); // Needed for non-D3D apps
	// const char* used once again:
	wcex.lpszClassName = NAME.c_str();

	if (!RegisterClassEx(&wcex)) return E_FAIL;

	// Create window
	g_hInst = InstanceHandle;
	RECT rc = { 0, 0, 640, 480 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	// A const char* can be parsed in as an LPCCHAR: 
	g_hWnd = CreateWindow(NAME.c_str(), TUTORIAL_NAME.c_str(), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left,
		rc.bottom - rc.top, NULL, NULL, InstanceHandle, NULL);
	if (!g_hWnd)
		return E_FAIL;

	ShowWindow(g_hWnd, nCmdShow);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////////////
// Called every time the application receives a message
//////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	// For Extra Exercise 01b:
	//float MouseXPosition = 0.0f;
	//float MouseYPosition = 0.0f;
	//float MouseWheelRotationDelta = 0.0f;

	// For Advanced Exercise 01:

	/**
	This result has been set to S_OK in execution so far
	(when calling ResizeBuffers).
	*/
	HRESULT ResizeResult = 0;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:
		ManageKeyPressed(wParam);
		break;

		/**
		// For altering the red...
		case WM_MOUSEMOVE:
		MouseXPosition = LOWORD(lParam);
		MouseYPosition = HIWORD(lParam);

		g_clear_colour[0] = (MouseXPosition / MouseYPosition) / 100.0f;
		break;
		// green...
		case WM_LBUTTONDOWN:
		g_clear_colour[1] += 0.010f;
		break;

		case WM_RBUTTONDOWN:
		g_clear_colour[1] -= 0.010f;
		break;

		// as well as blue components of g_clear_colour.
		case WM_MOUSEWHEEL:

		MouseWheelRotationDelta = GET_WHEEL_DELTA_WPARAM(wParam);

		// Modify the blue component accordingly:
		g_clear_colour[2] += MouseWheelRotationDelta / 1000.0f;
		break;
		*/
		// For Tutorial 02 Advanced 01:
		// CHECK TO SEE IF THIS IS THE OPTIMAL WAY TO GO ABOUT RESIZING THE BUFFERS!!OGea
	case WM_SIZE:
		if (g_pSwapChain)
		{
			// Clear the state of the device context first (so that ResizeBuffers() can succeed):
			g_pImmediateContext->ClearState();

			// Get the new bounds of the window:
			RECT ClientBounds;
			GetClientRect(g_hWnd, &ClientBounds);
			UINT NewWidth = ClientBounds.right - ClientBounds.left;
			UINT NewHeight = ClientBounds.bottom - ClientBounds.top;

			/**
			Then it is possible to resize the buffers correctly
			(Using DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH for the
			last parameter to allowing switching between full-screen
			and windowed mode. I'm also using DXGI_FORMAT_UNKNOWN, to
			preserve the existing format. Finally, I have set the first 3
			parameters as 0, to preserve the exitsting number of buffers,
			as well as having DXGI use the width and height of the client
			area of the target window, for then new width and height.)

			SOURCE: https://msdn.microsoft.com/en-us/library/bb174577(v=vs.85).aspx
			*/
			HRESULT ResizeResult = g_pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN,
				DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
		}
		break;

	default:
		// A break-point was thrown on the return line, if hWnd is not valid:
		// BREAKPOINT STILL THROWN AT TIMES GESU(GES[HJAD2!
		if (hWnd)
		{
			// Add or replace the above check with that of checking this value: hWnd->unused;
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}

	/**
	// Keep the colour components of g_clear_colour
	// within the bounds (0.0f to 1.0f):
	for (int ClearColourIterator = 0; ClearColourIterator < G_CLEAR_COLOUR_ARRAY_SIZE - 1; ClearColourIterator++)
	{
	if (g_clear_colour[ClearColourIterator] >= 1.0f)
	{
	g_clear_colour[ClearColourIterator] = 1.0f;
	}
	else if (g_clear_colour[ClearColourIterator] <= 0.0f)
	{
	g_clear_colour[ClearColourIterator] = 0.0f;
	}
	}
	*/
	return 0;
}

//////////////////////////////////////////////////////////////////////////////////////
// Create D3D device and swap chain
//////////////////////////////////////////////////////////////////////////////////////
HRESULT InitialiseD3D()
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(g_hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;

#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE, // comment out this line if you need to test D3D 11.0 functionality on hardware that doesn't support it
		D3D_DRIVER_TYPE_WARP, // comment this out also to use reference device
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC DefaultSwapChainDescription;
	ZeroMemory(&DefaultSwapChainDescription, sizeof(DefaultSwapChainDescription));
	DefaultSwapChainDescription.BufferCount = 1;
	DefaultSwapChainDescription.BufferDesc.Width = width;
	DefaultSwapChainDescription.BufferDesc.Height = height;
	DefaultSwapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	DefaultSwapChainDescription.BufferDesc.RefreshRate.Numerator = 60;
	DefaultSwapChainDescription.BufferDesc.RefreshRate.Denominator = 1;
	DefaultSwapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	DefaultSwapChainDescription.OutputWindow = g_hWnd;
	DefaultSwapChainDescription.SampleDesc.Count = 1;
	DefaultSwapChainDescription.SampleDesc.Quality = 0;
	DefaultSwapChainDescription.Windowed = true;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		g_driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(NULL, g_driverType, NULL,
			createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &DefaultSwapChainDescription, &g_pSwapChain,
			&g_pD3DDevice, &g_featureLevel, &g_pImmediateContext);
		if (SUCCEEDED(hr))
			break;
	}

	if (FAILED(hr))
		return hr;

	// Get pointer to back buffer texture
	ID3D11Texture2D *pBackBufferTexture;
	hr = g_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		(LPVOID*)&pBackBufferTexture);

	if (FAILED(hr)) return hr;

	// Use the back buffer texture pointer to create the render target view
	hr = g_pD3DDevice->CreateRenderTargetView(pBackBufferTexture, NULL,
		&g_pBackBufferRTView);
	pBackBufferTexture->Release();

	if (FAILED(hr)) return hr;

	// Set the render target view
	g_pImmediateContext->OMSetRenderTargets(1, &g_pBackBufferRTView, NULL);

	// Set the viewport
	D3D11_VIEWPORT viewport;

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	g_pImmediateContext->RSSetViewports(1, &viewport);


	return S_OK;
}

//////////////////////////////////////////////////////////////////////////////////////
// Clean up D3D objects
//////////////////////////////////////////////////////////////////////////////////////
void ShutdownD3D()
{
	if (g_pVertexBuffer) g_pVertexBuffer->Release();//'03-01'
	if (g_pInputLayout) g_pInputLayout->Release();//'03-01'
	if (g_pVertexShader) g_pVertexShader->Release();//'03-01'
	if (g_pPixelShader) g_pPixelShader->Release();//'03-01'
	if (g_pBackBufferRTView) g_pBackBufferRTView->Release();
	if (g_pSwapChain) g_pSwapChain->Release();
	if (g_pImmediateContext) g_pImmediateContext->Release();
	if (g_pD3DDevice) g_pD3DDevice->Release();
}

//////////////////////////////////////////////////////////////////////////////////////
// Init graphics
//////////////////////////////////////////////////////////////////////////////////////
HRESULT InitialiseGraphics()//03-01
{
	HRESULT hr = S_OK;

	// 'Set up and create vertex buffer'
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC; // 'Used by CPU and GPU'
	bufferDesc.ByteWidth = sizeof(POS_COL_VERTEX) * 3; // 'Total size of buffer, 3 vertices'
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER; // 'Use as a vertex buffer'
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; // 'Allow CPU access'
	hr = g_pD3DDevice->CreateBuffer(&bufferDesc, NULL, &g_pVertexBuffer); // 'Create the buffer'

	if (FAILED(hr)) // return error code on failure
	{
		return hr;
	}

	UpdateVertices();

	// 'Load and compile pixel and vertex shaders - use vs_5_0 to target DX11 hardware only'
	ID3DBlob *VS, *PS, *error;
	hr = D3DX11CompileFromFile("shaders.hlsl", 0, 0, "VShader", "vs_4_0", 0, 0, 0, &VS, &error, 0);

	if (error != 0) // 'check for shader compilation error'
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr)) // 'don't fail is error is just a warning'
		{
			return hr;
		};
	}

	hr = D3DX11CompileFromFile("shaders.hlsl", 0, 0, "PShader", "ps_4_0", 0, 0, 0, &PS, &error, 0);

	if (error != 0) // 'check for shader compilation error'
	{
		OutputDebugStringA((char*)error->GetBufferPointer());
		error->Release();
		if (FAILED(hr)) // 'don't fail is error is just a warning'
		{
			return hr;
		};
	}

	// 'Create shader objects'
	hr = g_pD3DDevice->CreateVertexShader(VS->GetBufferPointer(), VS->GetBufferSize(), NULL, &g_pVertexShader);

	if (FAILED(hr))
	{
		return hr;
	}

	hr = g_pD3DDevice->CreatePixelShader(PS->GetBufferPointer(), PS->GetBufferSize(), NULL, &g_pPixelShader);

	if (FAILED(hr))
	{
		return hr;
	}

	// 'Set the shader objects as active'
	g_pImmediateContext->VSSetShader(g_pVertexShader, 0, 0);
	g_pImmediateContext->PSSetShader(g_pPixelShader, 0, 0);

	// 'Create and set the input layout object'
	D3D11_INPUT_ELEMENT_DESC iedesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	hr = g_pD3DDevice->CreateInputLayout(iedesc, 2, VS->GetBufferPointer(), VS->GetBufferSize(), &g_pInputLayout);

	if (FAILED(hr))
	{
		return hr;
	}

	g_pImmediateContext->IASetInputLayout(g_pInputLayout);

	return S_OK;
}

/**
Handle the action(s) taken when certain keys are pressed
(This function would not account shift or ctrl pressed
whilst another key is being pressed):
*/
void ManageKeyPressed(int VirtualKeyCode)
{
	switch (VirtualKeyCode)
	{

	case VK_ESCAPE:
		DestroyWindow(g_hWnd);
		break;

	case VK_1:
		g_vertices[0].Pos.x += POSITION_MODIFICATION_SCALAR;
		break;

	case VK_2:
		g_vertices[0].Pos.x -= POSITION_MODIFICATION_SCALAR;
		break;

	case VK_3:
		g_vertices[0].Pos.y += POSITION_MODIFICATION_SCALAR;
		break;

	case VK_4:
		g_vertices[0].Pos.y -= POSITION_MODIFICATION_SCALAR;
		break;

	case VK_5:
		g_vertices[1].Pos.x += POSITION_MODIFICATION_SCALAR;
		break;

	case VK_6:
		g_vertices[1].Pos.x -= POSITION_MODIFICATION_SCALAR;
		break;

	case VK_7:
		g_vertices[1].Pos.y += POSITION_MODIFICATION_SCALAR;
		break;

	case VK_8:
		g_vertices[1].Pos.y -= POSITION_MODIFICATION_SCALAR;
		break;

	case VK_9:
		g_vertices[2].Pos.x += POSITION_MODIFICATION_SCALAR;
		break;

	case VK_0:
		g_vertices[2].Pos.x -= POSITION_MODIFICATION_SCALAR;
		break;

		// This is for the add key on the num. pad, resolve this:
	case VK_ADD:
		g_vertices[2].Pos.y += POSITION_MODIFICATION_SCALAR;
		break;

		// This is for the subtract key on the num. pad, resolve this:
	case VK_SUBTRACT:
		g_vertices[2].Pos.y -= POSITION_MODIFICATION_SCALAR;
		break;

	case VK_QKEY:
		g_vertices[0].Col.x += COLOUR_MODIFICATION_SCALAR;
		g_vertices[0].Col.y += COLOUR_MODIFICATION_SCALAR;
		g_vertices[0].Col.z += COLOUR_MODIFICATION_SCALAR;
		g_vertices[0].Col.w += COLOUR_MODIFICATION_SCALAR;
		break;

	case VK_WKEY:
		g_vertices[0].Col.x -= COLOUR_MODIFICATION_SCALAR;
		g_vertices[0].Col.y -= COLOUR_MODIFICATION_SCALAR;
		g_vertices[0].Col.z -= COLOUR_MODIFICATION_SCALAR;
		g_vertices[0].Col.w -= COLOUR_MODIFICATION_SCALAR;
		break;

	case VK_EKEY:
		g_vertices[1].Col.x += COLOUR_MODIFICATION_SCALAR;
		g_vertices[1].Col.y += COLOUR_MODIFICATION_SCALAR;
		g_vertices[1].Col.z += COLOUR_MODIFICATION_SCALAR;
		g_vertices[1].Col.w += COLOUR_MODIFICATION_SCALAR;
		break;

	case VK_RKEY:
		g_vertices[1].Col.x -= COLOUR_MODIFICATION_SCALAR;
		g_vertices[1].Col.y -= COLOUR_MODIFICATION_SCALAR;
		g_vertices[1].Col.z -= COLOUR_MODIFICATION_SCALAR;
		g_vertices[1].Col.w -= COLOUR_MODIFICATION_SCALAR;
		break;

	case VK_TKEY:
		g_vertices[2].Col.x += COLOUR_MODIFICATION_SCALAR;
		g_vertices[2].Col.y += COLOUR_MODIFICATION_SCALAR;
		g_vertices[2].Col.z += COLOUR_MODIFICATION_SCALAR;
		g_vertices[2].Col.w += COLOUR_MODIFICATION_SCALAR;
		break;

	case VK_YKEY:
		g_vertices[2].Col.x -= COLOUR_MODIFICATION_SCALAR;
		g_vertices[2].Col.y -= COLOUR_MODIFICATION_SCALAR;
		g_vertices[2].Col.z -= COLOUR_MODIFICATION_SCALAR;
		g_vertices[2].Col.w -= COLOUR_MODIFICATION_SCALAR;
		break;
	}

	// Keep the values in range:
	MaintainPositionValueRange();
	MaintainColourValueRange();

	// Then update the vertices:
	UpdateVertices();
}

void MaintainPositionValueRange()
{
	for (int Iterator = 0; Iterator < G_VERTICES_COUNT; Iterator++)
	{
		if (g_vertices[Iterator].Pos.x >= POSITION_UPPER_BOUND)
		{
			g_vertices[Iterator].Pos.x = POSITION_UPPER_BOUND;
		}
		else if (g_vertices[Iterator].Pos.x <= POSITION_LOWER_BOUND)
		{
			g_vertices[Iterator].Pos.x = POSITION_LOWER_BOUND;
		}

		if (g_vertices[Iterator].Pos.y >= POSITION_UPPER_BOUND)
		{
			g_vertices[Iterator].Pos.y = POSITION_UPPER_BOUND;
		}
		else if (g_vertices[Iterator].Pos.y <= POSITION_LOWER_BOUND)
		{
			g_vertices[Iterator].Pos.y = POSITION_LOWER_BOUND;
		}
	}
}

void MaintainColourValueRange()
{
	for (int Iterator = 0; Iterator < G_VERTICES_COUNT; Iterator++)
	{
		if (g_vertices[Iterator].Col.x >= COLOUR_UPPER_BOUND)
		{
			g_vertices[Iterator].Col.x = COLOUR_UPPER_BOUND;
		}
		else if (g_vertices[Iterator].Col.x <= COLOUR_LOWER_BOUND)
		{
			g_vertices[Iterator].Col.x = COLOUR_LOWER_BOUND;
		}

		if (g_vertices[Iterator].Col.y >= COLOUR_UPPER_BOUND)
		{
			g_vertices[Iterator].Col.y = COLOUR_UPPER_BOUND;
		}
		else if (g_vertices[Iterator].Col.y <= COLOUR_LOWER_BOUND)
		{
			g_vertices[Iterator].Col.y = COLOUR_LOWER_BOUND;
		}

		if (g_vertices[Iterator].Col.z >= COLOUR_UPPER_BOUND)
		{
			g_vertices[Iterator].Col.z = COLOUR_UPPER_BOUND;
		}
		else if (g_vertices[Iterator].Col.z <= COLOUR_LOWER_BOUND)
		{
			g_vertices[Iterator].Col.z = COLOUR_LOWER_BOUND;
		}
	}
}

// Render frame
void RenderFrame(void)
{
	// Vary the clear colour value:
	//std::default_random_engine DefaultRandomGenerator(std::random_device{}());
	// (By default, such a distribtion has a range from 0.0f to 1.0f, but I wanted
	// to make this clear):
	//std::uniform_real_distribution<float> RandomDistribution(0.0f, 1.0f); 

	//for (int ColourComponentIterator = 0; ColourComponentIterator < G_CLEAR_COLOUR_ARRAY_SIZE - 1; ColourComponentIterator++)
	//{
	//g_clear_colour[ColourComponentIterator] = RandomDistribution(DefaultRandomGenerator);
	//}

	g_pImmediateContext->ClearRenderTargetView(g_pBackBufferRTView, G_DEFAULT_CLEAR_COLOUR);

	// 'Set vertex buffer //03-01'
	UINT stride = sizeof(POS_COL_VERTEX);
	UINT offset = 0;
	g_pImmediateContext->IASetVertexBuffers(0, 1, &g_pVertexBuffer, &stride, &offset);

	// 'Select which primitive type to use //03-01'
	g_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// 'Draw the vertex buffer to the back buffer //03-01'
	g_pImmediateContext->Draw(3, 0);

	// RENDER HERE

	// Display what has just been rendered
	g_pSwapChain->Present(0, 0);
}

