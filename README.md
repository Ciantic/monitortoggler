Monitor utilities
=================

I personally recommend Restore Monitors instead of Monitor Toggler if you just happen 
to have Windows 7. Restore Monitors saves *current* settings of all monitors, and saves
them to file. Afterwards you can change settings and restore the old settings from file.
 

Restore Monitors 0.3
--------------------

    Usage: restoremonitors7.exe [<-save>] <filename>

      Capable of restoring monitors to saved state under Windows 7.
      Uses Windows 7 CCD API to save, and restore the settings from
      file.

      By giving only filename the program tries to open and restore
      the saved settings in the file.

       -save
           Used to save settings to file.

       -equal
           Prints '1' if current settings equals the one in the file,
           otherwise '0' or '  ERROR:...'.


      Authors:    Rob Bresalier (2017)
                  Jari Pennanen (2010) <jari.pennanen@gmail.com>
      License:    FreeBSD License, see COPYING
      Repository: http://github.com/Ciantic/monitortoggler
      

Monitor toggler 0.3.2
---------------------
    Usage: monitortoggler.exe <monitornumber> [<forceState>] [<apply>]

      Capable of attaching and deattaching monitors from command line.

      Note: Resolution / display settings should be configured from Windows dialog.
            At least in Windows 7 the changes are saved to registry, and next time
            you attach the same screen it retrieves same settings. Thus it seems
            to be useless to define them while attaching.

      monitornumber:
                 Monitor number, this may, and usually differs from monitor numbers
                 in the screen resolution dialog. So you have to try several numbers
                 until you find the right one :)

      forceState (optional):
                -1 = Toggles. (Tries to get the current mode, and toggles.) (default)
                 1 = Attach
                 0 = Detach

      apply (optional):
                 1 = Apply changes immediately (default)
                 0 = Do not apply changes
                     You have to call second time to apply changes

      Author:     Jari Pennanen (2010) <jari.pennanen@gmail.com>
      License:    FreeBSD License, see COPYING
      Repository: http://github.com/Ciantic/monitortoggler
