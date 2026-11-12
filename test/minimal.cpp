// Minimal program checking that wxWidgets set up by the action can be used.
//
// This is a console program, but it still uses wxCore and the third-party
// libraries bundled with wxWidgets, to check that they can be linked with.

#include <wx/init.h>
#include <wx/crt.h>
#include <wx/image.h>
#include <wx/imagpng.h>
#include <wx/mstream.h>
#include <wx/regex.h>
#include <wx/utils.h>

int main(int argc, char** argv)
{
    wxInitializer initializer(argc, argv);
    if ( !initializer.IsOk() )
    {
        wxPrintf("Failed to initialize wxWidgets.\n");
        return 1;
    }

    wxPrintf("Using %s\n", wxGetLibraryVersionInfo().GetVersionString());

    // Use libpng and zlib.
    wxImage::AddHandler(new wxPNGHandler);

    wxImage image(16, 16);
    image.SetRGB(wxRect(0, 0, 8, 8), 0xff, 0, 0);

    wxMemoryOutputStream out;
    if ( !image.SaveFile(out, wxBITMAP_TYPE_PNG) )
    {
        wxPrintf("Failed to save PNG image.\n");
        return 1;
    }

    wxMemoryInputStream in(out);
    wxImage loaded(in, wxBITMAP_TYPE_PNG);
    if ( !loaded.IsOk() || loaded.GetRed(0, 0) != 0xff || loaded.GetRed(15, 15) != 0 )
    {
        wxPrintf("Failed to load PNG image back.\n");
        return 1;
    }

    // Use the regex library.
    wxRegEx re("^v([0-9]+)\\.([0-9]+)$");
    if ( !re.Matches("v3.3") || re.GetMatch("v3.3", 2) != "3" )
    {
        wxPrintf("Regular expression unexpectedly didn't match.\n");
        return 1;
    }

    wxPrintf("All checks passed.\n");

    return 0;
}
