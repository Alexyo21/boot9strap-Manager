/*
*   main.c
*/

#include "crypto/crypto.h"
#include "manager.h"
#include "firm.h"
#include "utils.h"
#include "hid.h"
#include "fs.h"

#ifndef NTRBOOTHAX
//Boot9strap Mod Manager
void main(void)
{
    setupKeyslots();
	
	if(mountSd())
    {
		if(HID_PAD & BUTTON_L1)BS9Manager(false);
        loadFirm(false, false, 0);
        loadFirm(false, false, 0);
        unmountSd();
    }

    if(mountCtrNand())
    {
		if(HID_PAD & BUTTON_L1)BS9Manager(true);
        loadFirm(true, false, 0);
    }
	
    mcuPowerOff();
}

#else

//ntrboohax
void main(void)
{
    setupKeyslots();

	mountSd();
    BS9Manager(false);
    mcuPowerOff();
}
#endif


