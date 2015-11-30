# Building molecules "from the scratch" #

**Ctrl** (PC) or **⌘** (Mac) is primary key while building molecule.

Use Ctrl + Left Mouse Click on a PC or ⌘ + Left Mouse Click on a Mac **to add atoms**. The first atom is placed at the center of the scene. To add more atoms Ctrl/⌘ + Left Mouse Click on existing atom then holding Ctrl/⌘ move mouse pointer somewhere and finally release mouse button. New atom will appear bonded with the previous one.

By default carbons are added. But you can of course **choose any atom type** in Atom combo box on the right side of the window in "Toolbox". The most widely used atoms are available in the drop-down list. To choose something less used just start to type the symbol of atom (for example for Aluminium start to type "A" in the combo box) and then choose necessary atom type in the drop-down list.

If you want to change type of existing atom choose new type in Atom combo box as described above and Ctrl/⌘ + Left Mouse Click on existing atom.

**To bond existing atoms**:  Ctrl/⌘ + Left Mouse Click on the fisrt atom then holding Ctrl/⌘ move mouse pointer on the second atom and release mouse button.

Use Ctrl/⌘ + Left Mouse Click on existing bond **to cycle its order** (1 -> 2 -> 3 -> 1 -> ...).

**To delete atom or bond** use Ctrl/⌘ + Left Mouse Click on it.

Choose Molecule -> New... from main menu **to create a new tab** and start to build another molecule. Do not close tabs. The associated functionality is not ready.

# Molecular Mechanics #

QDalton is able **to pre-optimize molecules** using different force fields. Use Molecular Mechanics page in Toolbox. If you clicked Optimize button but nothing happens, open the log (View -> Log). If you see error message "Force field setup error." try to use another force field.

Structure -> Add hydrogens and build... menu item also uses force field pre-optimization but further to add hydrogens to atoms on the basis of valency. It may be useful in some situations.

# Open and import molecules #

You can always **open any `*`.mol file you made using QDalton**. At this moment the support for Dalton molecule input file keywords is limited. So in general you will be not able to open arbitrary `*`.mol file. Only those previously created in QDalton.

Also **QDalton is able to import geometry from a large variety of chemical formats**. Use Molecule -> Import from... menu item. Choose file and QDalton will try to figure out the file type. If it make a mistake correct it using File type combo box.

# Export molecule #
To export molecule picture to different image formats use Structure -> Export -> Image menu item. Use raster formats only (PNG, JPEG) as there is some bug with vector formats (EPS, PS).

# Known bugs and limitations #

  * Do not close tabs. The associated functionality is not ready.
  * The support for Dalton molecule input file keywords is rather limited.
  * Use raster formats only (PNG, JPEG) as there is some bug with vector formats (EPS, PS).