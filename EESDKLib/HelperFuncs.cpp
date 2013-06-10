
#include "StdAfx.h"

extern "C" {

static DWORD getRIID = CHashString(_T("GetRendererInterface")).GetUniqueID();

IRenderer *GetRendererInterface()
{
	IRenderer *theRenderer;
	EngineGetToolBox()->SendMessage(getRIID, sizeof(IRenderer *), &theRenderer);
	return theRenderer;
}

IBaseTextureObject *CreateTexture(IHashString *Format, UINT sizeX, UINT sizeY, UINT bitDepth, RENDER_TARGET_TYPE bRenderTargetTexture,
	UINT numMips, bool bAutoGenMipMaps, IHashString *Name)
{
	CHashString Msg_CreateTexMsg( _T("CreateTexture") );
	CREATETEXTUREPARAMS	cp;
	cp.bRenderTargetTexture = bRenderTargetTexture;
	cp.sizeX = sizeX;
	cp.sizeY = sizeY;
	cp.Name = Name;
	cp.Format = Format;
	cp.bitDepth = bitDepth;
	cp.numMips = numMips;
	cp.bAutoGenMipMaps = bAutoGenMipMaps;
	if (EngineGetToolBox()->SendMessage( Msg_CreateTexMsg.GetUniqueID(), sizeof( cp ), &cp ) == MSG_HANDLED )
	{
		return cp.TextureObjectInterface;
	}

	return NULL;
}


IBaseTextureObject *LoadTexture(const TCHAR *filename)
{
	if (!filename || !filename[0])
		return NULL;

	CHashString szName(filename);
	TEXTUREOBJECTPARAMS tobj;
	tobj.bLoad = true;
	tobj.Name = &szName;
	tobj.TextureObjectInterface = NULL;
	// AddTexture message loads and adds texture to texture Manager...
	static DWORD msgHash_AddTexture = CHashString(_T("AddTexture")).GetUniqueID();
	if ( EngineGetToolBox()->SendMessage(msgHash_AddTexture, sizeof(tobj), &tobj ) == MSG_HANDLED)
	{
		return (ITextureObject *)tobj.TextureObjectInterface;
	}

	return NULL;
}

bool LoadSound(const TCHAR *fileName)
{
	LOADFILEEXTPARAMS lfep;
	lfep.fileName = (TCHAR *)fileName;
	lfep.bInternalLoad = true;
	static DWORD msgHash_LoadFileByExtension = CHashString(_T("LoadFileByExtension")).GetUniqueID();
	if (EngineGetToolBox()->SendMessage(msgHash_LoadFileByExtension, sizeof(LOADFILEEXTPARAMS), &lfep) == MSG_HANDLED)
	{
			return true;
	}

	return false;
}

// this plays a streaming sound
bool PlaySound(const TCHAR *fileName, bool looping, bool autoRemove)
{
	PLAYSOUNDPARAMS psParams;
	CHashString szName(fileName);

	psParams.fileName = &szName;
	psParams.autoRemove = autoRemove;
	psParams.looping = looping;

	// PlaySound message auto preps streaming sound and plays it.
	static DWORD msgHash_PlaySound = CHashString(_T("PlaySound")).GetUniqueID();
	static CHashString smHS(_T("CSoundManager"));
	if ( EngineGetToolBox()->SendMessage(msgHash_PlaySound, sizeof(psParams), &psParams, NULL, &smHS ) == MSG_HANDLED)
	{
		return true;
	}

	return false;

}	

/*
// this plays a streaming sound
ISoundObject *LoadSound(const TCHAR *fileName)
{
	LOADFILEEXTPARAMS lfep;
	lfep.fileName = (TCHAR *)fileName;
	lfep.bInternalLoad = true;
	static DWORD msgHash_LoadFileByExtension = CHashString(_T("LoadFileByExtension")).GetUniqueID();
	if (EngineGetToolBox()->SendMessage(msgHash_LoadFileByExtension, sizeof(LOADFILEEXTPARAMS), &lfep) == MSG_HANDLED)
	{
		// now find the sound object if loaded
		IDTOOBJECTMAP *objMap = GetObjectMap(&hsSoundObjTypeName);
		DWORD findKey = stopName->GetUniqueID();

		if (objMap != NULL)
		{
			IDTOOBJECTMAP::iterator mapIter = objMap->find(findKey);
			return true;
	}

	return false;
}

bool PlaySound(const TCHAR *fileName)
{
	PLAYSOUNDPARAMS psParams;
	CHashString szName(fileName);

	psParams.fileName = &szName;
	psParams.autoRemove = autoRemove;
	psParams.looping = looping;

	// PlaySound message auto preps streaming sound and plays it.
	static DWORD msgHash_PlaySound = CHashString(_T("PlaySound")).GetUniqueID();
	static CHashString smHS(_T("CSoundManager"));
	if ( EngineGetToolBox()->SendMessage(msgHash_PlaySound, sizeof(psParams), &psParams, NULL, &smHS ) == MSG_HANDLED)
	{
		return true;
	}

	return false;
}
*/

}