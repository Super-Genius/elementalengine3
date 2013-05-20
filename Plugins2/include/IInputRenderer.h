///============================================================================
/// \file		IInputRenderer.h
/// \brief		Definition file for Input Renderer System
/// \date		05-21-2009
/// \author		Marivaldo Cabral
/// \note   Elemental Engine
///         Copyright (c)  2005-2008 Signature Devices, Inc.
///
///         This code is redistributable under the terms of the EE License.
///
///         This code is distributed without warranty or implied warranty of
///			merchantability or fitness for a particular purpose. See the 
///			EE License for more details.
///
///         You should have received a copy of the EE License along with this
///			code; If not, write to Signature Devices, Inc.,
///			3200 Bridge Parkway Suite 102, Redwood City, CA 94086 USA.
///============================================================================

#ifndef IINPUTRENDERER_H__
#define IINPUTRENDERER_H__
#include <string>
#include <vector>

/// \class IInputRenderer
/// \brief Interface bridge to EE input renderer for Wii
class IInputRenderer
{

public:

	//Textbox used to draw a simple text box on the screen
	struct TextBox
	{
		int id;
		int width, height; 
		float x, y, z; 
		int r, g, b, a;
		float textX, textY;
		bool zoom;
		std::string text;
		std::string texture;
	};

	/// Default Destructor
	virtual ~IInputRenderer() {}

	/// Start Interface Functions ///
	/// \brief Initializes the Renderer
	/// \return bool = 
	virtual bool Initialize() = 0;

	/// \brief This DeInitializes the renderer
	virtual void DeInitialize()= 0;

	/// \brief This function calls draw routines
	virtual void Update(){};

	/// Sets the viewport of the renderer
	/// \param xpos - int indicating x location
	/// \param ypos - int indicating y location
	/// \param width - int indicating the width
	/// \param height - int indicating the height
	virtual void SetViewport( const int xpos, const int ypos, const int width, const int height ){};

	/// Draw a text box on the screen
	/// \param textbox - Contains information of the textbox
	/// \param id - ID for TextBox object
	/// \param bOverrideText - bool indicating to override textbox with the same id (default = true)
	virtual void DrawTextBox( TextBox *textbox, const char* id, bool bOverrideTextBox = true) {};

	/// Draw a list of the text box on the screen
	/// \param textbox - Vector that Contains information of the textbox
	/// \param id - ID for TextBox object
	/// \param bOverrideText - bool indicating to override textbox with the same id (default = false)
	virtual void DrawTextBox( std::vector<TextBox*> textbox, const char* id, bool bOverrideText = false) {};

	/// Hide the TextBox by ID, used when want hide all Text box's at the same time.
	/// \param id - ID for TextBox object
	virtual void HideTextBox( const char* id ) {};

	/// Sets the ortho projection
	/// \param left - float indicating the left
	/// \param right - float indicating the right
	/// \param bottom - float indicating the bottom
	/// \param top - float indicating the top
	/// \param near - float indicating the near
	/// \param far - float indicating the far
	virtual void SetOrtho(float left, float right, float bottom, float top, float near, float far){};


	/// Draws a sphere
	/// \param x,y,z = the position of the sphere
	/// \param radius = the radius of the sphere
	/// \param colorR = red component of color
	/// \param colorG = green component of color
	/// \param colorB = blue component of color
	/// \param iResolution = the resolution with which to render the sphere (default = 4)
	virtual void DrawSphere( float x, float y, float z, float radius, int colorR, int colorG, int colorB, int iResolution=4 ){};

	/// Draws a 2D mask
	/// \param xpos - float of the x position
	/// \param ypos - float of the y position
	/// \param width - float of the width of rectangular mask
	/// \param height - float of the height of rectangular mask
	/// \param texture - ststring of the texture name
	virtual void Draw2DMask(const float xpos, const float ypos, const float width, const float height, const char* texture){};

	/// Draws a 2d line
	/// \param startX - start of the line (x coord)
	/// \param startY - start of the line (y coord)
	/// \param endX - end of the line (x coord)
	/// \param endY - end of the line (y coord)
	/// \param r - red component of the line color
	/// \param g - green component of the line color
	/// \param b - blue component of the line color
	/// \param width - width of the line
	virtual void Draw2DLine(const float startX, const float startY, const float endX, const float endY, const int r, const int g, const int b, const float width){};

	/// Draws a 3d line
	/// \param startX - start of the line (x coord)
	/// \param startY - start of the line (y coord)
	/// \param startZ - start of the line (z coord)
	/// \param endX - end of the line (x coord)
	/// \param endY - end of the line (y coord)
	/// \param endZ - end of the line (z coord)
	/// \param r - red component of the line color
	/// \param g - green component of the line color
	/// \param b - blue component of the line color
	/// \param width - width of the line
	/// \param bOverrideTransform - bool indicating to override transform (default = true)
	virtual void Draw3DLine(const float startX, const float startY, const float startZ, const float endX, const float endY, const float endZ, const int r, const int g, const int b, const float width, bool bOverrideTransform=true){};
	
	/// Draw a 3D point
	/// \param xpos - the x coordinate position
	/// \param ypos - the y coordinate position
	/// \param zpos - the z coordinate position
	/// \param r - red component of the line color
	/// \param g - green component of the line color
	/// \param b - blue component of the line color
	/// \param size - size of the 3d point
	virtual void DrawPoint(const float xpos, const float ypos, const float zpos, const int r, const int g, const int b, const float size){};

};


#endif /// IINPUTRENDERER_H__