# Waveform_Decomposition

# LasViewer

## Overview
LasViewer is an application created using the Microsoft Foundation Class Library. This application not only demonstrates the basic usage of Microsoft Foundation Classes but also serves as a starting point for writing your applications.

## Project Structure
### `LasViewer.vcproj`
The main project file created using the VC++ application wizard. It contains information about the version of Visual C++, platform choices, configurations, and project features.

### `LasViewer.h`
The main header file for the application. It includes other project-specific headers and declares the `CLasViewerApp` application class.

### `LasViewer.cpp`
Contains the main application source file for the `CLasViewerApp` class.

### `LasViewer.rc`
Lists all Microsoft Windows resources used in the program, including icons, bitmaps, and cursors stored in the RES sub-directory.

### Resources Directory
- `res\LasViewer.ico`: The icon file used as the application's icon.
- `res\LasViewer.rc2`: Contains resources that are not edited in Microsoft Visual C++.

### Dialog Class
- `LasViewerDlg.h`, `LasViewerDlg.cpp`: Contains the `CLasViewerDlg` class defining the behavior of the application's main dialog.

## Features
- **ActiveX Controls Support**
- **Printing and Print Preview Support**: Code generated by the application wizard for handling print, print setup, and print preview commands.

## Standard Files
- `StdAfx.h`, `StdAfx.cpp`: Used for generating pre-compiled header (PCH) files.
- `Resource.h`: Standard header file defining new resource IDs.
- `LasViewer.manifest`: Application manifest file for Windows XP.

## Additional Notes
- **TODOs**: Indicated sections of the source code that should be added or customized.
- **MFC DLLs**: Information about redistributing MFC DLLs and corresponding localized resources if the application's language differs from the OS's current locale settings.
