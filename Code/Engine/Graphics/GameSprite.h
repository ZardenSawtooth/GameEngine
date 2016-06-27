#ifndef GAMESPRITE_H
#define GAMESPRITE_H

//Texture and Sprites and other functions.
#include <d3dx9.h>
#include <string>

class GameSprite
{
public:
	//Constructors and Destructor
	GameSprite();
	GameSprite(float x, float y);
	~GameSprite();
	
	//Sprite Functions
	bool Initialize(LPDIRECT3DDEVICE9 device, std::string file, int width, int height);
	bool IsInitialized();
	virtual void Update(float gameTime, int i_keypress);
	virtual void UpdateHeight(float gameTime, int height);
	virtual void Draw(float gameTime);
private:
	//Pointers
	LPDIRECT3DTEXTURE9 tex;
	LPD3DXSPRITE sprite;

	//Attributes
	D3DXVECTOR3 position;
	D3DCOLOR color;
	bool initialized;
	RECT * m_spriteCoord = NULL;
};

#endif /* GAMESPRITE_H */