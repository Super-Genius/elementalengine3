///============================================================================
/// \note   Elemental Engine
///         Copyright (C) 2013 Social Systems Technology, Inc.
///
///         This code is redistributable under the terms of the EE License.
///
///         This code is distributed without warranty or implied warranty of
///			merchantability or fitness for a particular purpose. See the 
///			EE License for more details.
///
///         You should have received a copy of the EE License along with this
///			code; If not, write to Social Systems Technology, Inc.,
///			109 East 17th Street Suite 4210 Cheyenne, WY 82001 USA
///============================================================================

#ifndef CGLVERTEXSHADER_H
#define CGLVERTEXSHADER_H

class CVertexShader : public CObjectTemplate<IShader >
{
	CVertexShader(IHashString *parentName, IHashString *name);
public:
	~CVertexShader();
	virtual bool Apply( IShaderDefines *pDefines );
	virtual bool Reload( IShaderDefines *pDefines );
	virtual bool LoadVariant( IShaderDefines *pDefines, bool bCompile );
	virtual bool SetConstant( DWORD reg, float * data, int num );
	virtual bool SetConstant( DWORD reg, float x, float y, float z, float w );

public:
	/// IObject  functions
	/// \brief	Create the a instance of IObject
	/// \return	IComponent pointer
	static IComponent *Create(int nArgs, va_list argptr);
	/// \brief	The Initialization for the Object
	/// \param	IManager* pManager: Parent Manager
	virtual void Init();
	/// \brief	The Update Function for the Object
	/// \return	True on sucess, False on faulure
	virtual bool Update();
	/// \brief	The DeInit Function for the Object
	virtual void DeInit();
	/// \brief either reads or writes the entity information to a file
	virtual void Serialize(IArchive &ar);

private:
	void ReleaseShader();

	StdString m_Code;
	GLuint			m_VertexShader;
	//LPD3DXCONSTANTTABLE				m_VertexShaderConstTable; //release this after
	IRenderer	* m_Renderer;
	typedef list<unsigned long> SHADERVARIANTSLIST;
	typedef map<unsigned long, GLuint> SHADERVARIANTSMAP;
	SHADERVARIANTSMAP m_ShaderVariants;
	bool m_bVariantMissingOneTimeWarning;
};
#endif