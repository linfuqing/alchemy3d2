#pragma once

#include "../Graphics/AlchemyShader.h"

struct IDirect3DVertexShader9;
struct IDirect3DPixelShader9;
namespace alchemy
{
	class CD3D9Device;
	class CD3D9Shader : 
		public CShader
	{
		friend class CD3D9Device;
	public:
		typedef struct
		{
			IDirect3DVertexShader9* pVertexShader;
			IDirect3DPixelShader9* pPixelShader;
		}SHADER;

		CD3D9Shader(void);
		~CD3D9Shader(void);

		const SHADER& GetInstance()const;

		void Destroy();

	protected:
		bool _Create();

	private:
		SHADER m_Shader;
	};

	ALCHEMY_INLINE const CD3D9Shader::SHADER& CD3D9Shader::GetInstance()const
	{
		return m_Shader;
	}
}