Cross-platform ([Qt](http://qt.nokia.com/)-based) Graphical User Interface (GUI) for [Dalton](http://www.kjemi.uio.no/software/dalton/dalton.html) quantum chemistry program.

Despite the fact that [Qt is available on many platforms](http://doc.trolltech.com/4.6/supported-platforms.html) the task to provide users with binaries is incredibly complex. Especially for different Linux distros.

Binaries [are available](http://code.google.com/p/qdalton/downloads/list) for:
  * Windows in the form of zipped folder. Extract it somewhere and execute bin\QDalton.exe from QDalton folder. If you get error message during the program start download and install [Microsoft Visual C++ 2008 SP1 Redistributable Package](http://www.microsoft.com/downloads/details.aspx?familyid=A5C84275-3B97-4AB7-A40D-3802B2AF5FC2&displaylang=en).
  * Mac OS X in the form of zipped installer. Extract it and run QDalton 0.1.pkg. It will copy QDalton folder to /Applications.

If you use Linux follow [this guide](http://code.google.com/p/qdalton/wiki/BuildingOnLinux). Actually I was able to compile and run it [on Ubuntu 9.10](http://picasaweb.google.com/lh/photo/K1GN4GGSnQ5rlCWFNtDG2w?feat=directlink). And if you use any other `*`nix flavor I wish you good luck. =)

[Screenshots](http://picasaweb.google.com/kemiisto/QDalton#).

## QDalton 0.1 (25 April 2010) ##
  * View molecules using different view schemes ("Balls & Bonds", "Balls & Sticks", "Sticks", "VdW Spheres").
  * Construct new molecules "from the scratch". Generate simple molecule input files for Dalton.
  * Export molecule picture to different image formats. **Please use raster formats only (PNG, JPEG) as there is some bug with vector formats (EPS, PS).**
  * Import geometry from some widely used input/output files (Gaussian, GAMESS, etc.).
  * Use molecular mechanics for pre-optimization.
  * Read molecule input files **made by QDalton**.

## QDalton 0.2 (28 August 2010) ##
  * Attempt to stabilize everything from 0.1 release.

## QDalton 0.3 (scheduled for ... the New Year) ##
  * Like always: some bugs will be fixed, some will be added. ;-)
  * + Molecular mechanics conformational search.
  * + Ability to open Dalton output files.
  * + Extended support for Dalton molecule input file keywords.