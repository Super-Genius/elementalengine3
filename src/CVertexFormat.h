///============================================================================
/// \file	CVertexFormat.h
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

#ifndef CVERTEXFORMAT_H
#define CVERTEXFORMAT_H

namespace ElementalEngine
{

class CVertexFormat : public CObjectTemplate< IVertexFormat > 
{
	/// \brief The Default Constructor for CVertexFormat
	/// \param parentName = name of parent in hierarchy
	/// \param name = IHashString * name of this object
	CVertexFormat(IHashString *parentName, IHashString *name);
	
public:
	virtual ~CVertexFormat(){};
	virtual CHANNELDESCRIPTORLIST & GetChannelDescriptor();
	virtual DWORD GetTotalStride();
	virtual DWORD GetOffsetByChannel( IHashString * channelName );
	virtual DWORD GetOffsetByName( IHashString * Name );
	virtual void SetChannelDescriptor( CHANNELDESCRIPTORLIST &channelList );

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
	CHANNELDESCRIPTORLIST m_ChannelDescriptorList;
};

} // namespace EE

#endif // CVERTEXFORMAT_H