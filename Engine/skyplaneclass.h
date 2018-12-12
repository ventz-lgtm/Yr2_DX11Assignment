////////////////////////////////////////////////////////////////////////////////
// Filename: skyplaneclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SKYPLANECLASS_H_
#define _SKYPLANECLASS_H_


//////////////
// INCLUDES //
//////////////
#include <d3d11_1.h>
#include <directXMath.h>


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "textureclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: SkyPlaneClass
////////////////////////////////////////////////////////////////////////////////
class SkyPlaneClass
{
private:
	// The SkyPlaneType structure is used for storing the sky plane geometry.We generate position and texture coordinates for the plane and then store them in an array of SkyPlaneType.There are no normals since the clouds use the sky dome for color and lighting appearance.

	struct SkyPlaneType
	{
		float x, y, z;
		float tu, tv;
	};
	// The VertexType requires position and texture coordinates for rendering the sky plane.

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
	};

public:
	SkyPlaneClass();
	SkyPlaneClass(const SkyPlaneClass&);
	~SkyPlaneClass();

	bool Initialize(ID3D11Device*, WCHAR*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);
	void Frame();

	int GetIndexCount();
	ID3D11ShaderResourceView* GetCloudTexture1();
	ID3D11ShaderResourceView* GetCloudTexture2();

	float GetBrightness();
	float GetTranslation(int);

private:
	bool InitializeSkyPlane(int, float, float, float, int);
	void ShutdownSkyPlane();

	bool InitializeBuffers(ID3D11Device*, int);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	bool LoadTextures(ID3D11Device*, WCHAR*, WCHAR*);
	void ReleaseTextures();

private:
	// The m_skyPlane array is used to hold the plane geometry.

	SkyPlaneType* m_skyPlane;
	int m_vertexCount, m_indexCount;
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	// The sky plane will use two cloud textures that are rendered to it.

	TextureClass *m_CloudTexture1, *m_CloudTexture2;
	// The brightness of the clouds is stored here and set in the pixel shader during rendering.

	float m_brightness;
	// The cloud location and speed are stored in these two arrays.

	float m_translationSpeed[4];
	float m_textureTranslation[4];
};

#endif